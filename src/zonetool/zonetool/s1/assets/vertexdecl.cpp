#include <std_include.hpp>
#include "vertexdecl.hpp"

namespace zonetool::s1
{
	MaterialVertexDeclaration* vertex_decl::parse(const std::string& name, zone_memory* mem)
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

	void vertex_decl::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).vertexDecl;

			if (DB_IsXAssetDefault(XAssetType(this->type()), this->name().data()))
			{
				ZONETOOL_FATAL("vertexdecl \"%s\" not found.", name.data());
			}
		}
	}

	void vertex_decl::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void vertex_decl::load_depending(zone_base* zone)
	{
	}

	std::string vertex_decl::name()
	{
		return this->name_;
	}

	std::int32_t vertex_decl::type()
	{
		return ASSET_TYPE_VERTEXDECL;
	}

	void vertex_decl::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(3);

		dest->name = buf->write_str(this->name());

		buf->pop_stream();
	}

	void vertex_decl::dump(MaterialVertexDeclaration* asset)
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