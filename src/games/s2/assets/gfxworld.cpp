#include <std_include.hpp>
#include "s2.hpp"

namespace zonetool::s2
{
	namespace
	{
		using entry = const field_reader&;

		void read_sun(entry world, entry sun)
		{
			sun.bytes(8, 3, 20 * world.at<int>(20));
			sun.bytes(16, 3, 4 * world.at<int>(24));
			sun.runtime_bytes(24, 3, 4ull * (sun.at<std::uint32_t>(0) << 9));
			sun.runtime_bytes(32, 3, 4ull * (8 * sun.at<std::uint32_t>(0)));
		}

		void read_cells(entry w)
		{
			const auto cell_count = w.at<std::uint32_t>(88);
			w.bytes(128, 3, 4ull * cell_count);
			const auto portal_counts = w.at<int*>(128);

			w.aligned_entries(136, 127, 8, cell_count, [&](entry cell, const std::size_t index)
			{
				cell.aligned_entries(0, 7, 48, portal_counts[index], [](entry portal, std::size_t)
				{
					portal.bytes(32, 1, 2ull * portal.at<std::uint16_t>(30));
				});
			});

			w.entries(144, 24, cell_count, [](entry e, std::size_t)
			{
				e.bytes(8, 1, 2ull * e.at<std::uint16_t>(0));
			});

			w.entries(152, 56, cell_count, [](entry e, std::size_t)
			{
				e.entries(32, 80, e.at<std::uint16_t>(24), [](entry child, std::size_t)
				{
					child.bytes(40, 3, 12ull * child.at<std::uint8_t>(52));
				});
				e.bytes(40, 0, e.at<std::uint8_t>(26));
				e.bytes(48, 0, e.at<std::uint8_t>(27));
			});
		}

		void read_draw(entry d)
		{
			const auto reflection_probe_count = d.at<std::uint32_t>(0);
			d.asset_array(ASSET_TYPE_IMAGE, 8, reflection_probe_count);
			d.entries(16, 32, reflection_probe_count, [](entry probe, std::size_t)
			{
				probe.entries(16, 16, probe.at<std::uint32_t>(24), [](entry e, std::size_t)
				{
					e.bytes(0, 1, 2ull * e.at<std::uint32_t>(8));
				});
			});
			d.runtime_bytes(24, 7, 24ull * reflection_probe_count);
			d.bytes(40, 3, 12ull * d.at<std::uint32_t>(32));
			d.bytes(48, 0, d.at<std::uint32_t>(32));

			const auto lightmap_count = d.at<std::uint32_t>(56);
			d.asset_array(ASSET_TYPE_IMAGE, 64, lightmap_count);
			d.bytes(72, 3, 116ull * lightmap_count);
			d.asset(ASSET_TYPE_IMAGE, 80);

			const auto lightmap_override_count = d.at<std::uint32_t>(156);
			d.entries(160, 16, lightmap_override_count, [](entry e, std::size_t)
			{
				e.asset(ASSET_TYPE_IMAGE, 0);
				e.asset(ASSET_TYPE_IMAGE, 8);
			});
			d.runtime_bytes(168, 7, 24ull * lightmap_override_count);
			d.runtime_bytes(176, 7, 24ull * lightmap_override_count);
			d.asset(ASSET_TYPE_IMAGE, 184);
			d.asset(ASSET_TYPE_IMAGE, 192);
			d.asset_array(ASSET_TYPE_IMAGE, 208, d.at<std::uint32_t>(200));
			d.assets(ASSET_TYPE_GFXWORLD_TRANSIENT_ZONE, 640, 16);
			d.entries(768, 16, d.at<std::uint32_t>(632), [](entry e, std::size_t)
			{
				e.asset_array(ASSET_TYPE_MATERIAL, 8, e.at<std::uint32_t>(0));
			});
			d.bytes(784, 3, 2ull * d.at<std::uint32_t>(776));
		}

		void read_probe_grid(entry p)
		{
			const auto probe_count = p.at<std::uint32_t>(0);
			p.bytes(8, 63, 64ull * probe_count);
			p.bytes(40, 3, 12ull * probe_count);
			p.bytes(72, 3, 92ull * p.at<std::uint32_t>(64));
			p.bytes(88, 63, 16ull * p.at<std::uint32_t>(80));
			p.bytes(112, 63, 16ull * p.at<std::uint32_t>(80));
			p.bytes(136, 63, 64ull * p.at<std::uint32_t>(84));
			p.bytes(168, 63, 4ull * p.at<std::uint32_t>(160));
			p.bytes(200, 3, 64ull * p.at<std::uint32_t>(192));
			p.bytes(216, 63, 64ull * p.at<std::uint32_t>(208));
		}

