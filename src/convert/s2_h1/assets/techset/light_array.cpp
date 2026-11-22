#include <std_include.hpp>

#include "../../include.hpp"
#include "light_array.hpp"

namespace zonetool::s2::converter::h1::techset
{
	namespace
	{
		using alys::shader::detail::instruction_t;
		using alys::shader::detail::operand_t;

		constexpr std::uint32_t light_records_slot = 16u;
		constexpr std::uint32_t tile_bits_slot = 23u;
		constexpr std::uint32_t z_bins_slot = 25u;

		constexpr std::uint32_t position_rows = 0u;
		constexpr std::uint32_t diffuse_rows = 4u;
		constexpr std::uint32_t physical_size_rows = 8u;
		constexpr std::uint32_t fade_offset_rows = 12u;
		constexpr std::uint32_t spot_dir_rows = 16u;
		constexpr std::uint32_t spot_factors_rows = 20u;
		constexpr std::uint32_t types_row = 24u;
		constexpr std::uint32_t count_row = 25u;
		constexpr std::uint32_t shadow_lookup_rows = 26u;
		constexpr std::uint32_t shadow_types_row = 42u;
		constexpr std::uint32_t pixel_adjust_rows = 43u;

constexpr float light_type_sun = 1.0f;

		constexpr std::uint32_t spot_flags = 0x2FF;
		constexpr std::uint32_t omni_flags = 0x3FF;

		bool reads_resource(const instruction_t& instruction, const std::uint32_t slot)
		{
			return !dxbc::is_declaration(instruction) && std::ranges::any_of(instruction.operands, [&](const operand_t& operand)
			{
				return dxbc::is_resource(operand, slot);
			});
		}

		bool reads_any_resource(const instruction_t& instruction)
		{
			return !dxbc::is_declaration(instruction) && std::ranges::any_of(instruction.operands, [](const operand_t& operand)
			{
				return operand.type == D3D10_SB_OPERAND_TYPE_RESOURCE;
			});
		}

		operand_t light_row(const std::uint32_t row, const std::uint32_t index, const std::string& swizzle)
		{
			operand_t operand = asm_::cb(1, row).select(swizzle).swz();
			auto& entry = operand.indices[1];
			entry.representation = D3D10_SB_OPERAND_INDEX_IMMEDIATE32_PLUS_RELATIVE;
			entry.value.uint32 = row;
			entry.extra_operand = std::make_shared<operand_t>(asm_::r(index).select(1).scalar());
			return operand;
		}

		operand_t component_mask(operand_t operand, const std::uint32_t component)
		{
			operand.components.mask = static_cast<std::uint8_t>(1u << component);
			return operand;
		}

		struct temps
		{
			std::uint32_t mask;
			std::uint32_t index;
			std::uint32_t scratch;
			std::optional<std::uint32_t> shadow;
			std::uint32_t work[4];
		};

