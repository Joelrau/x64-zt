#include "std_include.hpp"
#include "gesture.hpp"

namespace zonetool::iw7
{
	const char* gesture::anim_names[GESTURE_ASSET_NUM] =
	{
		"GESTURE_ASSET_LEFT_CENTER",
		"GESTURE_ASSET_LEFT_DOWN",
		"GESTURE_ASSET_LEFT_LEFT",
		"GESTURE_ASSET_LEFT_RIGHT",
		"GESTURE_ASSET_LEFT_UP",
		"GESTURE_ASSET_RIGHT_CENTER",
		"GESTURE_ASSET_RIGHT_DOWN",
		"GESTURE_ASSET_RIGHT_LEFT",
		"GESTURE_ASSET_RIGHT_RIGHT",
		"GESTURE_ASSET_RIGHT_UP",
		"GESTURE_ASSET_IN",
		"GESTURE_ASSET_OUT",
		"GESTURE_ASSET_IN_ADDITIVE",
		"GESTURE_ASSET_OUT_ADDITIVE",
		"GESTURE_ASSET_PITCH",
		"GESTURE_ASSET_YAW",
		"GESTURE_ASSET_WEAPON_CHECK",
		"GESTURE_ASSET_WALK_ADDITIVE",
	};

	namespace
	{
		const char* get_anim_name_from_index(GestureDirectionalAssetIndex index)
		{
			return gesture::anim_names[index];
		}

		const char* get_anim_name_from_index(int index)
		{
			return gesture::anim_names[index];
		}

