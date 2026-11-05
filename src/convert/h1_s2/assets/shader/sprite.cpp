#include <std_include.hpp>

#include "../../include.hpp"
#include "dxbc.hpp"
#include "sprite.hpp"

namespace zonetool::h1::converter::s2::shader
{
	namespace
	{
		std::optional<sprite_input> find_sprite_input(const signature_element& element)
		{
			if ((element.name == "POSITION" || element.name == "SV_POSITION") && element.semantic_index == 0)
			{
				return sprite_input::position;
			}

			if (element.name == "COLOR" && element.semantic_index == 0)
			{
				return sprite_input::color;
			}

			if (element.name == "TEXCOORD" && element.semantic_index == 0)
			{
				return sprite_input::texcoord;
			}

			if (element.name == "NORMAL" && element.semantic_index == 0)
			{
				return sprite_input::normal;
			}

			if (element.name == "TEXCOORD" && element.semantic_index == 1)
			{
				return sprite_input::atlas_step;
			}

			return {};
		}

		void emit_unpack_unorm(alys::shader::assembler& a, const std::uint32_t dest, const alys::shader::detail::operand_t& packed)
		{
			a.ushr(asm_::r(dest).xyz(), packed, uint_literals(0, 10, 20, 0));
			a.and_(asm_::r(dest).xyz(), asm_::r(dest).select("xyzx"), uint_literal(1023));
			a.utof(asm_::r(dest).xyz(), asm_::r(dest).select("xyzx"));
			a.mul(asm_::r(dest).xyz(), asm_::r(dest).select("xyzx"), asm_::l(1.0f / 1023.0f));
		}

		void emit_encode_atlas_step(alys::shader::assembler& a, const std::uint32_t dest, const alys::shader::detail::operand_t& step)
		{
			a.mad(asm_::r(dest).x(), step, asm_::l(0.5f), asm_::l(0.5f));
			a.mov(asm_::r(dest).y(), asm_::l(0.5f));
			a.mad(asm_::r(dest).z(), step, step, asm_::l(-1.0f));
			a.sqrt(asm_::r(dest).z(), asm_::r(dest).z().neg());
			a.mad(asm_::r(dest).z(), asm_::r(dest).z(), asm_::l(0.5f), asm_::l(0.5f));
		}

		void emit_atlas_step(alys::shader::assembler& a, const std::uint32_t position, const std::uint32_t u_step, const std::uint32_t v_step,
			const std::uint32_t index, const std::uint32_t first, const std::uint32_t second)
		{
			a.iadd(asm_::r(index).y(), asm_::r(index).x(), uint_literal(16u));
			a.ld_raw(asm_::r(index).y(), asm_::r(index).y(), asm_::t(sprite_buffer_slot).select("xxxx"));
			a.ushr(asm_::r(index).y(), asm_::r(index).y(), uint_literal(16));
			a.f16tof32(asm_::r(index).y(), asm_::r(index).y());
			a.frc(asm_::r(position).w(), asm_::r(index).y());
			a.round_ni(asm_::r(index).y(), asm_::r(index).y());
			a.ftou(asm_::r(index).y(), asm_::r(index).y());
			a.iadd(asm_::r(index).y(), asm_::r(index).y(), uint_literal(1));

			a.mov(asm_::r(second).z(), asm_::r(second).y().abs());
			a.mov(asm_::r(second).w(), asm_::r(first).y().abs());
			a.rcp(asm_::r(first).zw(), asm_::r(second).select("xyzw"));
			a.round_ne(asm_::r(first).zw(), asm_::r(first).select("xyzw"));
			a.mul(asm_::r(first).y(), asm_::r(first).z(), asm_::r(first).w());
			a.ftou(asm_::r(first).yzw(), asm_::r(first).select("xyzw"));
			a.udiv(asm_::r(second).x(), asm_::r(index).z(), asm_::r(index).y(), asm_::r(first).z());
			a.udiv(asm_::r(second).y(), asm_::r(index).w(), asm_::r(index).y(), asm_::r(first).y());
			a.ieq(asm_::r(index).zw(), asm_::r(index).select("xyzw"), uint_literal(0));
			a.add(asm_::r(first).zw(), asm_::r(second).select("xyzw"), asm_::l(-1.0f));
			a.movc(asm_::r(second).x(), asm_::r(index).z(), asm_::r(first).z(), asm_::r(second).z());
			a.and_(asm_::r(second).y(), asm_::r(index).z(), asm_::r(second).w());
			a.movc(asm_::r(second).y(), asm_::r(index).w(), asm_::r(first).w(), asm_::r(second).y());

			emit_encode_atlas_step(a, u_step, asm_::r(second).select("xxxx").swz());
			emit_encode_atlas_step(a, v_step, asm_::r(second).select("yyyy").swz());
		}
	}

