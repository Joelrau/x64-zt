#include <std_include.hpp>

#include "../../include.hpp"
#include "dxbc.hpp"
#include "forward_lights.hpp"

namespace zonetool::h1::converter::s2::shader
{
	namespace
	{
		void emit_spot_shadow(alys::shader::assembler& a, const std::uint32_t acc, const alys::shader::detail::operand_t& position)
		{
			constexpr std::array<std::pair<float, float>, 8> taps
			{{
				{-0.7071f, 0.7071f}, {0.0f, -0.9308f}, {0.5997f, 0.5997f}, {-0.7516f, 0.0f},
				{0.4531f, -0.4531f}, {0.0f, 0.5148f}, {-0.2633f, -0.2633f}, {0.21f, 0.0f},
			}};

			const auto out = acc + 1;
			const auto fade = acc + 5;
			const auto light = acc + 8;
			const auto dir = acc + 9;
			const auto record = acc + 12;
			const auto shadow = acc + 13;
			const auto address = acc + 14;
			const auto records = asm_::t(spot_shadow_record_slot);

			a.ld_structured(asm_::r(record).xy(), asm_::r(light).x(), uint_literal(68u), asm_::t(forward_lights_slot).select("xyxx"));
			a.ushr(asm_::r(shadow).y(), asm_::r(record).x(), uint_literal(16));
			a.and_(asm_::r(shadow).z(), asm_::r(record).y(), uint_literal(0xFFFF));
			a.f16tof32(asm_::r(shadow).yz(), asm_::r(shadow).select("xyzx"));
			a.mul(asm_::r(record).w(), asm_::r(shadow).y(), asm_::r(shadow).w());
			a.movc(asm_::r(shadow).y(), asm_::r(light).z(), asm_::r(shadow).y(), asm_::r(record).w());
			a.ine(asm_::r(shadow).w(), asm_::r(shadow).x(), uint_literal(255u));
			a.lt(asm_::r(record).z(), asm_::l(0.0f), asm_::r(shadow).y());
			a.and_(asm_::r(shadow).w(), asm_::r(shadow).w(), asm_::r(record).z());
			a.if_nz(asm_::r(shadow).w());

			for (auto row = 0u; row < 4; row++)
			{
				a.ld_structured(asm_::r(record).select("xyzw"), asm_::r(shadow).x(), uint_literal(row * 16), records.select("xyzw"));
				const auto dest = row < 3 ? asm_::r(fade).select(std::string(1, "xyz"[row])) : asm_::r(out).x();
				a.dp3(dest, asm_::r(record).select("xyzx"), position);
				a.add(dest, dest, asm_::r(record).select("wwww").swz());
			}

			a.rcp(asm_::r(out).x(), asm_::r(out).x());
			a.mul(asm_::r(fade).xyz(), asm_::r(fade).select("xyzx"), asm_::r(out).select("xxxx"));
			a.ld_structured(asm_::r(record).select("xyzw"), asm_::r(shadow).x(), uint_literal(64u), records.select("xyzw"));

			const auto sum_taps = [&](const std::uint32_t texture, const char* slice, const char* sum)
			{
				a.round_z(asm_::r(address).z(), asm_::r(record).select(std::string(4, slice[0])).swz());
				a.mov(asm_::r(out).select(sum), asm_::l(0.0f));
				for (const auto& [x, y] : taps)
				{
					a.mad(asm_::r(address).xy(), asm_::r(record).select("xyxx"), asm_::l(x, y, 0.0f, 0.0f), asm_::r(fade).select("xyxx"));
					a.sample_c_lz_indexable(asm_::r(address).w(), asm_::r(address).select("xyzx"), asm_::t(texture).select("xxxx"),
						asm_::s(spot_shadow_sampler), asm_::r(fade).z());
					a.add(asm_::r(out).select(sum), asm_::r(out).select(std::string(4, sum[0])).swz(), asm_::r(address).select("wwww").swz());
				}

				a.mul(asm_::r(out).select(sum), asm_::r(out).select(std::string(4, sum[0])).swz(), asm_::l(0.125f));
			};

			a.mov(asm_::r(out).y(), asm_::l(0.0f));
			a.ge(asm_::r(shadow).w(), asm_::r(shadow).z(), asm_::l(0.0f));
			a.if_nz(asm_::r(shadow).w());
			sum_taps(spot_shadow_slot, "w", "y");
			a.endif();
			a.lt(asm_::r(shadow).w(), asm_::r(shadow).z(), asm_::l(1.0f));
			a.if_nz(asm_::r(shadow).w());
			sum_taps(spot_shadow_dynamic_slot, "z", "z");
			a.add(asm_::r(out).w(), asm_::r(out).y(), negate(asm_::r(out).select("zzzz").swz()));
			a.mad(asm_::r(out).y(), asm_::r(shadow).z(), asm_::r(out).w(), asm_::r(out).z());
			a.endif();

			a.add(asm_::r(out).y(), asm_::r(out).y(), asm_::l(-1.0f));
			a.mad(asm_::r(out).y(), asm_::r(shadow).y(), asm_::r(out).y(), asm_::l(1.0f));
			a.mul(asm_::r(dir).w(), asm_::r(dir).w(), asm_::r(out).y());
			a.endif();
		}
	}

