#include <std_include.hpp>
#include "zone_writer.hpp"

namespace zonetool
{
	zone_writer::zone_writer(const zone_layout& layout, const std::size_t block_count, const std::size_t capacity)
		: layout_(layout)
		, output_(std::make_unique_for_overwrite<std::uint8_t[]>(capacity))
		, output_capacity_(capacity)
		, positions_(block_count)
		, peaks_(block_count)
	{
	}

	void zone_writer::set_inline_asset_writer(inline_asset_writer writer)
	{
		this->inline_asset_writer_ = std::move(writer);
	}

	void zone_writer::set_inserted_check(inserted_check check)
	{
		this->inserted_check_ = std::move(check);
	}

	void zone_writer::set_asset_resolver(asset_resolver resolver)
	{
		this->asset_resolver_ = std::move(resolver);
	}

	void zone_writer::set_data_identity(data_identity identity)
	{
		this->data_identity_ = std::move(identity);
	}

	void zone_writer::set_external_pointer(external_pointer handler)
	{
		this->external_pointer_ = std::move(handler);
	}

	void zone_writer::push_stream(const std::uint8_t block)
	{
		if (block >= this->positions_.size())
		{
			throw std::runtime_error(std::format("block {} does not exist", block));
		}

		const auto target = std::ranges::contains(this->layout_.inherited_blocks, block) ? this->current_block_ : block;
		this->stream_stack_.emplace_back(this->current_block_, this->positions_[target]);
		this->current_block_ = target;
	}

	void zone_writer::pop_stream()
	{
		if (this->stream_stack_.empty())
		{
			throw std::runtime_error("stream stack is empty");
		}

		const auto state = this->stream_stack_.back();
		this->stream_stack_.pop_back();

		if (this->current_block_ == this->layout_.temp_block)
		{
			this->positions_[this->layout_.temp_block] = state.position_at_push;
		}
		else if (this->current_block_ == this->layout_.pop_aligned_block)
		{
			this->align(this->layout_.pop_alignment);
		}

		this->current_block_ = state.previous_block;
	}

	void zone_writer::align(const std::size_t alignment)
	{
		auto& position = this->positions_[this->current_block_];
		position = (position + alignment) & ~alignment;
		this->peaks_[this->current_block_] = std::max(this->peaks_[this->current_block_], position);
	}

	std::uint8_t zone_writer::current_stream() const
	{
		return this->current_block_;
	}

	void* zone_writer::write_bytes(const void* source, const std::size_t size)
	{
		auto& position = this->positions_[this->current_block_];
		const location start{this->current_block_, position};
		position += size;
		this->peaks_[this->current_block_] = std::max(this->peaks_[this->current_block_], position);

		const auto bytes = static_cast<const std::uint8_t*>(source);
		this->last_source_ = bytes + size;
		if (size && source && this->current_block_ != this->layout_.temp_block)
		{
			this->written_[reinterpret_cast<std::uintptr_t>(bytes)] = {reinterpret_cast<std::uintptr_t>(bytes + size), start};
			if (const auto identity = this->data_identity_ ? this->data_identity_(source) : std::nullopt)
			{
				this->written_identities_.emplace(*identity, written_range{*identity + size, start});
			}
		}

		if (!this->writes_file_bytes())
		{
			return const_cast<void*>(source);
		}

		if (this->output_size_ + size > this->output_capacity_)
		{
			throw std::runtime_error(std::format("the zone is larger than 0x{:X} bytes", this->output_capacity_));
		}

		const auto destination = this->output_.get() + this->output_size_;
		std::memcpy(destination, source, size);
		if (size)
		{
			this->chunks_[this->output_size_] = {size, start};
		}

		this->output_size_ += size;
		return destination;
	}

	void* zone_writer::read_bytes(const std::size_t size)
	{
		if (!this->last_source_)
		{
			throw std::runtime_error("read_bytes has no data to continue");
		}

		return this->write_bytes(this->last_source_, size);
	}

