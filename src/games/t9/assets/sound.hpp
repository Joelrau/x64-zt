#pragma once
#include "../t9.hpp"

namespace zonetool::t9
{
	class sound_bank
	{
	public:
		static void read(auto& reader, SndBank* asset);
	};

	class sound_asset
	{
	public:
		static void read(auto& reader, SndAsset* asset);
	};

	class sound_duck
	{
	public:
		static constexpr std::size_t alignment = 31;

		static void read(auto& reader, SndDuck* asset);
	};

	class sound_alias_modifier
	{
	public:
		static void read(auto& reader, SndAliasModifier* asset);
	};

	class sound_acoustics
	{
	public:
		static void read(auto& reader, SndAcoustics* asset);
	};

	class snd_driver_globals
	{
	public:
		static void read(auto& reader, SndDriverGlobals* asset);
	};
}
