#pragma once

namespace zonetool::iw7::converter::iw8::sound
{
	class asset_bank_builder
	{
	public:
		const zonetool::iw8::SndAssetBankEntry* add(SndBank* bank, SndStringHash asset_id);
		const zonetool::iw8::SndAssetBankEntry* add(const std::string& path, SndStringHash asset_id);
		std::string build(const std::string& zone_name) const;

	private:
		struct source_bank
		{
			std::ifstream file;
			std::vector<SndAssetBankEntry> entries;
			std::vector<std::array<char, 16>> checksums;
			std::vector<std::array<char, 16>> source_checksums;
			std::vector<std::array<char, 128>> names;
		};

		source_bank* open(const std::string& path);

		std::unordered_map<std::string, std::unique_ptr<source_bank>> sources_;
		std::vector<zonetool::iw8::SndAssetBankEntry> entries_;
		std::vector<std::array<char, 16>> checksums_;
		std::vector<std::array<char, 16>> source_checksums_;
		std::vector<std::array<char, 128>> names_;
		std::string data_;
	};

	int convert_volmod(int index);
	unsigned int convert_channel(unsigned int index);
	bool is_looping(const std::string& alias_name);
	zonetool::iw8::SndAlias convert_alias(const SndAlias& alias, const zonetool::iw8::SndAssetBankEntry* entry, const SndGlobals* globals);
	zonetool::iw8::SndIndexEntry* build_alias_index(const zonetool::iw8::SndAliasList* lists, unsigned short count, utils::memory::allocator& allocator);

	void dump(WeaponCompleteDef* asset);
}
