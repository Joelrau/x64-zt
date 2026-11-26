#include <std_include.hpp>
#include "dxil.hpp"

namespace dxil
{
	namespace
	{
		std::runtime_error error(const std::string& text)
		{
			return std::runtime_error("dxil: " + text);
		}

		template <typename T>
		T read_struct(const std::uint8_t* data, const std::size_t size, const std::size_t offset)
		{
			if (offset > size || size - offset < sizeof(T))
			{
				throw error("read past the end of the container");
			}

			T value;
			std::memcpy(&value, data + offset, sizeof(T));
			return value;
		}

		std::uint64_t operand(const record& entry, const std::size_t index)
		{
			if (index >= entry.operands.size())
			{
				throw error(std::format("record {} has no operand {}", entry.code, index));
			}

			return entry.operands[index];
		}

		std::string operand_text(const record& entry, const std::size_t first)
		{
			std::string text;
			for (auto i = first; i < entry.operands.size(); i++)
			{
				if (entry.operands[i] > 0xFF)
				{
					throw error("character out of range");
				}

				text += static_cast<char>(entry.operands[i]);
			}

			return text;
		}

		std::int64_t decode_signed(const std::uint64_t value)
		{
			const auto magnitude = static_cast<std::int64_t>(value >> 1);
			return (value & 1) ? -magnitude : magnitude;
		}

		char decode_char6(const std::uint64_t value)
		{
			if (value < 26)
			{
				return static_cast<char>('a' + value);
			}

			if (value < 52)
			{
				return static_cast<char>('A' + value - 26);
			}

			if (value < 62)
			{
				return static_cast<char>('0' + value - 52);
			}

			return value == 62 ? '.' : '_';
		}

		const metadata_entry& metadata_at(const bitcode_module& module, const std::uint64_t id)
		{
			if (id >= module.metadata.size())
			{
				throw error(std::format("metadata {} out of range", id));
			}

			return module.metadata[id];
		}

		const metadata_entry* node_operand(const bitcode_module& module, const metadata_entry& node, const std::size_t index)
		{
			if (node.kind != metadata_node || index >= node.operands.size())
			{
				throw error(std::format("metadata node has no operand {}", index));
			}

			const auto id = node.operands[index];
			return id ? &metadata_at(module, id - 1) : nullptr;
		}

		const metadata_entry& value_operand(const bitcode_module& module, const metadata_entry& node, const std::size_t index)
		{
			const auto value = node_operand(module, node, index);
			if (!value || value->kind != metadata_value)
			{
				throw error(std::format("metadata operand {} is not a value", index));
			}

			return *value;
		}

		std::int64_t integer_operand(const bitcode_module& module, const metadata_entry& node, const std::size_t index)
		{
			const auto& value = value_operand(module, node, index);
			if (value.value < module.global_value_count || value.value - module.global_value_count >= module.constants.size())
			{
				throw error(std::format("metadata operand {} is not a constant", index));
			}

			const auto& constant = module.constants[value.value - module.global_value_count];
			if (!constant.is_integer)
			{
				throw error(std::format("metadata operand {} is not an integer", index));
			}

			return constant.value;
		}

		const std::string& string_operand(const bitcode_module& module, const metadata_entry& node, const std::size_t index)
		{
			const auto value = node_operand(module, node, index);
			if (!value || value->kind != metadata_string)
			{
				throw error(std::format("metadata operand {} is not a string", index));
			}

			return value->text;
		}

		const metadata_entry& node_at(const metadata_entry* node)
		{
			if (!node || node->kind != metadata_node)
			{
				throw error("metadata is not a node");
			}

			return *node;
		}

		const llvm_type& type_at(const bitcode_module& module, const std::uint64_t id)
		{
			if (id >= module.types.size())
			{
				throw error(std::format("type {} out of range", id));
			}

			return module.types[id];
		}

