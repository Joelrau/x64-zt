#pragma once

namespace zonetool::h1
{
	namespace converter::h2
	{
		namespace techset
		{
			void dump(zonetool::h1::ComputeShader* asset);
			void dump(zonetool::h1::MaterialVertexShader* asset);
			void dump(zonetool::h1::MaterialHullShader* asset);
			void dump(zonetool::h1::MaterialDomainShader* asset);
			void dump(zonetool::h1::MaterialPixelShader* asset);

			template <typename T>
			T* remap(T* array, utils::memory::allocator& allocator)
			{
				const auto new_array = allocator.allocate_array<T>(
					zonetool::h2::MaterialTechniqueType::TECHNIQUE_COUNT);

				std::vector<int> debug_tech_indexes = {59, 119, 179, 239};
				auto current_debug_tech = 0;
				auto h1_tecniques_index = 0;

				for (auto i = 0u; i < zonetool::h2::MaterialTechniqueType::TECHNIQUE_COUNT; i++)
				{
					new_array[i] = array[h1_tecniques_index];
					if (current_debug_tech < debug_tech_indexes.size() && 
						h1_tecniques_index == debug_tech_indexes[current_debug_tech])
					{
						new_array[i + 1] = new_array[i];
						new_array[i + 2] = new_array[i];
						new_array[i + 3] = new_array[i];
						++current_debug_tech;
						i += 3;
					}

					++h1_tecniques_index;
				}

				return new_array;
			}

			zonetool::h2::MaterialTechniqueSet* convert(zonetool::h1::MaterialTechniqueSet* asset, utils::memory::allocator& allocator);
			void dump(zonetool::h1::MaterialTechniqueSet* asset);
		}
	}
}