#include <std_include.hpp>
#include "s2.hpp"

namespace zonetool::s2
{
	namespace
	{
		constexpr std::size_t align_1 = 0;
		constexpr std::size_t align_2 = 1;
		constexpr std::size_t align_4 = 3;
		constexpr std::size_t align_8 = 7;
		constexpr std::size_t align_16 = 15;
		constexpr std::size_t align_64 = 63;
		constexpr std::size_t align_128 = 127;
		constexpr std::size_t align_256 = 255;

		constexpr std::size_t dyn_ent_vis_bytes_per_word = 32;
		constexpr std::size_t shadow_vis_light_shift = 13;
		constexpr std::uint32_t shadow_vis_sun_bias = 0x2000;
		constexpr std::size_t unknown_vis_per_view = 9;
		constexpr std::size_t unknown_vis_view_count = 3;

		constexpr std::size_t pointer_offset(const std::size_t array_offset, const std::size_t index)
		{
			return array_offset + index * sizeof(void*);
		}

		template <typename V>
		void visit_skies(const V& w)
		{
			const auto& world = w.template as<GfxWorld>();
			w.entries(offsetof(GfxWorld, skies), sizeof(GfxSky), world.skyCount, [](const auto& e, std::size_t)
			{
				const auto& sky = e.template as<GfxSky>();
				e.array(S2_ARRAY_FIELD(GfxSky, skyStartSurfs), align_4, sky.skySurfCount);
				e.asset(ASSET_TYPE_IMAGE, offsetof(GfxSky, skyImage));
			});
		}

		template <typename V>
		void visit_dpvs_planes(const V& w)
		{
			const auto& world = w.template as<GfxWorld>();
			const auto planes = w.sub(offsetof(GfxWorld, dpvsPlanes));
			const auto cell_count = planes.template as<GfxWorldDpvsPlanes>().cellCount;

			planes.array(S2_ARRAY_FIELD(GfxWorldDpvsPlanes, planes), align_4, world.planeCount);
			planes.array(S2_ARRAY_FIELD(GfxWorldDpvsPlanes, nodes), align_4, world.nodeCount);
			planes.runtime_array(S2_ARRAY_FIELD(GfxWorldDpvsPlanes, sceneEntCellBits), align_4, cell_count << 9);
			planes.runtime_array(S2_ARRAY_FIELD(GfxWorldDpvsPlanes, unk_0x20), align_4, 8 * cell_count);
		}

		template <typename V>
		void visit_cells(const V& w)
		{
			const auto& world = w.template as<GfxWorld>();
			const auto cell_count = world.dpvsPlanes.cellCount;

			w.array(S2_ARRAY_FIELD(GfxWorld, aabbTreeCounts), align_4, cell_count);
			const auto tree_counts = world.aabbTreeCounts;

			w.aligned_entries(offsetof(GfxWorld, aabbTrees), align_128, sizeof(GfxCellTree), cell_count, [&](const auto& cell_tree, const std::size_t cell_index)
			{
				cell_tree.aligned_entries(offsetof(GfxCellTree, aabbTree), align_8, sizeof(GfxAabbTree), tree_counts[cell_index].aabbTreeCount, [](const auto& e, std::size_t)
				{
					const auto& tree = e.template as<GfxAabbTree>();
					e.array(S2_ARRAY_FIELD(GfxAabbTree, smodelIndexes), align_2, tree.smodelIndexCount);
				});
			});

			w.entries(offsetof(GfxWorld, cellTransientInfos), sizeof(GfxCellTransientInfo), cell_count, [](const auto& e, std::size_t)
			{
				const auto& info = e.template as<GfxCellTransientInfo>();
				e.array(S2_ARRAY_FIELD(GfxCellTransientInfo, indices), align_2, info.indexCount);
			});

			w.entries(offsetof(GfxWorld, cells), sizeof(GfxCell), cell_count, [](const auto& e, std::size_t)
			{
				const auto& cell = e.template as<GfxCell>();
				e.entries(offsetof(GfxCell, portals), sizeof(GfxPortal), cell.portalCount, [](const auto& p, std::size_t)
				{
					const auto& portal = p.template as<GfxPortal>();
					p.array(S2_ARRAY_FIELD(GfxPortal, vertices), align_4, portal.vertexCount);
				});
				e.array(S2_ARRAY_FIELD(GfxCell, reflectionProbes), align_1, cell.reflectionProbeCount);
				e.array(S2_ARRAY_FIELD(GfxCell, reflectionProbeReferences), align_1, cell.reflectionProbeReferenceCount);
			});
		}

