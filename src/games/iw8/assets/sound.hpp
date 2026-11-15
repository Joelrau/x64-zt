#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	void read_alias_list(zone_reader& reader, SndAliasList& list);

	class sound_globals
	{
	public:
		static void read(zone_reader& reader, SndGlobals* asset);
	};

	class sound_bank
	{
	public:
		static void read(zone_reader& reader, SndBankResident* asset);
	};

	class sound_bank_transient
	{
	public:
		static void read(zone_reader& reader, SndBankTransient* asset);
	};

	class sfx_package
	{
	public:
		static void read(zone_reader& reader, WeaponSFXPackage* asset);
	};
}
