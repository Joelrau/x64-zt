#include "std_include.hpp"
#include "physics_fx_pipeline.hpp"

namespace zonetool::iw7
{
	PhysicsFXPipeline* physics_fx_pipeline::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "physicsfxpipeline\\"s + name;

		assetmanager::reader read(mem);
		if (!read.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing physicsfxpipeline \"%s\"...", name.data());

		auto* asset = read.read_single<PhysicsFXPipeline>();
		asset->name = read.read_string();

		asset->unk.unk = read.read_array<unk_1453E1188>();

		read.close();
		return asset;
	}

	void physics_fx_pipeline::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).physicsFXPipeline;
		}
	}

	void physics_fx_pipeline::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void physics_fx_pipeline::load_depending(zone_base* zone)
	{
	}

	std::string physics_fx_pipeline::name()
	{
		return this->name_;
	}

	std::int32_t physics_fx_pipeline::type()
	{
		return ASSET_TYPE_PHYSICS_FX_PIPELINE;
	}

	void physics_fx_pipeline::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->unk.unk)
		{
			buf->align(3);
			buf->write(data->unk.unk, data->unk.numUnk);
			buf->clear_pointer(&dest->unk.unk);
		}

		buf->pop_stream();
	}

	void physics_fx_pipeline::dump(PhysicsFXPipeline* asset)
	{
		const auto path = "physicsfxpipeline\\"s + asset->name;

		assetmanager::dumper write;
		if (!write.open(path))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);

		write.dump_array(asset->unk.unk, asset->unk.numUnk);

		write.close();
	}
}