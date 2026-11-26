#pragma once
#include "../t9.hpp"

namespace zonetool::t9
{
	void read_gfx_buffer(auto& reader, GfxBuffer* buffer);

	class fx
	{
	public:
		static void read(auto& reader, FxEffectDef* asset);
	};

	class klf
	{
	public:
		static void read(auto& reader, Klf* asset);
	};

	class vector_field
	{
	public:
		static void read(auto& reader, VectorField* asset);
	};

	class fx_library_volume
	{
	public:
		static constexpr std::size_t alignment = 15;

		static void read(auto& reader, FxLibraryVolume* asset);
	};
}
