#include <std_include.hpp>
#include "zone_reader.hpp"

#include "../xfile/structs.hpp"

namespace zonetool
{
	namespace
	{
		std::uint8_t* allocate_block(const std::size_t size)
		{
			if (!size)
			{
				return nullptr;
			}

			const auto memory = VirtualAlloc(nullptr, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
			if (!memory)
			{
				throw std::runtime_error(std::format("cannot allocate 0x{:X} bytes of block memory", size));
			}

			return static_cast<std::uint8_t*>(memory);
		}

		std::uint64_t pointer_value(const void* pointer)
		{
			return reinterpret_cast<std::uint64_t>(pointer);
		}

		struct zone_offset
		{
			std::uint64_t block_index;
			std::uint64_t offset;
		};

		constexpr auto offset_tag_shift = 36;

		zone_offset split_offset(const void* pointer)
		{
			const auto value = pointer_value(pointer);
			return {(value >> 32) & 0xF, static_cast<std::uint32_t>(value) - 1ull};
		}
	}

	zone_reader::zone_reader(const std::string_view data, const std::span<const std::uint64_t> block_sizes,
		const zone_layout& layout)
		: data_(data)
		, layout_(layout)
	{
		this->blocks_.reserve(block_sizes.size());
		for (const auto size : block_sizes)
		{
			this->blocks_.emplace_back(allocate_block(size), size, 0, 0, 0);
		}
	}

	zone_reader::~zone_reader()
	{
		for (const auto& block : this->blocks_)
		{
			if (block.memory)
			{
				VirtualFree(block.memory, 0, MEM_RELEASE);
			}
		}
	}

	void zone_reader::set_inline_asset_loader(inline_asset_loader loader)
	{
		this->inline_asset_loader_ = std::move(loader);
	}

	void zone_reader::set_script_strings(std::vector<std::uint32_t> ids)
	{
		this->script_strings_ = std::move(ids);
	}

	void zone_reader::set_zone_index(const std::uint16_t index)
	{
		this->zone_index_ = index;
	}

	std::uint16_t zone_reader::zone_index() const
	{
		return this->zone_index_;
	}

	void zone_reader::push_stream(const std::uint8_t block)
	{
		if (block >= this->blocks_.size())
		{
			throw std::runtime_error(std::format("block {} does not exist", block));
		}

		this->stream_stack_.emplace_back(this->current_block_, this->blocks_[block].position);
		this->current_block_ = block;
	}

	void zone_reader::pop_stream()
	{
		if (this->stream_stack_.empty())
		{
			throw std::runtime_error("stream stack is empty");
		}

		const auto state = this->stream_stack_.back();
		this->stream_stack_.pop_back();

		if (this->current_block_ == XFILE_BLOCK_TEMP)
		{
			this->blocks_[XFILE_BLOCK_TEMP].position = state.position_at_push;
		}
		else if (this->current_block_ == this->layout_.pop_aligned_block)
		{
			this->align(15);
		}

		this->current_block_ = state.previous_block;
	}

	void zone_reader::align(const std::size_t alignment)
	{
		auto& block = this->current_block();
		block.position = ((block.base + block.position + alignment) & ~alignment) - block.base;
		block.largest_alignment = std::max(block.largest_alignment, alignment);
	}

	void zone_reader::set_block_base(const std::uint8_t block, const std::size_t base)
	{
		this->blocks_[block].base = base;
	}

	std::size_t zone_reader::end_anchored_data_start() const
	{
		if (!this->layout_.end_anchored_block)
		{
			return 0;
		}

		const auto& block = this->blocks_[*this->layout_.end_anchored_block];
		return block.size - block.position;
	}

	std::size_t zone_reader::end_anchored_alignment() const
	{
		return this->layout_.end_anchored_block ? this->blocks_[*this->layout_.end_anchored_block].largest_alignment : 0;
	}

	std::uint8_t zone_reader::current_stream() const
	{
		return this->current_block_;
	}

	std::size_t zone_reader::remaining() const
	{
		return this->data_.size() - this->data_position_;
	}

	void zone_reader::read_raw(void* destination, const std::size_t size)
	{
		if (this->remaining() < size)
		{
			throw std::runtime_error(std::format("read of 0x{:X} bytes passes the end of the zone", size));
		}

		std::memcpy(destination, this->data_.data() + this->data_position_, size);
		this->data_position_ += size;
	}

	void* zone_reader::read_bytes(const std::size_t size)
	{
		auto& block = this->current_block();
		const auto is_runtime = this->current_block_ == this->layout_.runtime_block;
		if (block.position + size > block.size)
		{
			// Runtime data holds no file bytes, and some loaders size it differently from the game.
			if (is_runtime)
			{
				return this->allocate_bytes(size);
			}

			throw std::runtime_error(std::format("block {} overflows at 0x{:X} + 0x{:X} (size 0x{:X})",
				this->current_block_, block.position, size, block.size));
		}

		const auto destination = block.memory + block.position;
		if (is_runtime)
		{
			std::memset(destination, 0, size);
		}
		else
		{
			this->read_raw(destination, size);
		}

		block.position += size;
		return destination;
	}

	bool zone_reader::is_inline(const void* pointer) const
	{
		const auto value = pointer_value(pointer);
		return std::ranges::contains(this->layout_.following_markers, value) || value == this->layout_.insert_marker;
	}

	bool zone_reader::is_insert(const void* pointer) const
	{
		return pointer_value(pointer) == this->layout_.insert_marker;
	}

