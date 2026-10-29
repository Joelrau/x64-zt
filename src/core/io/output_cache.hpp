#pragma once

namespace zonetool
{
	namespace cache_revision
	{
		constexpr std::uint32_t pak_streams = 1;
		constexpr std::uint32_t s2_h1_image = 1;
		constexpr std::uint32_t s2_h1_techset = 1;
		constexpr std::uint32_t s2_h1_techset_catalog = 2;
	}

	class fingerprint
	{
	public:
		fingerprint& add(std::string_view data);
		fingerprint& add_file_stamp(const std::filesystem::path& path);

		template <typename T> requires std::is_trivially_copyable_v<T>
		fingerprint& add_value(const T& value)
		{
			return this->add_bytes(&value, sizeof(T));
		}

		std::uint64_t value() const;

	private:
		fingerprint& add_bytes(const void* data, std::size_t size);

		std::uint64_t hash_{0xCBF29CE484222325};
	};

	class output_cache
	{
	public:
		explicit output_cache(std::filesystem::path manifest);

		bool is_current(const std::filesystem::path& output, std::uint64_t fingerprint) const;
		void update(const std::filesystem::path& output, std::uint64_t fingerprint);
		void save();

	private:
		struct entry
		{
			std::uint64_t fingerprint;
			std::uint64_t stamp;
		};

		std::filesystem::path manifest_;
		std::unordered_map<std::string, entry> entries_;
		bool disabled_{};
		bool changed_{};
	};

	class dump_cache_scope
	{
	public:
		explicit dump_cache_scope(const std::filesystem::path& dump_folder);
		~dump_cache_scope();

		dump_cache_scope(const dump_cache_scope&) = delete;
		dump_cache_scope& operator=(const dump_cache_scope&) = delete;

	private:
		output_cache cache_;
		output_cache* previous_;
	};

	bool is_dump_current(const std::filesystem::path& output, std::uint64_t fingerprint);
	void update_dump_cache(const std::filesystem::path& output, std::uint64_t fingerprint);
}
