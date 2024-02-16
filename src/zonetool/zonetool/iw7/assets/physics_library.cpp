#include "std_include.hpp"
#include "physics_library.hpp"

namespace zonetool::iw7
{
	PhysicsLibrary* physics_library::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "physicslibrary\\"s + name;

		assetmanager::reader read(mem);
		if (!read.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing physicslibrary \"%s\"...", name.data());

		auto* asset = read.read_single<PhysicsLibrary>();
		asset->name = read.read_string();

		asset->havokData = read.read_array<char>();

		read.close();
		return asset;
	}

	void physics_library::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).physicsLibrary;
		}
	}

	void physics_library::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void physics_library::load_depending(zone_base* zone)
	{
	}

	std::string physics_library::name()
	{
		return this->name_;
	}

	std::int32_t physics_library::type()
	{
		return ASSET_TYPE_PHYSICSLIBRARY;
	}

	void physics_library::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->havokData)
		{
			buf->align(15);
			buf->write(data->havokData, data->havokDataSize);
			buf->clear_pointer(&dest->havokData);
		}

		buf->pop_stream();
	}

	void physics_library::dump(PhysicsLibrary* asset)
	{
		const auto path = "physicslibrary\\"s + asset->name;

		assetmanager::dumper write;
		if (!write.open(path))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);

		write.dump_array(asset->havokData, asset->havokDataSize);

		write.close();
	}
}