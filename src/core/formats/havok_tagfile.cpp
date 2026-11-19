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

			std::uint32_t read_u32()
			{
				if (this->offset_ + sizeof(std::uint32_t) > this->end_)
				{
					throw std::runtime_error("havok tagfile: packed integer out of range");
				}

				std::uint32_t value{};
				std::memcpy(&value, this->data_ + this->offset_, sizeof(value));
				this->offset_ += sizeof(value);
				return value;
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
			info.body_flags = static_cast<std::uint32_t>(flags);
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
				info.type_flags = static_cast<std::uint32_t>(bodies.read());
			}
			if (flags & body_members)
			{
				info.raw_member_count = static_cast<std::uint32_t>(bodies.read());
				const auto count = info.raw_member_count & 0xFFFF;
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

		if (chunks.contains("THSH"))
		{
			packed_reader hashes(data, chunks["THSH"]);
			const auto count = hashes.read();
			for (auto i = 0u; i < count; i++)
			{
				const auto index = hashes.read();
				auto& info = this->types_.at(index);
				info.hash = hashes.read_u32();
				info.has_hash = true;
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

	namespace
	{
		void append_packed(std::string& out, std::uint64_t value)
		{
			if (value < 0x80)
			{
				out.push_back(static_cast<char>(value));
			}
			else if (value < 0x4000)
			{
				out.push_back(static_cast<char>(0x80 | (value >> 8)));
				out.push_back(static_cast<char>(value));
			}
			else if (value < 0x200000)
			{
				out.push_back(static_cast<char>(0xC0 | (value >> 16)));
				out.push_back(static_cast<char>(value >> 8));
				out.push_back(static_cast<char>(value));
			}
			else if (value < 0x8000000)
			{
				out.push_back(static_cast<char>(0xE0 | (value >> 24)));
				out.push_back(static_cast<char>(value >> 16));
				out.push_back(static_cast<char>(value >> 8));
				out.push_back(static_cast<char>(value));
			}
			else if (value < 0x800000000)
			{
				out.push_back(static_cast<char>(0xE8 | (value >> 32)));
				for (auto shift = 24; shift >= 0; shift -= 8)
				{
					out.push_back(static_cast<char>(value >> shift));
				}
			}
			else if (value < 0x800000000000000)
			{
				out.push_back(static_cast<char>(0xF0 | (value >> 56)));
				for (auto shift = 48; shift >= 0; shift -= 8)
				{
					out.push_back(static_cast<char>(value >> shift));
				}
			}
			else
			{
				out.push_back(static_cast<char>(0xF8));
				for (auto shift = 56; shift >= 0; shift -= 8)
				{
					out.push_back(static_cast<char>(value >> shift));
				}
			}
		}

		void append_value(std::string& out, const void* data, const std::size_t size)
		{
			out.append(static_cast<const char*>(data), size);
		}

		template <typename T>
		void append_number(std::string& out, const T& value)
		{
			append_value(out, &value, sizeof(value));
		}

		void align(std::string& out, const std::size_t alignment)
		{
			while (out.size() % alignment)
			{
				out.push_back('\0');
			}
		}

		std::uint32_t name_index(std::vector<std::string>& names, const std::string& name)
		{
			const auto found = std::ranges::find(names, name);
			if (found != names.end())
			{
				return static_cast<std::uint32_t>(found - names.begin());
			}

			names.push_back(name);
			return static_cast<std::uint32_t>(names.size() - 1);
		}

		void write_chunk(std::string& out, const std::string_view tag, std::string content, const std::uint32_t flags)
		{
			align(content, 4);
			const auto size = 8 + content.size();
			if (size >= 0x40000000)
			{
				throw std::runtime_error("havok tagfile: chunk too large");
			}

			const auto header = static_cast<std::uint32_t>(size) | (flags << 30);
			out.push_back(static_cast<char>(header >> 24));
			out.push_back(static_cast<char>(header >> 16));
			out.push_back(static_cast<char>(header >> 8));
			out.push_back(static_cast<char>(header));
			out.append(tag);
			out.append(content);
		}
	}

	writer::writer(std::vector<type_info> types)
		: types_(std::move(types))
	{
		this->items_.push_back({});
	}

	std::uint32_t writer::type_id(const std::string_view name) const
	{
		for (auto i = 1u; i < this->types_.size(); i++)
		{
			if (this->types_[i].name == name)
			{
				return i;
			}
		}

		throw std::runtime_error(std::format("havok tagfile: no type {}", name));
	}

	std::uint32_t writer::type_size(const std::uint32_t type) const
	{
		auto index = type;
		while (!this->types_.at(index).size && this->types_[index].parent)
		{
			index = this->types_[index].parent;
		}

		return this->types_[index].size;
	}

	writer::reference writer::add_object(const std::uint32_t type, const void* data)
	{
		const auto size = this->type_size(type);
		align(this->data_, 16);
		const auto offset = this->data_.size();
		append_value(this->data_, data, size);
		this->items_.push_back({type, item_object, static_cast<std::uint32_t>(offset), 1});
		return {static_cast<std::uint32_t>(this->items_.size() - 1), static_cast<std::uint32_t>(offset)};
	}

	writer::reference writer::add_array(const std::uint32_t element_type, const void* data, const std::size_t count)
	{
		const auto size = static_cast<std::size_t>(this->type_size(element_type)) * count;
		if (this->types_[element_type].name != "char")
		{
			align(this->data_, 16);
		}

		const auto offset = this->data_.size();
		if (size)
		{
			append_value(this->data_, data, size);
		}

		this->items_.push_back({element_type, item_array, static_cast<std::uint32_t>(offset),
			static_cast<std::uint32_t>(count)});
		return {static_cast<std::uint32_t>(this->items_.size() - 1), static_cast<std::uint32_t>(offset)};
	}

	void writer::set_pointer(const std::size_t field_offset, const std::uint32_t field_type, const std::size_t item)
	{
		if (field_offset + 4 > this->data_.size())
		{
			throw std::runtime_error("havok tagfile: pointer field is outside the data");
		}

		if (this->type_size(field_type) == 4)
		{
			const auto value = static_cast<std::uint32_t>(item);
			std::memcpy(this->data_.data() + field_offset, &value, sizeof(value));
		}
		else
		{
			const auto value = static_cast<std::uint64_t>(item);
			std::memcpy(this->data_.data() + field_offset, &value, sizeof(value));
		}

		if (item)
		{
			this->patches_[field_type].push_back(static_cast<std::uint32_t>(field_offset));
		}
	}

	std::string writer::finish(const std::uint32_t root_item)
	{
		if (!root_item || root_item >= this->items_.size())
		{
			throw std::runtime_error("havok tagfile: bad root item");
		}

		std::vector<std::string> type_names;
		std::string type_names_chunk;
		append_packed(type_names_chunk, this->types_.size());
		for (auto i = 1u; i < this->types_.size(); i++)
		{
			const auto& info = this->types_[i];
			append_packed(type_names_chunk, name_index(type_names, info.name));
			append_packed(type_names_chunk, info.templates.size());
			for (const auto& [template_name, template_value] : info.templates)
			{
				append_packed(type_names_chunk, name_index(type_names, template_name));
				append_packed(type_names_chunk, template_value);
			}
		}

		std::string names;
		for (const auto& name : type_names)
		{
			names.append(name);
			names.push_back('\0');
		}

		std::vector<std::string> member_names;
		std::string member_names_chunk;
		std::string bodies;
		for (auto i = 1u; i < this->types_.size(); i++)
		{
			const auto& info = this->types_[i];
			append_packed(bodies, i);
			append_packed(bodies, info.parent);
			append_packed(bodies, info.body_flags);
			if (info.body_flags & body_format)
			{
				append_packed(bodies, info.format);
			}
			if (info.body_flags & body_subtype)
			{
				append_packed(bodies, info.subtype);
			}
			if (info.body_flags & body_version)
			{
				append_packed(bodies, info.version);
			}
			if (info.body_flags & body_size)
			{
				append_packed(bodies, info.size);
				append_packed(bodies, info.alignment);
			}
			if (info.body_flags & body_type_flags)
			{
				append_packed(bodies, info.type_flags);
			}
			if (info.body_flags & body_members)
			{
				append_packed(bodies, info.raw_member_count);
				for (const auto& member : info.members)
				{
					append_packed(bodies, name_index(member_names, member.name));
					append_packed(bodies, member.flags);
					append_packed(bodies, member.offset);
					append_packed(bodies, member.type);
				}
			}
			if (info.body_flags & body_interfaces)
			{
				append_packed(bodies, 0);
			}
			if (info.body_flags & body_attribute)
			{
				append_packed(bodies, 0);
			}
		}
		bodies.append(2, '\0');

		std::string hashes;
		auto hash_count = 0u;
		for (auto i = 1u; i < this->types_.size(); i++)
		{
			if (this->types_[i].has_hash)
			{
				hash_count++;
			}
		}
		append_packed(hashes, hash_count);
		for (auto i = 1u; i < this->types_.size(); i++)
		{
			if (!this->types_[i].has_hash)
			{
				continue;
			}

			append_packed(hashes, i);
			append_number(hashes, this->types_[i].hash);
		}

		member_names_chunk.clear();
		for (const auto& name : member_names)
		{
			member_names_chunk.append(name);
			member_names_chunk.push_back('\0');
		}

		std::string item_table;
		for (const auto& entry : this->items_)
		{
			const auto type_and_flags = entry.type | (entry.flags << 24);
			append_number(item_table, type_and_flags);
			append_number(item_table, entry.offset);
			append_number(item_table, entry.count);
		}

		std::string patch_table;
		for (auto& [type, offsets] : this->patches_)
		{
			std::ranges::sort(offsets);
			append_number(patch_table, type);
			append_number(patch_table, static_cast<std::uint32_t>(offsets.size()));
			for (const auto offset : offsets)
			{
				append_number(patch_table, offset);
			}
		}

		std::string type_chunk;
		write_chunk(type_chunk, "TPTR", std::string(this->types_.size() * 8, '\0'), 1);
		write_chunk(type_chunk, "TSTR", std::move(names), 1);
		write_chunk(type_chunk, "TNA1", std::move(type_names_chunk), 1);
		write_chunk(type_chunk, "FSTR", std::move(member_names_chunk), 1);
		write_chunk(type_chunk, "TBDY", std::move(bodies), 1);
		write_chunk(type_chunk, "THSH", std::move(hashes), 1);
		write_chunk(type_chunk, "TPAD", {}, 1);

		std::string index_chunk;
		write_chunk(index_chunk, "ITEM", std::move(item_table), 1);
		write_chunk(index_chunk, "PTCH", std::move(patch_table), 1);

		std::string result;
		std::string contents;
		write_chunk(contents, "SDKV", "20180200", 1);
		write_chunk(contents, "DATA", std::move(this->data_), 1);
		write_chunk(contents, "TYPE", std::move(type_chunk), 0);
		write_chunk(contents, "INDX", std::move(index_chunk), 0);
		write_chunk(result, "TAG0", std::move(contents), 0);
		return result;
	}
}
