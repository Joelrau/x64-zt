#include <std_include.hpp>
#include "havok_tagfile.hpp"

#include <utils/string.hpp>

namespace havok::tagfile
{
	namespace
	{
		struct chunk
		{
			std::size_t begin;
			std::size_t end;
		};

		std::uint32_t read_big_endian(std::string_view data, const std::size_t offset)
		{
			const auto* bytes = reinterpret_cast<const std::uint8_t*>(data.data() + offset);
			return (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
		}

		void read_chunks(std::string_view data, std::size_t offset, const std::size_t end, std::unordered_map<std::string, chunk>& chunks)
		{
			while (offset + 8 <= end)
			{
				const auto size = read_big_endian(data, offset) & 0x3FFFFFFF;
				const std::string tag(data.substr(offset + 4, 4));
				if (size < 8 || offset + size > end)
				{
					throw std::runtime_error("havok tagfile: bad chunk size");
				}

				chunks[tag] = {offset + 8, offset + size};
				if (tag == "TAG0" || tag == "TYPE" || tag == "INDX")
				{
					read_chunks(data, offset + 8, offset + size, chunks);
				}

				offset += size;
			}
		}

		class packed_reader
		{
		public:
			packed_reader(std::string_view data, const chunk& range)
				: data_(reinterpret_cast<const std::uint8_t*>(data.data()))
				, offset_(range.begin)
				, end_(range.end)
			{
			}

			bool more() const
			{
				return this->offset_ < this->end_;
			}

			std::uint64_t read()
			{
				const auto first = this->data_[this->offset_];
				if ((first & 0x80) == 0)
				{
					return this->read_bytes(1, 0x7F);
				}
				if ((first & 0xC0) == 0x80)
				{
					return this->read_bytes(2, 0x3F);
				}
				if ((first & 0xE0) == 0xC0)
				{
					return this->read_bytes(3, 0x1F);
				}
				if ((first & 0xF8) == 0xE0)
				{
					return this->read_bytes(4, 0x7);
				}
				if ((first & 0xF8) == 0xE8)
				{
					return this->read_bytes(5, 0x7);
				}
				if ((first & 0xF8) == 0xF0)
				{
					return this->read_bytes(8, 0x7);
				}
				if ((first & 0xF8) == 0xF8)
				{
					return this->read_bytes(9, 0x0);
				}

				throw std::runtime_error("havok tagfile: bad packed integer");
			}

		private:
			std::uint64_t read_bytes(const std::size_t count, const std::uint8_t first_mask)
			{
				if (this->offset_ + count > this->end_)
				{
					throw std::runtime_error("havok tagfile: packed integer out of range");
				}

				std::uint64_t result = this->data_[this->offset_] & first_mask;
				for (auto i = 1u; i < count; i++)
				{
					result = (result << 8) | this->data_[this->offset_ + i];
				}

				this->offset_ += count;
				return result;
			}

			const std::uint8_t* data_;
			std::size_t offset_;
			std::size_t end_;
		};

		std::vector<std::string> read_strings(std::string_view data, const chunk& range)
		{
			return utils::string::split(std::string(data.substr(range.begin, range.end - range.begin)), '\0');
		}

		template <typename T>
		T read_value(std::string_view data, const std::size_t offset)
		{
			if (offset + sizeof(T) > data.size())
			{
				throw std::runtime_error("havok tagfile: read out of range");
			}

			T result{};
			std::memcpy(&result, data.data() + offset, sizeof(T));
			return result;
		}
	}

	value::value(const file* owner, const std::uint32_t type, const std::size_t offset)
		: owner_(owner)
		, type_(type)
		, offset_(offset)
	{
	}

	bool value::is_null() const
	{
		return !this->owner_;
	}

	std::string_view value::type_name() const
	{
		return this->owner_ ? std::string_view(this->owner_->type(this->type_).name) : std::string_view();
	}

	value value::operator[](const std::string_view member) const
	{
		if (!this->owner_)
		{
			return {};
		}

		const auto* found = this->owner_->find_member(this->type_, member);
		if (!found)
		{
			throw std::runtime_error(std::format("havok tagfile: {} has no member {}", this->type_name(), member));
		}

		return {this->owner_, found->type, this->offset_ + found->offset};
	}

