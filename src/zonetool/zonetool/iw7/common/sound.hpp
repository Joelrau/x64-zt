#pragma once

namespace zonetool::iw7
{
	class csv_writer
	{
	private:
		std::string csv_buffer;
		unsigned int line = 0;
		unsigned int column = 0;

	public:
		std::string get_buffer()
		{
			return csv_buffer;
		}

		void write_column(const std::string& value)
		{
			if (column > 0)
			{
				csv_buffer.append(",");
			}

			csv_buffer.append(value);
			column++;
		}

		void write_column(const char* value)
		{
			write_column(std::string(value));
		}

		template <typename T>
		void write_column(const T& value)
		{
			return write_column(std::to_string(value));
		}

		void increase_line()
		{
			csv_buffer.append("\n");
			line++;
			column = 0;
		}

		void save(const std::string& path)
		{
			auto file = filesystem::file(path);
			file.open("wb");
			file.write(this->csv_buffer.data(), this->csv_buffer.size());
			file.close();
		}
	};

	template<typename T> T get_value_index(const std::string& value, const std::string* lookup_table, size_t len, T default_value = 0)
	{
		if (value.empty())
			return default_value;

		for (T i = 0; i < len; i++)
		{
			if (lookup_table[i] == value)
				return i;
		}

		return default_value;
	}

	template<typename T> T get_value(const std::string& value)
	{
		if (!value.empty())
		{
			std::istringstream ss_(value);
			T out{};
			ss_ >> out;
			return out;
		}

		return {};
	}

	template<typename T> T get_json_value(const ordered_json& value)
	{
		if (!value.is_null())
		{
			return value.get<T>();
		}

		return {};
	}

	namespace
	{
		const std::string sound_path = "sound/";
		const std::string sound_path_globals = "sound/globals/";
		const std::string sound_path_assets = "sound_assets/";

		std::unordered_map<std::uint32_t, std::string> create_snd_hash_map()
		{
			std::unordered_map<std::uint32_t, std::string> result;

			// hash all global strings

			for (auto i = 0u; i < std::extent_v<decltype(VOLMODS)>; i++)
				result.emplace(snd_hash_name(VOLMODS[i].data()), VOLMODS[i]);

			for (auto i = 0u; i < std::extent_v<decltype(ENTCHANNELS)>; i++)
				result.emplace(snd_hash_name(ENTCHANNELS[i].data()), ENTCHANNELS[i]);

			for (auto i = 0u; i < std::extent_v<decltype(doppler_presets_s)>; i++)
				result.emplace(snd_hash_name(doppler_presets_s[i].data()), doppler_presets_s[i]);

			for (auto i = 0u; i < std::extent_v<decltype(occlusion_shapes_s)>; i++)
				result.emplace(snd_hash_name(occlusion_shapes_s[i].data()), occlusion_shapes_s[i]);

			for (auto i = 0u; i < std::extent_v<decltype(vf_curves_s)>; i++)
				result.emplace(snd_hash_name(vf_curves_s[i].data()), vf_curves_s[i]);

			for (auto i = 0u; i < std::extent_v<decltype(lpf_curves_s)>; i++)
				result.emplace(snd_hash_name(lpf_curves_s[i].data()), lpf_curves_s[i]);

			for (auto i = 0u; i < std::extent_v<decltype(hpf_curves_s)>; i++)
				result.emplace(snd_hash_name(hpf_curves_s[i].data()), hpf_curves_s[i]);

			for (auto i = 0u; i < std::extent_v<decltype(rvb_curves_s)>; i++)
				result.emplace(snd_hash_name(rvb_curves_s[i].data()), rvb_curves_s[i]);

			for (auto i = 0u; i < std::extent_v<decltype(speaker_maps_s)>; i++)
				result.emplace(snd_hash_name(speaker_maps_s[i].data()), speaker_maps_s[i]);

			for (auto i = 0u; i < std::extent_v<decltype(contexts_s)>; i++)
				result.emplace(snd_hash_name(contexts_s[i].data()), contexts_s[i]);

			for (auto i = 0u; i < std::extent_v<decltype(context_values_s)>; i++)
				result.emplace(snd_hash_name(context_values_s[i].data()), context_values_s[i]);

			for (auto i = 0u; i < std::extent_v<decltype(masters_s)>; i++)
				result.emplace(snd_hash_name(masters_s[i].data()), masters_s[i]);

			return result;
		}

		// hash lookup
		const std::unordered_map<std::uint32_t, std::string> snd_hash_map = create_snd_hash_map();

		std::string get_hashed_string(std::uint32_t hash)
		{
			if (!hash) return "";
			return snd_hash_map.contains(hash) ? snd_hash_map.at(hash) : "";
		}

		std::string duck_hash_lookup(SndBank* asset, std::uint32_t hash)
		{
			for (auto i = 0u; i < asset->duckCount; i++)
			{
				if (asset->ducks[i].id == hash)
				{
					return asset->ducks[i].name;
				}
			}
			return get_hashed_string(hash);
		}

		std::string FALSE_TRUE[] =
		{
			"False",
			"True",
		};
	}
}