		void emit_record_word(alys::shader::assembler& a, const operand_t& dest, const std::uint32_t word, const std::uint32_t base, const temps& t,
			const std::optional<std::uint32_t> single_flags)
		{
			const auto scratch = asm_::r(t.scratch);
			const auto row = [&](const std::uint32_t rows, const std::uint32_t component)
			{
				return light_row(base + rows, t.index, std::string(4, "xyzw"[component]));
			};

			const auto half2 = [&](const std::uint32_t rows)
			{
				a.f32tof16(scratch.select("xy"), light_row(base + rows, t.index, "xyxx"));
				a.ishl(scratch.select("y"), scratch.select("yyyy").swz(), dxbc::uint_literal(16));
				a.or_(dest, scratch.select("xxxx").swz(), scratch.select("yyyy").swz());
			};

			switch (word)
			{
			case 0: case 1: case 2: case 3:
				a.mov(dest, row(position_rows, word));
				break;
			case 4: case 5: case 6:
				a.mov(dest, row(spot_dir_rows, word - 4));
				break;
			case 7:
				if (single_flags)
				{
					a.mov(dest, dxbc::uint_literal(*single_flags));
					break;
				}

				a.ieq(scratch.select("xyzw"), asm_::r(t.index).select("xxxx").swz(), dxbc::uint_literals(0, 1, 2, 3));
				a.and_(scratch.select("xyzw"), scratch.select("xyzw").swz(), asm_::l(1.0f, 1.0f, 1.0f, 1.0f));
				a.dp4(scratch.select("x"), scratch.select("xyzw").swz(), asm_::cb(1, base + types_row).select("xyzw").swz());
				a.eq(scratch.select("x"), scratch.select("xxxx").swz(), asm_::l(2.0f));
				a.movc(dest, scratch.select("xxxx").swz(), dxbc::uint_literal(spot_flags), dxbc::uint_literal(omni_flags));
				break;
			case 8: case 9: case 10:
				if (!t.shadow)
				{
					a.mov(dest, row(diffuse_rows, word - 8));
					break;
				}

				a.ieq(scratch.select("xyzw"), asm_::r(t.index).select("xxxx").swz(), dxbc::uint_literals(0, 1, 2, 3));
				a.and_(scratch.select("xyzw"), scratch.select("xyzw").swz(), asm_::l(1.0f, 1.0f, 1.0f, 1.0f));
				a.dp4(scratch.select("x"), scratch.select("xyzw").swz(), asm_::r(*t.shadow).select("xyzw").swz());
				a.mul(dest, row(diffuse_rows, word - 8), scratch.select("xxxx").swz());
				break;
			case 11:
				half2(fade_offset_rows);
				break;
			case 12:
				a.mov(dest, row(physical_size_rows, 3));
				break;
			case 13: case 14: case 15:
				a.mov(dest, row(physical_size_rows, word - 13));
				break;
			case 16:
				half2(spot_factors_rows);
				break;
			case 17:
				a.f32tof16(dest, row(spot_factors_rows, 2));
				break;
			case 19:
				a.mov(dest, dxbc::uint_literal(0xFFFFFFFF));
				break;
			default:
				a.mov(dest, dxbc::uint_literal(0));
				break;
			}
		}

