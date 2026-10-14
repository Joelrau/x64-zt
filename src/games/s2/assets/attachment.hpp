#pragma once

namespace zonetool::s2
{
	class weapon_attachment
	{
	public:
		static bool is_string_field(const AttachmentField& field)
		{
			return static_cast<std::uint8_t>(field.type - 4) > 5;
		}

		static void read(zone_reader& reader, WeaponAttachment* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->szInternalName);
			reader.read_string(asset->szDisplayName);
			reader.read_asset_array(ASSET_TYPE_XMODEL, asset->worldModels, 2);
			reader.read_asset_array(ASSET_TYPE_XMODEL, asset->viewModels, 12);
			reader.read_asset_array(ASSET_TYPE_XMODEL, asset->worldModelsAdditional, 2);
			reader.read_asset_array(ASSET_TYPE_XMODEL, asset->viewModelsAdditional, 12);
			reader.read_asset_array(ASSET_TYPE_XMODEL, asset->reticleViewModels, 255);
			reader.read_script_string(asset->knifeAttachTagOverride);
			reader.read_array(asset->unkStruct80, 3);
			reader.read_array(asset->fieldOffsets, 1, asset->fieldCount);

			if (const auto fields = reader.read_array(asset->fields, 7, asset->fieldCount))
			{
				for (auto i = 0; i < asset->fieldCount; i++)
				{
					if (is_string_field(fields[i]))
					{
						reader.read_string(fields[i].value.string);
					}
				}
			}
			reader.pop_stream();
		}
	};
}