	std::vector<std::pair<std::uint32_t, sprite_input>> take_sprite_inputs(alys::shader::shader_object::signature& inputs)
	{
		std::vector<std::pair<std::uint32_t, sprite_input>> result;
		for (const auto& element : inputs)
		{
			const auto input = find_sprite_input(element);
			if (!input)
			{
				throw std::runtime_error(utils::string::va("sprite vertex shader reads %s%u", element.name.data(), element.semantic_index));
			}

			result.emplace_back(element.register_, *input);
		}

		signature_element vertex_id{};
		vertex_id.name = "SV_VertexID";
		vertex_id.system_value_type = alys::shader::VERTID;
		vertex_id.component_type = alys::shader::format_uint;
		vertex_id.mask = 0x1;
		vertex_id.rw_mask = 0x1;
		inputs = {vertex_id};
		return result;
	}

	void emit_sprite_fetch(alys::shader::assembler& a, const std::uint32_t temp, const bool atlas_blend)
	{
		const auto position = temp;
		const auto color = temp + 1;
		const auto texcoord = temp + 2;
		const auto normal = temp + 3;
		const auto atlas_step = temp + 4;
		const auto index = temp + 5;
		const auto first = temp + 6;
		const auto second = temp + 7;
		const auto buffer = asm_::t(sprite_buffer_slot);

		a.ushr(asm_::r(index).x(), asm_::v(0).x(), uint_literal(2));
		a.ishl(asm_::r(index).x(), asm_::r(index).x(), uint_literal(6u));
		a.and_(asm_::r(index).y(), asm_::v(0).x(), uint_literal(3));
		a.ld_raw(asm_::r(position).select("xyzw"), asm_::r(index).x(), buffer.select("xyzw"));
		a.iadd(asm_::r(index).z(), asm_::r(index).x(), uint_literal(20u));
		a.ld_raw(asm_::r(first).xyz(), asm_::r(index).z(), buffer.select("xyzx"));
		emit_unpack_unorm(a, normal, asm_::r(first).select("xxxx").swz());
		emit_unpack_unorm(a, texcoord, asm_::r(first).select("yyyy").swz());
		emit_unpack_unorm(a, color, asm_::r(first).select("zzzz").swz());
		a.mad(asm_::r(texcoord).xyz(), asm_::r(texcoord).select("xyzx"), asm_::l(2.0f), asm_::l(-1.0f));
		a.mad(asm_::r(color).xyz(), asm_::r(color).select("xyzx"), asm_::l(2.0f), asm_::l(-1.0f));

		a.sincos(asm_::r(first).x(), asm_::r(first).y(), asm_::r(position).w());
		a.mul(asm_::r(second).xyz(), asm_::r(color).select("xyzx"), asm_::r(first).select("xxxx"));
		a.mad(asm_::r(second).xyz(), asm_::r(texcoord).select("xyzx"), asm_::r(first).select("yyyy"), asm_::r(second).select("xyzx"));
		a.mul(asm_::r(color).xyz(), asm_::r(color).select("xyzx"), asm_::r(first).select("yyyy"));
		a.mad(asm_::r(color).xyz(), asm_::r(texcoord).select("xyzx"), asm_::r(first).select("xxxx"), asm_::r(color).select("xyzx").neg());

		a.iadd(asm_::r(index).z(), asm_::r(index).x(), uint_literal(48u));
		a.ld_raw(asm_::r(first).select("xyzw"), asm_::r(index).z(), buffer.select("xyzw"));
		a.and_(asm_::r(texcoord).select("xyzw"), asm_::r(first).select("xyzw"), uint_literal(0xFFFF));
		a.ushr(asm_::r(first).select("xyzw"), asm_::r(first).select("xyzw"), uint_literal(16));
		a.f16tof32(asm_::r(texcoord).select("xyzw"), asm_::r(texcoord).select("xyzw"));
		a.f16tof32(asm_::r(first).select("xyzw"), asm_::r(first).select("xyzw"));
		a.mul(asm_::r(second).xyz(), asm_::r(second).select("xyzx"), asm_::r(texcoord).select("zzzz"));
		a.mul(asm_::r(color).xyz(), asm_::r(color).select("xyzx"), asm_::r(first).select("zzzz"));

		a.ushr(asm_::r(index).z(), asm_::r(index).y(), uint_literal(1));
		a.xor_(asm_::r(index).w(), asm_::r(index).y(), asm_::r(index).z());
		a.and_(asm_::r(index).w(), asm_::r(index).w(), uint_literal(1));
		a.utof(asm_::r(index).zw(), asm_::r(index).select("xyzw"));
		a.mad(asm_::r(index).w(), asm_::r(index).w(), asm_::l(-1.0f), asm_::l(1.0f));
		a.mad(asm_::r(index).y(), asm_::r(index).z(), asm_::l(2.0f), asm_::l(-1.0f));
		a.add(asm_::r(index).y(), asm_::r(index).y(), asm_::r(texcoord).w());
		a.mad(asm_::r(position).xyz(), asm_::r(second).select("xyzx"), asm_::r(index).select("yyyy"), asm_::r(position).select("xyzx"));
		a.mad(asm_::r(index).y(), asm_::r(index).w(), asm_::l(2.0f), asm_::l(-1.0f));
		a.add(asm_::r(index).y(), asm_::r(index).y(), asm_::r(first).w());
		a.mad(asm_::r(position).xyz(), asm_::r(color).select("xyzx"), asm_::r(index).select("yyyy"), asm_::r(position).select("xyzx"));
		a.mov(asm_::r(position).w(), asm_::l(0.0f));
		a.mov(asm_::r(color).xz(), asm_::r(texcoord).select("xxyy"));
		a.mov(asm_::r(color).yw(), asm_::r(first).select("xxyy"));

		a.iadd(asm_::r(index).y(), asm_::r(index).x(), uint_literal(32u));
		a.ld_raw(asm_::r(first).xy(), asm_::r(index).y(), buffer.select("xyxx"));
		a.and_(asm_::r(second).xy(), asm_::r(first).select("xyxx"), uint_literal(0xFFFF));
		a.ushr(asm_::r(first).xy(), asm_::r(first).select("xyxx"), uint_literal(16));
		a.f16tof32(asm_::r(second).xy(), asm_::r(second).select("xyxx"));
		a.f16tof32(asm_::r(first).xy(), asm_::r(first).select("xyxx"));
		a.mad(asm_::r(texcoord).x(), asm_::r(index).z(), asm_::r(second).y(), asm_::r(second).x());
		a.mad(asm_::r(texcoord).y(), asm_::r(index).w(), asm_::r(first).y(), asm_::r(first).x());
		if (atlas_blend)
		{
			emit_atlas_step(a, position, normal, atlas_step, index, first, second);
		}
	}
}
