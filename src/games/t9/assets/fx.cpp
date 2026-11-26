#include <std_include.hpp>
#include "fx.hpp"

namespace zonetool::t9
{
	void read_gfx_buffer(auto& reader, GfxBuffer* buffer)
	{
		reader.push_stream(XFILE_BLOCK_PHYSICAL);
		reader.read_array(buffer->data, 255, static_cast<std::size_t>(buffer->count) * buffer->stride);
		reader.pop_stream();
	}

	namespace
	{
		void read_light_def(auto& reader, FxLightDef*& field)
		{
			if (const auto light = reader.read_array(field, 15))
			{
				reader.read_asset(ASSET_TYPE_IMAGE, light->image);
			}
		}

		void read_visual(auto& reader, FxElemVisual* visual, const FxElemType type)
		{
			switch (type)
			{
			case FX_ELEM_TYPE_MODEL:
				reader.read_asset(ASSET_TYPE_XMODEL, visual->model);
				break;
			case FX_ELEM_TYPE_RUNNER:
				reader.read_asset(ASSET_TYPE_FX, visual->effectDef);
				break;
			case FX_ELEM_TYPE_LIGHT:
				read_light_def(reader, visual->lightDef);
				break;
			case FX_ELEM_TYPE_VECTOR_FIELD:
				reader.read_asset(ASSET_TYPE_VECTORFIELD, visual->vectorField);
				break;
			case FX_ELEM_TYPE_UNKNOWN_9:
			case FX_ELEM_TYPE_SOUND:
			case FX_ELEM_TYPE_KLF:
			case FX_ELEM_TYPE_UNKNOWN_15:
			case FX_ELEM_TYPE_UNKNOWN_16:
				break;
			default:
				reader.read_asset(ASSET_TYPE_MATERIAL, visual->material);
				break;
			}
		}

		void read_visuals(auto& reader, FxElemVisuals* visuals, const FxElemDef* elem)
		{
			if (elem->elemType == FX_ELEM_TYPE_DECAL)
			{
				if (const auto decals = reader.read_array(visuals->decalArray, 7, elem->visualCount))
				{
					for (auto i = 0u; i < elem->visualCount; i++)
					{
						for (auto& material : decals[i].materials)
						{
							reader.read_asset(ASSET_TYPE_MATERIAL, material);
						}
					}
				}
			}
			else if (elem->visualCount <= 1)
			{
				read_visual(reader, &visuals->instance, elem->elemType);
			}
			else if (const auto entries = reader.read_array(visuals->array, 7, elem->visualCount))
			{
				for (auto i = 0u; i < elem->visualCount; i++)
				{
					read_visual(reader, &entries[i].visual, elem->elemType);
				}
			}
		}

		void read_extended(auto& reader, FxElemDef* elem)
		{
			switch (elem->elemType)
			{
			case FX_ELEM_TYPE_TRAIL:
				if (const auto trail = reader.read_array(elem->extended.trailDef, 7))
				{
					reader.read_array(trail->verts, 3, trail->vertCount);
					reader.read_array(trail->inds, 1, trail->indCount);
				}
				break;
			case FX_ELEM_TYPE_LIGHT:
				read_light_def(reader, elem->extended.lightDef);
				break;
			case FX_ELEM_TYPE_KLF:
				reader.read_asset(ASSET_TYPE_KLF, elem->extended.klf);
				break;
			default:
				break;
			}
		}

		void read_elem_def(auto& reader, FxElemDef* elem)
		{
			read_visuals(reader, &elem->visuals, elem);
			read_visuals(reader, &elem->secondaryVisuals, elem);
			read_extended(reader, elem);

			reader.read_array(elem->velSamples, 3, elem->velSampleCount + 1);
			reader.read_array(elem->samples2, 3, elem->sample2Count + 1);
			reader.read_array(elem->samples3, 3, elem->sample3Count + 1);
			reader.read_array(elem->visSamples, 3, elem->visSampleCount + 1);
			reader.read_array(elem->values, 3, elem->valueCount);
			reader.read_array(elem->records, 3, elem->recordCount + 1);

			for (auto& effect : elem->effects)
			{
				reader.read_asset(ASSET_TYPE_FX, effect);
			}

			reader.read_asset(ASSET_TYPE_SURFACEFXTABLE, elem->surfaceFxTable);
		}
	}

