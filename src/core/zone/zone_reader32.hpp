#pragma once

#include "x86_address_space.hpp"

namespace zonetool
{
	struct zone_layout32
	{
		std::uint8_t temp_block;
		std::uint8_t virtual_block;
		std::vector<std::uint8_t> runtime_blocks;
		std::size_t asset_alignment = 3;
	};

	class zone_reader32
	{
	public:
		using inline_asset_loader = std::function<void(std::int32_t type, x86::pointer<void>& field, std::uint32_t* insert_slot)>;

		zone_reader32(std::string_view data, std::span<const std::uint64_t> block_sizes, const zone_layout32& layout);

		zone_reader32(const zone_reader32&) = delete;
		zone_reader32& operator=(const zone_reader32&) = delete;

		void set_inline_asset_loader(inline_asset_loader loader);
		void set_script_strings(std::vector<std::uint32_t> ids);

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

		static bool is_inline(const std::uint32_t value);
		static bool is_insert(const std::uint32_t value);
		bool contains(const void* pointer) const;
		std::uint32_t* insert_pointer();

		template <typename T>
		T* read_array(x86::pointer<T>& field, const std::size_t alignment, const std::size_t count = 1)
		{
			if (!field.value)
			{
				return nullptr;
			}

			if (!is_inline(field.value))
			{
				this->resolve_offset(field.value, false);
				return nullptr;
			}

			this->align(alignment);
			const auto slot = is_insert(field.value) ? this->insert_pointer() : nullptr;
			field = this->read<T>(count);

			if (slot)
			{
				*slot = field.value;
			}

			return field;
		}

		template <typename T>
		T* read_aliased_array(x86::pointer<T>& field, const std::size_t alignment, const std::size_t count = 1)
		{
			if (field.value && !is_inline(field.value))
			{
				this->resolve_offset(field.value, true);
				return nullptr;
			}

			return this->read_array(field, alignment, count);
		}

		template <typename T, typename F>
		T* read_dynamic(x86::pointer<T>& field, const std::size_t alignment, const std::size_t header_size,
			const F& total_size, const bool alias = false)
		{
			if (!field.value)
			{
				return nullptr;
			}

			if (!is_inline(field.value))
			{
				this->resolve_offset(field.value, alias);
				return nullptr;
			}

			this->align(alignment);
			const auto slot = is_insert(field.value) ? this->insert_pointer() : nullptr;
			const auto data = static_cast<std::uint8_t*>(this->read_bytes(header_size));
			const auto total = static_cast<std::size_t>(total_size(reinterpret_cast<const T*>(data)));
			if (total > header_size)
			{
				this->read_bytes(total - header_size);
			}

			field = reinterpret_cast<T*>(data);
			if (slot)
			{
				*slot = field.value;
			}

			return field;
		}

		template <typename T>
		T* read_single(x86::pointer<T>& field, const std::size_t alignment)
		{
			return this->read_array(field, alignment, 1);
		}

		template <typename T>
		std::uint8_t* read_stream(x86::pointer<T>& field, const std::size_t alignment, const std::size_t element_size,
			const std::size_t count = 1)
		{
			auto& bytes = reinterpret_cast<x86::pointer<std::uint8_t>&>(field);
			return this->read_array(bytes, alignment, element_size * count);
		}

		template <typename T>
		void read_string(x86::pointer<T>& field)
		{
			static_assert(sizeof(T) == 1);

			if (!field.value)
			{
				return;
			}

			if (!is_inline(field.value))
			{
				this->resolve_offset(field.value, false);
				return;
			}

			const auto slot = is_insert(field.value) ? this->insert_pointer() : nullptr;
			field = reinterpret_cast<T*>(this->read_string_data());

			if (slot)
			{
				*slot = field.value;
			}
		}

		template <typename T>
		void read_asset(const std::int32_t type, x86::pointer<T>& field, const bool in_temp_block = true)
		{
			this->read_asset_pointer(type, reinterpret_cast<x86::pointer<void>&>(field), in_temp_block);
		}

		template <typename T>
		void read_strings(x86::pointer<x86::pointer<T>>& field, const std::size_t count)
		{
			if (const auto strings = this->read_array(field, 3, count))
			{
				for (std::size_t i = 0; i < count; i++)
				{
					this->read_string(strings[i]);
				}
			}
		}

		template <typename T>
		void read_asset_array(const std::int32_t type, x86::pointer<x86::pointer<T>>& field, const std::size_t count)
		{
			if (const auto assets = this->read_array(field, 3, count))
			{
				for (std::size_t i = 0; i < count; i++)
				{
					this->read_asset(type, assets[i]);
				}
			}
		}

		template <typename T>
		void read_script_string(T& field)
		{
			static_assert(sizeof(T) <= sizeof(std::uint32_t));
			const auto id = this->map_script_string(static_cast<std::uint32_t>(field));
			if (id > std::numeric_limits<T>::max())
			{
				throw std::runtime_error(std::format("script string id {} does not fit the field", id));
			}

			field = static_cast<T>(id);
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
			return static_cast<T*>(x86::address_space::allocate(sizeof(T) * count, alignof(T)));
		}

		template <typename T>
		T* duplicate(const T* data, const std::size_t count = 1)
		{
			const auto copy = this->allocate<std::remove_const_t<T>>(count);
			std::memcpy(copy, data, sizeof(T) * count);
			return copy;
		}

	private:
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
		};

		std::string_view data_;
		std::size_t data_position_{};

		std::vector<block> blocks_;
		std::uint8_t current_block_{};
		std::vector<stream_state> stream_stack_;

		zone_layout32 layout_;

		std::vector<std::uint32_t> script_strings_;
		inline_asset_loader inline_asset_loader_;

		block& current_block();
		bool is_runtime(std::uint8_t block_index) const;
		void resolve_offset(std::uint32_t& value, bool alias);
		char* read_string_data();
		std::uint32_t map_script_string(std::uint32_t index) const;
		void read_asset_pointer(std::int32_t type, x86::pointer<void>& field, bool in_temp_block);
	};
}