		void read_light_grid(entry g)
		{
			const auto row_axis = g.at<std::uint32_t>(20);
			const auto min = g.at<std::uint16_t>(8 + 2 * row_axis);
			const auto max = g.at<std::uint16_t>(14 + 2 * row_axis);
			g.bytes(32, 1, 2ull * (max - min + 1));
			g.bytes(48, 0, g.at<std::uint32_t>(40));
			g.bytes(64, 3, 8ull * g.at<std::uint32_t>(56));
			g.bytes(80, 1, 336ull * g.at<std::uint32_t>(72));
			g.bytes(128, 3, 4ull * g.at<std::uint8_t>(123));
			g.bytes(144, 3, 4ull * g.at<std::uint32_t>(136));
			g.bytes(160, 0, g.at<std::uint32_t>(152));

			for (auto i = 0u; i < 3; i++)
			{
				const auto volume = g.sub(840 + i * 80);
				volume.bytes(56, 3, 4ull * volume.at<int>(4));
				volume.bytes(72, 0, volume.at<int>(64));
			}

			read_probe_grid(g.sub(1080));
		}

		void read_vertex_data(entry v)
		{
			constexpr std::size_t element_sizes[] = {32, 28, 16, 20, 16, 16, 16, 16, 16, 16};
			for (auto i = 0u; i < std::size(element_sizes); i++)
			{
				const auto offset = 8 + i * 16;
				v.bytes(offset, 3, element_sizes[i] * v.at<std::uint32_t>(offset - 8));
			}

			v.asset(ASSET_TYPE_IMAGE, 168);
			v.bytes(184, 3, 4ull * v.at<std::uint32_t>(176));
			v.asset(ASSET_TYPE_IMAGE, 200);
			v.bytes(216, 1, 2ull * v.at<std::uint32_t>(208));
			v.bytes(240, 63, 64ull * v.at<std::uint32_t>(236));
			v.bytes(272, 63, 64ull * v.at<std::uint32_t>(264));
			v.bytes(296, 3, 4ull * v.at<std::uint32_t>(268));
			v.bytes(328, 63, 64ull * v.at<std::uint32_t>(324));
			v.bytes(360, 63, 64ull * v.at<std::uint32_t>(352));
			v.bytes(384, 3, 4ull * v.at<std::uint32_t>(356));
			v.bytes(416, 63, 64ull * v.at<std::uint32_t>(412));
			v.bytes(448, 63, 64ull * v.at<std::uint32_t>(440));
			v.bytes(472, 3, 4ull * v.at<std::uint32_t>(444));
		}

		void read_models(entry w)
		{
			w.entries(2832, 112, w.at<int>(2824), [](entry e, std::size_t)
			{
				e.bytes(40, 15, 0x40);
				e.bytes(48, 3, 12ull * e.at<int>(24));
				e.bytes(56, 3, 16ull * e.at<int>(28));
				e.bytes(64, 1, 2ull * e.at<int>(32));
				e.bytes(72, 3, 2ull * e.at<int>(36));
				e.runtime_bytes(80, 3, 8ull * e.at<std::uint32_t>(28));
			});

			const auto shadow_geometry = w.sub(2840);
			shadow_geometry.bytes(8, 1, 0);
			shadow_geometry.bytes(24, 1, 0);
			shadow_geometry.bytes(40, 1, 2ull * shadow_geometry.at<std::uint32_t>(32));
		}

		void read_dpvs_planes(entry w)
		{
			const auto cell_count = w.at<std::uint32_t>(88);
			const auto cell_words = (cell_count + 31) >> 5;
			const auto node_count = w.at<std::uint32_t>(56) - w.at<std::uint32_t>(52);
			const auto light_count = w.at<std::uint32_t>(4112);
			const auto shadow_count = w.at<std::uint32_t>(4116);

			w.runtime_bytes(3152, 3, 4ull * cell_count * cell_words);
			w.runtime_bytes(3160, 3, 4ull * cell_words);
			w.runtime_bytes(3168, 3, 12ull * light_count);
			w.runtime_bytes(3176, 3, 8ull * shadow_count);
			w.runtime_bytes(3184, 3, 4ull * static_cast<std::uint32_t>((node_count << 13) - 0x2000));
			w.runtime_bytes(3192, 3, 4ull * static_cast<std::uint32_t>(light_count * (node_count - 1)));
			w.runtime_bytes(3200, 3, 4ull * static_cast<std::uint32_t>(shadow_count * (node_count - 1)));
			w.runtime_bytes(3208, 1, 2ull * light_count);
		}

