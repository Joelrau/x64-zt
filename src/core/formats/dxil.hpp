#pragma once

namespace dxil
{
	struct cbuffer_field
	{
		std::string name;
		std::string type;
		std::uint32_t offset;
		std::uint32_t size;
		std::uint32_t elements;
	};

	struct cbuffer
	{
		std::string name;
		std::uint32_t size;
		std::vector<cbuffer_field> fields;

		const cbuffer_field* find(std::string_view field_name) const;
	};

	enum part_fourcc : std::uint32_t
	{
		part_dxbc = 0x43425844,
		part_dxil = 0x4C495844,
		part_stat = 0x54415453,
	};

	struct container_header
	{
		std::uint32_t magic;
		std::uint8_t digest[16];
		std::uint16_t major_version;
		std::uint16_t minor_version;
		std::uint32_t size;
		std::uint32_t part_count;
	};

	struct part_header
	{
		std::uint32_t fourcc;
		std::uint32_t size;
	};

	struct program_header
	{
		std::uint32_t program_version;
		std::uint32_t size_in_words;
		std::uint32_t bitcode_magic;
		std::uint32_t dxil_version;
		std::uint32_t bitcode_offset;
		std::uint32_t bitcode_size;
	};

	enum block_id : std::uint64_t
	{
		block_blockinfo = 0,
		block_module = 8,
		block_constants = 11,
		block_metadata = 15,
		block_type = 17,
	};

	enum abbrev_id : std::uint64_t
	{
		abbrev_end_block = 0,
		abbrev_enter_subblock = 1,
		abbrev_define = 2,
		abbrev_unabbreviated = 3,
		abbrev_first_defined = 4,
	};

	enum abbrev_encoding : std::uint32_t
	{
		encoding_literal = 0,
		encoding_fixed = 1,
		encoding_vbr = 2,
		encoding_array = 3,
		encoding_char6 = 4,
		encoding_blob = 5,
	};

	enum blockinfo_code : std::uint32_t
	{
		blockinfo_set_block_id = 1,
	};

	enum module_code : std::uint32_t
	{
		module_global_var = 7,
		module_function = 8,
		module_alias_old = 9,
		module_alias = 14,
	};

	enum type_code : std::uint32_t
	{
		type_code_num_entry = 1,
		type_code_float = 3,
		type_code_double = 4,
		type_code_opaque = 6,
		type_code_integer = 7,
		type_code_pointer = 8,
		type_code_half = 10,
		type_code_array = 11,
		type_code_vector = 12,
		type_code_struct_anon = 18,
		type_code_struct_name = 19,
		type_code_struct_named = 20,
	};

	enum constants_code : std::uint32_t
	{
		constants_set_type = 1,
		constants_null = 2,
		constants_integer = 4,
	};

	enum metadata_code : std::uint32_t
	{
		metadata_code_string = 1,
		metadata_code_value = 2,
		metadata_code_node = 3,
		metadata_code_name = 4,
		metadata_code_distinct_node = 5,
		metadata_code_kind = 6,
		metadata_code_named_node = 10,
		metadata_code_attachment = 11,
		metadata_code_strings = 35,
	};

	enum field_tag : std::int64_t
	{
		field_tag_matrix = 2,
		field_tag_cbuffer_offset = 3,
		field_tag_name = 6,
		field_tag_component_type = 7,
	};

	enum component_type : std::int64_t
	{
		component_invalid = 0,
		component_i1 = 1,
		component_i16 = 2,
		component_u16 = 3,
		component_i32 = 4,
		component_u32 = 5,
		component_i64 = 6,
		component_u64 = 7,
		component_f16 = 8,
		component_f32 = 9,
		component_f64 = 10,
	};

	enum resource_list : std::size_t
	{
		resource_list_srv = 0,
		resource_list_cbv = 2,
	};

	enum resource_kind : std::int64_t
	{
		resource_kind_structured_buffer = 12,
	};