		void emit_spot_shadows(alys::shader::assembler& a, const temps& t, const std::uint32_t base, const std::uint32_t position_input,
			const light_array_shadow_slots& slots, const bool single)
		{
			const auto shadow = asm_::r(*t.shadow);
			const auto w0 = asm_::r(t.work[0]);
			const auto w1 = asm_::r(t.work[1]);
			const auto w2 = asm_::r(t.work[2]);
			const auto w3 = asm_::r(t.work[3]);
			a.mov(shadow.select("xyzw"), asm_::l(1.0f, 1.0f, 1.0f, 1.0f));
			for (auto light = 0u; light < 4; light++)
			{
				if (!slots.textures[light])
				{
					continue;
				}

				const auto component = "xyzw"[light];
				const auto texture = asm_::t(*slots.textures[light]).select("xyzw");
				const auto sampler = asm_::s(slots.compare_sampler);
				const auto adjust = asm_::cb(1, base + pixel_adjust_rows + light);
				if (!single)
				{
					a.eq(w0.select("x"), asm_::cb(1, base + shadow_types_row).select(std::string(4, component)).swz(), asm_::l(2.0f));
					a.if_nz(w0.x());
				}

				a.mov(w1.select("xyz"), asm_::v(position_input).select("xyzx").swz());
				a.mov(w1.select("w"), asm_::l(1.0f));
				for (auto row = 0u; row < 4; row++)
				{
					a.dp4(w0.select(std::string(1, "xyzw"[row])), w1.select("xyzw").swz(), asm_::cb(1, base + shadow_lookup_rows + light * 4 + row).select("xyzw").swz());
				}

				a.div(w0.select("xyz"), w0.select("xyzx").swz(), w0.select("wwww").swz());
				a.mul(w0.select("z"), w0.select("zzzz").swz(), asm_::l(1.005f)); // reverse z bias against acne

				a.div(w1.select("xy"), w0.select("xyxx").swz(), adjust.select("xyxx").swz());
				a.mad(w1.select("xy"), w1.select("xyxx").swz(), asm_::l(2.0f, 2.0f, 0.0f, 0.0f), asm_::l(-0.5f, -0.5f, 0.0f, 0.0f));
				a.frc(w1.select("zw"), w1.select("xxxy").swz());
				a.add(w1.select("xy"), w1.select("xyxx").swz(), w1.select("zwzz").swz().neg());
				a.mul(w1.select("xy"), w1.select("xyxx").swz(), adjust.select("xyxx").swz());
				a.mul(w1.select("xy"), w1.select("xyxx").swz(), asm_::l(0.5f, 0.5f, 0.0f, 0.0f));

				a.gather4_c_indexable(w2.select("xyzw"), w1.select("xyxx").swz(), texture.swz(), sampler, w0.z());
				a.mad(w3.select("xz"), w1.select("zzww").swz(), asm_::l(-1.0f, 0.0f, -1.0f, 0.0f), asm_::l(1.0f, 0.0f, 1.0f, 0.0f));
				a.mov(w3.select("y"), asm_::l(1.0f));
				a.mul(w3.select("w"), w3.select("xxxx").swz(), w3.select("zzzz").swz());
				a.dp4(w0.select("x"), w2.select("xyzw").swz(), w3.select("xyzw").swz());

				a.mad(w2.select("xy"), adjust.select("xyxx").swz(), asm_::l(1.0f, 0.0f, 0.0f, 0.0f), w1.select("xyxx").swz());
				a.gather4_c_indexable(w2.select("xyzw"), w2.select("xyxx").swz(), texture.swz(), sampler, w0.z());
				a.add(w3.select("w"), w1.select("wwww").swz().neg(), asm_::l(1.0f));
				a.mov(w3.select("xy"), w1.select("zzzz").swz());
				a.mov(w3.select("x"), asm_::l(1.0f));
				a.mul(w3.select("z"), w1.select("zzzz").swz(), w3.select("wwww").swz());
				a.dp4(w0.select("y"), w2.select("xyzw").swz(), w3.select("xyzw").swz());
				a.add(w0.select("x"), w0.select("xxxx").swz(), w0.select("yyyy").swz());

				a.mad(w2.select("xy"), adjust.select("xyxx").swz(), asm_::l(0.0f, 1.0f, 0.0f, 0.0f), w1.select("xyxx").swz());
				a.gather4_c_indexable(w2.select("xyzw"), w2.select("xyxx").swz(), texture.swz(), sampler, w0.z());
				a.add(w3.select("w"), w1.select("zzzz").swz().neg(), asm_::l(1.0f));
				a.mul(w3.select("x"), w3.select("wwww").swz(), w1.select("wwww").swz());
				a.mov(w3.select("y"), w1.select("wwww").swz());
				a.mov(w3.select("z"), asm_::l(1.0f));
				a.dp4(w0.select("y"), w2.select("xyzw").swz(), w3.select("xyzw").swz());
				a.add(w0.select("x"), w0.select("xxxx").swz(), w0.select("yyyy").swz());

				a.add(w2.select("xy"), adjust.select("xyxx").swz(), w1.select("xyxx").swz());
				a.gather4_c_indexable(w2.select("xyzw"), w2.select("xyxx").swz(), texture.swz(), sampler, w0.z());
				a.mov(w3.select("x"), w1.select("wwww").swz());
				a.mul(w3.select("y"), w1.select("zzzz").swz(), w1.select("wwww").swz());
				a.mov(w3.select("z"), w1.select("zzzz").swz());
				a.mov(w3.select("w"), asm_::l(1.0f));
				a.dp4(w0.select("y"), w2.select("xyzw").swz(), w3.select("xyzw").swz());
				a.add(w0.select("x"), w0.select("xxxx").swz(), w0.select("yyyy").swz());

				a.mad(w0.select("x"), w0.select("xxxx").swz(), asm_::l(1.0f / 9.0f), asm_::l(-1.0f));
				a.mad(shadow.select(std::string(1, component)), asm_::cb(1, base + spot_factors_rows + light).select("wwww").swz(), w0.select("xxxx").swz(), asm_::l(1.0f));
				if (!single)
				{
					a.endif();
				}
			}
		}

		std::uint32_t free_slot(const instruction_list& instructions, const std::uint32_t type, std::set<std::uint32_t> taken)
		{
			for (const auto& instruction : instructions)
			{
				if (dxbc::is_declaration(instruction) && !instruction.operands.empty() && instruction.operands[0].type == type)
				{
					taken.insert(instruction.operands[0].indices[0].value.uint32);
				}
			}

			auto slot = 0u;
			while (taken.contains(slot))
			{
				slot++;
			}

			return slot;
		}