		template <typename V>
		void visit_portal_groups(const V& w)
		{
			const auto& world = w.template as<GfxWorld>();
			w.entries(offsetof(GfxWorld, portalGroup), sizeof(GfxPortalGroup), world.portalGroupCount, [](const auto& e, std::size_t)
			{
				const auto& group = e.template as<GfxPortalGroup>();
				e.string(offsetof(GfxPortalGroup, targetName));
				e.array(S2_ARRAY_FIELD(GfxPortalGroup, gfxPortalArray), align_2, group.numPortals);
			});
			w.array(S2_ARRAY_FIELD(GfxWorld, portalDistanceAnchorsAndCloseDistSquared), align_4, world.portalDistanceAnchorCount);
		}

		template <typename V>
		void visit_draw(const V& d)
		{
			const auto& draw = d.template as<GfxWorldDraw>();

			d.asset_array(ASSET_TYPE_IMAGE, offsetof(GfxWorldDraw, reflectionProbes), draw.reflectionProbeCount);
			d.entries(offsetof(GfxWorldDraw, reflectionProbeOrigins), sizeof(GfxReflectionProbe), draw.reflectionProbeCount, [](const auto& e, std::size_t)
			{
				const auto& probe = e.template as<GfxReflectionProbe>();
				e.entries(offsetof(GfxReflectionProbe, probeVolumes), sizeof(GfxReflectionProbeVolume), probe.probeVolumeCount, [](const auto& v, std::size_t)
				{
					const auto& volume = v.template as<GfxReflectionProbeVolume>();
					v.array(S2_ARRAY_FIELD(GfxReflectionProbeVolume, probeVolumePlanes), align_2, volume.probeVolumePlaneCount);
				});
			});
			d.runtime_array(S2_ARRAY_FIELD(GfxWorldDraw, reflectionProbeTextures), align_8, draw.reflectionProbeCount);
			d.array(S2_ARRAY_FIELD(GfxWorldDraw, reflectionProbeReferenceOrigins), align_4, draw.reflectionProbeReferenceCount);
			d.array(S2_ARRAY_FIELD(GfxWorldDraw, reflectionProbeReferences), align_1, draw.reflectionProbeReferenceCount);

			d.asset_array(ASSET_TYPE_IMAGE, offsetof(GfxWorldDraw, fogSplineImages), draw.fogSplineCount);
			d.array(S2_ARRAY_FIELD(GfxWorldDraw, fogSplineInfo), align_4, draw.fogSplineCount);
			d.asset(ASSET_TYPE_IMAGE, offsetof(GfxWorldDraw, iesLookup));

			d.entries(offsetof(GfxWorldDraw, lightmapOverrides), sizeof(GfxLightmapArray), draw.lightmapOverrideCount, [](const auto& e, std::size_t)
			{
				e.asset(ASSET_TYPE_IMAGE, offsetof(GfxLightmapArray, primary));
				e.asset(ASSET_TYPE_IMAGE, offsetof(GfxLightmapArray, secondary));
			});
			d.runtime_array(S2_ARRAY_FIELD(GfxWorldDraw, lightmapPrimaryTextures), align_8, draw.lightmapOverrideCount);
			d.runtime_array(S2_ARRAY_FIELD(GfxWorldDraw, lightmapSecondaryTextures), align_8, draw.lightmapOverrideCount);
			d.asset(ASSET_TYPE_IMAGE, offsetof(GfxWorldDraw, lightmapOverridePrimary));
			d.asset(ASSET_TYPE_IMAGE, offsetof(GfxWorldDraw, lightmapOverrideSecondary));
			d.asset_array(ASSET_TYPE_IMAGE, offsetof(GfxWorldDraw, images), draw.imageCount);

			d.assets(ASSET_TYPE_GFXWORLD_TRANSIENT_ZONE, offsetof(GfxWorldDraw, transientZones), std::size(draw.transientZones));
			d.entries(offsetof(GfxWorldDraw, transientZoneMaterials), sizeof(GfxTransientZoneMaterials), draw.transientZoneCount, [](const auto& e, std::size_t)
			{
				const auto& zone = e.template as<GfxTransientZoneMaterials>();
				e.asset_array(ASSET_TYPE_MATERIAL, offsetof(GfxTransientZoneMaterials, materials), zone.materialCount);
			});
			d.array(S2_ARRAY_FIELD(GfxWorldDraw, indices), align_4, draw.indexCount);
		}

