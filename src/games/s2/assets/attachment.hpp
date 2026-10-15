#pragma once

namespace zonetool::s2
{
	struct attachment_layout
	{
		static bool is_string_field(const AttachmentField& field)
		{
			return static_cast<std::uint8_t>(field.type - 4) > 5;
		}

		template <typename V>
		static void visit(const V& w)
		{
			w.strings({offsetof(WeaponAttachment, szInternalName), offsetof(WeaponAttachment, szDisplayName)});
			w.asset_array(ASSET_TYPE_XMODEL, offsetof(WeaponAttachment, worldModels), 2);
			w.asset_array(ASSET_TYPE_XMODEL, offsetof(WeaponAttachment, viewModels), 12);
			w.asset_array(ASSET_TYPE_XMODEL, offsetof(WeaponAttachment, worldModelsAdditional), 2);
			w.asset_array(ASSET_TYPE_XMODEL, offsetof(WeaponAttachment, viewModelsAdditional), 12);
			w.asset_array(ASSET_TYPE_XMODEL, offsetof(WeaponAttachment, reticleViewModels), 255);
			w.script_string(offsetof(WeaponAttachment, knifeAttachTagOverride));
			w.bytes(offsetof(WeaponAttachment, unkStruct80), 3, sizeof(*WeaponAttachment{}.unkStruct80));

			const auto field_count = w.template at<int>(offsetof(WeaponAttachment, fieldCount));
			w.bytes(offsetof(WeaponAttachment, fieldOffsets), 1, sizeof(unsigned short) * field_count);
			w.entries(offsetof(WeaponAttachment, fields), sizeof(AttachmentField), field_count, [](const auto& field, std::size_t)
			{
				if (is_string_field(field.template at<AttachmentField>(0)))
				{
					field.string(offsetof(AttachmentField, value));
				}
			});
		}
	};

	class weapon_attachment : public field_layout_asset<WeaponAttachment, ASSET_TYPE_ATTACHMENT, attachment_layout>
	{
	};
}