		std::optional<std::uint32_t> find_position_input(alys::shader::shader_object& shader)
		{
			try
			{
				return dxbc::find_signature_element(shader, alys::shader::chunk_isgn, [](const dxbc::signature_element& element)
				{
					return element.name == "TEXCOORD" && element.semantic_index == 5;
				}, "TEXCOORD").register_;
			}
			catch (const std::exception&)
			{
				return {};
			}
		}

		std::vector<std::pair<std::size_t, std::size_t>> outer_loops(const instruction_list& instructions)
		{
			std::vector<std::pair<std::size_t, std::size_t>> result;
			std::vector<std::size_t> open;
			for (auto i = 0u; i < instructions.size(); i++)
			{
				if (instructions[i].opcode.type == D3D10_SB_OPCODE_LOOP)
				{
					open.push_back(i);
				}
				else if (instructions[i].opcode.type == D3D10_SB_OPCODE_ENDLOOP && !open.empty())
				{
					const auto begin = open.back();
					open.pop_back();
					if (open.empty())
					{
						result.emplace_back(begin, i);
					}
				}
			}

			return result;
		}

		void replace_grid_params(operand_t& operand, const std::pair<std::uint32_t, std::uint32_t>& grid)
		{
			if (operand.type == D3D10_SB_OPERAND_TYPE_CONSTANT_BUFFER && operand.indices[0].value.uint32 == grid.first
				&& operand.indices[1].representation == D3D10_SB_OPERAND_INDEX_IMMEDIATE32 && operand.indices[1].value.uint32 == grid.second)
			{
				operand = dxbc::uint_literals(0, 0, 0, 0);
			}
		}

		void declare_light_rows(instruction_list& instructions, const std::uint32_t rows)
		{
			auto declaration = std::ranges::find_if(instructions, [](const instruction_t& instruction)
			{
				return instruction.opcode.type == D3D10_SB_OPCODE_DCL_CONSTANT_BUFFER && instruction.operands[0].indices[0].value.uint32 == 1u;
			});

			if (declaration == instructions.end())
			{
				const auto first = std::ranges::find_if(instructions, [](const instruction_t& instruction)
				{
					return instruction.opcode.type == D3D10_SB_OPCODE_DCL_CONSTANT_BUFFER;
				});

				if (first == instructions.end())
				{
					throw std::runtime_error("light array shader has no constant buffer");
				}

				auto copy = *first;
				copy.operands[0].indices[0].value.uint32 = 1u;
				copy.operands[0].indices[1].value.uint32 = 0u;
				declaration = instructions.insert(first, copy);
			}

			auto& size = declaration->operands[0].indices[1].value.uint32;
			size = std::max(size, rows);
			declaration->opcode.controls |= D3D10_SB_CONSTANT_BUFFER_DYNAMIC_INDEXED;
		}
	}

	const std::vector<light_array_const>& light_array_object_consts(const light_source_t source)
	{
		static const auto array_consts = []
		{
			constexpr std::int32_t lookup_matrices[]
			{
				zonetool::h1::CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX1,
				zonetool::h1::CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX2, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX3,
			};

			std::vector<light_array_const> result;
			for (auto light = 0u; light < 4; light++)
			{
				const auto index = static_cast<std::int32_t>(light);
				result.push_back({position_rows + light, zonetool::h1::CONST_SRC_CODE_LIGHT_POSITION_DB_ARRAY_0 + index, 1});
				result.push_back({diffuse_rows + light, zonetool::h1::CONST_SRC_CODE_LIGHT_DIFFUSE_DB_ARRAY_0 + index, 1});
				result.push_back({physical_size_rows + light, zonetool::h1::CONST_SRC_CODE_LIGHT_PHYSICALSIZE_DB_ARRAY_0 + index, 1});
				result.push_back({fade_offset_rows + light, zonetool::h1::CONST_SRC_CODE_LIGHT_FADEOFFSET_DB_ARRAY_0 + index, 1});
				result.push_back({spot_dir_rows + light, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTDIR_DB_ARRAY_0 + index, 1});
				result.push_back({spot_factors_rows + light, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTFACTORS_DB_ARRAY_0 + index, 1});
				result.push_back({shadow_lookup_rows + light * 4, lookup_matrices[light], 4});
				result.push_back({pixel_adjust_rows + light, zonetool::h1::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST_ARRAY_0 + index, 1});
			}

			result.push_back({types_row, zonetool::h1::CONST_SRC_CODE_LIGHT_DYN_TYPES, 1});
			result.push_back({count_row, zonetool::h1::CONST_SRC_CODE_LIGHT_DYN_COUNT, 1});
			result.push_back({shadow_types_row, zonetool::h1::CONST_SRC_CODE_LIGHT_DYN_SHADOW_TYPES, 1});
			std::ranges::sort(result, {}, &light_array_const::row);
			return result;
		}();

		static const std::vector<light_array_const> single_consts
		{
			{position_rows, zonetool::h1::CONST_SRC_CODE_LIGHT_POSITION, 1},
			{diffuse_rows, zonetool::h1::CONST_SRC_CODE_LIGHT_DIFFUSE, 1},
			{physical_size_rows, zonetool::h1::CONST_SRC_CODE_LIGHT_PHYSICALSIZE, 1},
			{fade_offset_rows, zonetool::h1::CONST_SRC_CODE_LIGHT_FADEOFFSET, 1},
			{spot_dir_rows, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTDIR, 1},
			{spot_factors_rows, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTFACTORS, 1},
			{shadow_lookup_rows, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX, 4},
			{pixel_adjust_rows, zonetool::h1::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST, 1},
		};

		return source == LIGHT_SOURCE_ARRAY ? array_consts : single_consts;
	}

