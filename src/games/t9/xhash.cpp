#include <std_include.hpp>
#include "t9.hpp"

namespace zonetool::t9
{
	namespace
	{
		std::unordered_map<std::uint64_t, std::string> load_hash_names()
		{
			std::unordered_map<std::uint64_t, std::string> names;

			std::error_code error;
			for (const auto& entry : std::filesystem::directory_iterator("hashes/t9", error))
			{
				if (entry.path().extension() != ".txt")
				{
					continue;
				}

				std::ifstream stream(entry.path());
				std::string line;
				while (std::getline(stream, line))
				{
					if (!line.empty() && line.back() == '\r')
					{
						line.pop_back();
					}

					if (!line.empty())
					{
						names.emplace(xhash(line).value, line);
					}
				}
			}

			return names;
		}

		const std::unordered_map<std::uint64_t, std::string>& hash_names()
		{
			static const auto names = load_hash_names();
			return names;
		}
	}

	XHash xhash(const std::string_view text)
	{
		auto value = 0xCBF29CE484222325ull;
		for (const auto c : text)
		{
			value ^= static_cast<std::uint64_t>(c >= 'A' && c <= 'Z' ? c + 0x20 : c);
			value *= 0x100000001B3ull;
		}

		return {value & 0x7FFFFFFFFFFFFFFFull};
	}

	XHash xhash_from_name(const std::string_view name)
	{
		if (name.size() == 21 && name.starts_with("hash_"))
		{
			std::uint64_t value{};
			const auto digits = name.substr(5);
			if (std::from_chars(digits.data(), digits.data() + digits.size(), value, 16).ec == std::errc{})
			{
				return {value};
			}
		}

		return xhash(name);
	}

	std::string xhash_name(const XHash hash)
	{
		if (!hash.value)
		{
			return {};
		}

		const auto value = hash.value & 0x7FFFFFFFFFFFFFFFull; // bit 63 marks a default asset
		const auto& names = hash_names();
		if (const auto name = names.find(value); name != names.end())
		{
			return name->second;
		}

		return std::format("hash_{:016x}", value);
	}
}
