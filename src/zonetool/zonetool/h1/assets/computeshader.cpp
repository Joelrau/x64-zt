#include <std_include.hpp>
#include "computeshader.hpp"

namespace zonetool::h1
{
	ComputeShader* IComputeShader::parse(const std::string& name, ZoneMemory* mem)
	{
		const auto path = "techsets\\" + name + ".computeshader";

		assetmanager::reader read(mem);
		if (!read.open(path))
		{
			return nullptr;
		}

		//ZONETOOL_INFO("Parsing computeshader \"%s\"...", name.data());

		auto* asset = read.read_single<ComputeShader>();
		asset->name = read.read_string();
		asset->prog.loadDef.program = read.read_array<unsigned char>();
		read.close();

		return asset;
	}

	void IComputeShader::init(const std::string& name, ZoneMemory* mem)
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
			this->asset_ = DB_FindXAssetHeader_Safe(XAssetType(this->type()), this->name().data()).computeShader;

			if (DB_IsXAssetDefault(XAssetType(this->type()), this->name().data()))
			{
				ZONETOOL_FATAL("computeshader \"%s\" not found.", name.data());
			}
		}
	}

	void IComputeShader::prepare(ZoneBuffer* buf, ZoneMemory* mem)
	{
	}

	void IComputeShader::load_depending(IZone* zone)
	{
	}

	std::string IComputeShader::name()
	{
		return this->name_;
	}

	std::int32_t IComputeShader::type()
	{
		return ASSET_TYPE_COMPUTESHADER;
	}

	void IComputeShader::write(IZone* zone, ZoneBuffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(3);
		dest->name = buf->write_str(this->name());

		buf->push_stream(0);
		if (data->prog.loadDef.program)
		{
			dest->prog.loadDef.program = buf->write_s(3, data->prog.loadDef.program, data->prog.loadDef.programSize);
		}
		buf->pop_stream();

		buf->pop_stream();
	}

	void IComputeShader::dump(ComputeShader* asset)
	{
		const auto path = "techsets\\"s + asset->name + ".computeshader"s;

		assetmanager::dumper write;
		if (!write.open(path))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);
		write.dump_array(asset->prog.loadDef.program, asset->prog.loadDef.programSize);
		write.close();
	}
}