#include <std_include.hpp>

#include "imagefile.hpp"

#include <utils/flags.hpp>

namespace zonetool::imagefile
{
	namespace
	{
		struct reusable_streams_header
		{
			std::uint64_t inputs;
			std::uint64_t pak;
		};

		std::string get_reusable_streams_path(const std::string& fastfile)
		{
			return "zonetool_cache\\" + fastfile + ".pak.streams";
		}
	}

	std::optional<std::vector<XStreamFile>> find_reusable_streams(const std::string& fastfile, const std::string& pak_path, const std::uint64_t inputs)
	{
		std::string data;
		if (utils::flags::has_flag("no_cache") || !utils::io::read_file(get_reusable_streams_path(fastfile), &data)
			|| data.size() < sizeof(reusable_streams_header) || (data.size() - sizeof(reusable_streams_header)) % sizeof(XStreamFile))
		{
			return {};
		}

		const auto header = reinterpret_cast<const reusable_streams_header*>(data.data());
		if (header->inputs != inputs || header->pak != fingerprint().add_file_stamp(pak_path).value())
		{
			return {};
		}

		std::vector<XStreamFile> streams((data.size() - sizeof(reusable_streams_header)) / sizeof(XStreamFile));
		std::memcpy(streams.data(), data.data() + sizeof(reusable_streams_header), streams.size() * sizeof(XStreamFile));
		return streams;
	}

	void save_reusable_streams(const std::string& fastfile, const std::string& pak_path, const std::uint64_t inputs, const std::vector<XStreamFile>& streams)
	{
		const reusable_streams_header header{inputs, fingerprint().add_file_stamp(pak_path).value()};
		std::string data(reinterpret_cast<const char*>(&header), sizeof(header));
		data.append(reinterpret_cast<const char*>(streams.data()), streams.size() * sizeof(XStreamFile));
		utils::io::write_file(get_reusable_streams_path(fastfile), data);
	}
}