		std::uint64_t element_type(const bitcode_module& module, const std::uint64_t id)
		{
			const auto& type = type_at(module, id);
			if (type.elements.empty())
			{
				throw error(std::format("type {} has no element type", id));
			}

			return type.elements[0];
		}

		struct_annotations read_struct_annotations(const bitcode_module& module)
		{
			struct_annotations annotations;
			const auto type_annotations = module.named_metadata.find("dx.typeAnnotations");
			if (type_annotations == module.named_metadata.end())
			{
				return annotations;
			}

			for (const auto id : type_annotations->second)
			{
				const auto& list = node_at(&metadata_at(module, id));
				if (list.operands.empty() || integer_operand(module, list, 0) != 0)
				{
					continue;
				}

				for (auto i = 1u; i + 1 < list.operands.size(); i += 2)
				{
					annotations[value_operand(module, list, i).type] = &node_at(node_operand(module, list, i + 1));
				}
			}

			return annotations;
		}

		field_annotation read_field_annotation(const bitcode_module& module, const metadata_entry& node)
		{
			field_annotation field{};
			for (auto i = 0u; i + 1 < node.operands.size(); i += 2)
			{
				switch (integer_operand(module, node, i))
				{
				case field_tag_matrix:
				{
					const auto& matrix = node_at(node_operand(module, node, i + 1));
					field.rows = static_cast<std::uint32_t>(integer_operand(module, matrix, 0));
					field.columns = static_cast<std::uint32_t>(integer_operand(module, matrix, 1));
					field.orientation = integer_operand(module, matrix, 2);
					break;
				}
				case field_tag_cbuffer_offset:
					field.offset = static_cast<std::uint32_t>(integer_operand(module, node, i + 1));
					break;
				case field_tag_name:
					field.name = string_operand(module, node, i + 1);
					break;
				case field_tag_component_type:
					field.component = integer_operand(module, node, i + 1);
					break;
				}
			}

			return field;
		}

		std::string scalar_name(const std::int64_t component, const llvm_type& type)
		{
			switch (component)
			{
			case component_i1: return "bool";
			case component_i16: return "int16_t";
			case component_u16: return "uint16_t";
			case component_i32: return "int";
			case component_u32: return "uint";
			case component_i64: return "int64_t";
			case component_u64: return "uint64_t";
			case component_f16: return "half";
			case component_f32: return "float";
			case component_f64: return "double";
			}

			switch (type.kind)
			{
			case type_half: return "half";
			case type_float: return "float";
			case type_double: return "double";
			case type_integer: return type.count == 1 ? "bool" : "int";
			default: return "unknown";
			}
		}

		std::uint32_t scalar_size(const std::int64_t component, const llvm_type& type)
		{
			switch (component)
			{
			case component_i1:
			case component_i32:
			case component_u32:
			case component_f32:
				return 4;
			case component_i16:
			case component_u16:
			case component_f16:
				return 2;
			case component_i64:
			case component_u64:
			case component_f64:
				return 8;
			}

			switch (type.kind)
			{
			case type_half: return 2;
			case type_double: return 8;
			case type_integer: return type.count == 64 ? 8 : type.count == 16 ? 2 : 4;
			default: return 4;
			}
		}

