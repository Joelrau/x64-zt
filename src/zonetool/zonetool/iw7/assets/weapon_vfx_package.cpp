#include "std_include.hpp"
#include "weapon_vfx_package.hpp"

namespace zonetool::iw7
{
	WeaponVFXPackage* weapon_vfx_package::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "vfxpkg\\"s + name + ".json"s;
		filesystem::file file(path);
		file.open("rb");

		if (file.get_fp())
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing vfxpkg \"%s\"...", name.data());

		const auto size = file.size();
		auto bytes = file.read_bytes(size);
		file.close();

		auto data = json::parse(bytes);

		auto asset = mem->allocate<WeaponVFXPackage>();

		asset->name = mem->duplicate_string(data["name"].get<std::string>());

		//

		return asset;
	}

	void weapon_vfx_package::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).weaponVFXPackage;
		}
	}

	void weapon_vfx_package::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void weapon_vfx_package::load_depending(zone_base* zone)
	{
	}

	std::string weapon_vfx_package::name()
	{
		return this->name_;
	}

	std::int32_t weapon_vfx_package::type()
	{
		return ASSET_TYPE_VFX_PACKAGE;
	}

	void weapon_vfx_package::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		//

		buf->pop_stream();
	}

#define DUMP_STRING(__field__) \
	static_assert(std::is_same_v<decltype(asset->__field__), const char*>, "Field is not of type const char*"); \
	asset->__field__ ? data[#__field__] = asset->__field__ : data[#__field__] = "";

#define DUMP_FXCOMBINED(__field__) \
	if (asset->__field__.u.data) \
	{ \
		data[#__field__]["type"] = asset->__field__.type; \
		data[#__field__]["fx"] = asset->__field__.u.fx->name; \
	} \
	else \
	{ \
		data[#__field__] = nullptr; \
	}

	void dump_vfx(WeaponVFXPackageDefs* asset, ordered_json& data)
	{
		DUMP_FXCOMBINED(viewFlashEffect);
		DUMP_FXCOMBINED(worldFlashEffect);
		DUMP_FXCOMBINED(viewFlashADSEffect);
		DUMP_FXCOMBINED(viewFlashOverchargeEffect);
		DUMP_FXCOMBINED(viewFlashOverchargeADSEffect);
		DUMP_FXCOMBINED(worldFlashOverchargeEffect);
		DUMP_FXCOMBINED(viewShellEjectEffect);
		DUMP_FXCOMBINED(worldShellEjectEffect);
		DUMP_FXCOMBINED(viewLastShotEjectEffect);
		DUMP_FXCOMBINED(worldLastShotEjectEffect);
		DUMP_FXCOMBINED(viewMagEjectEffect);
	}

	void weapon_vfx_package::dump(WeaponVFXPackage* asset)
	{
		const auto path = "vfxpkg\\"s + asset->name + ".json"s;
		auto file = filesystem::file(path);
		file.open("wb");

		ordered_json data;

		DUMP_STRING(name);
		if (asset->vfx)
		{
			dump_vfx(asset->vfx, data["vfx"]);
		}
		else
		{
			data["vfx"] = nullptr;
		}

		auto str = data.dump(4);
		data.clear();
		file.write(str);
		file.close();
	}
}