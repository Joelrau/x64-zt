#include <std_include.hpp>
#include "zone_reader32.hpp"

namespace zonetool
{
	namespace
	{
		constexpr std::uint32_t following_marker = 0xFFFFFFFF;
		constexpr std::uint32_t insert_marker = 0xFFFFFFFE;
		constexpr std::uint32_t offset_bits = 28;
	}

	zone_reader32::zone_reader32(const std::string_view data, const std::span<const std::uint64_t> block_sizes,
		const zone_layout32& layout)
		: data_(data)
		, layout_(layout)
	{
		this->blocks_.reserve(block_sizes.size());
		for (const auto size : block_sizes)
		{
			const auto memory = size ? static_cast<std::uint8_t*>(x86::address_space::allocate(size, 16)) : nullptr;
			this->blocks_.emplace_back(memory, size, 0);
		}
	}

	void zone_reader32::set_inline_asset_loader(inline_asset_loader loader)
	{
		this->inline_asset_loader_ = std::move(loader);
	}

	void zone_reader32::set_script_strings(std::vector<std::uint32_t> ids)
	{
		this->script_strings_ = std::move(ids);
	}

	void zone_reader32::push_stream(const std::uint8_t block)
	{
		if (block >= this->blocks_.size())
		{
			throw std::runtime_error(std::format("block {} does not exist", block));
		}

		this->stream_stack_.emplace_back(this->current_block_, this->blocks_[block].position);
		this->current_block_ = block;
	}

	void zone_reader32::pop_stream()
	{
		if (this->stream_stack_.empty())
		{
			throw std::runtime_error("stream stack is empty");
		}

		const auto state = this->stream_stack_.back();
		this->stream_stack_.pop_back();

		if (this->current_block_ == this->layout_.temp_block)
		{
			this->blocks_[this->layout_.temp_block].position = state.position_at_push;
		}

		this->current_block_ = state.previous_block;
	}

	void zone_reader32::align(const std::size_t alignment)
	{
		auto& block = this->current_block();
		block.position = (block.position + alignment) & ~alignment;
	}

	std::uint8_t zone_reader32::current_stream() const
	{
		return this->current_block_;
	}

	std::size_t zone_reader32::remaining() const
	{
		return this->data_.size() - this->data_position_;
	}

	void zone_reader32::read_raw(void* destination, const std::size_t size)
	{
		if (this->remaining() < size)
		{
			throw std::runtime_error(std::format("read of 0x{:X} bytes passes the end of the zone", size));
		}

		std::memcpy(destination, this->data_.data() + this->data_position_, size);
		this->data_position_ += size;
	}

	void* zone_reader32::read_bytes(const std::size_t size)
	{
		auto& block = this->current_block();
		const auto runtime = this->is_runtime(this->current_block_);
		if (block.position + size > block.size)
		{
			if (runtime)
			{
				return x86::address_space::allocate(size, 4);
			}

			throw std::runtime_error(std::format("block {} overflows at 0x{:X} + 0x{:X} (size 0x{:X})",
				this->current_block_, block.position, size, block.size));
		}

		const auto destination = block.memory + block.position;
		if (!runtime)
		{
			this->read_raw(destination, size);
		}

		block.position += size;
		return destination;
	}

	bool zone_reader32::is_inline(const std::uint32_t value)
	{
		return value == following_marker || value == insert_marker;
	}

	bool zone_reader32::is_insert(const std::uint32_t value)
	{
		return value == insert_marker;
	}

	bool zone_reader32::contains(const void* pointer) const
	{
		const auto address = static_cast<const std::uint8_t*>(pointer);
		return std::ranges::any_of(this->blocks_, [&](const block& block)
		{
			return address >= block.memory && address < block.memory + block.size;
		});
	}

	std::uint32_t* zone_reader32::insert_pointer()
	{
		this->push_stream(this->layout_.virtual_block);
		this->align(3);

		auto& block = this->current_block();
		if (block.position + sizeof(std::uint32_t) > block.size)
		{
			throw std::runtime_error("insert pointer overflows the virtual block");
		}

		const auto slot = reinterpret_cast<std::uint32_t*>(block.memory + block.position);
		block.position += sizeof(std::uint32_t);

		this->pop_stream();
		return slot;
	}

	zone_reader32::block& zone_reader32::current_block()
	{
		return this->blocks_[this->current_block_];
	}

	bool zone_reader32::is_runtime(const std::uint8_t block_index) const
	{
		return std::ranges::contains(this->layout_.runtime_blocks, block_index);
	}

	void zone_reader32::resolve_offset(std::uint32_t& value, const bool alias)
	{
		const auto offset = value - 1;
		const auto block_index = offset >> offset_bits;
		const auto block_offset = offset & ((1u << offset_bits) - 1);

		if (is_inline(value) || block_index >= this->blocks_.size() || block_offset >= this->blocks_[block_index].size)
		{
			throw std::runtime_error(std::format("pointer 0x{:08X} is not a valid zone offset", value));
		}

		const auto address = this->blocks_[block_index].memory + block_offset;
		value = alias ? *reinterpret_cast<const std::uint32_t*>(address) : x86::address_space::offset_of(address);
	}

	char* zone_reader32::read_string_data()
	{
		const auto start = this->data_.data() + this->data_position_;
		const auto end = std::memchr(start, 0, this->remaining());
		if (!end)
		{
			throw std::runtime_error("string passes the end of the zone");
		}

		return static_cast<char*>(this->read_bytes(static_cast<const char*>(end) - start + 1));
	}

	std::uint32_t zone_reader32::map_script_string(const std::uint32_t index) const
	{
		if (index >= this->script_strings_.size())
		{
			throw std::runtime_error(std::format("script string {} does not exist (count {})", index, this->script_strings_.size()));
		}

		return this->script_strings_[index];
	}

	void zone_reader32::read_asset_pointer(const std::int32_t type, x86::pointer<void>& field, const bool in_temp_block)
	{
		if (in_temp_block)
		{
			this->push_stream(this->layout_.temp_block);
		}

		if (field.value)
		{
			if (is_inline(field.value))
			{
				this->align(this->layout_.asset_alignment);
				const auto slot = is_insert(field.value) ? this->insert_pointer() : nullptr;
				this->inline_asset_loader_(type, field, slot);
			}
			else
			{
				this->resolve_offset(field.value, in_temp_block);
			}
		}

		if (in_temp_block)
		{
			this->pop_stream();
		}
	}
}