		cbuffer_field make_leaf(const bitcode_module& module, const field_annotation& annotation, const std::uint64_t type_id,
			const std::uint32_t elements)
		{
			auto scalar_id = type_id;
			std::uint32_t vector_size = 1;
			for (auto depth = 0u; type_at(module, scalar_id).kind == type_array || type_at(module, scalar_id).kind == type_vector; depth++)
			{
				const auto& type = type_at(module, scalar_id);
				if (depth == 8)
				{
					throw error("field type nesting too deep");
				}

				if (type.kind == type_vector)
				{
					vector_size = static_cast<std::uint32_t>(type.count);
				}

				scalar_id = element_type(module, scalar_id);
			}

			const auto& scalar = type_at(module, scalar_id);
			const auto name = scalar_name(annotation.component, scalar);
			const auto size = scalar_size(annotation.component, scalar);

			cbuffer_field field{};
			field.name = annotation.name;
			field.elements = elements;

			std::uint32_t element_size;
			if (annotation.rows)
			{
				const auto row_major = annotation.orientation == matrix_row_major;
				field.type = std::format("{}{}{}x{}", row_major ? "row_major " : "column_major ", name, annotation.rows, annotation.columns);
				element_size = row_major ? (annotation.rows - 1) * 16 + annotation.columns * size
					: (annotation.columns - 1) * 16 + annotation.rows * size;
			}
			else if (vector_size > 1)
			{
				field.type = std::format("{}{}", name, vector_size);
				element_size = vector_size * size;
			}
			else
			{
				field.type = name;
				element_size = size;
			}

			field.size = elements ? (elements - 1) * ((element_size + 15) & ~15u) + element_size : element_size;
			return field;
		}

		std::uint32_t peel_arrays(const bitcode_module& module, const field_annotation& field, std::uint64_t& type_id)
		{
			std::uint32_t elements = 0;
			for (auto depth = 0u; type_at(module, type_id).kind == type_array; depth++)
			{
				const auto& type = type_at(module, type_id);
				if (field.rows && type_at(module, element_type(module, type_id)).kind == type_vector)
				{
					break;
				}

				if (depth == 8)
				{
					throw error("array nesting too deep");
				}

				elements = (elements ? elements : 1) * static_cast<std::uint32_t>(type.count);
				type_id = element_type(module, type_id);
			}

			return elements;
		}

		std::uint32_t packed_alignment(const bitcode_module& module, const std::uint64_t type_id, const std::uint32_t depth = 0)
		{
			if (depth > 16)
			{
				throw error("type nesting too deep");
			}

			const auto& type = type_at(module, type_id);
			switch (type.kind)
			{
			case type_integer: return type.count == 64 ? 8 : type.count == 16 ? 2 : 4;
			case type_half: return 2;
			case type_double: return 8;
			case type_array:
			case type_vector: return packed_alignment(module, element_type(module, type_id), depth + 1);
			case type_struct:
			{
				std::uint32_t alignment = 1;
				for (const auto element : type.elements)
				{
					alignment = std::max(alignment, packed_alignment(module, element, depth + 1));
				}

				return alignment;
			}
			default: return 4;
			}
		}

		std::uint32_t align_up(const std::uint32_t value, const std::uint32_t alignment)
		{
			return (value + alignment - 1) / alignment * alignment;
		}

		std::uint32_t packed_size(const bitcode_module& module, const std::uint64_t type_id, const std::uint32_t depth = 0)
		{
			if (depth > 16)
			{
				throw error("type nesting too deep");
			}

			const auto& type = type_at(module, type_id);
			switch (type.kind)
			{
			case type_integer: return type.count == 64 ? 8 : type.count == 16 ? 2 : 4;
			case type_half: return 2;
			case type_double: return 8;
			case type_array:
			case type_vector:
			{
				const auto element = element_type(module, type_id);
				return static_cast<std::uint32_t>(type.count) * align_up(packed_size(module, element, depth + 1), packed_alignment(module, element));
			}
			case type_struct:
			{
				std::uint32_t size = 0;
				for (const auto element : type.elements)
				{
					size = align_up(size, packed_alignment(module, element)) + packed_size(module, element, depth + 1);
				}

				return align_up(size, packed_alignment(module, type_id));
			}
			default: return 4;
			}
		}

