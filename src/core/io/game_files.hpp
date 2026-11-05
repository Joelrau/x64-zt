#pragma once

namespace zonetool::game_files
{
	class source
	{
	public:
		virtual ~source() = default;

		virtual std::string description() const = 0;
		virtual std::vector<std::string> list(std::string_view extension) const = 0;
		virtual bool exists(const std::string& name) const = 0;
		virtual std::filesystem::path path(const std::string& name) const = 0;
		virtual bool read(const std::string& name, std::string& data) const = 0;
		virtual bool read(const std::string& name, std::uint64_t offset, std::size_t size, void* buffer) const = 0;
	};

	using opener = std::unique_ptr<source>(*)(const std::filesystem::path& game_path);

	std::unique_ptr<source> open_folder(const std::filesystem::path& folder);
	std::unique_ptr<source> open_casc(const std::filesystem::path& game_path);

	void set(std::unique_ptr<source> source);
	const source& get();

	std::vector<std::filesystem::path> list_zones();
	std::optional<std::filesystem::path> find_zone(std::string_view name);
	bool read(const std::filesystem::path& path, std::string& data);
	bool read(const std::filesystem::path& path, std::uint64_t offset, std::size_t size, void* buffer);
}