		template <typename V>
		void visit_light_grid(const V& g)
		{
			const auto& grid = g.template as<GfxLightGrid>();
			const auto row_min = grid.mins[grid.rowAxis];
			const auto row_max = grid.maxs[grid.rowAxis];

			g.array(S2_ARRAY_FIELD(GfxLightGrid, rowDataStart), align_2, row_max - row_min + 1);
			g.array(S2_ARRAY_FIELD(GfxLightGrid, rawRowData), align_1, grid.rawRowDataSize);
			g.array(S2_ARRAY_FIELD(GfxLightGrid, entries), align_4, grid.entryCount);
			g.array(S2_ARRAY_FIELD(GfxLightGrid, colors), align_2, grid.colorCount);
			g.array(S2_ARRAY_FIELD(GfxLightGrid, stageLightingContrastGain), align_4, grid.stageCount);
			g.array(S2_ARRAY_FIELD(GfxLightGrid, paletteEntryAddress), align_4, grid.paletteEntryCount);
			g.array(S2_ARRAY_FIELD(GfxLightGrid, paletteBitstream), align_1, grid.paletteBitstreamSize);

			for (auto i = 0u; i < std::size(grid.tree); i++)
			{
				const auto tree_visitor = g.sub(offsetof(GfxLightGrid, tree) + i * sizeof(GfxLightGridTree));
				const auto& tree = tree_visitor.template as<GfxLightGridTree>();
				tree_visitor.array(S2_ARRAY_FIELD(GfxLightGridTree, p_nodeTable), align_4, tree.nodeCount);
				tree_visitor.array(S2_ARRAY_FIELD(GfxLightGridTree, p_leafTable), align_1, tree.leafTableSize);
			}
		}

		template <typename V>
		void visit_probe_data(const V& p)
		{
			const auto& probes = p.template as<GfxLightGridProbeData>();

			p.array(S2_ARRAY_FIELD(GfxLightGridProbeData, probes), align_64, probes.probeCount);
			p.array(S2_ARRAY_FIELD(GfxLightGridProbeData, probePositions), align_4, probes.probeCount);
			p.array(S2_ARRAY_FIELD(GfxLightGridProbeData, zones), align_4, probes.zoneCount);
			p.array(S2_ARRAY_FIELD(GfxLightGridProbeData, tetrahedrons), align_64, probes.tetrahedronCount);
			p.array(S2_ARRAY_FIELD(GfxLightGridProbeData, tetrahedronNeighbors), align_64, probes.tetrahedronCount);
			p.array(S2_ARRAY_FIELD(GfxLightGridProbeData, tetrahedronVisibility), align_64, probes.tetrahedronCountVisible);
			p.array(S2_ARRAY_FIELD(GfxLightGridProbeData, voxelStartTetrahedron), align_64, probes.voxelStartTetrahedronCount);
			p.array(S2_ARRAY_FIELD(GfxLightGridProbeData, smodelProbeGrids), align_4, probes.smodelProbeGridCount);
			p.array(S2_ARRAY_FIELD(GfxLightGridProbeData, smodelProbes), align_64, probes.smodelProbeCount);
		}

