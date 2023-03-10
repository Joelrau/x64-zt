#pragma once
#include "converter.hpp"

namespace zonetool::s1
{
	namespace converter::h1::techset
	{
		void dump_stateinfo(const std::string& techset, zonetool::h1::Material* mat);
		void dump_statebits(const std::string& techset, unsigned char* statebits);
		void dump_statebits_map(const std::string& techset, zonetool::h1::GfxStateBits* map, unsigned char count);
		void dump_constant_buffer_indexes(const std::string& techset, unsigned char* cbi);
		void dump_constant_buffer_def_array(const std::string& techset, unsigned char count, zonetool::h1::MaterialConstantBufferDef* def);
		void dump_technique(zonetool::h1::MaterialTechnique* asset);
		void dump(MaterialTechniqueSet* asset, ZoneMemory* mem);
	}
}