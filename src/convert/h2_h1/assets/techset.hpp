#pragma once

namespace zonetool::h2
{
	namespace converter::h1
	{
		namespace techset
		{
			void dump(zonetool::h2::ComputeShader* asset);
			void dump(zonetool::h2::MaterialVertexShader* asset);
			void dump(zonetool::h2::MaterialHullShader* asset);
			void dump(zonetool::h2::MaterialDomainShader* asset);
			void dump(zonetool::h2::MaterialPixelShader* asset);

			template <typename T>
			T* remap(T* array, utils::memory::allocator& allocator)
			{
				const auto new_array = allocator.allocate_array<T>(
					zonetool::h1::MaterialTechniqueType::TECHNIQUE_COUNT);

				std::vector<int> debug_tech_indexes = {
					zonetool::h2::TECHNIQUE_DEBUG_TEXDENSITY, 
					zonetool::h2::TECHNIQUE_INSTANCED_DEBUG_TEXDENSITY, 
					zonetool::h2::TECHNIQUE_SUBDIV_PATCH_DEBUG_TEXDENSITY, 
					zonetool::h2::TECHNIQUE_NO_DISPLACEMENT_DEBUG_TEXDENSITY};
				auto current_debug_tech = 0;
				auto h2_tecniques_index = 0;
				for (auto i = 0u; i < zonetool::h1::MaterialTechniqueType::TECHNIQUE_COUNT; i++)
				{
					if (current_debug_tech < debug_tech_indexes.size() && 
						h2_tecniques_index == debug_tech_indexes[current_debug_tech])
					{
						++current_debug_tech;
						h2_tecniques_index += 3;
					}

					new_array[i] = array[h2_tecniques_index];

					++h2_tecniques_index;
				}

				return new_array;
			}

			zonetool::h1::MaterialTechniqueSet* convert(zonetool::h2::MaterialTechniqueSet* asset, utils::memory::allocator& allocator);
			void dump(zonetool::h2::MaterialTechniqueSet* asset);
		}
	}
}