		void add_struct_fields(field_walk& walk, const std::uint64_t struct_id, const std::uint32_t base_offset, const std::string& prefix,
			const std::uint32_t depth)
		{
			if (depth > 16 || ++walk.visits > 0x10000)
			{
				throw error("struct nesting too large");
			}

			const auto& type = type_at(walk.module, struct_id);
			const auto annotation = walk.annotations.find(struct_id);
			if (type.kind != type_struct || annotation == walk.annotations.end())
			{
				return;
			}

			std::uint32_t packed_offset = 0;
			for (auto i = 0u; i < type.elements.size(); i++)
			{
				packed_offset = align_up(packed_offset, packed_alignment(walk.module, type.elements[i]));
				const auto element_offset = packed_offset;
				packed_offset += packed_size(walk.module, type.elements[i]);

				const auto field_node = node_operand(walk.module, *annotation->second, i + 1);
				if (!field_node)
				{
					continue;
				}

				auto field = read_field_annotation(walk.module, node_at(field_node));
				if (walk.packed)
				{
					field.offset = element_offset;
				}

				auto field_type = type.elements[i];
				const auto elements = peel_arrays(walk.module, field, field_type);

				if (!field.rows && type_at(walk.module, field_type).kind == type_struct)
				{
					const auto nested_prefix = std::format("{}{}{}.", prefix, field.name, elements ? "[0]" : "");
					add_struct_fields(walk, field_type, base_offset + field.offset, nested_prefix, depth + 1);
					continue;
				}

				auto leaf = make_leaf(walk.module, field, field_type, elements);
				leaf.name = prefix + leaf.name;
				leaf.offset = base_offset + field.offset;
				walk.fields.push_back(std::move(leaf));
			}
		}

		std::uint64_t resource_type(const bitcode_module& module, const metadata_entry& node)
		{
			auto type = value_operand(module, node, 1).type;
			for (auto depth = 0u; type_at(module, type).kind == type_pointer || type_at(module, type).kind == type_array; depth++)
			{
				if (depth == 8)
				{
					throw error("resource type nesting too deep");
				}

				type = element_type(module, type);
			}

			return type;
		}

		cbuffer read_cbuffer(const bitcode_module& module, const struct_annotations& annotations, const metadata_entry& node)
		{
			cbuffer result{};
			result.name = string_operand(module, node, 2);
			result.size = static_cast<std::uint32_t>(integer_operand(module, node, 6));

			field_walk walk{module, annotations, result.fields, 0, false};
			add_struct_fields(walk, resource_type(module, node), 0, "", 0);
			return result;
		}

		std::uint32_t read_element_stride(const bitcode_module& module, const metadata_entry& node)
		{
			const auto properties = node.operands.size() > 8 ? node_operand(module, node, 8) : nullptr;
			for (auto i = 0u; properties && i + 1 < properties->operands.size(); i += 2)
			{
				if (integer_operand(module, *properties, i) == resource_property_element_stride)
				{
					return static_cast<std::uint32_t>(integer_operand(module, *properties, i + 1));
				}
			}

			return 0;
		}

		cbuffer read_structured_buffer(const bitcode_module& module, const struct_annotations& annotations, const metadata_entry& node)
		{
			cbuffer result{};
			result.name = string_operand(module, node, 2);
			result.size = read_element_stride(module, node);

			auto type = resource_type(module, node);
			const auto& buffer_class = type_at(module, type);
			if (buffer_class.kind == type_struct && buffer_class.name.starts_with("class.StructuredBuffer") && !buffer_class.elements.empty())
			{
				type = buffer_class.elements[0];
			}

			field_walk walk{module, annotations, result.fields, 0, true};
			add_struct_fields(walk, type, 0, "", 0);
			return result;
		}

		const metadata_entry* find_resource_list(const bitcode_module& module, const resource_list list)
		{
			const auto resources = module.named_metadata.find("dx.resources");
			if (resources == module.named_metadata.end() || resources->second.empty())
			{
				return nullptr;
			}

			return node_operand(module, node_at(&metadata_at(module, resources->second[0])), list);
		}
	}

	const cbuffer_field* cbuffer::find(const std::string_view field_name) const
	{
		for (const auto& field : this->fields)
		{
			if (field.name == field_name)
			{
				return &field;
			}
		}

		return nullptr;
	}