		void read_aabb_trees(entry w)
		{
			const auto tree_count = w.at<std::uint32_t>(56);
			const auto read_tree = [](entry e, std::size_t)
			{
				e.bytes(8, 3, 4ull * e.at<std::uint16_t>(0));
				e.bytes(16, 1, 2ull * e.at<std::uint16_t>(2));
			};
			w.entries(3216, 24, tree_count, read_tree);
			w.entries(3224, 24, tree_count, read_tree);
			w.entries(3232, 16, tree_count, [](entry e, std::size_t)
			{
				e.entries(8, 88, e.at<std::uint32_t>(0), [](entry child, std::size_t)
				{
					child.bytes(80, 3, 20ull * child.at<std::uint32_t>(72));
				});
			});
		}

		void read_dpvs_runtime(entry d)
		{
			const auto smodel_count = d.at<std::uint32_t>(72);
			const auto surface_count = d.at<std::uint32_t>(76);
			const auto smodel_visibility = [&](const std::size_t offset)
			{
				d.runtime_bytes(offset, 127, 4ull * smodel_count);
			};
			const auto surface_visibility = [&](const std::size_t offset)
			{
				d.runtime_bytes(offset, 127, 4ull * surface_count);
			};

			smodel_visibility(88);
			smodel_visibility(96);
			smodel_visibility(104);
			surface_visibility(400);
			surface_visibility(408);
			surface_visibility(416);
			for (auto offset = 120u; offset <= 176; offset += 8)
			{
				d.runtime_bytes(offset, 127, 0);
			}
			for (auto offset = 432u; offset <= 488; offset += 8)
			{
				d.runtime_bytes(offset, 127, 0);
			}
			d.runtime_bytes(712, 127, 4ull * 2 * surface_count);
		}

		void read_dpvs_visibility(entry d)
		{
			const auto smodel_count = d.at<std::uint32_t>(72);
			const auto surface_count = d.at<std::uint32_t>(76);
			d.runtime_bytes(112, 127, 4ull * smodel_count);
			d.runtime_bytes(424, 127, 4ull * surface_count);
			for (auto view = 0u; view < 3; view++)
			{
				for (auto i = 0u; i < 9; i++)
				{
					d.runtime_bytes(184 + view * 72 + i * 8, 127, 4ull * smodel_count);
				}
				for (auto i = 0u; i < 9; i++)
				{
					d.runtime_bytes(496 + view * 72 + i * 8, 127, 4ull * surface_count);
				}
			}
			d.runtime_bytes(720, 127, 4ull * d.at<std::uint32_t>(80));
			d.runtime_bytes(728, 127, 4ull * smodel_count);
			d.runtime_bytes(736, 127, 4ull * surface_count);
		}

		void read_surface_buffers(entry e)
		{
			if (const auto info = e.bytes(16, 7, 0x100))
			{
				const field_reader buffer(e.reader(), info);
				e.reader().push_stream(XFILE_BLOCK_RUNTIME_UNREAD);
				buffer.bytes(64, 255, buffer.at<std::uint32_t>(16));
				e.reader().pop_stream();
			}
			e.asset(ASSET_TYPE_MATERIAL, 32);
		}

