#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace tonemapping
		{
			struct film_values
			{
				std::array<float, 3> dark;
				std::array<float, 3> medium;
				std::array<float, 3> light;
				float desaturation;
			};

			struct lightset_entry
			{
				std::string name;
				std::string default_value;
				std::optional<std::function<std::string(ToneMapping*)>> convert = {};
			};

			struct range
			{
				float min;
				float max;
				float step;
			};

			film_values get_film_values(const std::string& vision, const ColorizationSet* set);
			zonetool::h1::StringTable* convert(ToneMapping* asset, utils::memory::allocator& allocator);
			void dump(ToneMapping* asset);
		}
	}
}