	light_array_result feed_light_array(alys::shader::shader_object& shader, instruction_list& instructions, const light_array_request& request)
	{
		const auto reads = [&](const std::uint32_t slot)
		{
			return std::ranges::any_of(instructions, [&](const instruction_t& instruction)
			{
				return reads_resource(instruction, slot);
			});
		};

		if (!reads(light_records_slot) || !reads(tile_bits_slot) || !reads(z_bins_slot))
		{
			return {};
		}

		const auto single = request.source != LIGHT_SOURCE_ARRAY;
		const auto single_flags = request.source == LIGHT_SOURCE_OMNI ? std::optional(omni_flags) : single ? std::optional(spot_flags) : std::nullopt;
		std::optional<light_array_shadow_slots> shadows;
		const auto position_input = find_position_input(shader);
		if (position_input && request.source != LIGHT_SOURCE_SPOT && request.source != LIGHT_SOURCE_OMNI)
		{
			light_array_shadow_slots slots{};
			std::set<std::uint32_t> textures;
			for (auto light = request.sun_first ? 1u : 0u; light < (single ? 1u : 4u); light++)
			{
				const auto slot = free_slot(instructions, D3D10_SB_OPERAND_TYPE_RESOURCE, textures);
				textures.insert(slot);
				slots.textures[light] = slot;
			}

			slots.compare_sampler = free_slot(instructions, D3D10_SB_OPERAND_TYPE_SAMPLER, request.reserved_samplers);
			if (slots.compare_sampler < 16)
			{
				shadows = slots;
			}
		}

		auto temps_declaration = std::ranges::find_if(instructions, [](const instruction_t& instruction)
		{
			return instruction.opcode.type == D3D10_SB_OPCODE_DCL_TEMPS;
		});

		if (temps_declaration == instructions.end())
		{
			throw std::runtime_error("light array shader declares no temps");
		}

		const auto first_temp = temps_declaration->operands[0].custom.u.value;
		temps_declaration->operands[0].custom.u.value = first_temp + (shadows ? 8 : 3);
		temps t{first_temp, first_temp + 1, first_temp + 2};
		if (shadows)
		{
			t.shadow = first_temp + 3;
			t.work[0] = first_temp + 4;
			t.work[1] = first_temp + 5;
			t.work[2] = first_temp + 6;
			t.work[3] = first_temp + 7;
		}

		std::vector<bool> in_loop(instructions.size());
		for (const auto& [begin, end] : outer_loops(instructions))
		{
			const auto lights = std::any_of(instructions.begin() + begin, instructions.begin() + end, [](const instruction_t& instruction)
			{
				return reads_resource(instruction, light_records_slot);
			});

			for (auto i = begin; lights && i <= end; i++)
			{
				in_loop[i] = true;
			}
		}

		const auto base = request.object_row;
		instruction_list result;
		auto prologue = false;
		for (auto i = 0u; i < instructions.size(); i++)
		{
			auto& instruction = instructions[i];
			if (dxbc::is_declaration(instruction))
			{
				result.emplace_back(std::move(instruction));
				continue;
			}

			alys::shader::shader_object scratch;
			auto a = scratch.get_assembler();
			if (!prologue)
			{
				const auto mask = asm_::r(t.mask);
				const auto other = asm_::r(t.scratch);
				if (single)
				{
					a.mov(mask.select("x"), dxbc::uint_literal(1));
				}
				else
				{
					a.lt(mask.select("xyzw"), asm_::l(0.0f, 1.0f, 2.0f, 3.0f), asm_::cb(1, base + count_row).select("xxxx").swz());
					a.ne(other.select("xyzw"), asm_::cb(1, base + types_row).select("xyzw").swz(), asm_::l(light_type_sun, light_type_sun, light_type_sun, light_type_sun));
					a.and_(mask.select("xyzw"), mask.select("xyzw").swz(), other.select("xyzw").swz());
					a.and_(mask.select("xyzw"), mask.select("xyzw").swz(), dxbc::uint_literals(1, 2, 4, 8));
					a.or_(mask.select("xy"), mask.select("xyxx").swz(), mask.select("zwzz").swz());
					a.or_(mask.select("x"), mask.select("xxxx").swz(), mask.select("yyyy").swz());
				}

				if (shadows)
				{
					emit_spot_shadows(a, t, base, *position_input, *shadows, single);
				}

				prologue = true;
			}

			if (request.grid_params)
			{
				for (auto& operand : instruction.operands)
				{
					replace_grid_params(operand, *request.grid_params);
				}
			}

			const auto& dest = instruction.operands.empty() ? operand_t{} : instruction.operands[0];
			if (reads_resource(instruction, light_records_slot))
			{
				if (instruction.opcode.type != D3D11_SB_OPCODE_LD_STRUCTURED)
				{
					throw std::runtime_error("light records are read without ld_structured");
				}

				const auto& offset = instruction.operands[2];
				const auto& resource = instruction.operands[3];
				if (offset.type != D3D10_SB_OPERAND_TYPE_IMMEDIATE32)
				{
					throw std::runtime_error("light record offset is not a literal");
				}

				a.mov(asm_::r(t.index).select("x"), instruction.operands[1]);
				for (auto component = 0u; component < 4; component++)
				{
					if (dest.components.mask & (1u << component))
					{
						emit_record_word(a, component_mask(dest, component), offset.immediate_values[0].uint32 / 4 + resource.components.names[component], base, t, single_flags);
					}
				}
			}
			else if (reads_resource(instruction, tile_bits_slot))
			{
				a.mov(dest, asm_::r(t.mask).select("xxxx").swz());
			}
			else if (reads_resource(instruction, z_bins_slot))
			{
				a.mov(dest, dxbc::uint_literals(3, 3, 3, 3));
			}
			else if (in_loop[i] && reads_any_resource(instruction))
			{
				if (dest.type != D3D10_SB_OPERAND_TYPE_TEMP)
				{
					throw std::runtime_error("light loop resource read without a temp destination");
				}

				a.mov(dest, dxbc::uint_literals(0, 0, 0, 0));
			}
			else
			{
				a(instruction);
			}

			auto& code = scratch.get_instructions();
			std::move(code.begin(), code.end(), std::back_inserter(result));
		}

		if (shadows)
		{
			alys::shader::shader_object scratch;
			auto a = scratch.get_assembler();
			for (const auto& texture : shadows->textures)
			{
				if (texture)
				{
					a.dcl_resource(asm_::t(*texture), asm_::texture2d, dxbc::float4_type);
				}
			}

			a.dcl_sampler(asm_::s(shadows->compare_sampler), D3D10_SB_SAMPLER_MODE_COMPARISON);
			const auto first_code = std::ranges::find_if(result, [](const instruction_t& instruction)
			{
				return !dxbc::is_declaration(instruction);
			});

			auto& code = scratch.get_instructions();
			result.insert(first_code, code.begin(), code.end());
		}

		instructions = std::move(result);
		declare_light_rows(instructions, base + light_array_object_rows);
		return {true, shadows};
	}
}