		void read_dpvs(entry world, entry d)
		{
			read_dpvs_runtime(d);
			d.bytes(792, 127, 4ull * 2 * d.at<std::uint32_t>(72));
			read_dpvs_visibility(d);

			d.bytes(744, 3, 4ull * d.at<std::uint32_t>(4));

			const auto surface_material_count = world.at<std::uint32_t>(28);
			d.entries(752, 56, surface_material_count, [](entry e, std::size_t)
			{
				read_surface_buffers(e);
			});
			d.bytes(760, 3, 64ull * surface_material_count);
			d.runtime_bytes(768, 1, 8ull * surface_material_count);

			const auto smodel_inst_count = d.at<std::uint32_t>(0);
			d.bytes(776, 3, 36ull * smodel_inst_count);
			d.entries(784, 128, smodel_inst_count, [](entry e, std::size_t)
			{
				e.asset(ASSET_TYPE_XMODEL, 72);
			});
			const auto smodel_insts = d.at<std::uint8_t*>(784);
			d.entries(800, 24, smodel_inst_count, [&](entry e, const std::size_t index)
			{
				const auto flags = *reinterpret_cast<std::uint16_t*>(smodel_insts + index * 128 + 84);
				if ((flags & 0x180) != 0 && (flags & 0x80) != 0)
				{
					e.bytes(0, 1, 20ull * e.at<std::uint32_t>(16));
				}
			});
			d.runtime_bytes(808, 127, 4ull * d.at<std::uint32_t>(76));
			d.bytes(816, 1, 2ull * smodel_inst_count);
			d.runtime_bytes(832, 127, 4ull * static_cast<std::uint32_t>(d.at<std::uint32_t>(824) * d.at<std::uint32_t>(828)));
			d.runtime_bytes(840, 3, 8ull * d.at<std::uint32_t>(4));
			d.runtime_bytes(848, 7, 8ull * smodel_inst_count);
		}

		void read_globals(entry world, entry g)
		{
			const auto cell_count = world.at<std::uint32_t>(88);
			const auto count0 = g.at<std::uint32_t>(0);
			const auto count1 = g.at<std::uint32_t>(4);
			g.runtime_bytes(16, 3, 4ull * static_cast<std::uint32_t>(count0 * cell_count));
			g.runtime_bytes(24, 3, 4ull * static_cast<std::uint32_t>(count1 * cell_count));
			for (const auto offset : {32u, 40u, 48u})
			{
				g.runtime_bytes(offset, 15, 32ull * count0);
			}
			for (const auto offset : {64u, 72u, 80u})
			{
				g.runtime_bytes(offset, 15, 32ull * count1);
			}
			g.runtime_bytes(56, 15, 32ull * count0);
			g.runtime_bytes(88, 15, 32ull * count1);
		}
	}

	void gfx_world::read(zone_reader& reader, GfxWorld* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);

		const field_reader w(reader, asset);
		w.strings({0, 8});
		w.entries(40, 56, w.at<int>(32), [](entry sky, std::size_t)
		{
			sky.bytes(8, 3, 4ull * sky.at<int>(0));
			sky.asset(ASSET_TYPE_IMAGE, 16);
		});
		read_sun(w, w.sub(88));
		read_cells(w);
		w.entries(160, 24, w.at<int>(48), [](entry e, std::size_t)
		{
			e.string(0);
			e.bytes(8, 1, 4ull * e.at<std::uint16_t>(20));
		});
		w.bytes(176, 3, 16ull * w.at<std::uint32_t>(168));
		read_draw(w.sub(192));
		read_light_grid(w.sub(992));
		read_vertex_data(w.sub(2320));
		read_models(w);
		w.aligned_entries(2816, 15, 64, w.at<std::uint32_t>(56), [](entry e, std::size_t)
		{
			e.bytes(32, 3, 2ull * e.at<std::uint32_t>(28));
			e.bytes(40, 3, 32ull * e.at<std::uint32_t>(24));
		});
		w.bytes(2896, 3, 96ull * w.at<int>(2888));
		w.aligned_entries(2960, 15, 16, w.at<int>(2956), [](entry e, std::size_t)
		{
			e.asset(ASSET_TYPE_MATERIAL, 0);
		});
		w.asset(ASSET_TYPE_MATERIAL, 2976);
		w.asset(ASSET_TYPE_MATERIAL, 2984);
		w.asset(ASSET_TYPE_IMAGE, 3144);
		read_dpvs_planes(w);
		read_aabb_trees(w);
		read_dpvs(w, w.sub(3240));
		read_globals(w, w.sub(4104));
		w.bytes(4208, 3, 72ull * w.at<std::uint32_t>(4204));
		w.entries(4224, 40, w.at<std::uint32_t>(4220), [](entry e, std::size_t)
		{
			e.asset(ASSET_TYPE_IMAGE, 32);
		});
		w.strings({4264, 4272, 4280, 4288});

		reader.pop_stream();
	}
}