	value value::operator[](const std::size_t index) const
	{
		const auto range = this->elements();
		if (index >= range.count)
		{
			throw std::runtime_error("havok tagfile: array index out of range");
		}

		return {this->owner_, range.type, range.offset + index * this->owner_->type_size(range.type)};
	}

	std::size_t value::size() const
	{
		return this->elements().count;
	}

	value value::deref() const
	{
		if (!this->owner_)
		{
			return {};
		}

		const auto index = read_value<std::uint64_t>(this->owner_->data(), this->offset_);
		if (!index)
		{
			return {};
		}

		const auto& target = this->owner_->get_item(index);
		return {this->owner_, target.type, target.offset};
	}

	std::string_view value::string() const
	{
		const auto index = read_value<std::uint64_t>(this->owner_->data(), this->offset_);
		if (!index)
		{
			return {};
		}

		const auto& target = this->owner_->get_item(index);
		const auto text = this->owner_->data().substr(target.offset, target.count);
		return text.substr(0, text.find('\0'));
	}

	std::int64_t value::as_int() const
	{
		const auto& info = this->owner_->resolve(this->type_);
		const auto is_signed = (info.format & format_signed) != 0;
		const auto data = this->owner_->data();

		if (info.format & format_int8)
		{
			return is_signed ? read_value<std::int8_t>(data, this->offset_) : read_value<std::uint8_t>(data, this->offset_);
		}
		if (info.format & format_int16)
		{
			return is_signed ? read_value<std::int16_t>(data, this->offset_) : read_value<std::uint16_t>(data, this->offset_);
		}
		if (info.format & format_int32)
		{
			return is_signed ? read_value<std::int32_t>(data, this->offset_) : read_value<std::uint32_t>(data, this->offset_);
		}

		return read_value<std::int64_t>(data, this->offset_);
	}

	double value::as_float() const
	{
		if (this->owner_->type_size(this->type_) == sizeof(double))
		{
			return read_value<double>(this->owner_->data(), this->offset_);
		}

		return read_value<float>(this->owner_->data(), this->offset_);
	}

	bool value::as_bool() const
	{
		return read_value<std::uint8_t>(this->owner_->data(), this->offset_) != 0;
	}

	value::element_range value::elements() const
	{
		if (!this->owner_)
		{
			return {};
		}

		const auto& info = this->owner_->resolve(this->type_);
		if ((info.format & format_kind_mask) != kind_array)
		{
			throw std::runtime_error(std::format("havok tagfile: {} is not an array", this->type_name()));
		}

		if (info.format & format_fixed_size)
		{
			const auto element_type = info.subtype ? info.subtype : this->owner_->find_template(this->type_, "tT").value_or(0);
			const auto count = this->owner_->find_template(this->type_, "vN").value_or(info.format >> 8);
			return {element_type, this->offset_, count};
		}

		const auto index = this->owner_->type_size(this->type_) == sizeof(std::uint32_t)
			? read_value<std::uint32_t>(this->owner_->data(), this->offset_)
			: read_value<std::uint64_t>(this->owner_->data(), this->offset_);
		if (!index)
		{
			return {};
		}

		const auto& target = this->owner_->get_item(index);
		return {target.type, target.offset, target.count};
	}