		template <typename V>
		void visit_unknown_data(const V& u)
		{
			const auto& data = u.template as<GfxWorldUnknownData>();

			const auto visit_array = [&](const std::size_t array_offset, const auto& unknown_array)
			{
				using array_type = std::remove_cvref_t<decltype(unknown_array)>;
				u.array(array_offset + offsetof(array_type, data), sizeof(*unknown_array.data), align_4, unknown_array.count);
			};

			visit_array(offsetof(GfxWorldUnknownData, array0), data.array0);
			visit_array(offsetof(GfxWorldUnknownData, array1), data.array1);
			visit_array(offsetof(GfxWorldUnknownData, array2), data.array2);
			visit_array(offsetof(GfxWorldUnknownData, array3), data.array3);
			visit_array(offsetof(GfxWorldUnknownData, array4), data.array4);
			visit_array(offsetof(GfxWorldUnknownData, array5), data.array5);
			visit_array(offsetof(GfxWorldUnknownData, array6), data.array6);
			visit_array(offsetof(GfxWorldUnknownData, array7), data.array7);
			visit_array(offsetof(GfxWorldUnknownData, array8), data.array8);
			visit_array(offsetof(GfxWorldUnknownData, array9), data.array9);

			u.asset(ASSET_TYPE_IMAGE, offsetof(GfxWorldUnknownData, image0));
			u.array(S2_ARRAY_FIELD(GfxWorldUnknownData, image0Data), align_4, data.image0DataCount);
			u.asset(ASSET_TYPE_IMAGE, offsetof(GfxWorldUnknownData, image1));
			u.array(S2_ARRAY_FIELD(GfxWorldUnknownData, image1Data), align_2, data.image1DataCount);

			for (auto i = 0u; i < std::size(data.groups); i++)
			{
				const auto group_visitor = u.sub(offsetof(GfxWorldUnknownData, groups) + i * sizeof(GfxWorldUnknownBufferGroup));
				const auto& group = group_visitor.template as<GfxWorldUnknownBufferGroup>();
				group_visitor.array(S2_ARRAY_FIELD(GfxWorldUnknownBufferGroup, data), align_64, group.count);
				group_visitor.array(S2_ARRAY_FIELD(GfxWorldUnknownBufferGroup, dataA), align_64, group.countA);
				group_visitor.array(S2_ARRAY_FIELD(GfxWorldUnknownBufferGroup, dataB), align_4, group.countB);
			}
		}

		template <typename V>
		void visit_voxel_trees(const V& w)
		{
			const auto& world = w.template as<GfxWorld>();
			w.entries(offsetof(GfxWorld, voxelTree), sizeof(GfxVoxelTree), world.voxelTreeCount, [](const auto& e, std::size_t)
			{
				const auto& tree = e.template as<GfxVoxelTree>();
				e.array(S2_ARRAY_FIELD(GfxVoxelTree, voxelTreeHeader), align_16, 1);
				e.array(S2_ARRAY_FIELD(GfxVoxelTree, voxelTopDownViewNodeArray), align_4, tree.voxelTopDownViewNodeCount);
				e.array(S2_ARRAY_FIELD(GfxVoxelTree, voxelInternalNodeArray), align_4, tree.voxelInternalNodeCount);
				e.array(S2_ARRAY_FIELD(GfxVoxelTree, voxelLeafNodeArray), align_2, tree.voxelLeafNodeCount);
				e.array(S2_ARRAY_FIELD(GfxVoxelTree, lightListArray), align_4, tree.lightListArraySize);
				e.runtime_array(S2_ARRAY_FIELD(GfxVoxelTree, voxelInternalNodeDynamicLightList), align_4, tree.voxelInternalNodeCount);
			});
		}

