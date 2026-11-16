#pragma once
#include "../include.hpp"

namespace zonetool::iw8::converter::iw7::sound
{
	using name_table = std::unordered_map<std::uint32_t, std::string>;

	struct alias_source
	{
		SndBank* bank;
		SndAliasList* list;
	};

	struct sab_source
	{
		std::function<bool(std::uint64_t, std::size_t, void*)> read;
		std::vector<SndAssetBankEntry> entries;
	};

	struct bank_audio
	{
		std::string loaded_data;
		std::optional<sab_source> loaded;
		std::optional<sab_source> streamed;
		bool opened;
	};

	struct sound_names
	{
		name_table vf_curves;
		name_table lpf_curves;
		name_table hpf_curves;
		name_table rvb_curves;
		name_table speaker_maps;
		name_table occlusion_shapes;
		name_table doppler_presets;
		std::unordered_map<std::uint32_t, const SndDuck*> ducks;
	};

	struct converter_state
	{
		std::unordered_map<std::string, alias_source> aliases;
		std::unordered_map<SndBank*, bank_audio> audio;
		sound_names names;
		const SndGlobals* globals;
	};

	void set_iw7_folder(const std::filesystem::path& folder);
	void dump(WeaponCompleteDef* asset, const std::vector<std::string>& notetrack_aliases);
}