	bitstream_reader::bitstream_reader(const std::uint8_t* data, const std::size_t size)
		: data(data)
		, bit_count(static_cast<std::uint64_t>(size) * 8)
		, bit_position(0)
	{
	}

	std::uint64_t bitstream_reader::read(const std::uint32_t width)
	{
		if (width > 64 || width > this->remaining_bits())
		{
			throw error("read past the end of the bitstream");
		}

		std::uint64_t value = 0;
		for (auto done = 0u; done < width;)
		{
			const auto bit = static_cast<std::uint32_t>(this->bit_position & 7);
			const auto count = std::min(8 - bit, width - done);
			const auto bits = (this->data[this->bit_position >> 3] >> bit) & ((1u << count) - 1);
			value |= static_cast<std::uint64_t>(bits) << done;
			done += count;
			this->bit_position += count;
		}

		return value;
	}

	std::uint64_t bitstream_reader::read_vbr(const std::uint32_t width)
	{
		if (width < 2 || width > 32)
		{
			throw error(std::format("bad vbr width {}", width));
		}

		const auto continue_bit = 1ull << (width - 1);
		std::uint64_t value = 0;
		for (auto shift = 0u;; shift += width - 1)
		{
			if (shift >= 64)
			{
				throw error("vbr value too large");
			}

			const auto piece = this->read(width);
			value |= (piece & (continue_bit - 1)) << shift;
			if (!(piece & continue_bit))
			{
				return value;
			}
		}
	}

	void bitstream_reader::align32()
	{
		const auto aligned = (this->bit_position + 31) & ~31ull;
		if (aligned > this->bit_count)
		{
			throw error("align past the end of the bitstream");
		}

		this->bit_position = aligned;
	}

	void bitstream_reader::skip_words(const std::uint64_t count)
	{
		if (count > this->remaining_bits() / 32)
		{
			throw error("skip past the end of the bitstream");
		}

		this->bit_position += count * 32;
	}

	std::uint64_t bitstream_reader::position() const
	{
		return this->bit_position;
	}

	std::uint64_t bitstream_reader::remaining_bits() const
	{
		return this->bit_count - this->bit_position;
	}

	bitcode_reader::bitcode_reader(const std::uint8_t* data, const std::size_t size)
		: stream(data, size)
	{
	}

	bitcode_module bitcode_reader::read()
	{
		if (this->stream.read(32) != 0xDEC04342)
		{
			throw error("bad bitcode magic");
		}

		while (!this->module_read)
		{
			if (this->stream.read(2) != abbrev_enter_subblock)
			{
				throw error("expected a top level block");
			}

			this->enter_block(0);
		}

		return std::move(this->module);
	}

	void bitcode_reader::enter_block(const std::uint32_t depth)
	{
		if (depth > 16)
		{
			throw error("block nesting too deep");
		}

		const auto id = this->stream.read_vbr(8);
		const auto abbrev_width = this->stream.read_vbr(4);
		this->stream.align32();
		const auto word_count = this->stream.read(32);

		const auto parsed = id == block_blockinfo || id == block_module || id == block_constants || id == block_metadata || id == block_type;
		if (!parsed)
		{
			this->stream.skip_words(word_count);
			return;
		}

		if (abbrev_width < 1 || abbrev_width > 32 || word_count > this->stream.remaining_bits() / 32)
		{
			throw error(std::format("bad header of block {}", id));
		}

		const auto end = this->stream.position() + word_count * 32;
		this->read_block(id, static_cast<std::uint32_t>(abbrev_width), depth);
		if (this->stream.position() != end)
		{
			throw error(std::format("block {} size mismatch", id));
		}

		if (id == block_module)
		{
			this->module_read = true;
		}
	}

