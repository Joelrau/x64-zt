#include <std_include.hpp>
#include "animpackage.hpp"

namespace zonetool::s4
{
	void weapon_anim_package::read(zone_reader& reader, WeaponAnimPackage* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_asset_array(ASSET_TYPE_XANIMPARTS, asset->anims, 677);
		reader.read_single(asset->timers, 1);
		reader.read_asset_array(ASSET_TYPE_BLEND_SPACE_2D, asset->blendSpaces, 8);
		reader.read_array(asset->footstep, 3, 13);
		reader.pop_stream();
	}

	void weapon_anim_package::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		write_asset_array(zone, buf, ASSET_TYPE_XANIMPARTS, dest->anims, 677);
		write_array(buf, dest->timers, 1, 1);
		write_asset_array(zone, buf, ASSET_TYPE_BLEND_SPACE_2D, dest->blendSpaces, 8);
		write_array(buf, dest->footstep, 13, 3);
		buf->pop_stream();
	}

	void weapon_anim_package::dump(WeaponAnimPackage* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("animpkg\\"s + asset->name + ".animpkgb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		dump_asset_array(dump, asset->anims, 677);
		dump.dump_single(asset->timers);
		dump_asset_array(dump, asset->blendSpaces, 8);
		dump.dump_array(asset->footstep, 13);
		dump.close();
	}

	WeaponAnimPackage* weapon_anim_package::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("animpkg\\"s + name + ".animpkgb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing animpkg \"%s\"...", name.data());

		auto* asset = read.read_single<WeaponAnimPackage>();
		asset->name = read.read_string();
		parse_asset_array(read, asset->anims, 677);
		asset->timers = read.read_single<WeaponAnimPackageStateTimers>();
		parse_asset_array(read, asset->blendSpaces, 8);
		asset->footstep = read.read_array<FootstepAnim>();
		read.close();
		return asset;
	}

	void weapon_anim_package::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<WeaponAnimPackage>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void weapon_anim_package::load_depending(zone_base* zone)
	{
		if (this->referenced())
		{
			return;
		}

		add_asset_array(zone, ASSET_TYPE_XANIMPARTS, this->asset_->anims, 677);
		add_asset_array(zone, ASSET_TYPE_BLEND_SPACE_2D, this->asset_->blendSpaces, 8);
	}

	std::string weapon_anim_package::name()
	{
		return this->name_;
	}

	std::int32_t weapon_anim_package::type()
	{
		return ASSET_TYPE_ANIM_PACKAGE;
	}
}
