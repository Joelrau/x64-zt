#include <std_include.hpp>
#include "physicslibrary.hpp"

namespace zonetool::s4
{
	void physics_library::read(zone_reader& reader, PhysicsLibrary* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_array(asset->havokData, 15, asset->havokDataSize);
		reader.pop_stream();
	}

	void physics_library::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());

		write_array(buf, dest->havokData, data->havokDataSize, 15);

		buf->pop_stream();
	}

	void physics_library::dump(PhysicsLibrary* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("physicslibrary\\"s + asset->name + ".physicslibraryb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		dump.dump_array(asset->havokData, asset->havokDataSize);
		dump.close();
	}

	PhysicsLibrary* physics_library::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("physicslibrary\\"s + name + ".physicslibraryb"))
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
			this->asset_ = mem->allocate<PhysicsLibrary>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	std::string physics_library::name()
	{
		return this->name_;
	}

	std::int32_t physics_library::type()
	{
		return ASSET_TYPE_PHYSICSLIBRARY;
	}
}
