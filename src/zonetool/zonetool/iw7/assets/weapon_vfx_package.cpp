#include "std_include.hpp"
#include "weapon_vfx_package.hpp"

namespace zonetool::iw7
{
#define PARSE_STRING(__field__) \
	static_assert(std::is_same_v<decltype(asset->__field__), const char*>, "Field is not of type const char*"); \
	!data[#__field__].is_null() && !data[#__field__].empty() ? asset->__field__ = mem->duplicate_string(data[#__field__].get<std::string>()) : asset->__field__ = nullptr;

#define PARSE_FXCOMBINED(__field__) \
	if (!data[#__field__].is_null()) \
	{ \
		asset->__field__.u.data = mem->manual_allocate<char>(sizeof(const char*)); \
		asset->__field__.type = data[#__field__]["type"].get<FxCombinedType>(); \
		asset->__field__.u.fx->name = mem->duplicate_string(data[#__field__]["fx"].get<std::string>()); \
	} \
	else \
	{ \
		asset->__field__.u.data = nullptr; \
	}

	void parse_vfx(WeaponVFXPackageDefs* asset, ordered_json& data, zone_memory* mem)
	{
		PARSE_FXCOMBINED(viewFlashEffect);
		PARSE_FXCOMBINED(worldFlashEffect);
		PARSE_FXCOMBINED(viewFlashADSEffect);
		PARSE_FXCOMBINED(viewFlashOverchargeEffect);
		PARSE_FXCOMBINED(viewFlashOverchargeADSEffect);
		PARSE_FXCOMBINED(worldFlashOverchargeEffect);
		PARSE_FXCOMBINED(viewShellEjectEffect);
		PARSE_FXCOMBINED(worldShellEjectEffect);
		PARSE_FXCOMBINED(viewLastShotEjectEffect);
		PARSE_FXCOMBINED(worldLastShotEjectEffect);
		PARSE_FXCOMBINED(viewMagEjectEffect);
	}

	WeaponVFXPackage* weapon_vfx_package::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "vfxpkg\\"s + name + ".json"s;

		auto file = filesystem::file(path);
		if (!file.exists())
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing vfxpkg \"%s\"...", name.data());

		// parse json file
		file.open("rb");
		ordered_json data = json::parse(file.read_bytes(file.size()));
		file.close();

		auto asset = mem->allocate<WeaponVFXPackage>();

		PARSE_STRING(name);
		if (!data["vfx"].is_null())
		{
			asset->vfx = mem->allocate<WeaponVFXPackageDefs>();
			parse_vfx(asset->vfx, data["vfx"], mem);
		}
		else
		{
			asset->vfx = nullptr;
		}

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
		auto* asset = this->asset_;
		if (asset->vfx)
		{
#define LOAD_DEPENDING_FXCOMBINED(__field__) \
			if(asset->vfx->__field__.u.data) \
			{ \
				if(asset->vfx->__field__.type == FX_COMBINED_PARTICLE_SYSTEM) \
				{ \
					zone->add_asset_of_type(ASSET_TYPE_VFX, asset->vfx->__field__.u.particleSystemDef->name); \
				} \
				else \
				{ \
					zone->add_asset_of_type(ASSET_TYPE_FX, asset->vfx->__field__.u.fx->name); \
				} \
			}

			LOAD_DEPENDING_FXCOMBINED(viewFlashEffect);
			LOAD_DEPENDING_FXCOMBINED(worldFlashEffect);
			LOAD_DEPENDING_FXCOMBINED(viewFlashADSEffect);
			LOAD_DEPENDING_FXCOMBINED(viewFlashOverchargeEffect);
			LOAD_DEPENDING_FXCOMBINED(viewFlashOverchargeADSEffect);
			LOAD_DEPENDING_FXCOMBINED(worldFlashOverchargeEffect);
			LOAD_DEPENDING_FXCOMBINED(viewShellEjectEffect);
			LOAD_DEPENDING_FXCOMBINED(worldShellEjectEffect);
			LOAD_DEPENDING_FXCOMBINED(viewLastShotEjectEffect);
			LOAD_DEPENDING_FXCOMBINED(worldLastShotEjectEffect);
			LOAD_DEPENDING_FXCOMBINED(viewMagEjectEffect);

#undef LOAD_DEPENDING_FXCOMBINED
		}
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

		if (data->vfx)
		{
			buf->align(7);
			dest->vfx = buf->write(data->vfx);

#define WRITE_FXCOMBINED(__field__) \
			if(data->vfx->__field__.u.data) \
			{ \
				if(data->vfx->__field__.type == FX_COMBINED_PARTICLE_SYSTEM) \
				{ \
					dest->vfx->__field__.u.particleSystemDef = reinterpret_cast<ParticleSystemDef*>(zone->get_asset_pointer(ASSET_TYPE_VFX, data->vfx->__field__.u.particleSystemDef->name)); \
				} \
				else \
				{ \
					dest->vfx->__field__.u.fx = reinterpret_cast<FxEffectDef*>(zone->get_asset_pointer(ASSET_TYPE_FX, data->vfx->__field__.u.particleSystemDef->name)); \
				} \
			}

			WRITE_FXCOMBINED(viewFlashEffect);
			WRITE_FXCOMBINED(worldFlashEffect);
			WRITE_FXCOMBINED(viewFlashADSEffect);
			WRITE_FXCOMBINED(viewFlashOverchargeEffect);
			WRITE_FXCOMBINED(viewFlashOverchargeADSEffect);
			WRITE_FXCOMBINED(worldFlashOverchargeEffect);
			WRITE_FXCOMBINED(viewShellEjectEffect);
			WRITE_FXCOMBINED(worldShellEjectEffect);
			WRITE_FXCOMBINED(viewLastShotEjectEffect);
			WRITE_FXCOMBINED(worldLastShotEjectEffect);
			WRITE_FXCOMBINED(viewMagEjectEffect);

#undef WRITE_FXCOMBINED

			buf->clear_pointer(&dest->vfx);
		}

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