	void zone_writer::finish()
	{
		const auto start = reinterpret_cast<std::uintptr_t>(this->output_.get());
		const auto end = start + this->output_size_;
		for (const auto& [field, value] : this->fixups_)
		{
			const auto address = reinterpret_cast<std::uintptr_t>(field);
			if (address >= start && address + sizeof(void*) <= end)
			{
				*field = reinterpret_cast<void*>(value);
			}
		}

		this->fixups_.clear();
	}

	std::string_view zone_writer::data() const
	{
		return {reinterpret_cast<const char*>(this->output_.get()), this->output_size_};
	}

	std::vector<zone_writer::written_chunk> zone_writer::written_chunks() const
	{
		std::vector<written_chunk> chunks;
		chunks.reserve(this->written_.size());
		for (const auto& [source, range] : this->written_)
		{
			chunks.emplace_back(reinterpret_cast<const void*>(source), static_cast<std::size_t>(range.end - source), range.start.block, range.start.offset);
		}

		return chunks;
	}

	std::vector<std::uint64_t> zone_writer::block_sizes() const
	{
		std::vector<std::uint64_t> sizes(this->positions_.size());
		for (std::size_t i = 0; i < sizes.size(); i++)
		{
			sizes[i] = std::max(this->positions_[i], this->peaks_[i]);
		}

		return sizes;
	}

	bool zone_writer::writes_file_bytes() const
	{
		return this->current_block_ != this->layout_.runtime_block && !std::ranges::contains(this->layout_.unread_blocks, this->current_block_)
			&& !this->is_reserved_block();
	}

	bool zone_writer::is_reserved_block() const
	{
		return std::ranges::contains(this->layout_.reserved_blocks, this->current_block_);
	}

	bool zone_writer::is_inserted(const void* data) const
	{
		return this->inserted_check_ && this->inserted_check_(data);
	}

	std::uint64_t zone_writer::following_marker() const
	{
		return this->layout_.following_markers.front();
	}

	std::uint64_t zone_writer::encode(const location& location) const
	{
		if (this->layout_.offset_format == zone_offset_format::treyarch)
		{
			return ((static_cast<std::uint64_t>(location.block) << 60) | location.offset) + 1;
		}

		return (this->layout_.offset_tag << 36) | (static_cast<std::uint64_t>(location.block) << 32) | (location.offset + 1);
	}

	std::optional<zone_writer::location> zone_writer::find_range(const std::map<std::uint64_t, written_range>& ranges, const std::uint64_t key)
	{
		auto range = ranges.upper_bound(key);
		if (range == ranges.begin())
		{
			return {};
		}

		--range;
		if (key >= range->second.end)
		{
			return {};
		}

		return location{range->second.start.block, static_cast<std::size_t>(range->second.start.offset + (key - range->first))};
	}

	std::optional<zone_writer::location> zone_writer::find_written(const void* source) const
	{
		if (const auto written = find_range(this->written_, reinterpret_cast<std::uintptr_t>(source)))
		{
			return written;
		}

		if (const auto identity = this->data_identity_ ? this->data_identity_(source) : std::nullopt)
		{
			return find_range(this->written_identities_, *identity);
		}

		return {};
	}

	std::optional<zone_writer::location> zone_writer::find_slot(const void* data) const
	{
		if (const auto slot = this->slots_.find(data); slot != this->slots_.end())
		{
			return slot->second;
		}

		if (const auto identity = this->data_identity_ ? this->data_identity_(data) : std::nullopt)
		{
			if (const auto slot = this->slot_identities_.find(*identity); slot != this->slot_identities_.end())
			{
				return slot->second;
			}
		}

		return {};
	}

	void zone_writer::add_slot(const void* data, const location& slot)
	{
		this->slots_[data] = slot;
		if (const auto identity = this->data_identity_ ? this->data_identity_(data) : std::nullopt)
		{
			this->slot_identities_.emplace(*identity, slot);
		}
	}