	bool zone_reader::is_offset(const void* pointer) const
	{
		if (this->is_inline(pointer))
		{
			return false;
		}

		// Stock zones leave the bits above the block index clear, and zonetool sets them to the pointer mask.
		// Any other value is a raw heap pointer that the game ignores.
		const auto tag = pointer_value(pointer) >> offset_tag_shift;
		if (tag != 0 && tag != this->layout_.offset_tag)
		{
			return false;
		}

		const auto [block_index, offset] = split_offset(pointer);
		return block_index < this->blocks_.size() && offset < this->blocks_[block_index].size;
	}

	bool zone_reader::contains(const void* pointer) const
	{
		const auto address = static_cast<const std::uint8_t*>(pointer);
		return std::ranges::any_of(this->blocks_, [&](const block& block)
		{
			return address >= block.memory && address < block.memory + block.size;
		});
	}

	void zone_reader::resolve_deferred_offsets()
	{
		if (!this->layout_.end_anchored_block)
		{
			return;
		}

		const auto block_index = *this->layout_.end_anchored_block;
		const auto data_start = this->end_anchored_data_start();
		for (const auto& deferred : this->deferred_offsets_)
		{
			if (deferred.offset < data_start)
			{
				throw std::runtime_error(std::format("offset 0x{:X} in block {} points before the data of the block",
					deferred.offset, block_index));
			}

			const auto address = this->offset_address(block_index, deferred.offset - data_start);
			*deferred.field = deferred.target == offset_target::alias ? *reinterpret_cast<void**>(address) : address;
		}

		this->deferred_offsets_.clear();
	}

	std::size_t zone_reader::count_misplaced_strings() const
	{
		const auto data_start = this->end_anchored_data_start();
		return static_cast<std::size_t>(std::ranges::count_if(this->deferred_offsets_, [&](const deferred_offset& deferred)
		{
			return deferred.target == offset_target::string
				&& (deferred.offset < data_start || !std::ranges::binary_search(this->string_positions_, deferred.offset - data_start));
		}));
	}

	void zone_reader::move_deferred_offsets(const void* source, const std::size_t size, void* destination)
	{
		const auto source_start = static_cast<const std::uint8_t*>(source);
		for (auto& deferred : this->deferred_offsets_)
		{
			const auto field = reinterpret_cast<std::uint8_t*>(deferred.field);
			if (field >= source_start && field < source_start + size)
			{
				deferred.field = reinterpret_cast<void**>(static_cast<std::uint8_t*>(destination) + (field - source_start));
			}
		}
	}

	void** zone_reader::insert_pointer()
	{
		this->push_stream(this->layout_.virtual_block);
		this->align(7);

		auto& block = this->current_block();
		if (block.position + sizeof(void*) > block.size)
		{
			throw std::runtime_error("insert pointer overflows the virtual block");
		}

		const auto slot = reinterpret_cast<void**>(block.memory + block.position);
		block.position += sizeof(void*);

		this->pop_stream();
		return slot;
	}

	zone_reader::block& zone_reader::current_block()
	{
		return this->blocks_[this->current_block_];
	}

	std::uint8_t* zone_reader::offset_address(const std::uint8_t block_index, const std::size_t offset) const
	{
		return this->blocks_[block_index].memory + offset;
	}

	void zone_reader::resolve_offset(void** field, const offset_target target)
	{
		if (!this->is_offset(*field))
		{
			throw std::runtime_error(std::format("pointer 0x{:016X} is not a valid zone offset", pointer_value(*field)));
		}

		const auto [block_index, offset] = split_offset(*field);
		if (block_index == this->layout_.end_anchored_block)
		{
			this->deferred_offsets_.emplace_back(field, offset, target, this->blocks_[block_index].position);
			return;
		}

		const auto address = this->offset_address(static_cast<std::uint8_t>(block_index), offset);
		*field = target == offset_target::alias ? *reinterpret_cast<void**>(address) : address;
	}

	char* zone_reader::read_string_data()
	{
		const auto start = this->data_.data() + this->data_position_;
		const auto end = std::memchr(start, 0, this->remaining());
		if (!end)
		{
			throw std::runtime_error("string passes the end of the zone");
		}

		const auto size = static_cast<const char*>(end) - start + 1;
		if (this->current_block_ == this->layout_.end_anchored_block)
		{
			this->string_positions_.push_back(this->current_block().position);
		}

		return static_cast<char*>(this->read_bytes(size));
	}

	std::uint32_t zone_reader::map_script_string(const std::uint32_t index) const
	{
		if (index >= this->script_strings_.size())
		{
			throw std::runtime_error(std::format("script string {} does not exist (count {})", index, this->script_strings_.size()));
		}

		return this->script_strings_[index];
	}

	void zone_reader::read_asset_pointer(const std::int32_t type, void** field)
	{
		this->push_stream(XFILE_BLOCK_TEMP);

		if (*field)
		{
			if (this->is_inline(*field))
			{
				this->align(3);
				const auto slot = this->is_insert(*field) ? this->insert_pointer() : nullptr;
				this->inline_asset_loader_(type, field, slot);
			}
			else
			{
				this->resolve_offset(field, offset_target::alias);
			}
		}

		this->pop_stream();
	}

	void* zone_reader::allocate_bytes(const std::size_t size)
	{
		auto& allocation = this->allocations_.emplace_back(std::make_unique<std::uint8_t[]>(size));
		return allocation.get();
	}
}
