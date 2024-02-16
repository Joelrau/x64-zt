#include "std_include.hpp"
#include "physics_fx_shape.hpp"

namespace zonetool::iw7
{
	PhysicsFXShape* physics_fx_shape::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "physicsfxshape\\"s + name;

		assetmanager::reader read(mem);
		if (!read.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing physicsfxshape \"%s\"...", name.data());

		auto* asset = read.read_single<PhysicsFXShape>();
		asset->name = read.read_string();

		asset->vecs = read.read_array<vec3_t>();

		read.close();
		return asset;
	}

	void physics_fx_shape::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).physicsFXShape;
		}
	}

	void physics_fx_shape::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void physics_fx_shape::load_depending(zone_base* zone)
	{
	}

	std::string physics_fx_shape::name()
	{
		return this->name_;
	}

	std::int32_t physics_fx_shape::type()
	{
		return ASSET_TYPE_PHYSICS_FX_SHAPE;
	}

	void physics_fx_shape::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->vecs)
		{
			buf->align(3);
			buf->write(data->vecs, data->size);
			buf->clear_pointer(&dest->vecs);
		}

		buf->pop_stream();
	}

	void physics_fx_shape::dump(PhysicsFXShape* asset)
	{
		const auto path = "physicsfxshape\\"s + asset->name;

		assetmanager::dumper write;
		if (!write.open(path))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);

		write.dump_array(asset->vecs, asset->size);

		write.close();
	}
}