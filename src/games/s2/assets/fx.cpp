#include <std_include.hpp>
#include "s2.hpp"

namespace zonetool::s2
{
	namespace
	{
		enum fx_elem_type : std::uint8_t
		{
			FX_ELEM_TYPE_TRAIL = 5,
			FX_ELEM_TYPE_FLARE = 6,
			FX_ELEM_TYPE_PARTICLE_SIM_ANIMATION = 7,
			FX_ELEM_TYPE_SPARK_FOUNTAIN = 10,
			FX_ELEM_TYPE_MODEL = 11,
			FX_ELEM_TYPE_OMNI_LIGHT = 12,
			FX_ELEM_TYPE_SPOT_LIGHT = 13,
			FX_ELEM_TYPE_SOUND = 14,
			FX_ELEM_TYPE_VECTORFIELD = 15,
			FX_ELEM_TYPE_DECAL = 16,
			FX_ELEM_TYPE_RUNNER = 17,
			FX_ELEM_TYPE_STRING_18 = 18,
			FX_ELEM_TYPE_STRING_19 = 19,
			FX_ELEM_TYPE_INLINE_20 = 20,
		};

		void read_visual(zone_reader& reader, const std::uint8_t elem_type, FxElemVisuals* visual)
		{
			switch (elem_type)
			{
			case FX_ELEM_TYPE_MODEL:
				reader.read_asset(ASSET_TYPE_XMODEL, visual->model);
				break;
			case FX_ELEM_TYPE_RUNNER:
				reader.read_string(visual->effectDef.name);
				break;
			case FX_ELEM_TYPE_SOUND:
			case FX_ELEM_TYPE_VECTORFIELD:
			case FX_ELEM_TYPE_STRING_18:
			case FX_ELEM_TYPE_STRING_19:
				reader.read_string(visual->soundName);
				break;
			case FX_ELEM_TYPE_INLINE_20:
				break;
			case FX_ELEM_TYPE_PARTICLE_SIM_ANIMATION:
				reader.read_asset(ASSET_TYPE_PARTICLE_SIM_ANIMATION, visual->particleSimAnimation);
				break;
			case FX_ELEM_TYPE_OMNI_LIGHT:
				break;
			case FX_ELEM_TYPE_SPOT_LIGHT:
				reader.read_asset(ASSET_TYPE_LIGHT_DEF, visual->lightDef);
				break;
			default:
				reader.read_asset(ASSET_TYPE_MATERIAL, visual->material);
				break;
			}
		}

		void read_visuals(zone_reader& reader, FxElemDef* elem)
		{
			if (elem->elemType == FX_ELEM_TYPE_DECAL)
			{
				if (const auto marks = reader.read_array(elem->visuals.markArray, 7, elem->visualCount))
				{
					for (auto i = 0; i < elem->visualCount; i++)
					{
						for (auto& material : marks[i].materials)
						{
							reader.read_asset(ASSET_TYPE_MATERIAL, material);
						}
					}
				}
			}
			else if (elem->visualCount > 1)
			{
				if (const auto visuals = reader.read_array(elem->visuals.array, 7, elem->visualCount))
				{
					for (auto i = 0; i < elem->visualCount; i++)
					{
						read_visual(reader, elem->elemType, &visuals[i]);
					}
				}
			}
			else
			{
				read_visual(reader, elem->elemType, &elem->visuals.instance);
			}
		}

