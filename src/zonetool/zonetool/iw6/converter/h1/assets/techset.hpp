#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace techset
		{
			namespace ree
			{
				void dump_stateinfo(const std::string& techset, const std::string& material, zonetool::h1::Material* mat);
				void dump_statebits(const std::string& techset, const std::string& material, unsigned char* statebits);
				void dump_statebits_map(const std::string& techset, const std::string& material, GfxStateBits* map, unsigned char count);
				void dump_constant_buffer_indexes(const std::string& techset, const std::string& material, unsigned char* cbi);
				void dump_constant_buffer_def_array(const std::string& techset, const std::string& material, 
					unsigned char count, zonetool::h1::MaterialConstantBufferDef* def);
				void dump_technique(zonetool::h1::MaterialTechnique* asset);
				void dump(MaterialTechniqueSet* asset, zone_memory* mem);
			}

			extern std::unordered_map <std::int32_t, std::int32_t> technique_index_map;
			extern std::unordered_map<std::string, zonetool::h1::MaterialTechniqueSet*> converted_techset_assets;

			zonetool::h1::MaterialTechniqueSet* convert(MaterialTechniqueSet* asset, zone_memory* mem);
			void dump(MaterialTechniqueSet* asset, zone_memory* mem);
		}
	}
}