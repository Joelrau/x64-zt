#pragma once

#include "../game_mode.hpp"

namespace zonetool
{
	struct zone_layout
	{
		std::uint64_t offset_tag;
		std::vector<std::uint64_t> following_markers;
		std::uint64_t insert_marker;
		std::uint8_t runtime_block;
		std::uint8_t virtual_block;
		std::optional<std::uint8_t> pop_aligned_block;
		// IW7 zones place the data of this block at its end, so offsets count from (block size - data size).
		std::optional<std::uint8_t> end_anchored_block;
	};

	class zone_reader
	{
	public:
		using inline_asset_loader = std::function<void(std::int32_t type, void** field, void** insert_slot)>;

		zone_reader(std::string_view data, std::span<const std::uint64_t> block_sizes, const zone_layout& layout);
		~zone_reader();

		zone_reader(const zone_reader&) = delete;
		zone_reader& operator=(const zone_reader&) = delete;

		game::game_mode mode = game::h1;

		void set_inline_asset_loader(inline_asset_loader loader);
		void set_script_strings(std::vector<std::uint32_t> ids);
		void set_zone_index(std::uint16_t index);
		std::uint16_t zone_index() const;
		void set_block_base(std::uint8_t block, std::size_t base);
		std::size_t end_anchored_data_start() const;
		std::size_t end_anchored_alignment() const;

		void push_stream(std::uint8_t block);
		void pop_stream();
		void align(std::size_t alignment);

		std::uint8_t current_stream() const;
		std::size_t remaining() const;

		void read_raw(void* destination, std::size_t size);
		void* read_bytes(std::size_t size);

		template <typename T>
		T* read(const std::size_t count = 1)
		{
			return static_cast<T*>(this->read_bytes(sizeof(T) * count));
		}

		bool is_inline(const void* pointer) const;
		bool is_insert(const void* pointer) const;
		bool is_offset(const void* pointer) const;
		bool contains(const void* pointer) const;
		template <typename T>
		void resolve_pointer(T*& field)
		{
			this->resolve_offset(field_address(field), offset_target::pointer);
		}

		template <typename T>
		void resolve_alias(T*& field)
		{
			this->resolve_offset(field_address(field), offset_target::alias);
		}

		void resolve_deferred_offsets();
		std::size_t count_misplaced_strings() const;
		void move_deferred_offsets(const void* source, std::size_t size, void* destination);
		void** insert_pointer();

		template <typename T>
		T* read_array(T*& field, const std::size_t alignment, const std::size_t count = 1)
		{
			if (!field)
			{
				return nullptr;
			}

			// The game reads most arrays inline when the pointer is not null, and ignores its value.
			// Third-party zones store raw heap pointers there, so only a valid offset is followed.
			if (this->is_offset(field))
			{
				this->resolve_pointer(field);
				return nullptr;
			}

			this->align(alignment);
			const auto slot = this->is_insert(field) ? this->insert_pointer() : nullptr;
			field = this->read<T>(count);

			if (slot)
			{
				*slot = const_cast<std::remove_const_t<T>*>(field);
			}

			return field;
		}

		template <typename T>
		T* read_aliased_array(T*& field, const std::size_t alignment, const std::size_t count = 1)
		{
			if (field && this->is_offset(field))
			{
				this->resolve_alias(field);
				return nullptr;
			}

			return this->read_array(field, alignment, count);
		}

		template <typename T>
		std::uint8_t* read_stream(T*& field, const std::size_t alignment, const std::size_t element_size, const std::size_t count = 1)
		{
			auto& bytes = reinterpret_cast<std::uint8_t*&>(field);
			return this->read_array(bytes, alignment, element_size * count);
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

			if (!field)
			{
				return;
			}

			if (!this->is_inline(field))
			{
				this->resolve_offset(field_address(field), offset_target::string);
				return;
			}

			const auto slot = this->is_insert(field) ? this->insert_pointer() : nullptr;
			field = reinterpret_cast<T*>(this->read_string_data());

			if (slot)
			{
				*slot = const_cast<std::remove_const_t<T>*>(field);
			}
		}