		void read_extended(zone_reader& reader, FxElemDef* elem)
		{
			auto& extended = elem->extended;

			switch (elem->elemType)
			{
			case FX_ELEM_TYPE_TRAIL:
				if (const auto trail = reader.read_single(extended.trailDef, 7))
				{
					reader.read_array(trail->verts, 3, trail->vertCount);
					reader.read_array(trail->inds, 1, trail->indCount);
				}
				break;
			case FX_ELEM_TYPE_SPARK_FOUNTAIN:
				reader.read_single(extended.sparkFountainDef, 3);
				break;
			case FX_ELEM_TYPE_SPOT_LIGHT:
				if (const auto spot = reader.read_single(extended.spotLightDef, 7))
				{
					reader.read_string(spot->name);
				}
				break;
			case FX_ELEM_TYPE_OMNI_LIGHT:
				if (const auto omni = reader.read_single(extended.omniLightDef, 7))
				{
					reader.read_string(omni->name);
				}
				break;
			case FX_ELEM_TYPE_FLARE:
				if (const auto flare = reader.read_single(extended.flareDef, 7))
				{
					reader.read_array(flare->intensityX, 3, flare->intensityXIntervalCount + 1);
					reader.read_array(flare->intensityY, 3, flare->intensityYIntervalCount + 1);
					reader.read_array(flare->srcCosIntensity, 3, flare->srcCosIntensityIntervalCount + 1);
					reader.read_array(flare->srcCosScale, 3, flare->srcCosScaleIntervalCount + 1);
				}
				break;
			default:
				reader.read_single(extended.unknownBytes, 0);
				break;
			}
		}

		void read_elem(zone_reader& reader, FxElemDef* elem)
		{
			reader.read_stream(elem->velSamples, 3, 148, elem->velIntervalCount + 1);
			reader.read_stream(elem->visSamples, 3, 120, elem->visStateIntervalCount + 1);
			read_visuals(reader, elem);
			reader.read_string(elem->effectOnImpact.name);
			reader.read_string(elem->effectOnDeath.name);
			reader.read_string(elem->effectEmitted.name);
			read_extended(reader, elem);
			reader.read_string(elem->name);
		}
	}

	void fx_effect_def::read(zone_reader& reader, FxEffectDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		const auto elem_count = asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission;
		if (const auto elems = reader.read_array(asset->elemDefs, 7, elem_count))
		{
			for (auto i = 0; i < elem_count; i++)
			{
				read_elem(reader, &elems[i]);
			}
		}

		reader.pop_stream();
	}

	namespace
	{
		constexpr std::size_t vel_sample_size = 148;
		constexpr std::size_t vis_sample_size = 120;

		int elem_count(const FxEffectDef* asset)
		{
			return asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission;
		}

		template <typename F>
		void for_each_visual(FxElemDef* elem, F&& callback)
		{
			if (elem->elemType == FX_ELEM_TYPE_DECAL)
			{
				return;
			}

			if (elem->visualCount > 1)
			{
				for (auto i = 0; elem->visuals.array && i < elem->visualCount; i++)
				{
					callback(elem->visuals.array[i]);
				}
			}
			else
			{
				callback(elem->visuals.instance);
			}
		}

		// effects reference each other by name, so a cycle would recurse and add an effect twice without this guard
		thread_local std::unordered_set<std::string> loading_effects;

		void add_effect_by_name(zone_base* zone, const char* name)
		{
			if (name && *name && !loading_effects.contains(name))
			{
				zone->add_asset_of_type(ASSET_TYPE_FX, name);
			}
		}

		void add_visual_dependencies(zone_base* zone, const std::uint8_t elem_type, const FxElemVisuals& visual)
		{
			switch (elem_type)
			{
			case FX_ELEM_TYPE_MODEL:
				add_dependency(zone, ASSET_TYPE_XMODEL, visual.model);
				break;
			case FX_ELEM_TYPE_RUNNER:
				add_effect_by_name(zone, visual.effectDef.name);
				break;
			case FX_ELEM_TYPE_SOUND:
			case FX_ELEM_TYPE_VECTORFIELD:
			case FX_ELEM_TYPE_STRING_18:
			case FX_ELEM_TYPE_STRING_19:
			case FX_ELEM_TYPE_INLINE_20:
			case FX_ELEM_TYPE_OMNI_LIGHT:
				break;
			case FX_ELEM_TYPE_PARTICLE_SIM_ANIMATION:
				add_dependency(zone, ASSET_TYPE_PARTICLE_SIM_ANIMATION, visual.particleSimAnimation);
				break;
			case FX_ELEM_TYPE_SPOT_LIGHT:
				add_dependency(zone, ASSET_TYPE_LIGHT_DEF, visual.lightDef);
				break;
			default:
				add_dependency(zone, ASSET_TYPE_MATERIAL, visual.material);
				break;
			}
		}

