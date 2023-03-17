#include <std_include.hpp>
#include "vertexdecl.hpp"

namespace zonetool::iw6
{
	MaterialVertexDeclaration* IVertexDecl::parse(const std::string& name, ZoneMemory* mem)
	{
		const auto path = "techsets\\" + name + ".vertexdecl";

		assetmanager::reader read(mem);
		if (!read.open(path))
		{
			return nullptr;
		}

		//ZONETOOL_INFO("Parsing vertexdecl \"%s\"...", name.data());

		auto* asset = read.read_single<MaterialVertexDeclaration>();
		asset->name = read.read_string();
		read.close();

		return asset;
	}

	void IVertexDecl::init(const std::string& name, ZoneMemory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->Alloc<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->StrDup(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = DB_FindXAssetHeader_Safe(XAssetType(this->type()), this->name().data()).vertexDecl;

			if (DB_IsXAssetDefault(XAssetType(this->type()), this->name().data()))
			{
				ZONETOOL_FATAL("vertexdecl \"%s\" not found.", name.data());
			}
		}
	}

	void IVertexDecl::prepare(ZoneBuffer* buf, ZoneMemory* mem)
	{
	}

	void IVertexDecl::load_depending(IZone* zone)
	{
	}

	std::string IVertexDecl::name()
	{
		return this->name_;
	}

	std::int32_t IVertexDecl::type()
	{
		return ASSET_TYPE_VERTEXDECL;
	}

	void IVertexDecl::write(IZone* zone, ZoneBuffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(3);

		dest->name = buf->write_str(this->name());

		buf->pop_stream();
	}

	void IVertexDecl::dump(MaterialVertexDeclaration* asset)
	{
		const auto path = "techsets\\"s + asset->name + ".vertexdecl"s;

		assetmanager::dumper write;
		if (!write.open(path))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);
		write.close();
	}
}