		template <typename V>
		void visit_light_lists(const V& l)
		{
			const auto& lists = l.template as<GfxWorldLightLists>();
			l.array(S2_ARRAY_FIELD(GfxWorldLightLists, surfaceListOffsets), align_2, 0);
			l.array(S2_ARRAY_FIELD(GfxWorldLightLists, smodelListOffsets), align_2, 0);
			l.array(S2_ARRAY_FIELD(GfxWorldLightLists, lists), align_2, lists.listsSize);
		}

		template <typename V>
		void visit_frustum_lights(const V& w)
		{
			const auto& world = w.template as<GfxWorld>();
			w.aligned_entries(offsetof(GfxWorld, frustumLights), align_16, sizeof(GfxFrustumLights), world.primaryLightCount, [](const auto& e, std::size_t)
			{
				const auto& lights = e.template as<GfxFrustumLights>();
				e.array(S2_ARRAY_FIELD(GfxFrustumLights, indices), align_4, lights.indexCount);
				e.array(S2_ARRAY_FIELD(GfxFrustumLights, vertices), align_4, lights.vertexCount);
			});
		}

		template <typename V>
		void visit_scene_runtime_arrays(const V& w)
		{
			const auto& world = w.template as<GfxWorld>();
			const auto cell_count = world.dpvsPlanes.cellCount;
			const auto cell_words = (cell_count + 31) >> 5;
			const auto non_sun_light_count = world.primaryLightCount - world.lastSunPrimaryLightIndex;
			const auto dyn_model_count = world.dpvsDyn.dynEntClientCount[0];
			const auto dyn_brush_count = world.dpvsDyn.dynEntClientCount[1];

			w.runtime_array(S2_ARRAY_FIELD(GfxWorld, cellCasterBits), align_4, static_cast<std::size_t>(cell_count) * cell_words);
			w.runtime_array(S2_ARRAY_FIELD(GfxWorld, cellHasSunLitSurfsBits), align_4, cell_words);
			w.runtime_array(S2_ARRAY_FIELD(GfxWorld, sceneDynModel), align_4, dyn_model_count);
			w.runtime_array(S2_ARRAY_FIELD(GfxWorld, sceneDynBrush), align_4, dyn_brush_count);
			w.runtime_array(S2_ARRAY_FIELD(GfxWorld, primaryLightEntityShadowVis), align_4,
				static_cast<std::uint32_t>((non_sun_light_count << shadow_vis_light_shift) - shadow_vis_sun_bias));
			w.runtime_array(pointer_offset(offsetof(GfxWorld, primaryLightDynEntShadowVis), 0), sizeof(unsigned int), align_4,
				static_cast<std::uint32_t>(dyn_model_count * (non_sun_light_count - 1)));
			w.runtime_array(pointer_offset(offsetof(GfxWorld, primaryLightDynEntShadowVis), 1), sizeof(unsigned int), align_4,
				static_cast<std::uint32_t>(dyn_brush_count * (non_sun_light_count - 1)));
			w.runtime_array(S2_ARRAY_FIELD(GfxWorld, nonSunPrimaryLightForModelDynEnt), align_2, dyn_model_count);
		}

		template <typename V>
		void visit_shadow_geometry_and_light_region(const V& w)
		{
			const auto& world = w.template as<GfxWorld>();
			const auto visit_shadow_geometry = [](const auto& e, std::size_t)
			{
				const auto& geometry = e.template as<GfxShadowGeometry>();
				e.array(S2_ARRAY_FIELD(GfxShadowGeometry, sortedSurfIndex), align_4, geometry.surfaceCount);
				e.array(S2_ARRAY_FIELD(GfxShadowGeometry, smodelIndex), align_2, geometry.smodelCount);
			};
			w.entries(offsetof(GfxWorld, shadowGeom), sizeof(GfxShadowGeometry), world.primaryLightCount, visit_shadow_geometry);
			w.entries(offsetof(GfxWorld, shadowGeomOptimized), sizeof(GfxShadowGeometry), world.primaryLightCount, visit_shadow_geometry);
			w.entries(offsetof(GfxWorld, lightRegion), sizeof(GfxLightRegion), world.primaryLightCount, [](const auto& e, std::size_t)
			{
				const auto& region = e.template as<GfxLightRegion>();
				e.entries(offsetof(GfxLightRegion, hulls), sizeof(GfxLightRegionHull), region.hullCount, [](const auto& h, std::size_t)
				{
					const auto& hull = h.template as<GfxLightRegionHull>();
					h.array(S2_ARRAY_FIELD(GfxLightRegionHull, axis), align_4, hull.axisCount);
				});
			});
		}