		void write_visual(zone_base* zone, zone_buffer* buf, const std::uint8_t elem_type, const FxElemVisuals& visual, FxElemVisuals& dest)
		{
			switch (elem_type)
			{
			case FX_ELEM_TYPE_MODEL:
				dest.model = asset_reference(zone, ASSET_TYPE_XMODEL, visual.model);
				break;
			case FX_ELEM_TYPE_RUNNER:
				write_string(buf, visual.effectDef.name, dest.effectDef.name);
				break;
			case FX_ELEM_TYPE_SOUND:
			case FX_ELEM_TYPE_VECTORFIELD:
			case FX_ELEM_TYPE_STRING_18:
			case FX_ELEM_TYPE_STRING_19:
				write_string(buf, visual.soundName, dest.soundName);
				break;
			case FX_ELEM_TYPE_INLINE_20:
			case FX_ELEM_TYPE_OMNI_LIGHT:
				break;
			case FX_ELEM_TYPE_PARTICLE_SIM_ANIMATION:
				dest.particleSimAnimation = asset_reference(zone, ASSET_TYPE_PARTICLE_SIM_ANIMATION, visual.particleSimAnimation);
				break;
			case FX_ELEM_TYPE_SPOT_LIGHT:
				dest.lightDef = asset_reference(zone, ASSET_TYPE_LIGHT_DEF, visual.lightDef);
				break;
			default:
				dest.material = asset_reference(zone, ASSET_TYPE_MATERIAL, visual.material);
				break;
			}
		}

		void write_visuals(zone_base* zone, zone_buffer* buf, FxElemDef* elem, FxElemDef* dest)
		{
			if (elem->elemType == FX_ELEM_TYPE_DECAL)
			{
				if (elem->visuals.markArray)
				{
					buf->align(7);
					auto* marks = buf->write(elem->visuals.markArray, elem->visualCount);
					for (auto i = 0; i < elem->visualCount; i++)
					{
						for (auto j = 0u; j < std::size(marks[i].materials); j++)
						{
							marks[i].materials[j] = asset_reference(zone, ASSET_TYPE_MATERIAL, elem->visuals.markArray[i].materials[j]);
						}
					}
					buf->clear_pointer(&dest->visuals.markArray);
				}
			}
			else if (elem->visualCount > 1)
			{
				if (elem->visuals.array)
				{
					buf->align(7);
					auto* visuals = buf->write(elem->visuals.array, elem->visualCount);
					for (auto i = 0; i < elem->visualCount; i++)
					{
						write_visual(zone, buf, elem->elemType, elem->visuals.array[i], visuals[i]);
					}
					buf->clear_pointer(&dest->visuals.array);
				}
			}
			else
			{
				write_visual(zone, buf, elem->elemType, elem->visuals.instance, dest->visuals.instance);
			}
		}

		void write_extended(zone_buffer* buf, FxElemDef* elem, FxElemDef* dest)
		{
			const auto& extended = elem->extended;
			if (!extended.unknownDef)
			{
				return;
			}

			switch (elem->elemType)
			{
			case FX_ELEM_TYPE_TRAIL:
			{
				buf->align(7);
				auto* trail = buf->write(extended.trailDef);
				write_inline(buf, 3, extended.trailDef->verts, extended.trailDef->vertCount, trail->verts);
				write_inline(buf, 1, extended.trailDef->inds, extended.trailDef->indCount, trail->inds);
				buf->clear_pointer(&dest->extended.trailDef);
				break;
			}
			case FX_ELEM_TYPE_SPARK_FOUNTAIN:
				write_inline(buf, 3, extended.sparkFountainDef, 1, dest->extended.sparkFountainDef);
				break;
			case FX_ELEM_TYPE_SPOT_LIGHT:
			{
				buf->align(7);
				auto* spot = buf->write(extended.spotLightDef);
				write_string(buf, extended.spotLightDef->name, spot->name);
				buf->clear_pointer(&dest->extended.spotLightDef);
				break;
			}
			case FX_ELEM_TYPE_OMNI_LIGHT:
			{
				buf->align(7);
				auto* omni = buf->write(extended.omniLightDef);
				write_string(buf, extended.omniLightDef->name, omni->name);
				buf->clear_pointer(&dest->extended.omniLightDef);
				break;
			}
			case FX_ELEM_TYPE_FLARE:
			{
				auto* flare = extended.flareDef;
				buf->align(7);
				auto* dest_flare = buf->write(flare);
				write_inline(buf, 3, flare->intensityX, flare->intensityXIntervalCount + 1, dest_flare->intensityX);
				write_inline(buf, 3, flare->intensityY, flare->intensityYIntervalCount + 1, dest_flare->intensityY);
				write_inline(buf, 3, flare->srcCosIntensity, flare->srcCosIntensityIntervalCount + 1, dest_flare->srcCosIntensity);
				write_inline(buf, 3, flare->srcCosScale, flare->srcCosScaleIntervalCount + 1, dest_flare->srcCosScale);
				buf->clear_pointer(&dest->extended.flareDef);
				break;
			}
			default:
				write_inline(buf, 0, extended.unknownBytes, 1, dest->extended.unknownBytes);
				break;
			}
		}

