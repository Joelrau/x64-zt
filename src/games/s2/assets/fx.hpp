#pragma once

namespace zonetool::s2
{
	class light_def
	{
	public:
		static void read(zone_reader& reader, LightDef* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			reader.read_string(asset->attenuationName);
			reader.pop_stream();
		}
	};

	class particle_sim_animation
	{
	public:
		static void read(zone_reader& reader, FxParticleSimAnimation* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			reader.read_asset(ASSET_TYPE_MATERIAL, asset->material);
			reader.read_stream(asset->frames, 31, 32, asset->frameCount);
			reader.read_stream(asset->samples, 3, 8, asset->sampleCount);
			reader.read_stream(asset->states, 3, 16, asset->stateCount);
			reader.pop_stream();
		}
	};

	class fx_effect_def
	{
	public:
		static void read(zone_reader& reader, FxEffectDef* asset);
	};

	class impact_fx
	{
	public:
		static void read(zone_reader& reader, FxImpactTable* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			if (const auto table = reader.read_array(asset->table, 7, 23))
			{
				for (auto i = 0; i < 23; i++)
				{
					read_effects(reader, table[i].nonflesh);
					read_effects(reader, table[i].flesh);
				}
			}
			if (const auto surfaces = reader.read_array(asset->surfaces, 7, 72))
			{
				for (auto i = 0; i < 72; i++)
				{
					reader.read_string(surfaces[i].name);
					reader.read_asset(ASSET_TYPE_FX, surfaces[i].effect);
				}
			}
			reader.pop_stream();
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

	class surface_fx
	{
	public:
		static void read(zone_reader& reader, SurfaceFxTable* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			if (const auto table = reader.read_array(asset->table, 7, 11))
			{
				for (auto i = 0; i < 11; i++)
				{
					for (auto& effect : table[i].surfaceEffect)
					{
						reader.read_asset(ASSET_TYPE_FX, effect);
					}
				}
			}
			reader.pop_stream();
		}
	};

	class vector_field
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
					const auto& blocks = sub_fields[i].numBlocks;
					reader.read_array(sub_fields[i].vectorData, 3, blocks[0] * blocks[1] * blocks[2]);
				}
			}
			reader.pop_stream();
		}
	};
}