	enum resource_property : std::int64_t
	{
		resource_property_element_stride = 1,
	};

	enum matrix_orientation : std::int64_t
	{
		matrix_row_major = 1,
		matrix_column_major = 2,
	};

	struct abbrev_op
	{
		abbrev_encoding encoding;
		std::uint64_t value;
	};

	using abbrev = std::vector<abbrev_op>;

	struct record
	{
		std::uint32_t code;
		std::vector<std::uint64_t> operands;
	};

	enum llvm_type_kind
	{
		type_other,
		type_integer,
		type_half,
		type_float,
		type_double,
		type_pointer,
		type_array,
		type_vector,
		type_struct,
	};

	struct llvm_type
	{
		llvm_type_kind kind;
		std::string name;
		std::uint64_t count;
		std::vector<std::uint64_t> elements;
	};

	struct llvm_constant
	{
		bool is_integer;
		std::int64_t value;
	};

	enum metadata_kind
	{
		metadata_other,
		metadata_string,
		metadata_value,
		metadata_node,
	};

	struct metadata_entry
	{
		metadata_kind kind;
		std::string text;
		std::uint64_t type;
		std::uint64_t value;
		std::vector<std::uint64_t> operands;
	};

	struct bitcode_module
	{
		std::vector<llvm_type> types;
		std::vector<llvm_constant> constants;
		std::uint64_t global_value_count;
		std::vector<metadata_entry> metadata;
		std::unordered_map<std::string, std::vector<std::uint64_t>> named_metadata;
	};

	using struct_annotations = std::unordered_map<std::uint64_t, const metadata_entry*>;

	struct field_walk
	{
		const bitcode_module& module;
		const struct_annotations& annotations;
		std::vector<cbuffer_field>& fields;
		std::uint32_t visits;
		bool packed;
	};

	struct field_annotation
	{
		std::string name;
		std::uint32_t offset;
		std::int64_t component;
		std::uint32_t rows;
		std::uint32_t columns;
		std::int64_t orientation;
	};

	class bitstream_reader
	{
	public:
		bitstream_reader(const std::uint8_t* data, std::size_t size);

		std::uint64_t read(std::uint32_t width);
		std::uint64_t read_vbr(std::uint32_t width);
		void align32();
		void skip_words(std::uint64_t count);
		std::uint64_t position() const;
		std::uint64_t remaining_bits() const;

	private:
		const std::uint8_t* data;
		std::uint64_t bit_count;
		std::uint64_t bit_position;
	};

	class bitcode_reader
	{
	public:
		bitcode_reader(const std::uint8_t* data, std::size_t size);

		bitcode_module read();

	private:
		void read_block(std::uint64_t id, std::uint32_t abbrev_width, std::uint32_t depth);
		void enter_block(std::uint32_t depth);
		abbrev read_abbrev_definition();
		record read_record(std::uint64_t id, const std::vector<abbrev>& abbrevs);
		std::uint64_t read_operand(const abbrev_op& op);
		void add_record(std::uint64_t block, const record& entry);
		void add_type(const record& entry);
		void add_constant(const record& entry);
		void add_metadata(const record& entry);

		bitstream_reader stream;
		std::unordered_map<std::uint64_t, std::vector<abbrev>> block_abbrevs;
		std::optional<std::uint64_t> blockinfo_target;
		std::string struct_name;
		std::string metadata_name;
		bool module_read = false;
		bitcode_module module{};
	};

	std::optional<std::span<const std::uint8_t>> find_bitcode(const std::uint8_t* data, std::size_t size);
	bitcode_module read_module(const std::uint8_t* data, std::size_t size);
	std::vector<cbuffer> read_cbuffers(const std::uint8_t* data, std::size_t size);
	std::vector<cbuffer> read_structured_buffers(const std::uint8_t* data, std::size_t size);
	const cbuffer* find_cbuffer(const std::vector<cbuffer>& cbuffers, std::string_view name);
}