		void write_elem(zone_base* zone, zone_buffer* buf, FxElemDef* elem, FxElemDef* dest)
		{
			write_inline_bytes(buf, 3, elem->velSamples, vel_sample_size * (elem->velIntervalCount + 1), dest->velSamples);
			write_inline_bytes(buf, 3, elem->visSamples, vis_sample_size * (elem->visStateIntervalCount + 1), dest->visSamples);
			write_visuals(zone, buf, elem, dest);
			write_string(buf, elem->effectOnImpact.name, dest->effectOnImpact.name);
			write_string(buf, elem->effectOnDeath.name, dest->effectOnDeath.name);
			write_string(buf, elem->effectEmitted.name, dest->effectEmitted.name);
			write_extended(buf, elem, dest);
			write_string(buf, elem->name, dest->name);
		}

		void dump_visual(assetmanager::dumper& write, const std::uint8_t elem_type, const FxElemVisuals& visual)
		{
			switch (elem_type)
			{
			case FX_ELEM_TYPE_MODEL:
				write.dump_asset(visual.model);
				break;
			case FX_ELEM_TYPE_RUNNER:
				write.dump_string(visual.effectDef.name);
				break;
			case FX_ELEM_TYPE_SOUND:
			case FX_ELEM_TYPE_VECTORFIELD:
			case FX_ELEM_TYPE_STRING_18:
			case FX_ELEM_TYPE_STRING_19:
				write.dump_string(visual.soundName);
				break;
			case FX_ELEM_TYPE_INLINE_20:
			case FX_ELEM_TYPE_OMNI_LIGHT:
				break;
			case FX_ELEM_TYPE_PARTICLE_SIM_ANIMATION:
				write.dump_asset(visual.particleSimAnimation);
				break;
			case FX_ELEM_TYPE_SPOT_LIGHT:
				write.dump_asset(visual.lightDef);
				break;
			default:
				write.dump_asset(visual.material);
				break;
			}
		}

		void parse_visual(assetmanager::reader& read, const std::uint8_t elem_type, FxElemVisuals& visual)
		{
			switch (elem_type)
			{
			case FX_ELEM_TYPE_MODEL:
				visual.model = read.read_asset<XModel>();
				break;
			case FX_ELEM_TYPE_RUNNER:
				visual.effectDef.name = read.read_string();
				break;
			case FX_ELEM_TYPE_SOUND:
			case FX_ELEM_TYPE_VECTORFIELD:
			case FX_ELEM_TYPE_STRING_18:
			case FX_ELEM_TYPE_STRING_19:
				visual.soundName = read.read_string();
				break;
			case FX_ELEM_TYPE_INLINE_20:
			case FX_ELEM_TYPE_OMNI_LIGHT:
				break;
			case FX_ELEM_TYPE_PARTICLE_SIM_ANIMATION:
				visual.particleSimAnimation = read.read_asset<FxParticleSimAnimation>();
				break;
			case FX_ELEM_TYPE_SPOT_LIGHT:
				visual.lightDef = read.read_asset<LightDef>();
				break;
			default:
				visual.material = read.read_asset<Material>();
				break;
			}
		}

