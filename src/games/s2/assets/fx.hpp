#pragma once

namespace zonetool::s2
{
	class light_def : public asset_base<LightDef, ASSET_TYPE_LIGHT_DEF>
	{
	public:
		static void read(zone_reader& reader, LightDef* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			reader.read_string(asset->attenuationName);
			reader.pop_stream();
		}

		LightDef* parse(const std::string& name, zone_memory* mem) override
		{
			assetmanager::reader read(mem);
			if (!read.open(dump_path(ASSET_TYPE_LIGHT_DEF, name.data())))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing lightdef \"%s\"...", name.data());

			auto* asset = read.read_single<LightDef>();
			asset->name = read.read_string();
			asset->attenuationName = read.read_string();
			read.close();

			return asset;
		}

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			buf->push_stream(XFILE_BLOCK_VIRTUAL);
			dest->name = this->write_name(buf);
			write_string(buf, data->attenuationName, dest->attenuationName);
			buf->pop_stream();
		}

		static void dump(LightDef* asset)
		{
			assetmanager::dumper write;
			if (!write.open(dump_path(ASSET_TYPE_LIGHT_DEF, asset->name)))
			{
				return;
			}

			write.dump_single(asset);
			write.dump_string(asset->name);
			write.dump_string(asset->attenuationName);
			write.close();
		}
	};

	class particle_sim_animation : public asset_base<FxParticleSimAnimation, ASSET_TYPE_PARTICLE_SIM_ANIMATION>
	{
	public:
		static constexpr std::size_t frame_size = 32;
		static constexpr std::size_t sample_size = 8;
		static constexpr std::size_t state_size = 16;

		static void read(zone_reader& reader, FxParticleSimAnimation* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			reader.read_asset(ASSET_TYPE_MATERIAL, asset->material);
			reader.read_stream(asset->frames, 31, frame_size, asset->frameCount);
			reader.read_stream(asset->samples, 3, sample_size, asset->sampleCount);
			reader.read_stream(asset->states, 3, state_size, asset->stateCount);
			reader.pop_stream();
		}

		FxParticleSimAnimation* parse(const std::string& name, zone_memory* mem) override
		{
			assetmanager::reader read(mem);
			if (!read.open(dump_path(ASSET_TYPE_PARTICLE_SIM_ANIMATION, name.data())))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing particlesimanimation \"%s\"...", name.data());

			auto* asset = read.read_single<FxParticleSimAnimation>();
			asset->name = read.read_string();
			asset->material = read.read_asset<Material>();
			asset->frames = read.read_array<char>();
			asset->samples = read.read_array<char>();
			asset->states = read.read_array<char>();
			read.close();

			return asset;
		}

		void load_depending(zone_base* zone) override
		{
			add_dependency(zone, ASSET_TYPE_MATERIAL, this->asset_->material);
		}

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			buf->push_stream(XFILE_BLOCK_VIRTUAL);
			dest->name = this->write_name(buf);
			dest->material = asset_reference(zone, ASSET_TYPE_MATERIAL, data->material);
			write_inline_bytes(buf, 31, data->frames, frame_size * data->frameCount, dest->frames);
			write_inline_bytes(buf, 3, data->samples, sample_size * data->sampleCount, dest->samples);
			write_inline_bytes(buf, 3, data->states, state_size * data->stateCount, dest->states);
			buf->pop_stream();
		}

		static void dump(FxParticleSimAnimation* asset)
		{
			assetmanager::dumper write;
			if (!write.open(dump_path(ASSET_TYPE_PARTICLE_SIM_ANIMATION, asset->name)))
			{
				return;
			}

			write.dump_single(asset);
			write.dump_string(asset->name);
			write.dump_asset(asset->material);
			write.dump_array(asset->frames, static_cast<std::uint32_t>(frame_size * asset->frameCount));
			write.dump_array(asset->samples, static_cast<std::uint32_t>(sample_size * asset->sampleCount));
			write.dump_array(asset->states, static_cast<std::uint32_t>(state_size * asset->stateCount));
			write.close();
		}
	};

	class fx_effect_def : public asset_base<FxEffectDef, ASSET_TYPE_FX>
	{
	public:
		static void read(zone_reader& reader, FxEffectDef* asset);

		FxEffectDef* parse(const std::string& name, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;
		void write(zone_base* zone, zone_buffer* buf) override;
		static void dump(FxEffectDef* asset);
	};

	template <std::size_t N>
	void add_effect_dependencies(zone_base* zone, FxEffectDef* const (&effects)[N])
	{
		for (const auto* effect : effects)
		{
			add_dependency(zone, ASSET_TYPE_FX, effect);
		}
	}

	template <std::size_t N>
	void write_effect_references(zone_base* zone, FxEffectDef* const (&effects)[N], FxEffectDef* (&dest)[N])
	{
		for (std::size_t i = 0; i < N; i++)
		{
			dest[i] = asset_reference(zone, ASSET_TYPE_FX, effects[i]);
		}
	}

	template <std::size_t N>
	void dump_effect_references(assetmanager::dumper& write, FxEffectDef* const (&effects)[N])
	{
		for (auto* effect : effects)
		{
			write.dump_asset(effect);
		}
	}

	template <std::size_t N>
	void parse_effect_references(assetmanager::reader& read, FxEffectDef* (&effects)[N])
	{
		for (auto& effect : effects)
		{
			effect = read.read_asset<FxEffectDef>();
		}
	}

	class impact_fx : public asset_base<FxImpactTable, ASSET_TYPE_IMPACT_FX>
	{
	public:
		static constexpr std::size_t entry_count = 23;
		static constexpr std::size_t surface_count = 72;

		static void read(zone_reader& reader, FxImpactTable* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			if (const auto table = reader.read_array(asset->table, 7, entry_count))
			{
				for (std::size_t i = 0; i < entry_count; i++)
				{
					read_effects(reader, table[i].nonflesh);
					read_effects(reader, table[i].flesh);
				}
			}
			if (const auto surfaces = reader.read_array(asset->surfaces, 7, surface_count))
			{
				for (std::size_t i = 0; i < surface_count; i++)
				{
					reader.read_string(surfaces[i].name);
					reader.read_asset(ASSET_TYPE_FX, surfaces[i].effect);
				}
			}
			reader.pop_stream();
		}

		FxImpactTable* parse(const std::string& name, zone_memory* mem) override
		{
			assetmanager::reader read(mem);
			if (!read.open(dump_path(ASSET_TYPE_IMPACT_FX, name.data())))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing impactfx \"%s\"...", name.data());

			auto* asset = read.read_single<FxImpactTable>();
			asset->name = read.read_string();
			asset->table = read.read_array<FxImpactEntry>();
			for (std::size_t i = 0; asset->table && i < entry_count; i++)
			{
				parse_effect_references(read, asset->table[i].nonflesh);
				parse_effect_references(read, asset->table[i].flesh);
			}
			asset->surfaces = read.read_array<FxImpactSurfaceEntry>();
			for (std::size_t i = 0; asset->surfaces && i < surface_count; i++)
			{
				asset->surfaces[i].name = read.read_string();
				asset->surfaces[i].effect = read.read_asset<FxEffectDef>();
			}
			read.close();

			return asset;
		}

		void load_depending(zone_base* zone) override
		{
			for (std::size_t i = 0; this->asset_->table && i < entry_count; i++)
			{
				add_effect_dependencies(zone, this->asset_->table[i].nonflesh);
				add_effect_dependencies(zone, this->asset_->table[i].flesh);
			}
			for (std::size_t i = 0; this->asset_->surfaces && i < surface_count; i++)
			{
				add_dependency(zone, ASSET_TYPE_FX, this->asset_->surfaces[i].effect);
			}
		}

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			buf->push_stream(XFILE_BLOCK_VIRTUAL);
			dest->name = this->write_name(buf);
			if (data->table)
			{
				buf->align(7);
				auto* table = buf->write(data->table, entry_count);
				for (std::size_t i = 0; i < entry_count; i++)
				{
					write_effect_references(zone, data->table[i].nonflesh, table[i].nonflesh);
					write_effect_references(zone, data->table[i].flesh, table[i].flesh);
				}
				buf->clear_pointer(&dest->table);
			}
			if (data->surfaces)
			{
				buf->align(7);
				auto* surfaces = buf->write(data->surfaces, surface_count);
				for (std::size_t i = 0; i < surface_count; i++)
				{
					write_string(buf, data->surfaces[i].name, surfaces[i].name);
					surfaces[i].effect = asset_reference(zone, ASSET_TYPE_FX, data->surfaces[i].effect);
				}
				buf->clear_pointer(&dest->surfaces);
			}
			buf->pop_stream();
		}

		static void dump(FxImpactTable* asset)
		{
			assetmanager::dumper write;
			if (!write.open(dump_path(ASSET_TYPE_IMPACT_FX, asset->name)))
			{
				return;
			}

			write.dump_single(asset);
			write.dump_string(asset->name);
			write.dump_array(asset->table, entry_count);
			for (std::size_t i = 0; asset->table && i < entry_count; i++)
			{
				dump_effect_references(write, asset->table[i].nonflesh);
				dump_effect_references(write, asset->table[i].flesh);
			}
			write.dump_array(asset->surfaces, surface_count);
			for (std::size_t i = 0; asset->surfaces && i < surface_count; i++)
			{
				write.dump_string(asset->surfaces[i].name);
				write.dump_asset(asset->surfaces[i].effect);
			}
			write.close();
		}

	private:
		template <std::size_t N>
		static void read_effects(zone_reader& reader, FxEffectDef* (&effects)[N])
		{
			for (auto& effect : effects)
			{
				reader.read_asset(ASSET_TYPE_FX, effect);
			}
		}
	};

	class surface_fx : public asset_base<SurfaceFxTable, ASSET_TYPE_SURFACE_FX>
	{
	public:
		static constexpr std::size_t entry_count = 11;

		static void read(zone_reader& reader, SurfaceFxTable* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			if (const auto table = reader.read_array(asset->table, 7, entry_count))
			{
				for (std::size_t i = 0; i < entry_count; i++)
				{
					for (auto& effect : table[i].surfaceEffect)
					{
						reader.read_asset(ASSET_TYPE_FX, effect);
					}
				}
			}
			reader.pop_stream();
		}

		SurfaceFxTable* parse(const std::string& name, zone_memory* mem) override
		{
			assetmanager::reader read(mem);
			if (!read.open(dump_path(ASSET_TYPE_SURFACE_FX, name.data())))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing surfacefx \"%s\"...", name.data());

			auto* asset = read.read_single<SurfaceFxTable>();
			asset->name = read.read_string();
			asset->table = read.read_array<SurfaceFxEntry>();
			for (std::size_t i = 0; asset->table && i < entry_count; i++)
			{
				parse_effect_references(read, asset->table[i].surfaceEffect);
			}
			read.close();

			return asset;
		}

		void load_depending(zone_base* zone) override
		{
			for (std::size_t i = 0; this->asset_->table && i < entry_count; i++)
			{
				add_effect_dependencies(zone, this->asset_->table[i].surfaceEffect);
			}
		}

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			buf->push_stream(XFILE_BLOCK_VIRTUAL);
			dest->name = this->write_name(buf);
			if (data->table)
			{
				buf->align(7);
				auto* table = buf->write(data->table, entry_count);
				for (std::size_t i = 0; i < entry_count; i++)
				{
					write_effect_references(zone, data->table[i].surfaceEffect, table[i].surfaceEffect);
				}
				buf->clear_pointer(&dest->table);
			}
			buf->pop_stream();
		}

		static void dump(SurfaceFxTable* asset)
		{
			assetmanager::dumper write;
			if (!write.open(dump_path(ASSET_TYPE_SURFACE_FX, asset->name)))
			{
				return;
			}

			write.dump_single(asset);
			write.dump_string(asset->name);
			write.dump_array(asset->table, entry_count);
			for (std::size_t i = 0; asset->table && i < entry_count; i++)
			{
				dump_effect_references(write, asset->table[i].surfaceEffect);
			}
			write.close();
		}
	};

	class vector_field : public asset_base<VectorField, ASSET_TYPE_VECTORFIELD>
	{
	public:
		static void read(zone_reader& reader, VectorField* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			if (const auto sub_fields = reader.read_array(asset->subFields, 15, asset->numSubFields))
			{
				for (auto i = 0u; i < asset->numSubFields; i++)
				{
					reader.read_array(sub_fields[i].vectorData, 3, block_count(sub_fields[i]));
				}
			}
			reader.pop_stream();
		}

		VectorField* parse(const std::string& name, zone_memory* mem) override
		{
			assetmanager::reader read(mem);
			if (!read.open(dump_path(ASSET_TYPE_VECTORFIELD, name.data())))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing vectorfield \"%s\"...", name.data());

			auto* asset = read.read_single<VectorField>();
			asset->name = read.read_string();
			asset->subFields = read.read_array<VectorSubField>();
			for (auto i = 0u; asset->subFields && i < asset->numSubFields; i++)
			{
				asset->subFields[i].vectorData = read.read_array<char[16]>();
			}
			read.close();

			return asset;
		}

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			buf->push_stream(XFILE_BLOCK_VIRTUAL);
			dest->name = this->write_name(buf);
			if (data->subFields)
			{
				buf->align(15);
				auto* sub_fields = buf->write(data->subFields, data->numSubFields);
				for (auto i = 0u; i < data->numSubFields; i++)
				{
					write_inline(buf, 3, data->subFields[i].vectorData, block_count(data->subFields[i]), sub_fields[i].vectorData);
				}
				buf->clear_pointer(&dest->subFields);
			}
			buf->pop_stream();
		}

		static void dump(VectorField* asset)
		{
			assetmanager::dumper write;
			if (!write.open(dump_path(ASSET_TYPE_VECTORFIELD, asset->name)))
			{
				return;
			}

			write.dump_single(asset);
			write.dump_string(asset->name);
			write.dump_array(asset->subFields, asset->numSubFields);
			for (auto i = 0u; asset->subFields && i < asset->numSubFields; i++)
			{
				write.dump_array(asset->subFields[i].vectorData, block_count(asset->subFields[i]));
			}
			write.close();
		}

	private:
		static std::uint32_t block_count(const VectorSubField& field)
		{
			return field.numBlocks[0] * field.numBlocks[1] * field.numBlocks[2];
		}
	};
}