	std::map<std::uint32_t, std::uint32_t> relocate_light_textures(const std::vector<alys::shader::detail::instruction_t>& instructions)
	{
		std::map<std::uint32_t, std::uint32_t> result;
		auto next = 26u;
		for (const auto slot : forward_light_slots)
		{
			if (!declares_resource(instructions, slot))
			{
				continue;
			}

			while (declares_resource(instructions, next))
			{
				next++;
			}

			result.emplace(slot, next++);
		}

		return result;
	}

	void declare_forward_lights(alys::shader::assembler& a, const primary_lights& lights)
	{
		a.dcl_resource_structured(asm_::t(forward_lights_slot), 104u);
		a.dcl_resource(asm_::t(lights.cluster_row ? light_cluster_bits_slot : light_tile_bits_slot), asm_::buffer, uint4_type);
		if (lights.spot_shadows)
		{
			a.dcl_resource_structured(asm_::t(spot_shadow_record_slot), 96u);
			a.dcl_resource(asm_::t(spot_shadow_slot), asm_::texture2darray, float4_type);
			a.dcl_resource(asm_::t(spot_shadow_dynamic_slot), asm_::texture2darray, float4_type);
			a.dcl_sampler(asm_::s(spot_shadow_sampler), D3D10_SB_SAMPLER_MODE_COMPARISON);
		}
	}

	std::uint8_t light_screen_mask(const primary_lights& lights)
	{
		return lights.cluster_row ? 0xB : 0x3;
	}

