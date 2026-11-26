#pragma once

#include "zone_reader.hpp"

namespace zonetool
{
	class zone_writer
	{
	public:
		using inline_asset_writer = std::function<void(std::int32_t type, const void* header)>;
		using inserted_check = std::function<bool(const void* data)>;
		using asset_resolver = std::function<const void*(std::int32_t type, const void* header)>;
		using data_identity = std::function<std::optional<std::uint64_t>(const void* data)>;
		using external_pointer = std::function<bool(const void* target, std::uint8_t block, std::size_t offset)>;

		struct written_chunk
		{
			const void* source;
			std::size_t size;
			std::uint8_t block;
			std::size_t offset;
		};

		zone_writer(const zone_layout& layout, std::size_t block_count, std::size_t capacity);

		zone_writer(const zone_writer&) = delete;
		zone_writer& operator=(const zone_writer&) = delete;

		game::game_mode mode = game::h1;

		void set_inline_asset_writer(inline_asset_writer writer);
		void set_inserted_check(inserted_check check);
		void set_asset_resolver(asset_resolver resolver);
		void set_data_identity(data_identity identity);
		void set_external_pointer(external_pointer handler);

		void push_stream(std::uint8_t block);
		void pop_stream();
		void align(std::size_t alignment);

		void push_asset()
		{
		}

		void pop_asset()
		{
		}

		std::uint8_t current_stream() const;

		void* write_bytes(const void* source, std::size_t size);
		void* read_bytes(std::size_t size);

		template <typename T>
		T* read(const std::size_t count = 1)
		{
			return static_cast<T*>(this->read_bytes(sizeof(T) * count));
		}

		template <typename T>
		void resolve_pointer(T*& field)
		{
			this->write_offset(field_address(field));
		}

		template <typename T>
		T* read_array(T*& field, const std::size_t alignment, const std::size_t count = 1)
		{
			return static_cast<T*>(this->write_array(field_address(field), alignment, sizeof(T) * count));
		}

		template <typename T>
		T* read_aliased_array(T*& field, const std::size_t alignment, const std::size_t count = 1)
		{
			if (field && this->write_alias(field, field_address(field)))
			{
				return nullptr;
			}

			return this->read_array(field, alignment, count);
		}

		template <typename T>
		std::uint8_t* read_stream(T*& field, const std::size_t alignment, const std::size_t element_size, const std::size_t count = 1)
		{
			return static_cast<std::uint8_t*>(this->write_array(field_address(field), alignment, element_size * count));
		}

		template <typename T>
		T* read_single(T*& field, const std::size_t alignment)
		{
			return this->read_array(field, alignment, 1);
		}

		template <typename T>
		void read_string(T*& field)
		{
			static_assert(sizeof(T) == 1);
			this->write_string(field_address(field));
		}

		template <typename T>
		void read_asset(const std::int32_t type, T*& field)
		{
			this->write_asset(type, field_address(field));
		}

		template <typename T>
		void read_asset_array(const std::int32_t type, T**& field, const std::size_t count)
		{
			if (const auto assets = this->read_array(field, 7, count))
			{
				for (std::size_t i = 0; i < count; i++)
				{
					this->read_asset(type, assets[i]);
				}
			}
		}

		void finish();

		std::string_view data() const;
		std::vector<std::uint64_t> block_sizes() const;
		std::vector<written_chunk> written_chunks() const;

	private:
		struct location
		{
			std::uint8_t block;
			std::size_t offset;
		};

		struct written_range
		{
			std::uint64_t end;
			location start;
		};

		struct output_chunk
		{
			std::size_t size;
			location start;
		};

		struct stream_state
		{
			std::uint8_t previous_block;
			std::size_t position_at_push;
		};

		struct fixup
		{
			void** field;
			std::uint64_t value;
		};

		zone_layout layout_;
		std::unique_ptr<std::uint8_t[]> output_;
		std::size_t output_size_{};
		std::size_t output_capacity_{};

		std::vector<std::size_t> positions_;
		std::vector<std::size_t> peaks_;
		std::uint8_t current_block_{};
		std::vector<stream_state> stream_stack_;

		const std::uint8_t* last_source_{};
		std::map<std::uint64_t, written_range> written_;
		std::map<std::uint64_t, written_range> written_identities_;
		std::map<std::size_t, output_chunk> chunks_;
		std::unordered_map<const void*, location> slots_;
		std::unordered_map<std::uint64_t, location> slot_identities_;
		std::vector<fixup> fixups_;

		inline_asset_writer inline_asset_writer_;
		inserted_check inserted_check_;
		asset_resolver asset_resolver_;
		data_identity data_identity_;
		external_pointer external_pointer_;

		template <typename T>
		static void** field_address(T*& field)
		{
			return reinterpret_cast<void**>(const_cast<std::remove_const_t<T>**>(&field));
		}

		bool writes_file_bytes() const;
		bool is_reserved_block() const;
		bool is_inserted(const void* data) const;
		std::uint64_t following_marker() const;
		std::uint64_t encode(const location& location) const;
		static std::optional<location> find_range(const std::map<std::uint64_t, written_range>& ranges, std::uint64_t key);
		std::optional<location> find_written(const void* source) const;
		std::optional<location> find_slot(const void* data) const;
		void add_slot(const void* data, const location& slot);
		std::optional<location> find_output_location(const void* address) const;
		location insert_slot();
		void add_fixup(void** field, std::uint64_t value);
		void remember_slot(const void* data, void** field);

		void* write_array(void** field, std::size_t alignment, std::size_t size);
		void write_offset(void** field);
		bool write_alias(const void* data, void** field);
		void write_string(void** field);
		void write_asset(std::int32_t type, void** field);
	};
}