		GestureDirectionalAssetIndex get_anim_index_from_name(const char* name)
		{
			for (unsigned int i = 0; i < GESTURE_ASSET_NUM; i++)
			{
				const char* anim_name = gesture::anim_names[i];
				if (!_stricmp(anim_name, name))
				{
					return static_cast<GestureDirectionalAssetIndex>(i);
				}
			}

			ZONETOOL_FATAL("Invalid animation index returned for name: \"%s\"", name);
			//return WEAP_ANIM_INVALID;
		}
	}

#define PARSE_STRING(__field__) \
	static_assert(std::is_same_v<decltype(asset->__field__), const char*>, "Field is not of type const char*"); \
	!data[#__field__].is_null() && !data[#__field__].get<std::string>().empty() ? asset->__field__ = mem->duplicate_string(data[#__field__].get<std::string>()) : asset->__field__ = nullptr;

#define PARSE_FIELD(__field__) \
	if (!data[#__field__].is_null()) asset->__field__ = data[#__field__].get<decltype(asset->__field__)>();

#define PARSE_ASSET_ARR(__field__, __size__) \
	if (!data[#__field__].is_null()) \
	{ \
		asset->__field__ = mem->allocate<typename std::remove_reference<decltype(*asset->__field__)>::type>(__size__); \
		for (auto idx##__field__ = 0u; idx##__field__ < (unsigned int)__size__; idx##__field__++) \
		{ \
			if (!data[#__field__][idx##__field__].is_null() && !data[#__field__][idx##__field__].get<std::string>().empty()) \
			{ \
				asset->__field__[idx##__field__] = mem->manual_allocate<typename std::remove_reference<decltype(*asset->__field__[idx##__field__])>::type>(sizeof(const char*)); \
				asset->__field__[idx##__field__]->name = mem->duplicate_string(data[#__field__][idx##__field__].get<std::string>()); \
			} \
			else \
			{ \
				asset->__field__[idx##__field__] = nullptr; \
			} \
		} \
	} \
	else \
	{ \
		asset->__field__ = nullptr; \
	}

	namespace
	{
		void parse(MovementTime* asset, ordered_json& data)
		{
			PARSE_FIELD(time);
			PARSE_FIELD(isLeadIn);
		}

		void parse(FootstepTime* asset, ordered_json& data)
		{
			PARSE_FIELD(time);
			PARSE_FIELD(isLeft);
		}

		void parse(FootstepAnim* asset, ordered_json& data)
		{
			PARSE_FIELD(leftCount);
			for (auto i = 0; i < 4; i++)
			{
				parse(&asset->step[i], data["step"][i]);
			}
			for (auto i = 0; i < 4; i++)
			{
				parse(&asset->movement[i], data["movement"][i]);
			}
		}
	}

	void parse_settings(GestureLookAroundSettings* asset, ordered_json& data)
	{
		PARSE_FIELD(yawLerpIn);
		PARSE_FIELD(yawLerpOut);
		PARSE_FIELD(walkTime);
		parse(&asset->walkFootStepAnim, data["footstep"]);
	}

	void parse_settings(GestureDirectionalSettings* asset, ordered_json& data)
	{
		PARSE_FIELD(maxAngle);
		PARSE_FIELD(lerpAtMaxAngle);
		PARSE_FIELD(widthCushionAngle);
		PARSE_FIELD(lerpAtMinCushionAngle);
		PARSE_FIELD(lerpAtMaxCushionAngle);
		PARSE_FIELD(limitLeft);
		PARSE_FIELD(limitRight);
		PARSE_FIELD(limitUp);
		PARSE_FIELD(limitDown);
	}

	void parse_settings(GestureWeaponSettings* asset, ordered_json& data)
	{
		PARSE_FIELD(blendToStates);
		PARSE_FIELD(hideReticle);
		PARSE_FIELD(fireDelay);
		PARSE_FIELD(sprintDelay);
		PARSE_FIELD(useLeftIdleAkimbo);
		PARSE_FIELD(splitAnimsAkimbo);
		PARSE_FIELD(blendToDemeanorLoop);
		PARSE_FIELD(blendOutRaise);
	}

	Gesture* gesture::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "gesture\\"s + name + ".json"s;

		auto file = filesystem::file(path);
		if (!file.exists())
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing gesture \"%s\"...", name.data());

		// parse json file
		file.open("rb");
		ordered_json data = json::parse(file.read_bytes(file.size()));
		file.close();

		auto asset = mem->allocate<Gesture>();
		asset->name = mem->duplicate_string(name);

		PARSE_FIELD(type);
		PARSE_FIELD(priority);
		PARSE_FIELD(looping);

		parse_settings(&asset->weaponSettings, data["weaponSettings"]);
		parse_settings(&asset->directionalSettings, data["directionalSettings"]);
		parse_settings(&asset->lookAroundSettings, data["lookAroundSettings"]);

		//PARSE_ASSET_ARR(anims, GESTURE_ASSET_NUM);
		if (!data["anims"].is_null()) {
			asset->anims = mem->allocate<typename std::remove_reference<decltype(*asset->anims)>::type>(GESTURE_ASSET_NUM); for (auto idxanims = 0u; idxanims < (unsigned int)GESTURE_ASSET_NUM; idxanims++) {
				if (!data["anims"][get_anim_name_from_index(idxanims)].is_null() && !data["anims"][get_anim_name_from_index(idxanims)].get<std::string>().empty()) {
					asset->anims[idxanims] = mem->manual_allocate<typename std::remove_reference<decltype(*asset->anims[idxanims])>::type>(sizeof(const char*)); asset->anims[idxanims]->name = mem->duplicate_string(data["anims"][get_anim_name_from_index(idxanims)].get<std::string>());
				}
				else {
					asset->anims[idxanims] = nullptr;
				}
			}
		}
		else {
			asset->anims = nullptr;
		};

		return asset;
	}

	void gesture::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).gesture;
		}
	}

	void gesture::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void gesture::load_depending(zone_base* zone)
	{
		auto* asset = this->asset_;
		
		if (asset->anims)
		{
			for (auto i = 0; i < GESTURE_ASSET_NUM; i++)
			{
				if (asset->anims[i])
				{
					zone->add_asset_of_type(ASSET_TYPE_XANIMPARTS, asset->anims[i]->name);
				}
			}
		}
	}

	std::string gesture::name()
	{
		return this->name_;
	}

	std::int32_t gesture::type()
	{
		return ASSET_TYPE_GESTURE;
	}

	void gesture::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->anims)
		{
			buf->align(7);
			dest->anims = buf->write(data->anims, GESTURE_ASSET_NUM);
			for (auto i = 0; i < GESTURE_ASSET_NUM; i++)
			{
				if (data->anims[i])
				{
					dest->anims[i] = reinterpret_cast<XAnimParts*>(zone->get_asset_pointer(ASSET_TYPE_XANIMPARTS, data->anims[i]->name));
				}
			}
			buf->clear_pointer(&dest->anims);
		}

		buf->pop_stream();
	}

#define DUMP_STRING(__field__) \
	static_assert(std::is_same_v<decltype(asset->__field__), const char*>, "Field is not of type const char*"); \
	asset->__field__ ? data[#__field__] = asset->__field__ : data[#__field__] = "";

#define DUMP_FIELD(__field__) \
	data[#__field__] = asset->__field__;

#define DUMP_ASSET_ARR(__field__, __size__) \
	if (asset->__field__ && __size__) \
	{ \
		for (auto idx##__field__ = 0u; idx##__field__ < (unsigned int)__size__; idx##__field__++) \
		{ \
			if (asset->__field__[idx##__field__]) \
			{ \
				data[#__field__][idx##__field__] = asset->__field__[idx##__field__]->name; \
			} \
			else \
			{ \
				data[#__field__][idx##__field__] = ""; \
			} \
		} \
	} \
	else \
	{ \
		data[#__field__] = nullptr; \
	}

	namespace
	{
		void dump(MovementTime* asset, ordered_json& data)
		{
			DUMP_FIELD(time);
			DUMP_FIELD(isLeadIn);
		}

		void dump(FootstepTime* asset, ordered_json& data)
		{
			DUMP_FIELD(time);
			DUMP_FIELD(isLeft);
		}

		void dump(FootstepAnim* asset, ordered_json& data)
		{
			DUMP_FIELD(leftCount);
			for (auto i = 0; i < 4; i++)
			{
				dump(&asset->step[i], data["step"][i]);
			}
			for (auto i = 0; i < 4; i++)
			{
				dump(&asset->movement[i], data["movement"][i]);
			}
		}
	}

	void dump_settings(GestureLookAroundSettings* asset, ordered_json& data)
	{
		DUMP_FIELD(yawLerpIn);
		DUMP_FIELD(yawLerpOut);
		DUMP_FIELD(walkTime);
		dump(&asset->walkFootStepAnim, data["footstep"]);
	}

	void dump_settings(GestureDirectionalSettings* asset, ordered_json& data)
	{
		DUMP_FIELD(maxAngle);
		DUMP_FIELD(lerpAtMaxAngle);
		DUMP_FIELD(widthCushionAngle);
		DUMP_FIELD(lerpAtMinCushionAngle);
		DUMP_FIELD(lerpAtMaxCushionAngle);
		DUMP_FIELD(limitLeft);
		DUMP_FIELD(limitRight);
		DUMP_FIELD(limitUp);
		DUMP_FIELD(limitDown);
	}

	void dump_settings(GestureWeaponSettings* asset, ordered_json& data)
	{
		DUMP_FIELD(blendToStates);
		DUMP_FIELD(hideReticle);
		DUMP_FIELD(fireDelay);
		DUMP_FIELD(sprintDelay);
		DUMP_FIELD(useLeftIdleAkimbo);
		DUMP_FIELD(splitAnimsAkimbo);
		DUMP_FIELD(blendToDemeanorLoop);
		DUMP_FIELD(blendOutRaise);
	}

	void gesture::dump(Gesture* asset)
	{
		const auto path = "gesture\\"s + asset->name + ".json"s;
		auto file = filesystem::file(path);
		file.open("wb");

		ordered_json data;

		DUMP_FIELD(type);
		DUMP_FIELD(priority);
		DUMP_FIELD(looping);
		
		dump_settings(&asset->weaponSettings, data["weaponSettings"]);
		dump_settings(&asset->directionalSettings, data["directionalSettings"]);
		dump_settings(&asset->lookAroundSettings, data["lookAroundSettings"]);

		//DUMP_ASSET_ARR(anims, GESTURE_ASSET_NUM);
		if (asset->anims && GESTURE_ASSET_NUM) {
			for (auto idxanims = 0u; idxanims < (unsigned int)GESTURE_ASSET_NUM; idxanims++) {
				if (asset->anims[idxanims]) {
					data["anims"][get_anim_name_from_index(idxanims)] = asset->anims[idxanims]->name;
				}
				else {
					data["anims"][get_anim_name_from_index(idxanims)] = "";
				}
			}
		}
		else {
			data["anims"] = nullptr;
		};

		auto str = data.dump(4);
		data.clear();
		file.write(str);
		file.close();
	}
}