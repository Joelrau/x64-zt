#include <std_include.hpp>
#include "output_cache.hpp"

#include <utils/flags.hpp>
#include <utils/io.hpp>
#include <utils/string.hpp>

namespace zonetool
{
	namespace
	{
		constexpr std::string_view manifest_header = "zonetool output cache 1\n";

		output_cache* active_dump_cache{};

		std::optional<std::uint64_t> get_file_stamp(const std::filesystem::path& path)
		{
			std::error_code error;
			const auto size = std::filesystem::file_size(path, error);
			if (error)
			{
				return {};
			}

			const auto time = std::filesystem::last_write_time(path, error);
			if (error)
			{
				return {};
			}

			return fingerprint().add_value(size).add_value(time.time_since_epoch().count()).value();
		}
	}

	fingerprint& fingerprint::add(const std::string_view data)
	{
		return this->add_value(data.size()).add_bytes(data.data(), data.size());
	}

	fingerprint& fingerprint::add_bytes(const void* data, const std::size_t size)
	{
		const auto bytes = static_cast<const std::uint8_t*>(data);
		for (auto i = 0ull; i < size; i++)
		{
			this->hash_ ^= bytes[i];
			this->hash_ *= 0x100000001B3;
		}

		return *this;
	}

	fingerprint& fingerprint::add_file_stamp(const std::filesystem::path& path)
	{
		return this->add(path.generic_string()).add_value(get_file_stamp(path).value_or(0));
	}

	std::uint64_t fingerprint::value() const
	{
		return this->hash_;
	}

	output_cache::output_cache(std::filesystem::path manifest)
		: manifest_(std::move(manifest))
		, disabled_(utils::flags::has_flag("no_cache"))
	{
		std::string data;
		if (!utils::io::read_file(this->manifest_.string(), &data) || !data.starts_with(manifest_header))
		{
			return;
		}

		std::istringstream stream(data.substr(manifest_header.size()));
		std::string line;
		while (std::getline(stream, line))
		{
			const auto fields = utils::string::split(line, '\t');
			if (fields.size() == 3)
			{
				this->entries_.insert_or_assign(fields[2], entry{std::stoull(fields[0], nullptr, 16), std::stoull(fields[1], nullptr, 16)});
			}
		}
	}

	bool output_cache::is_current(const std::filesystem::path& output, const std::uint64_t fingerprint) const
	{
		if (this->disabled_)
		{
			return false;
		}

		const auto entry = this->entries_.find(output.generic_string());
		return entry != this->entries_.end() && entry->second.fingerprint == fingerprint && get_file_stamp(output) == entry->second.stamp;
	}

	void output_cache::update(const std::filesystem::path& output, const std::uint64_t fingerprint)
	{
		if (const auto stamp = get_file_stamp(output))
		{
			this->entries_.insert_or_assign(output.generic_string(), entry{fingerprint, *stamp});
			this->changed_ = true;
		}
	}

	void output_cache::save()
	{
		if (!this->changed_)
		{
			return;
		}

		std::string data(manifest_header);
		for (const auto& [output, entry] : this->entries_)
		{
			data += std::format("{:X}\t{:X}\t{}\n", entry.fingerprint, entry.stamp, output);
		}

		utils::io::write_file(this->manifest_.string(), data);
		this->changed_ = false;
	}

	dump_cache_scope::dump_cache_scope(const std::filesystem::path& dump_folder)
		: cache_(dump_folder / ".zonetool_cache")
		, previous_(active_dump_cache)
	{
		active_dump_cache = &this->cache_;
	}

	dump_cache_scope::~dump_cache_scope()
	{
		active_dump_cache = this->previous_;
		this->cache_.save();
	}

	bool is_dump_current(const std::filesystem::path& output, const std::uint64_t fingerprint)
	{
		return active_dump_cache && active_dump_cache->is_current(output, fingerprint);
	}

	void update_dump_cache(const std::filesystem::path& output, const std::uint64_t fingerprint)
	{
		if (active_dump_cache)
		{
			active_dump_cache->update(output, fingerprint);
		}
	}
}