	void bitcode_reader::read_block(const std::uint64_t id, const std::uint32_t abbrev_width, const std::uint32_t depth)
	{
		auto abbrevs = this->block_abbrevs[id];
		if (id == block_blockinfo)
		{
			this->blockinfo_target.reset();
		}

		while (true)
		{
			const auto entry_id = this->stream.read(abbrev_width);
			if (entry_id == abbrev_end_block)
			{
				this->stream.align32();
				return;
			}

			if (entry_id == abbrev_enter_subblock)
			{
				this->enter_block(depth + 1);
				continue;
			}

			if (entry_id == abbrev_define)
			{
				auto definition = this->read_abbrev_definition();
				if (id != block_blockinfo)
				{
					abbrevs.push_back(std::move(definition));
				}
				else if (this->blockinfo_target)
				{
					this->block_abbrevs[*this->blockinfo_target].push_back(std::move(definition));
				}
				else
				{
					throw error("blockinfo abbrev without a target block");
				}

				continue;
			}

			const auto entry = this->read_record(entry_id, abbrevs);
			if (id == block_blockinfo)
			{
				if (entry.code == blockinfo_set_block_id)
				{
					this->blockinfo_target = operand(entry, 0);
				}

				continue;
			}

			this->add_record(id, entry);
		}
	}

	abbrev bitcode_reader::read_abbrev_definition()
	{
		const auto op_count = this->stream.read_vbr(5);
		if (op_count > this->stream.remaining_bits())
		{
			throw error("abbrev too large");
		}

		abbrev definition;
		for (auto i = 0ull; i < op_count; i++)
		{
			if (this->stream.read(1))
			{
				definition.push_back({encoding_literal, this->stream.read_vbr(8)});
				continue;
			}

			const auto encoding = static_cast<abbrev_encoding>(this->stream.read(3));
			if (encoding == encoding_fixed || encoding == encoding_vbr)
			{
				const auto width = this->stream.read_vbr(5);
				if (width > (encoding == encoding_fixed ? 64u : 32u) || (width == 1 && encoding == encoding_vbr))
				{
					throw error(std::format("bad abbrev width {}", width));
				}

				definition.push_back(width ? abbrev_op{encoding, width} : abbrev_op{encoding_literal, 0});
				continue;
			}

			if (encoding != encoding_array && encoding != encoding_char6 && encoding != encoding_blob)
			{
				throw error(std::format("bad abbrev encoding {}", static_cast<std::uint32_t>(encoding)));
			}

			definition.push_back({encoding, 0});
		}

		return definition;
	}

	std::uint64_t bitcode_reader::read_operand(const abbrev_op& op)
	{
		switch (op.encoding)
		{
		case encoding_literal:
			return op.value;
		case encoding_fixed:
			return this->stream.read(static_cast<std::uint32_t>(op.value));
		case encoding_vbr:
			return this->stream.read_vbr(static_cast<std::uint32_t>(op.value));
		case encoding_char6:
			return static_cast<std::uint8_t>(decode_char6(this->stream.read(6)));
		default:
			throw error("array or blob used as an element");
		}
	}

	record bitcode_reader::read_record(const std::uint64_t id, const std::vector<abbrev>& abbrevs)
	{
		std::vector<std::uint64_t> values;
		if (id == abbrev_unabbreviated)
		{
			values.push_back(this->stream.read_vbr(6));
			const auto count = this->stream.read_vbr(6);
			if (count > this->stream.remaining_bits())
			{
				throw error("record too large");
			}

			for (auto i = 0ull; i < count; i++)
			{
				values.push_back(this->stream.read_vbr(6));
			}
		}
		else
		{
			if (id - abbrev_first_defined >= abbrevs.size())
			{
				throw error(std::format("undefined abbrev {}", id));
			}

			const auto& definition = abbrevs[id - abbrev_first_defined];
			for (auto i = 0u; i < definition.size(); i++)
			{
				const auto& op = definition[i];
				if (op.encoding == encoding_array)
				{
					if (i + 2 != definition.size())
					{
						throw error("array is not the second last abbrev op");
					}

					const auto count = this->stream.read_vbr(6);
					if (count > this->stream.remaining_bits())
					{
						throw error("array too large");
					}

					for (auto j = 0ull; j < count; j++)
					{
						values.push_back(this->read_operand(definition[i + 1]));
					}

					break;
				}

				if (op.encoding == encoding_blob)
				{
					if (i + 1 != definition.size())
					{
						throw error("blob is not the last abbrev op");
					}

					const auto count = this->stream.read_vbr(6);
					this->stream.align32();
					if (count > this->stream.remaining_bits() / 8)
					{
						throw error("blob too large");
					}

					for (auto j = 0ull; j < count; j++)
					{
						values.push_back(this->stream.read(8));
					}

					this->stream.align32();
					break;
				}

				values.push_back(this->read_operand(op));
			}
		}