	file::file(std::string_view data)
	{
		std::unordered_map<std::string, chunk> chunks;
		read_chunks(data, 0, data.size(), chunks);
		for (const auto* name : {"DATA", "TSTR", "TNA1", "FSTR", "TBDY", "ITEM"})
		{
			if (!chunks.contains(name))
			{
				throw std::runtime_error(std::format("havok tagfile: missing {} chunk", name));
			}
		}

		const auto& data_chunk = chunks["DATA"];
		this->data_ = data.substr(data_chunk.begin, data_chunk.end - data_chunk.begin);

		const auto type_strings = read_strings(data, chunks["TSTR"]);
		const auto member_strings = read_strings(data, chunks["FSTR"]);

		packed_reader names(data, chunks["TNA1"]);
		this->types_.resize(names.read());
		for (auto i = 1u; i < this->types_.size(); i++)
		{
			auto& info = this->types_[i];
			info.name = type_strings.at(names.read());
			const auto template_count = names.read();
			for (auto t = 0u; t < template_count; t++)
			{
				const auto& template_name = type_strings.at(names.read());
				info.templates.emplace_back(template_name, static_cast<std::uint32_t>(names.read()));
			}
		}

		packed_reader bodies(data, chunks["TBDY"]);
		while (bodies.more())
		{
			const auto index = bodies.read();
			if (!index)
			{
				continue;
			}

			auto& info = this->types_.at(index);
			info.parent = static_cast<std::uint32_t>(bodies.read());
			const auto flags = bodies.read();
			if (flags & body_format)
			{
				info.format = static_cast<std::uint32_t>(bodies.read());
			}
			if (flags & body_subtype)
			{
				info.subtype = static_cast<std::uint32_t>(bodies.read());
			}
			if (flags & body_version)
			{
				info.version = static_cast<std::uint32_t>(bodies.read());
			}
			if (flags & body_size)
			{
				info.size = static_cast<std::uint32_t>(bodies.read());
				info.alignment = static_cast<std::uint32_t>(bodies.read());
			}
			if (flags & body_type_flags)
			{
				bodies.read();
			}
			if (flags & body_members)
			{
				const auto count = bodies.read() & 0xFFFF;
				for (auto m = 0u; m < count; m++)
				{
					type_member member{};
					member.name = member_strings.at(bodies.read());
					member.flags = static_cast<std::uint32_t>(bodies.read());
					member.offset = static_cast<std::uint32_t>(bodies.read());
					member.type = static_cast<std::uint32_t>(bodies.read());
					info.members.emplace_back(std::move(member));
				}
			}
			if (flags & body_interfaces)
			{
				const auto count = bodies.read();
				for (auto m = 0u; m < count * 2; m++)
				{
					bodies.read();
				}
			}
			if (flags & body_attribute)
			{
				bodies.read();
			}
		}

		const auto& items = chunks["ITEM"];
		for (auto offset = items.begin; offset + 12 <= items.end; offset += 12)
		{
			const auto type_and_flags = read_value<std::uint32_t>(data, offset);
			this->items_.push_back({type_and_flags & 0xFFFFFF, type_and_flags >> 24,
				read_value<std::uint32_t>(data, offset + 4), read_value<std::uint32_t>(data, offset + 8)});
		}
	}

	value file::root() const
	{
		const auto& target = this->get_item(1);
		return {this, target.type, target.offset};
	}

	const type_info& file::type(const std::uint32_t index) const
	{
		return this->types_.at(index);
	}

	const type_info& file::resolve(std::uint32_t index) const
	{
		while ((this->types_.at(index).format & format_kind_mask) == kind_void && this->types_[index].parent)
		{
			index = this->types_[index].parent;
		}

		return this->types_[index];
	}

	std::uint32_t file::type_size(std::uint32_t index) const
	{
		while (!this->types_.at(index).size && this->types_[index].parent)
		{
			index = this->types_[index].parent;
		}

		return this->types_[index].size;
	}

	const type_member* file::find_member(std::uint32_t type, const std::string_view name) const
	{
		while (type)
		{
			for (const auto& member : this->types_.at(type).members)
			{
				if (member.name == name)
				{
					return &member;
				}
			}

			type = this->types_[type].parent;
		}

		return nullptr;
	}

	std::optional<std::uint32_t> file::find_template(std::uint32_t type, const std::string_view name) const
	{
		while (type)
		{
			for (const auto& [template_name, template_value] : this->types_.at(type).templates)
			{
				if (template_name == name)
				{
					return template_value;
				}
			}

			type = this->types_[type].parent;
		}

		return {};
	}

	const item& file::get_item(const std::uint64_t index) const
	{
		return this->items_.at(index);
	}

	std::string_view file::data() const
	{
		return this->data_;
	}
}
