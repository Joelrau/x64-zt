#pragma once

namespace zonetool
{
	template <ASSET_TEMPLATE>
	class laser_def : public asset_interface
	{
		typedef TYPEOF_MEMBER(S, laserTag) scr_string_t;
		typedef TYPEOF_MEMBER(S, laserMaterial) Material;
		typedef TYPEOF_MEMBER(S, laserEndEffect) FxEffectDef;

	private:
		std::string name_;
		S* asset_ = nullptr;

		std::vector<std::pair<scr_string_t*, const char*>> script_strings;

		void add_script_string(scr_string_t* ptr, const char* str)
		{
			for (std::uint32_t i = 0; i < this->script_strings.size(); i++)
			{
				if (this->script_strings[i].first == ptr)
				{
					return;
				}
			}
			this->script_strings.push_back(std::pair<scr_string_t*, const char*>(ptr, str));
		}

		const char* get_script_string(scr_string_t* ptr)
		{
			for (std::uint32_t i = 0; i < this->script_strings.size(); i++)
			{
				if (this->script_strings[i].first == ptr)
				{
					return this->script_strings[i].second;
				}
			}
			return nullptr;
		}

	public:
		S* parse(const std::string& name, zone_memory* mem)
		{
			const auto path = "lasers\\"s + name + ".laser"s;

			assetmanager::reader read(mem);
			if (!read.open(path))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing laser \"%s\"...", name.data());

			const auto asset = read.read_single<S>();
			read.read_string(&asset->name);
			read.read_asset(&asset->laserMaterial);
			read.read_asset(&asset->laserLightMaterial);
			read.read_asset(&asset->friendlyTeamLaser);
			read.read_asset(&asset->laserEndEffect);

			this->add_script_string(&asset->laserTag, read.read_string());

			return asset;
		}

		void init(const std::string& name, zone_memory* mem) override
		{
			this->name_ = name;

			if (this->referenced())
			{
				this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
				this->asset_->name = mem->duplicate_string(name);
			}

			this->asset_ = parse(name, mem);
			if (!this->asset_)
			{
				this->asset_ = db_find_x_asset_header_safe<H, E>(this->type(), this->name().data()).laser;
			}
		}

		void prepare(zone_buffer* buf, zone_memory* mem) override
		{
			auto* data = this->asset_;

			data->laserTag = static_cast<scr_string_t>(buf->write_scriptstring(
				this->get_script_string(&data->laserTag)));
		}

		void load_depending(zone_base* zone) override
		{
			auto data = this->asset_;

			if (data->laserMaterial)
			{
				zone->add_asset_of_type(Types::ASSET_TYPE_MATERIAL, data->laserMaterial->name);
			}

			if (data->laserLightMaterial)
			{
				zone->add_asset_of_type(Types::ASSET_TYPE_MATERIAL, data->laserLightMaterial->name);
			}

			if (data->laserEndEffect)
			{
				zone->add_asset_of_type(Types::ASSET_TYPE_FX, data->laserEndEffect->name);
			}

			if (data->friendlyTeamLaser)
			{
				zone->add_asset_of_type(Types::ASSET_TYPE_LASER, data->friendlyTeamLaser->name);
			}
		}

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }

		std::string name() override
		{
			return this->name_;
		}

		std::int32_t type() override
		{
			return Type;
		}

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto data = this->asset_;
			auto dest = buf->write(data);

			buf->push_stream(Streams::XFILE_BLOCK_VIRTUAL);

			dest->name = buf->write_str(this->name());

			if (data->laserMaterial)
			{
				dest->laserMaterial = reinterpret_cast<Material*>(
					zone->get_asset_pointer(Types::ASSET_TYPE_MATERIAL, data->laserMaterial->name)
					);
			}

			if (data->laserLightMaterial)
			{
				dest->laserLightMaterial = reinterpret_cast<Material*>(
					zone->get_asset_pointer(Types::ASSET_TYPE_MATERIAL, data->laserLightMaterial->name)
					);
			}

			if (data->laserEndEffect)
			{
				dest->laserEndEffect = reinterpret_cast<FxEffectDef*>(
					zone->get_asset_pointer(Types::ASSET_TYPE_FX, data->laserEndEffect->name)
					);
			}

			if (data->friendlyTeamLaser)
			{
				dest->friendlyTeamLaser = reinterpret_cast<S*>(
					zone->get_asset_pointer(Types::ASSET_TYPE_LASER, data->friendlyTeamLaser->name)
					);
			}

			buf->pop_stream();
		}

		static void dump(S* asset)
		{
			const auto path = "lasers\\"s + asset->name + ".laser";

			assetmanager::dumper write;
			if (!write.open(path))
			{
				return;
			}

			write.dump_single(asset);
			write.dump_string(asset->name);
			write.dump_asset(asset->laserMaterial);
			write.dump_asset(asset->laserLightMaterial);
			write.dump_asset(asset->friendlyTeamLaser);
			write.dump_asset(asset->laserEndEffect);

			write.dump_string(SL_ConvertToString(asset->laserTag));

			write.close();
		}
	};
}