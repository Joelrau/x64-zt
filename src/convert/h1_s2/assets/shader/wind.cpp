#include <std_include.hpp>

#include "../../include.hpp"
#include "dxbc.hpp"
#include "layout.hpp"
#include "wind.hpp"

namespace zonetool::h1::converter::s2::shader
{
	wind_inputs add_wind_inputs(alys::shader::shader_object::signature& inputs)
	{
		const auto add = [&](const char* name, const std::uint32_t semantic_index)
		{
			signature_element element{};
			element.name = name;
			element.semantic_index = semantic_index;
			element.system_value_type = alys::shader::NONE;
			element.component_type = alys::shader::format_float;
			element.register_ = std::ranges::max(inputs, {}, &signature_element::register_).register_ + 1;
			element.mask = 0xF;
			element.rw_mask = 0x7;
			inputs.emplace_back(element);
			return element.register_;
		};

		const auto normal = std::ranges::find_if(inputs, [](const signature_element& element)
		{
			return element.name == "NORMAL" && element.semantic_index == 0;
		});

		if (normal != inputs.end() && (normal->rw_mask & 0x7) != 0x7)
		{
			throw std::runtime_error("reactive motion vertex shader reads part of the normal");
		}

		wind_inputs result{};
		result.declare_normal = normal == inputs.end();
		result.normal = result.declare_normal ? add("NORMAL", 0) : normal->register_;
		result.color = add("COLOR", 1);
		return result;
	}

	std::unordered_map<std::uint32_t, std::uint32_t> order_wind_inputs(alys::shader::shader_object::signature& inputs, const bool added_normal)
	{
		std::vector<signature_element*> order;
		std::vector<signature_element*> system_values;
		signature_element* color = nullptr;
		signature_element* normal = nullptr;
		for (auto& element : inputs)
		{
			if (element.system_value_type != alys::shader::NONE)
			{
				system_values.emplace_back(&element);
			}
			else if (element.name == "COLOR" && element.semantic_index == 1)
			{
				color = &element;
			}
			else if (added_normal && element.name == "NORMAL")
			{
				normal = &element;
			}
			else
			{
				order.emplace_back(&element);
			}
		}

		const auto by_register = [](const signature_element* element) { return element->register_; };
		std::ranges::sort(order, {}, by_register);
		std::ranges::sort(system_values, {}, by_register);

		const auto find = [&](const char* name, const std::uint32_t index)
		{
			return std::ranges::find_if(order, [&](const signature_element* element)
			{
				return element->name == name && element->semantic_index == index;
			});
		};

		if (color)
		{
			const auto color0 = find("COLOR", 0);
			order.insert(color0 != order.end() ? color0 + 1 : order.begin() + (order.empty() ? 0 : 1), color);
		}

		if (normal)
		{
			const auto texcoord = find("TEXCOORD", 0);
			order.insert(texcoord != order.end() ? texcoord + 1 : order.end(), normal);
		}

		order.insert(order.end(), system_values.begin(), system_values.end());

		std::unordered_map<std::uint32_t, std::uint32_t> registers;
		for (auto i = 0u; i < order.size(); i++)
		{
			if (!registers.emplace(order[i]->register_, i).second)
			{
				throw std::runtime_error("vertex shader inputs share a register");
			}
		}

		for (auto& element : inputs)
		{
			element.register_ = registers.at(element.register_);
		}

		std::ranges::sort(inputs, {}, &signature_element::register_);
		return registers;
	}

	void remap_input_registers(alys::shader::detail::operand_t& operand, const std::unordered_map<std::uint32_t, std::uint32_t>& registers)
	{
		for (auto& index : operand.indices)
		{
			if (index.extra_operand)
			{
				remap_input_registers(*index.extra_operand, registers);
			}
		}

		if (operand.type == D3D10_SB_OPERAND_TYPE_INPUT)
		{
			operand.indices[0].value.uint32 = registers.at(operand.indices[0].value.uint32);
		}
	}

	bool reads_reactive_motion(const alys::shader::detail::instruction_t& instruction)
	{
		return std::ranges::any_of(instruction.operands, [](const alys::shader::detail::operand_t& operand)
		{
			return operand.type == D3D10_SB_OPERAND_TYPE_CONSTANT_BUFFER && operand.indices[0].value.uint32 == h1_reactive_motion_buffer;
		});
	}

