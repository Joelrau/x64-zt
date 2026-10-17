#pragma once

namespace zonetool::s2
{
	struct attachment_layout
	{
		static constexpr std::size_t world_model_count = 2;
		static constexpr std::size_t view_model_count = 12;
		static constexpr std::size_t reticle_view_model_count = 255;

		static bool is_string_field(const AttachmentField& field)
		{
			return static_cast<std::uint8_t>(field.type - 4) > 5;
		}

		template <typename V>
		static void visit(const V& w)
		{
			w.strings({offsetof(WeaponAttachment, szInternalName), offsetof(WeaponAttachment, szDisplayName)});
			w.asset_array(ASSET_TYPE_XMODEL, offsetof(WeaponAttachment, worldModels), world_model_count);
			w.asset_array(ASSET_TYPE_XMODEL, offsetof(WeaponAttachment, viewModels), view_model_count);
			w.asset_array(ASSET_TYPE_XMODEL, offsetof(WeaponAttachment, worldModelsAdditional), world_model_count);
			w.asset_array(ASSET_TYPE_XMODEL, offsetof(WeaponAttachment, viewModelsAdditional), view_model_count);
			w.asset_array(ASSET_TYPE_XMODEL, offsetof(WeaponAttachment, reticleViewModels), reticle_view_model_count);
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
	public:
		WeaponAttachment* parse(const std::string& name, zone_memory* mem) override;
		static void dump(WeaponAttachment* asset);
	};
}