	void emit_forward_light_loop_begin(alys::shader::assembler& a, const primary_lights& lights, const std::uint32_t acc, const std::uint32_t screen_input,
		const alys::shader::detail::operand_t& position)
	{
		const auto out = acc + 1;
		const auto light_position = acc + 2;
		const auto diffuse = acc + 3;
		const auto physical_size = acc + 4;
		const auto fade = acc + 5;
		const auto specular = acc + 6;
		const auto tile = acc + 7;
		const auto light = acc + 8;
		const auto dir = acc + 9;
		const auto color = acc + 10;
		const auto spot = acc + 11;
		const auto shadow = acc + 13;
		const auto stable = s2_stable_constant_buffer;
		const auto rt_size = asm_::cb(stable, lights.render_target_size_row);
		const auto tile_parms = asm_::cb(stable, lights.tile_parms_row);
		const auto tile_words = tile_parms.select("xxxx").swz();
		const auto bits_slot = lights.cluster_row ? light_cluster_bits_slot : light_tile_bits_slot;

		a.mov(asm_::r(acc).select("xyzw"), asm_::l(0.0f, 0.0f, 0.0f, 0.0f));
		a.mov(asm_::r(out).select("xyzw"), asm_::l(0.0f, 0.0f, 0.0f, 0.0f));

		if (lights.cluster_row)
		{
			const auto cells = asm_::cb(stable, *lights.cluster_row);
			const auto slice_starts = asm_::cb(stable, *lights.cluster_row + 1);
			const auto slice_scales = asm_::cb(stable, *lights.cluster_row + 2);
			a.mul(asm_::r(light).xy(), asm_::v(screen_input).select("xyxx"), rt_size.select("zwzz"));
			a.mul(asm_::r(light).xy(), asm_::r(light).select("xyxx"), rt_size.select("xyxx"));
			a.mul(asm_::r(light).xy(), asm_::r(light).select("xyxx"), cells.select("zwzz"));
			a.add(asm_::r(light).zw(), tile_parms.select("xxzw"), cells.select("xxxy"));
			a.add(asm_::r(light).zw(), asm_::r(light).select("xxzw"), asm_::l(0.0f, 0.0f, -1.0f, -1.0f));
			a.mul(asm_::r(light).zw(), asm_::r(light).select("xxzw"), cells.select("xxzw"));
			a.ftou(asm_::r(light).select("xyzw"), asm_::r(light).select("xyzw"));
			a.add(asm_::r(dir).x(), asm_::v(screen_input).select("wwww").swz(), asm_::l(-4.0f));
			a.add(asm_::r(dir).y(), asm_::r(dir).x(), negate(slice_starts.select("xxxx").swz()));
			a.add(asm_::r(dir).z(), asm_::r(dir).y(), negate(slice_starts.select("yyyy").swz()));
			a.add(asm_::r(dir).w(), asm_::r(dir).z(), negate(slice_starts.select("zzzz").swz()));
			a.mul_sat(asm_::r(dir).select("xyzw"), asm_::r(dir).select("xyzw"), slice_scales.select("xyzw"));
			a.dp4(asm_::r(dir).x(), asm_::r(dir).select("xyzw"), asm_::l(0.25f, 0.25f, 0.25f, 0.25f));
			a.mul(asm_::r(dir).x(), asm_::r(dir).x(), asm_::l(32.0f));
			a.ftou(asm_::r(dir).x(), asm_::r(dir).x());
			a.umin(asm_::r(dir).x(), asm_::r(dir).x(), uint_literal(31));
			a.imad(asm_::r(tile).z(), asm_::r(light).w(), asm_::r(light).z(), uint_literal(0));
			a.imad(asm_::r(tile).y(), asm_::r(light).z(), asm_::r(light).y(), asm_::r(light).x());
			a.imad(asm_::r(tile).x(), asm_::r(dir).x(), asm_::r(tile).z(), asm_::r(tile).y());
		}
		else
		{
			a.mul(asm_::r(tile).xy(), asm_::v(screen_input).select("xyxx"), rt_size.select("zwzz"));
			a.mul(asm_::r(tile).xy(), asm_::r(tile).select("xyxx"), rt_size.select("xyxx"));
			a.ftou(asm_::r(tile).xy(), asm_::r(tile).select("xyxx"));
			a.ushr(asm_::r(tile).xy(), asm_::r(tile).select("xyxx"), uint_literals(3, 3, 0, 0));
			a.add(asm_::r(tile).z(), rt_size.select("xxxx").swz(), asm_::l(7.0f));
			a.mul(asm_::r(tile).z(), asm_::r(tile).z(), asm_::l(0.125f));
			a.ftou(asm_::r(tile).z(), asm_::r(tile).z());
			a.imad(asm_::r(tile).x(), asm_::r(tile).y(), asm_::r(tile).z(), asm_::r(tile).x());
		}

		a.mov(asm_::r(tile).y(), uint_literal(0));
		a.loop();
		a.uge(asm_::r(tile).z(), asm_::r(tile).y(), tile_words);
		a.breakc_nz(asm_::r(tile).z());
		a.imad(asm_::r(tile).z(), asm_::r(tile).x(), tile_words, asm_::r(tile).y());
		a.ld_indexable(asm_::r(tile).z(), asm_::r(tile).select("zzzz"), asm_::t(bits_slot).select("xxxx"));
		a.ishl(asm_::r(tile).w(), asm_::r(tile).y(), uint_literal(5));
		a.loop();
		a.breakc_z(asm_::r(tile).z());
		a.firstbit_lo(asm_::r(light).x(), asm_::r(tile).z());
		a.ishl(asm_::r(light).y(), uint_literal(1), asm_::r(light).x());
		a.xor_(asm_::r(tile).z(), asm_::r(tile).z(), asm_::r(light).y());
		a.iadd(asm_::r(light).x(), asm_::r(light).x(), asm_::r(tile).w());

		const auto lights_buffer = asm_::t(forward_lights_slot);
		a.ld_structured(asm_::r(light_position).select("xyzw"), asm_::r(light).x(), uint_literal(0), lights_buffer.select("xyzw"));
		a.ld_structured(asm_::r(dir).select("xyzw"), asm_::r(light).x(), uint_literal(16), lights_buffer.select("xyzw"));
		a.ld_structured(asm_::r(color).select("xyzw"), asm_::r(light).x(), uint_literal(32), lights_buffer.select("xyzw"));
		a.ld_structured(asm_::r(physical_size).select("xyzw"), asm_::r(light).x(), uint_literal(48), lights_buffer.select("yzwx"));
		a.ld_structured(asm_::r(spot).xy(), asm_::r(light).x(), uint_literal(64), lights_buffer.select("xyxx"));
		if (lights.spot_shadows)
		{
			a.and_(asm_::r(shadow).x(), asm_::r(dir).w(), uint_literal(255));
		}

		a.ubfe(asm_::r(light).z(), uint_literal(2), uint_literal(8), asm_::r(dir).w());
		a.ieq(asm_::r(light).z(), asm_::r(light).z(), uint_literal(GFX_LIGHT_TYPE_SPOT));
		a.ushr(asm_::r(light).w(), asm_::r(spot).x(), uint_literal(16));
		a.and_(asm_::r(spot).xy(), asm_::r(spot).select("xyxx"), uint_literal(0xFFFF));
		a.and_(asm_::r(spot).z(), asm_::r(color).w(), uint_literal(0xFFFF));
		a.ushr(asm_::r(spot).w(), asm_::r(color).w(), uint_literal(16));
		a.f16tof32(asm_::r(spot).select("xyzw"), asm_::r(spot).select("xyzw"));
		a.f16tof32(asm_::r(light).w(), asm_::r(light).w());

		a.add(asm_::r(fade).xyz(), asm_::r(light_position).select("xyzx"), negate(position));
		a.dp3(asm_::r(dir).w(), asm_::r(fade).select("xyzx"), asm_::r(dir).select("xyzx"));
		a.add(asm_::r(dir).w(), asm_::r(dir).w(), asm_::r(light_position).w());
		a.mad_sat(asm_::r(dir).w(), asm_::r(dir).w(), asm_::r(spot).z(), asm_::r(spot).w());
		a.mul(asm_::r(dir).w(), asm_::r(dir).w(), asm_::r(dir).w());
		a.dp3(asm_::r(light).y(), asm_::r(fade).select("xyzx"), asm_::r(fade).select("xyzx"));
		a.mov(asm_::r(out).x(), asm_::r(light).y());
		a.rsq(asm_::r(light).y(), asm_::r(light).y());
		a.mul(asm_::r(fade).xyz(), asm_::r(fade).select("xyzx"), asm_::r(light).select("yyyy"));

		a.mul(asm_::r(out).x(), asm_::r(out).x(), asm_::r(light).y());
		a.mul(asm_::r(out).z(), asm_::r(light_position).w(), asm_::l(20.0f));
		a.mad_sat(asm_::r(out).y(), asm_::r(out).x().neg(), asm_::r(out).z(), asm_::l(20.0f));

		a.dp3(asm_::r(light).y(), asm_::r(fade).select("xyzx"), asm_::r(dir).select("xyzx"));
		a.mad_sat(asm_::r(light).y(), asm_::r(light).y(), asm_::r(spot).x(), asm_::r(light).w());
		if (lights.spot_shadows)
		{
			a.mov(asm_::r(shadow).w(), asm_::r(light).y());
		}
		a.ge(asm_::r(light).w(), asm_::l(0.0f), asm_::r(light).y());
		a.log(asm_::r(light).y(), asm_::r(light).y());
		a.mul(asm_::r(light).y(), asm_::r(light).y(), asm_::r(spot).y());
		a.exp(asm_::r(light).y(), asm_::r(light).y());
		a.movc(asm_::r(light).y(), asm_::r(light).w(), asm_::l(0.0f), asm_::r(light).y());
		a.mul(asm_::r(dir).w(), asm_::r(dir).w(), asm_::r(light).y());
		a.movc(asm_::r(dir).w(), asm_::r(light).z(), asm_::r(dir).w(), asm_::l(1.0f));
		a.mul(asm_::r(dir).w(), asm_::r(dir).w(), asm_::r(out).y());
		if (lights.spot_shadows)
		{
			emit_spot_shadow(a, acc, position);
		}

		a.mul(asm_::r(diffuse).xyz(), asm_::r(color).select("xyzx"), asm_::r(dir).select("wwww"));
		a.mov(asm_::r(diffuse).w(), asm_::l(0.0f));
		a.mov(asm_::r(specular).select("xyzw"), asm_::r(diffuse).select("xyzw"));
		a.movc(asm_::r(fade).xy(), asm_::r(light).select("zzzz"), asm_::l(0.0f, -1.0f, 0.0f, 0.0f), asm_::r(spot).select("zwzz"));
		a.mov(asm_::r(fade).zw(), asm_::l(0.0f, 0.0f, 0.0f, 0.0f));
	}

	void emit_forward_light_loop_end(alys::shader::assembler& a, const std::uint32_t acc, const std::optional<std::uint32_t> lit)
	{
		const auto out = acc + 1;
		const auto tile = acc + 7;

		a.max(asm_::r(out).xyz(), asm_::r(out).select("xyzx"), asm_::l(0.0f));
		a.min(asm_::r(out).xyz(), asm_::r(out).select("xyzx"), asm_::l(65504.0f));
		a.add(asm_::r(acc).xyz(), asm_::r(acc).select("xyzx"), asm_::r(out).select("xyzx"));
		a.mov(asm_::r(acc).w(), asm_::r(out).w());
		a.endloop();
		a.iadd(asm_::r(tile).y(), asm_::r(tile).y(), uint_literal(1));
		a.endloop();
		if (lit)
		{
			a.add(asm_::r(*lit).xyz(), asm_::r(*lit).select("xyzx"), asm_::r(acc).select("xyzx"));
			a.mov(asm_::o(0).select("xyzw"), asm_::r(*lit).select("xyzw"));
		}
		else
		{
			a.mov(asm_::o(0).select("xyzw"), asm_::r(acc).select("xyzw"));
		}

		a.ret();
	}
}