		template <typename V>
		void visit_dpvs_primary_vis_data(const V& d)
		{
			const auto& dpvs = d.template as<GfxWorldDpvsStatic>();

			const auto visit_vis_array = [&](const std::size_t array_offset, const std::size_t index, const std::size_t count)
			{
				d.runtime_array(pointer_offset(array_offset, index), sizeof(unsigned int), align_128, count);
			};

			for (auto i = 0u; i < 3; i++)
			{
				visit_vis_array(offsetof(GfxWorldDpvsStatic, smodelVisData), i, dpvs.smodelVisDataCount);
			}
			for (auto i = 0u; i < 3; i++)
			{
				visit_vis_array(offsetof(GfxWorldDpvsStatic, surfaceVisData), i, dpvs.surfaceVisDataCount);
			}
			for (auto i = 0u; i < std::size(dpvs.smodelVisDataUnused); i++)
			{
				visit_vis_array(offsetof(GfxWorldDpvsStatic, smodelVisDataUnused), i, 0);
			}
			for (auto i = 0u; i < std::size(dpvs.surfaceVisDataUnused); i++)
			{
				visit_vis_array(offsetof(GfxWorldDpvsStatic, surfaceVisDataUnused), i, 0);
			}
			d.runtime_array(S2_ARRAY_FIELD(GfxWorldDpvsStatic, unk_0x2C8), align_128, 2ull * dpvs.surfaceVisDataCount);
		}

		template <typename V>
		void visit_dpvs_secondary_vis_data(const V& d)
		{
			const auto& dpvs = d.template as<GfxWorldDpvsStatic>();

			d.runtime_array(pointer_offset(offsetof(GfxWorldDpvsStatic, smodelVisData), 3), sizeof(unsigned int), align_128, dpvs.smodelVisDataCount);
			d.runtime_array(pointer_offset(offsetof(GfxWorldDpvsStatic, surfaceVisData), 3), sizeof(unsigned int), align_128, dpvs.surfaceVisDataCount);

			for (auto view = 0u; view < unknown_vis_view_count; view++)
			{
				for (auto i = 0u; i < unknown_vis_per_view; i++)
				{
					d.runtime_array(pointer_offset(offsetof(GfxWorldDpvsStatic, smodelUnknownVisData), view * unknown_vis_per_view + i),
						sizeof(unsigned int), align_128, dpvs.smodelVisDataCount);
				}
				for (auto i = 0u; i < unknown_vis_per_view; i++)
				{
					d.runtime_array(pointer_offset(offsetof(GfxWorldDpvsStatic, surfaceUnknownVisData), view * unknown_vis_per_view + i),
						sizeof(unsigned int), align_128, dpvs.surfaceVisDataCount);
				}
			}

			d.runtime_array(S2_ARRAY_FIELD(GfxWorldDpvsStatic, primaryLightVisData), align_128, dpvs.primaryLightVisDataCount);
			d.runtime_array(S2_ARRAY_FIELD(GfxWorldDpvsStatic, unk_0x2D8), align_128, dpvs.smodelVisDataCount);
			d.runtime_array(S2_ARRAY_FIELD(GfxWorldDpvsStatic, unk_0x2E0), align_128, dpvs.surfaceVisDataCount);
		}

		template <typename V>
		void visit_surface(const V& e)
		{
			e.aligned_entries(offsetof(GfxSurface, buffer), align_8, sizeof(GfxSurfaceBuffer), 1, [](const auto& b, std::size_t)
			{
				const auto& buffer = b.template as<GfxSurfaceBuffer>();
				b.unread_bytes(offsetof(GfxSurfaceBuffer, data), align_256, buffer.dataSize);
			});
			e.asset(ASSET_TYPE_MATERIAL, offsetof(GfxSurface, material));
		}