	std::optional<zone_writer::location> zone_writer::find_output_location(const void* address) const
	{
		const auto output = this->output_.get();
		const auto bytes = static_cast<const std::uint8_t*>(address);
		if (bytes < output || bytes >= output + this->output_size_)
		{
			return {};
		}

		const auto index = static_cast<std::size_t>(bytes - output);
		auto chunk = this->chunks_.upper_bound(index);
		if (chunk == this->chunks_.begin())
		{
			return {};
		}

		--chunk;
		if (index >= chunk->first + chunk->second.size)
		{
			return {};
		}

		return location{chunk->second.start.block, chunk->second.start.offset + (index - chunk->first)};
	}

	zone_writer::location zone_writer::insert_slot()
	{
		this->push_stream(this->layout_.virtual_block);
		this->align(7);

		auto& position = this->positions_[this->current_block_];
		const location slot{this->current_block_, position};
		position += sizeof(void*);
		this->peaks_[this->current_block_] = std::max(this->peaks_[this->current_block_], position);

		this->pop_stream();
		return slot;
	}

	void zone_writer::add_fixup(void** field, const std::uint64_t value)
	{
		this->fixups_.emplace_back(field, value);
	}

	void zone_writer::remember_slot(const void* data, void** field)
	{
		if (this->slots_.contains(data))
		{
			return;
		}

		if (const auto slot = this->find_output_location(field); slot && slot->block != this->layout_.temp_block)
		{
			this->add_slot(data, *slot);
		}
	}

	void* zone_writer::write_array(void** field, const std::size_t alignment, const std::size_t size)
	{
		if (!*field)
		{
			return nullptr;
		}

		if (const auto written = this->find_written(*field))
		{
			this->add_fixup(field, this->encode(*written));
			return nullptr;
		}

		this->align(alignment);
		const auto source = *field;
		const auto inserted = this->is_inserted(source);
		if (inserted)
		{
			this->add_slot(source, this->insert_slot());
		}

		const auto copy = this->write_bytes(source, size);
		this->add_fixup(field, inserted ? this->layout_.insert_marker : this->following_marker());
		this->remember_slot(source, field);
		*field = copy;
		return this->is_reserved_block() ? nullptr : copy;
	}

	void zone_writer::write_offset(void** field)
	{
		const auto written = this->find_written(*field);
		if (!written)
		{
			const auto location = this->find_output_location(field);
			if (location && this->external_pointer_ && this->external_pointer_(*field, location->block, location->offset))
			{
				this->add_fixup(field, 0);
				return;
			}

			throw std::runtime_error(std::format("pointer 0x{:016X} points to data that is not written", reinterpret_cast<std::uintptr_t>(*field)));
		}

		this->add_fixup(field, this->encode(*written));
	}

	bool zone_writer::write_alias(const void* data, void** field)
	{
		const auto slot = this->find_slot(data);
		if (!slot)
		{
			return false;
		}

		this->add_fixup(field, this->encode(*slot));
		return true;
	}

	void zone_writer::write_string(void** field)
	{
		if (!*field)
		{
			return;
		}

		if (const auto written = this->find_written(*field))
		{
			this->add_fixup(field, this->encode(*written));
			return;
		}

		const auto source = static_cast<const char*>(*field);
		const auto inserted = this->is_inserted(source);
		if (inserted)
		{
			this->add_slot(source, this->insert_slot());
		}

		const auto copy = this->write_bytes(source, std::strlen(source) + 1);
		this->add_fixup(field, inserted ? this->layout_.insert_marker : this->following_marker());
		*field = copy;
	}

	void zone_writer::write_asset(const std::int32_t type, void** field)
	{
		this->push_stream(this->layout_.temp_block);

		const auto header = *field && this->asset_resolver_ ? this->asset_resolver_(type, *field) : *field;
		if (header && !this->write_alias(header, field))
		{
			this->align(this->layout_.asset_alignment);
			const auto inserted = this->is_inserted(header);
			if (inserted)
			{
				this->add_slot(header, this->insert_slot());
			}

			this->add_fixup(field, inserted ? this->layout_.insert_marker : this->following_marker());
			this->remember_slot(header, field);
			this->inline_asset_writer_(type, header);
		}

		this->pop_stream();
	}
}