		void dump_visuals(assetmanager::dumper& write, FxElemDef* elem)
		{
			if (elem->elemType == FX_ELEM_TYPE_DECAL)
			{
				write.dump_array(elem->visuals.markArray, elem->visualCount);
				for (auto i = 0; elem->visuals.markArray && i < elem->visualCount; i++)
				{
					for (auto* material : elem->visuals.markArray[i].materials)
					{
						write.dump_asset(material);
					}
				}
			}
			else if (elem->visualCount > 1)
			{
				write.dump_array(elem->visuals.array, elem->visualCount);
				for (auto i = 0; elem->visuals.array && i < elem->visualCount; i++)
				{
					dump_visual(write, elem->elemType, elem->visuals.array[i]);
				}
			}
			else
			{
				dump_visual(write, elem->elemType, elem->visuals.instance);
			}
		}

		void parse_visuals(assetmanager::reader& read, FxElemDef* elem)
		{
			if (elem->elemType == FX_ELEM_TYPE_DECAL)
			{
				elem->visuals.markArray = read.read_array<FxElemMarkVisuals>();
				for (auto i = 0; elem->visuals.markArray && i < elem->visualCount; i++)
				{
					for (auto& material : elem->visuals.markArray[i].materials)
					{
						material = read.read_asset<Material>();
					}
				}
			}
			else if (elem->visualCount > 1)
			{
				elem->visuals.array = read.read_array<FxElemVisuals>();
				for (auto i = 0; elem->visuals.array && i < elem->visualCount; i++)
				{
					parse_visual(read, elem->elemType, elem->visuals.array[i]);
				}
			}
			else
			{
				parse_visual(read, elem->elemType, elem->visuals.instance);
			}
		}

		void dump_extended(assetmanager::dumper& write, FxElemDef* elem)
		{
			const auto& extended = elem->extended;
			switch (elem->elemType)
			{
			case FX_ELEM_TYPE_TRAIL:
				write.dump_single(extended.trailDef);
				if (extended.trailDef)
				{
					write.dump_array(extended.trailDef->verts, extended.trailDef->vertCount);
					write.dump_array(extended.trailDef->inds, extended.trailDef->indCount);
				}
				break;
			case FX_ELEM_TYPE_SPARK_FOUNTAIN:
				write.dump_single(extended.sparkFountainDef);
				break;
			case FX_ELEM_TYPE_SPOT_LIGHT:
				write.dump_single(extended.spotLightDef);
				if (extended.spotLightDef)
				{
					write.dump_string(extended.spotLightDef->name);
				}
				break;
			case FX_ELEM_TYPE_OMNI_LIGHT:
				write.dump_single(extended.omniLightDef);
				if (extended.omniLightDef)
				{
					write.dump_string(extended.omniLightDef->name);
				}
				break;
			case FX_ELEM_TYPE_FLARE:
				write.dump_single(extended.flareDef);
				if (const auto flare = extended.flareDef)
				{
					write.dump_array(flare->intensityX, flare->intensityXIntervalCount + 1);
					write.dump_array(flare->intensityY, flare->intensityYIntervalCount + 1);
					write.dump_array(flare->srcCosIntensity, flare->srcCosIntensityIntervalCount + 1);
					write.dump_array(flare->srcCosScale, flare->srcCosScaleIntervalCount + 1);
				}
				break;
			default:
				write.dump_single(extended.unknownBytes);
				break;
			}
		}

		void parse_extended(assetmanager::reader& read, FxElemDef* elem)
		{
			auto& extended = elem->extended;
			switch (elem->elemType)
			{
			case FX_ELEM_TYPE_TRAIL:
				extended.trailDef = read.read_single<FxTrailDef>();
				if (extended.trailDef)
				{
					extended.trailDef->verts = read.read_array<FxTrailVertex>();
					extended.trailDef->inds = read.read_array<unsigned short>();
				}
				break;
			case FX_ELEM_TYPE_SPARK_FOUNTAIN:
				extended.sparkFountainDef = read.read_single<FxSparkFountainDef>();
				break;
			case FX_ELEM_TYPE_SPOT_LIGHT:
				extended.spotLightDef = read.read_single<FxSpotLightDef>();
				if (extended.spotLightDef)
				{
					extended.spotLightDef->name = read.read_string();
				}
				break;
			case FX_ELEM_TYPE_OMNI_LIGHT:
				extended.omniLightDef = read.read_single<FxOmniLightDef>();
				if (extended.omniLightDef)
				{
					extended.omniLightDef->name = read.read_string();
				}
				break;
			case FX_ELEM_TYPE_FLARE:
				extended.flareDef = read.read_single<FxFlareDef>();
				if (const auto flare = extended.flareDef)
				{
					flare->intensityX = read.read_array<float>();
					flare->intensityY = read.read_array<float>();
					flare->srcCosIntensity = read.read_array<float>();
					flare->srcCosScale = read.read_array<float>();
				}
				break;
			default:
				extended.unknownBytes = read.read_single<char>();
				break;
			}
		}

