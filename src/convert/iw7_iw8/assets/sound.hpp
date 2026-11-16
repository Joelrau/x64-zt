#pragma once

namespace zonetool::iw7::converter::iw8::sound
{
	class asset_bank_builder
	{
	public:
		const zonetool::iw8::SndAssetBankEntry* add(SndBank* bank, SndStringHash asset_id);
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

		source_bank* open(SndBank* bank);

		std::unordered_map<std::string, std::unique_ptr<source_bank>> sources_;
		std::vector<zonetool::iw8::SndAssetBankEntry> entries_;
		std::vector<std::array<char, 16>> checksums_;
		std::vector<std::array<char, 16>> source_checksums_;
		std::vector<std::array<char, 128>> names_;
		std::string data_;
	};

	void dump(WeaponCompleteDef* asset);
}
