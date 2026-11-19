#include <std_include.hpp>
#include "script_strings.hpp"

namespace zonetool
{
	namespace
	{
		struct string_table
		{
			std::mutex mutex;
			std::deque<std::string> strings{""};
			std::unordered_map<std::string_view, std::uint32_t> ids;
		};

		string_table& get_table()
		{
			static string_table table;
			return table;
		}
	}

	std::uint32_t SL_GetString(const char* string)
	{
		if (!string)
		{
			return 0;
		}

		auto& table = get_table();
		std::lock_guard _(table.mutex);

		if (const auto entry = table.ids.find(string); entry != table.ids.end())
		{
			return entry->second;
		}

		const auto id = static_cast<std::uint32_t>(table.strings.size());
		const auto& stored = table.strings.emplace_back(string);
		table.ids.emplace(stored, id);
		return id;
	}

	const char* SL_ConvertToString(const std::uint32_t id)
	{
		if (!id)
		{
			return nullptr;
		}

		auto& table = get_table();
		std::lock_guard _(table.mutex);

		if (id >= table.strings.size())
		{
			return nullptr;
		}

		return table.strings[id].data();
	}
}