		if (values.empty())
		{
			throw error("record without a code");
		}

		record entry{};
		entry.code = static_cast<std::uint32_t>(values[0]);
		entry.operands.assign(values.begin() + 1, values.end());
		return entry;
	}

	void bitcode_reader::add_record(const std::uint64_t block, const record& entry)
	{
		switch (block)
		{
		case block_module:
			if (entry.code == module_global_var || entry.code == module_function || entry.code == module_alias_old || entry.code == module_alias)
			{
				this->module.global_value_count++;
			}
			break;
		case block_type:
			this->add_type(entry);
			break;
		case block_constants:
			this->add_constant(entry);
			break;
		case block_metadata:
			this->add_metadata(entry);
			break;
		}
	}

	void bitcode_reader::add_type(const record& entry)
	{
		if (entry.code == type_code_num_entry)
		{
			return;
		}

		if (entry.code == type_code_struct_name)
		{
			this->struct_name = operand_text(entry, 0);
			return;
		}

		llvm_type type{};
		switch (entry.code)
		{
		case type_code_integer:
			type.kind = type_integer;
			type.count = operand(entry, 0);
			break;
		case type_code_half:
			type.kind = type_half;
			break;
		case type_code_float:
			type.kind = type_float;
			break;
		case type_code_double:
			type.kind = type_double;
			break;
		case type_code_pointer:
			type.kind = type_pointer;
			type.elements.push_back(operand(entry, 0));
			break;
		case type_code_array:
		case type_code_vector:
			type.kind = entry.code == type_code_array ? type_array : type_vector;
			type.count = operand(entry, 0);
			type.elements.push_back(operand(entry, 1));
			break;
		case type_code_struct_anon:
		case type_code_struct_named:
			if (entry.operands.empty())
			{
				throw error("struct type without a packed flag");
			}

			type.kind = type_struct;
			type.elements.assign(entry.operands.begin() + 1, entry.operands.end());
			if (entry.code == type_code_struct_named)
			{
				type.name = std::move(this->struct_name);
				this->struct_name.clear();
			}
			break;
		case type_code_opaque:
			type.name = std::move(this->struct_name);
			this->struct_name.clear();
			break;
		}

		this->module.types.push_back(std::move(type));
	}

	void bitcode_reader::add_constant(const record& entry)
	{
		switch (entry.code)
		{
		case constants_set_type:
			return;
		case constants_null:
			this->module.constants.push_back({true, 0});
			return;
		case constants_integer:
			this->module.constants.push_back({true, decode_signed(operand(entry, 0))});
			return;
		default:
			this->module.constants.push_back({false, 0});
		}
	}

	void bitcode_reader::add_metadata(const record& entry)
	{
		metadata_entry metadata{};
		switch (entry.code)
		{
		case metadata_code_name:
			this->metadata_name = operand_text(entry, 0);
			return;
		case metadata_code_named_node:
			this->module.named_metadata[this->metadata_name] = entry.operands;
			this->metadata_name.clear();
			return;
		case metadata_code_kind:
		case metadata_code_attachment:
			return;
		case metadata_code_strings:
			throw error("metadata strings record is not supported");
		case metadata_code_string:
			metadata.kind = metadata_string;
			metadata.text = operand_text(entry, 0);
			break;
		case metadata_code_value:
			metadata.kind = metadata_value;
			metadata.type = operand(entry, 0);
			metadata.value = operand(entry, 1);
			break;
		case metadata_code_node:
		case metadata_code_distinct_node:
			metadata.kind = metadata_node;
			metadata.operands = entry.operands;
			break;
		}

		this->module.metadata.push_back(std::move(metadata));
	}