	void emit_foliage_wind(alys::shader::assembler& a, const alys::shader::detail::operand_t& dest, const std::uint32_t position, const wind_inputs& inputs,
		const foliage_wind& wind, const std::array<alys::shader::detail::operand_t, 3>& world, const std::uint32_t temp)
	{
		const auto flutter = temp;
		const auto work = temp + 1;
		const auto bent = temp + 2;
		const auto rest = temp + 3;
		const auto time = [&](const char* swizzle) { return asm_::cb(s2_stable_constant_buffer, wind.time_row).select(swizzle); };
		const auto strength = [&](const char* swizzle) { return asm_::cb(s2_stable_constant_buffer, wind.wind_row).select(swizzle); };
		const auto params = [&](const char* swizzle) { return asm_::cb(s2_stable_constant_buffer, wind.params_row).select(swizzle); };
		const auto all = [](const std::uint32_t reg) { return asm_::r(reg).select("xyzw"); };

		a.push_controls(D3D11_SB_INSTRUCTION_PRECISE_VALUES_MASK >> 11);
		a.mul(asm_::r(flutter).zw(), asm_::v(inputs.color).select("yyyy"), asm_::l(0.0f, 0.0f, 3.14f, 3.14f));
		a.add(asm_::r(flutter).xy(), asm_::r(flutter).select("wwww"), asm_::r(flutter).select("wwww"));
		a.add(all(flutter), all(flutter), time("wwww"));
		a.mul(all(flutter), all(flutter), asm_::l(1.975f, 0.793f, 0.375f, 0.193f));
		a.frc(all(flutter), all(flutter));
		a.mul(all(flutter), all(flutter), asm_::l(2.0f, 2.0f, 2.0f, 2.0f));
		a.add(all(flutter), all(flutter), asm_::l(-1.0f, -1.0f, -1.0f, -1.0f));
		a.mul(all(flutter), all(flutter), strength("xxxx"));
		a.mul(all(flutter), all(flutter), params("xxxx"));
		a.add(all(flutter), all(flutter), asm_::l(0.5f, 0.5f, 0.5f, 0.5f));
		a.frc(all(flutter), all(flutter));
		a.mul(all(flutter), all(flutter), asm_::l(2.0f, 2.0f, 2.0f, 2.0f));
		a.add(all(flutter), all(flutter), asm_::l(-1.0f, -1.0f, -1.0f, -1.0f));
		a.mul(all(work), all(flutter).abs(), asm_::l(2.0f, 2.0f, 2.0f, 2.0f));
		a.add(all(work), all(work).neg(), asm_::l(3.0f, 3.0f, 3.0f, 3.0f));
		a.mul(all(flutter), all(flutter).abs(), all(flutter).abs());
		a.mul(all(flutter), all(flutter), all(work));
		a.add(asm_::r(flutter).xyz(), asm_::r(flutter).select("yywy"), asm_::r(flutter).select("xxzx"));

		a.mul(asm_::r(work).xyz(), asm_::v(inputs.normal).select("xyzx"), asm_::l(2.0f, 2.0f, 2.0f, 0.0f));
		a.add(asm_::r(work).xyz(), asm_::r(work).select("xyzx"), asm_::l(-1.0f, -1.0f, -1.0f, 0.0f));
		a.dp3(asm_::r(work).w(), asm_::r(work).select("xyzx"), asm_::r(work).select("xyzx"));
		a.rsq(asm_::r(work).w(), asm_::r(work).w());
		a.mul(asm_::r(work).xy(), asm_::r(work).select("wwww"), asm_::r(work).select("xyxx"));
		a.mul(asm_::r(bent).zw(), asm_::v(inputs.color).select("xxxz"), params("yyyz"));
		a.mul(asm_::r(bent).xy(), asm_::r(work).select("xyxx"), asm_::r(bent).select("zzzz"));
		a.mul(asm_::r(flutter).xyz(), asm_::r(flutter).select("xyzx"), asm_::r(bent).select("xywx"));
		a.add(asm_::r(flutter).xyz(), asm_::r(flutter).select("xyzx"), asm_::v(position).select("xyzx"));

		for (auto row = 0u; row < 3; row++)
		{
			a.dp3(asm_::r(bent).select(alys::shader::component_x + row), asm_::r(flutter).select("xyzx"), world[row]);
			a.dp3(asm_::r(rest).select(alys::shader::component_x + row), asm_::v(position).select("xyzx"), world[row]);
		}

		a.mul(asm_::r(work).x(), time("wwww"), strength("yyyy"));
		a.sincos(asm_::r(work).x(), asm_::r(work).y(), asm_::r(work).select("xxxx"));
		a.add(asm_::r(work).x(), asm_::r(work).x(), asm_::l(0.1f));
		a.add(asm_::r(work).x(), asm_::r(work).x(), asm_::l(1.0f));
		a.mul(asm_::r(work).y(), strength("xxxx"), asm_::l(0.001f));
		a.mul(asm_::r(work).y(), asm_::r(work).y(), params("wwww"));
		a.mul(asm_::r(work).x(), asm_::r(work).x(), asm_::r(work).y());
		a.max(asm_::r(work).y(), asm_::r(bent).z(), asm_::l(0.0f));
		a.mul(asm_::r(work).x(), asm_::r(work).x(), asm_::r(work).y());
		a.add(asm_::r(work).x(), asm_::r(work).x(), asm_::l(1.0f));
		a.mul(asm_::r(work).x(), asm_::r(work).x(), asm_::r(work).x());
		a.mul(asm_::r(work).y(), asm_::r(work).x(), asm_::r(work).x());
		a.add(asm_::r(work).y(), asm_::r(work).x().neg(), asm_::r(work).y());
		a.dp3(asm_::r(work).z(), asm_::r(bent).select("xyzx"), asm_::r(bent).select("xyzx"));
		a.sqrt(asm_::r(work).z(), asm_::r(work).z());
		a.mul(asm_::r(work).xy(), asm_::r(work).select("yyyy"), strength("zwzz"));
		a.add(asm_::r(bent).xy(), asm_::r(work).select("xyxx"), asm_::r(bent).select("xyxx"));

		a.dp3(asm_::r(work).w(), asm_::r(bent).select("xyzx"), asm_::r(bent).select("xyzx"));
		a.max(asm_::r(work).w(), asm_::r(work).w(), asm_::l(1e-12f));
		a.rsq(asm_::r(work).w(), asm_::r(work).w());
		a.mul(asm_::r(work).w(), asm_::r(work).w(), asm_::r(work).z());
		a.mul(asm_::r(bent).xyz(), asm_::r(bent).select("xyzx"), asm_::r(work).select("wwww"));
		a.add(dest, asm_::r(bent).select("xyzx"), asm_::r(rest).select("xyzx").neg());
		a.pop_controls();
	}
}