	void fx::read(auto& reader, FxEffectDef* asset)
	{
		const auto count = asset->elemDefCounts[0] + asset->elemDefCounts[1] + asset->elemDefCounts[2];
		if (const auto elems = reader.read_array(asset->elemDefs, 7, count))
		{
			for (auto i = 0; i < count; i++)
			{
				read_elem_def(reader, &elems[i]);
			}
		}
	}

	void klf::read(auto& reader, Klf* asset)
	{
		reader.read_string(asset->name);

		if (const auto entries = reader.read_array(asset->entries, 7, asset->entryCount))
		{
			for (auto i = 0u; i < asset->entryCount; i++)
			{
				reader.read_string(entries[i].name);
				if (const auto buffer = reader.read_array(entries[i].buffer, 15))
				{
					read_gfx_buffer(reader, buffer);
				}

				reader.read_asset(ASSET_TYPE_IMAGE, entries[i].image);
			}
		}

		reader.push_stream(XFILE_BLOCK_PHYSICAL);
		reader.read_array(asset->records, 15, asset->entryCount);
		reader.pop_stream();

		reader.push_stream(XFILE_BLOCK_PHYSICAL);
		reader.read_array(asset->values, 15, asset->valueCount);
		reader.pop_stream();

		if (const auto buffer = reader.read_array(asset->buffer1, 15))
		{
			read_gfx_buffer(reader, buffer);
		}

		if (const auto buffer = reader.read_array(asset->buffer2, 15))
		{
			read_gfx_buffer(reader, buffer);
		}

		reader.read_asset(ASSET_TYPE_IMAGE, asset->image);
	}

	void vector_field::read(auto& reader, VectorField* asset)
	{
		reader.read_array(asset->data, 15, asset->dataSize);
	}

	void fx_library_volume::read(auto& reader, FxLibraryVolume* asset)
	{
		reader.read_array(asset->values1, 7, asset->count1);
		reader.read_array(asset->values2, 7, asset->count2);
		reader.read_array(asset->values3, 7, asset->count3);
		reader.read_array(asset->values4, 3, asset->count4);
		reader.read_array(asset->values5, 3, asset->count5);
		reader.read_array(asset->values6, 255, asset->count6);
		reader.read_array(asset->values7, 3, asset->count7);
		reader.read_array(asset->values8, 3, asset->count8);
		reader.read_asset_array(ASSET_TYPE_MATERIAL, asset->materials, asset->materialCount);
		reader.read_asset_array(ASSET_TYPE_IMAGE, asset->images, asset->imageCount);
		reader.read_array(asset->values11, 255, asset->count11);
		reader.read_array(asset->values12, 255, asset->count12);

		for (auto i = 0; i < 4; i++)
		{
			reader.push_stream(XFILE_BLOCK_PHYSICAL);
			reader.read_array(asset->buffers[i], 255, asset->bufferSizes[i]);
			reader.pop_stream();
		}
	}
}

namespace zonetool::t9
{
	template void read_gfx_buffer(zone_reader&, GfxBuffer*);
	template void fx::read(zone_reader&, FxEffectDef*);
	template void klf::read(zone_reader&, Klf*);
	template void vector_field::read(zone_reader&, VectorField*);
	template void fx_library_volume::read(zone_reader&, FxLibraryVolume*);
	template void read_gfx_buffer(zone_writer&, GfxBuffer*);
	template void fx::read(zone_writer&, FxEffectDef*);
	template void klf::read(zone_writer&, Klf*);
	template void vector_field::read(zone_writer&, VectorField*);
	template void fx_library_volume::read(zone_writer&, FxLibraryVolume*);
}