	std::optional<std::span<const std::uint8_t>> find_bitcode(const std::uint8_t* data, const std::size_t size)
	{
		const auto header = read_struct<container_header>(data, size, 0);
		if (header.magic != part_dxbc || header.size > size)
		{
			return {};
		}

		std::optional<std::span<const std::uint8_t>> program;
		for (auto i = 0u; i < header.part_count; i++)
		{
			const auto offset = read_struct<std::uint32_t>(data, header.size, sizeof(container_header) + i * sizeof(std::uint32_t));
			const auto part = read_struct<part_header>(data, header.size, offset);
			const auto part_data = offset + sizeof(part_header);
			if (part.size > header.size - part_data || (part.fourcc != part_stat && part.fourcc != part_dxil))
			{
				continue;
			}

			const auto program_data = data + part_data;
			const auto program_info = read_struct<program_header>(program_data, part.size, 0);
			const auto bitcode_start = offsetof(program_header, bitcode_magic) + static_cast<std::size_t>(program_info.bitcode_offset);
			if (program_info.bitcode_magic != part_dxil || bitcode_start > part.size || program_info.bitcode_size > part.size - bitcode_start)
			{
				continue;
			}

			program = std::span(program_data + bitcode_start, program_info.bitcode_size);
			if (part.fourcc == part_stat)
			{
				return program;
			}
		}

		return program;
	}

	bitcode_module read_module(const std::uint8_t* data, const std::size_t size)
	{
		const auto bitcode = find_bitcode(data, size);
		if (!bitcode)
		{
			throw error("container has no program part");
		}

		return bitcode_reader(bitcode->data(), bitcode->size()).read();
	}

	std::vector<cbuffer> read_cbuffers(const std::uint8_t* data, const std::size_t size)
	{
		try
		{
			const auto module = read_module(data, size);
			const auto cbuffer_list = find_resource_list(module, resource_list_cbv);
			if (!cbuffer_list)
			{
				return {};
			}

			const auto annotations = read_struct_annotations(module);

			std::vector<cbuffer> cbuffers;
			for (auto i = 0u; i < node_at(cbuffer_list).operands.size(); i++)
			{
				cbuffers.push_back(read_cbuffer(module, annotations, node_at(node_operand(module, *cbuffer_list, i))));
			}

			return cbuffers;
		}
		catch (const std::exception&)
		{
			return {};
		}
	}

	std::vector<cbuffer> read_structured_buffers(const std::uint8_t* data, const std::size_t size)
	{
		try
		{
			const auto module = read_module(data, size);
			const auto srv_list = find_resource_list(module, resource_list_srv);
			if (!srv_list)
			{
				return {};
			}

			const auto annotations = read_struct_annotations(module);

			std::vector<cbuffer> buffers;
			for (auto i = 0u; i < node_at(srv_list).operands.size(); i++)
			{
				const auto& node = node_at(node_operand(module, *srv_list, i));
				if (integer_operand(module, node, 6) == resource_kind_structured_buffer)
				{
					buffers.push_back(read_structured_buffer(module, annotations, node));
				}
			}

			return buffers;
		}
		catch (const std::exception&)
		{
			return {};
		}
	}

	const cbuffer* find_cbuffer(const std::vector<cbuffer>& cbuffers, const std::string_view name)
	{
		for (const auto& entry : cbuffers)
		{
			if (entry.name == name)
			{
				return &entry;
			}
		}

		return nullptr;
	}
}