		void dump_elem(assetmanager::dumper& write, FxElemDef* elem)
		{
			write.dump_array(elem->velSamples, static_cast<std::uint32_t>(vel_sample_size * (elem->velIntervalCount + 1)));
			write.dump_array(elem->visSamples, static_cast<std::uint32_t>(vis_sample_size * (elem->visStateIntervalCount + 1)));
			dump_visuals(write, elem);
			write.dump_string(elem->effectOnImpact.name);
			write.dump_string(elem->effectOnDeath.name);
			write.dump_string(elem->effectEmitted.name);
			dump_extended(write, elem);
			write.dump_string(elem->name);
		}

		void parse_elem(assetmanager::reader& read, FxElemDef* elem)
		{
			elem->velSamples = read.read_array<char>();
			elem->visSamples = read.read_array<char>();
			parse_visuals(read, elem);
			elem->effectOnImpact.name = read.read_string();
			elem->effectOnDeath.name = read.read_string();
			elem->effectEmitted.name = read.read_string();
			parse_extended(read, elem);
			elem->name = read.read_string();
		}
	}

	FxEffectDef* fx_effect_def::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open(dump_path(ASSET_TYPE_FX, name.data())))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing fx \"%s\"...", name.data());

		auto* asset = read.read_single<FxEffectDef>();
		asset->name = read.read_string();
		asset->elemDefs = read.read_array<FxElemDef>();
		for (auto i = 0; asset->elemDefs && i < elem_count(asset); i++)
		{
			parse_elem(read, &asset->elemDefs[i]);
		}
		read.close();

		return asset;
	}

	void fx_effect_def::load_depending(zone_base* zone)
	{
		loading_effects.insert(this->name());

		auto* data = this->asset_;
		for (auto i = 0; data->elemDefs && i < elem_count(data); i++)
		{
			auto* elem = &data->elemDefs[i];
			if (elem->elemType == FX_ELEM_TYPE_DECAL)
			{
				for (auto j = 0; elem->visuals.markArray && j < elem->visualCount; j++)
				{
					for (const auto* material : elem->visuals.markArray[j].materials)
					{
						add_dependency(zone, ASSET_TYPE_MATERIAL, material);
					}
				}
			}
			for_each_visual(elem, [&](const FxElemVisuals& visual)
			{
				add_visual_dependencies(zone, elem->elemType, visual);
			});

			add_effect_by_name(zone, elem->effectOnImpact.name);
			add_effect_by_name(zone, elem->effectOnDeath.name);
			add_effect_by_name(zone, elem->effectEmitted.name);
		}

		loading_effects.erase(this->name());
	}

	void fx_effect_def::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = this->write_name(buf);

		if (data->elemDefs)
		{
			buf->align(7);
			auto* elems = buf->write(data->elemDefs, elem_count(data));
			for (auto i = 0; i < elem_count(data); i++)
			{
				write_elem(zone, buf, &data->elemDefs[i], &elems[i]);
			}
			buf->clear_pointer(&dest->elemDefs);
		}

		buf->pop_stream();
	}

	void fx_effect_def::dump(FxEffectDef* asset)
	{
		assetmanager::dumper write;
		if (!write.open(dump_path(ASSET_TYPE_FX, asset->name)))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);
		write.dump_array(asset->elemDefs, elem_count(asset));
		for (auto i = 0; asset->elemDefs && i < elem_count(asset); i++)
		{
			dump_elem(write, &asset->elemDefs[i]);
		}
		write.close();
	}
}