		template <typename T>
		void read_asset(const std::int32_t type, T*& field)
		{
			this->read_asset_pointer(type, field_address(field));
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

		template <typename T>
		void read_name_reference_array(T**& field, const std::size_t count)
		{
			if (const auto references = this->read_array(field, 7, count))
			{
				for (std::size_t i = 0; i < count; i++)
				{
					this->read_name_reference(references[i]);
				}
			}
		}

		template <typename T>
		void read_script_string_array(T*& field, const std::size_t count)
		{
			if (const auto strings = this->read_array(field, 3, count))
			{
				this->read_script_strings(strings, count);
			}
		}

		template <typename T>
		void read_name_reference(T*& field)
		{
			auto& name_slot = reinterpret_cast<const char**&>(field);
			if (!name_slot)
			{
				return;
			}

			if (this->read_single(name_slot, 7))
			{
				this->read_string(*name_slot);
			}

			field = this->reference_stub<T>(*name_slot);
		}

		template <typename T>
		T* reference_stub(const char* name)
		{
			auto& stub = this->reference_stubs_[{std::type_index(typeid(T)), name}];
			if (!stub)
			{
				const auto reference = this->allocate<T>();
				reference->name = name;
				stub = reference;
			}

			return static_cast<T*>(stub);
		}

		template <typename T>
		void read_script_string(T& field)
		{
			static_assert(sizeof(T) <= sizeof(std::uint32_t));
			field = static_cast<T>(this->map_script_string(static_cast<std::uint32_t>(field)));
		}

		template <typename T>
		void read_script_strings(T* fields, const std::size_t count)
		{
			for (std::size_t i = 0; i < count; i++)
			{
				this->read_script_string(fields[i]);
			}
		}

		template <typename T>
		T* allocate(const std::size_t count = 1)
		{
			return static_cast<T*>(this->allocate_bytes(sizeof(T) * count));
		}

		template <typename T>
		T* duplicate(const T* data, const std::size_t count = 1)
		{
			const auto copy = this->allocate<std::remove_const_t<T>>(count);
			std::memcpy(copy, data, sizeof(T) * count);
			return copy;
		}

	private:
		enum class offset_target
		{
			pointer,
			alias,
			string,
		};

		struct deferred_offset
		{
			void** field;
			std::size_t offset;
			offset_target target;
			std::size_t position;
		};

		struct stream_state
		{
			std::uint8_t previous_block;
			std::size_t position_at_push;
		};

		struct block
		{
			std::uint8_t* memory;
			std::size_t size;
			std::size_t position;
			std::size_t base;
			std::size_t largest_alignment;
		};

		std::string_view data_;
		std::size_t data_position_{};

		std::vector<block> blocks_;
		std::uint8_t current_block_{};
		std::vector<stream_state> stream_stack_;

		zone_layout layout_;

		std::vector<std::uint32_t> script_strings_;
		std::uint16_t zone_index_{};
		inline_asset_loader inline_asset_loader_;

		std::vector<std::unique_ptr<std::uint8_t[]>> allocations_;
		std::map<std::pair<std::type_index, std::string_view>, void*> reference_stubs_;
		std::vector<deferred_offset> deferred_offsets_;
		std::vector<std::size_t> string_positions_;

		template <typename T>
		static void** field_address(T*& field)
		{
			return reinterpret_cast<void**>(const_cast<std::remove_const_t<T>**>(&field));
		}

		block& current_block();
		std::uint8_t* offset_address(std::uint8_t block_index, std::size_t offset) const;
		void resolve_offset(void** field, offset_target target);
		char* read_string_data();
		std::uint32_t map_script_string(std::uint32_t index) const;
		void read_asset_pointer(std::int32_t type, void** field);
		void* allocate_bytes(std::size_t size);
	};
}