		template <typename V>
		void visit_dpvs_static(const V& w, const V& d)
		{
			const auto& world = w.template as<GfxWorld>();
			const auto& dpvs = d.template as<GfxWorldDpvsStatic>();

			visit_dpvs_primary_vis_data(d);
			d.array(S2_ARRAY_FIELD(GfxWorldDpvsStatic, unk_0x318), align_128, 2ull * dpvs.smodelVisDataCount);
			visit_dpvs_secondary_vis_data(d);

			d.array(S2_ARRAY_FIELD(GfxWorldDpvsStatic, sortedSurfIndex), align_4, dpvs.staticSurfaceCount);
			d.entries(offsetof(GfxWorldDpvsStatic, surfaces), sizeof(GfxSurface), world.surfaceCount, [](const auto& e, std::size_t)
			{
				visit_surface(e);
			});
			d.array(S2_ARRAY_FIELD(GfxWorldDpvsStatic, surfacesBounds), align_4, world.surfaceCount);
			d.runtime_array(S2_ARRAY_FIELD(GfxWorldDpvsStatic, unk_0x300), align_2, world.surfaceCount);

			d.array(S2_ARRAY_FIELD(GfxWorldDpvsStatic, smodelInsts), align_4, dpvs.smodelCount);
			d.entries(offsetof(GfxWorldDpvsStatic, smodelDrawInsts), sizeof(GfxStaticModelDrawInst), dpvs.smodelCount, [](const auto& e, std::size_t)
			{
				e.asset(ASSET_TYPE_XMODEL, offsetof(GfxStaticModelDrawInst, model));
			});
			d.entries(offsetof(GfxWorldDpvsStatic, smodelLightingInsts), sizeof(GfxStaticModelLighting), dpvs.smodelCount, [&](const auto& e, const std::size_t index)
			{
				const auto flags = dpvs.smodelDrawInsts[index].flags;
				if ((flags & STATIC_MODEL_FLAG_VERTEXLIT_LIGHTING) != 0)
				{
					const auto& lighting = e.template as<GfxStaticModelLighting>().vertexLightingInfo;
					e.array(S2_ARRAY_FIELD(GfxStaticModelLighting, vertexLightingInfo.lightingValues), align_2, lighting.numLightingValues);
				}
			});
			d.runtime_array(S2_ARRAY_FIELD(GfxWorldDpvsStatic, unk_0x328), align_128, dpvs.surfaceVisDataCount);
			d.array(S2_ARRAY_FIELD(GfxWorldDpvsStatic, sortedSmodelIndices), align_2, dpvs.smodelCount);
			d.runtime_array(S2_ARRAY_FIELD(GfxWorldDpvsStatic, surfaceCastsSunShadowOpt), align_128,
				static_cast<std::uint32_t>(dpvs.sunShadowOptCount * dpvs.sunSurfVisDataCount));
			d.runtime_array(S2_ARRAY_FIELD(GfxWorldDpvsStatic, surfaceMaterials), align_4, dpvs.staticSurfaceCount);
			d.runtime_array(S2_ARRAY_FIELD(GfxWorldDpvsStatic, unk_0x350), align_8, dpvs.smodelCount);
		}

		template <typename V>
		void visit_dpvs_dynamic(const V& w, const V& d)
		{
			const auto& world = w.template as<GfxWorld>();
			const auto& dyn = d.template as<GfxWorldDpvsDynamic>();
			const auto cell_count = world.dpvsPlanes.cellCount;

			const auto visit_vis_data = [&](const std::size_t set, const std::size_t index)
			{
				d.runtime_array(pointer_offset(offsetof(GfxWorldDpvsDynamic, dynEntVisData), set * std::size(dyn.dynEntVisData[0]) + index),
					sizeof(unsigned char), align_16, dyn_ent_vis_bytes_per_word * dyn.dynEntClientWordCount[set]);
			};

			for (auto set = 0u; set < std::size(dyn.dynEntCellBits); set++)
			{
				d.runtime_array(pointer_offset(offsetof(GfxWorldDpvsDynamic, dynEntCellBits), set), sizeof(unsigned int), align_4,
					static_cast<std::uint32_t>(dyn.dynEntClientWordCount[set] * cell_count));
			}
			for (auto set = 0u; set < std::size(dyn.dynEntVisData); set++)
			{
				for (auto index = 0u; index < 3; index++)
				{
					visit_vis_data(set, index);
				}
			}
			for (auto set = 0u; set < std::size(dyn.dynEntVisData); set++)
			{
				visit_vis_data(set, 3);
			}
		}
	}

	template <typename V>
	void gfx_world_layout::visit(const V& w)
	{
		const auto& world = w.template as<GfxWorld>();

		w.strings({offsetof(GfxWorld, name), offsetof(GfxWorld, baseName)});
		visit_skies(w);
		visit_dpvs_planes(w);
		visit_cells(w);
		visit_portal_groups(w);
		visit_draw(w.sub(offsetof(GfxWorld, draw)));
		visit_light_grid(w.sub(offsetof(GfxWorld, lightGrid)));
		visit_probe_data(w.sub(offsetof(GfxWorld, probeData)));
		visit_unknown_data(w.sub(offsetof(GfxWorld, unknownData)));
		visit_voxel_trees(w);
		visit_light_lists(w.sub(offsetof(GfxWorld, lightLists)));
		visit_frustum_lights(w);
		w.array(S2_ARRAY_FIELD(GfxWorld, models), align_4, world.modelCount);
		w.aligned_entries(offsetof(GfxWorld, materialMemory), align_16, sizeof(MaterialMemory), world.materialMemoryCount, [](const auto& e, std::size_t)
		{
			e.asset(ASSET_TYPE_MATERIAL, offsetof(MaterialMemory, material));
		});
		w.asset(ASSET_TYPE_MATERIAL, offsetof(GfxWorld, sun) + offsetof(sunflare_t, spriteMaterial));
		w.asset(ASSET_TYPE_MATERIAL, offsetof(GfxWorld, sun) + offsetof(sunflare_t, flareMaterial));
		w.asset(ASSET_TYPE_IMAGE, offsetof(GfxWorld, outdoorImage));
		visit_scene_runtime_arrays(w);
		visit_shadow_geometry_and_light_region(w);
		visit_dpvs_static(w, w.sub(offsetof(GfxWorld, dpvs)));
		visit_dpvs_dynamic(w, w.sub(offsetof(GfxWorld, dpvsDyn)));
		w.array(S2_ARRAY_FIELD(GfxWorld, heroOnlyLights), align_4, world.heroOnlyLightCount);
		w.entries(offsetof(GfxWorld, mdaoVolumes), sizeof(MdaoVolume), world.mdaoVolumesCount, [](const auto& e, std::size_t)
		{
			e.asset(ASSET_TYPE_IMAGE, offsetof(MdaoVolume, volumeData));
		});
		w.strings({
			offsetof(GfxWorld, buildInfo) + offsetof(GfxBuildInfo, bspCommandline),
			offsetof(GfxWorld, buildInfo) + offsetof(GfxBuildInfo, lightCommandline),
			offsetof(GfxWorld, buildInfo) + offsetof(GfxBuildInfo, bspTimestamp),
			offsetof(GfxWorld, buildInfo) + offsetof(GfxBuildInfo, lightTimestamp),
		});
	}

	template void gfx_world_layout::visit(const field_reader&);
	template void gfx_world_layout::visit(const field_writer&);
	template void gfx_world_layout::visit(const field_dumper&);
	template void gfx_world_layout::visit(const field_parser&);
	template void gfx_world_layout::visit(const field_dependencies&);
	template void gfx_world_layout::visit(const field_preparer&);
}
