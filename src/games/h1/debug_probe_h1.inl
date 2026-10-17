namespace zonetool::h1
{
	inline void probe_hex_h1(FILE* fp, const void* data, std::size_t size)
	{
		if (!data) { fprintf(fp, "  null\n"); return; }
		const auto p = static_cast<const std::uint32_t*>(data);
		for (std::size_t i = 0; i < size / 4; i++)
		{
			if (i % 8 == 0) fprintf(fp, "\n  %04zX:", i * 4);
			fprintf(fp, " %08X", p[i]);
		}
		fprintf(fp, "\n");
	}

	inline void probe_h1_world(GfxWorld* w)
	{
		const auto fp = fopen("probe_h1world.txt", "wb");
		if (!fp) return;
		fprintf(fp, "world %s\nheader:", w->name);
		probe_hex_h1(fp, w, 176);
		fprintf(fp, "planes %d nodes %d cells %d\nnodes:", w->planeCount, w->nodeCount, w->dpvsPlanes.cellCount);
		for (auto i = 0; i < std::min(w->nodeCount, 64); i++) fprintf(fp, " %u", w->dpvsPlanes.nodes[i]);
		fprintf(fp, "\nplanes:");
		probe_hex_h1(fp, w->dpvsPlanes.planes, 20 * 4);
		fprintf(fp, "sceneEntCellBits %p\n", w->dpvsPlanes.sceneEntCellBits);
		for (auto ci = 0; ci < std::min(w->dpvsPlanes.cellCount, 3); ci++)
		{
			const auto count = w->aabbTreeCounts[ci].aabbTreeCount;
			const auto tree = w->aabbTrees[ci].aabbTree;
			fprintf(fp, "cell %d trees %d portals %u\n", ci, count, w->cells[ci].portalCount);
			auto surf_total = 0u, leaf_surfs = 0u, max_end = 0u;
			for (auto ti = 0; ti < count; ti++)
			{
				const auto& t = tree[ti];
				if (ti < 12) fprintf(fp, "  node %d mid %g %g %g half %g %g %g childOff %d childCount %u smodels %u start %d count %u\n", ti,
					t.bounds.midPoint[0], t.bounds.midPoint[1], t.bounds.midPoint[2], t.bounds.halfSize[0], t.bounds.halfSize[1], t.bounds.halfSize[2],
					t.childrenOffset, t.childCount, t.smodelIndexCount, t.startSurfIndex, t.surfaceCount);
				if (!t.childCount) leaf_surfs += t.surfaceCount;
				max_end = std::max(max_end, static_cast<unsigned>(t.startSurfIndex + t.surfaceCount));
			}
			surf_total = tree ? tree[0].surfaceCount : 0;
			fprintf(fp, "  root surfaces %u leaf surfaces %u max end %u static %u\n", surf_total, leaf_surfs, max_end, w->dpvs.staticSurfaceCount);
		}
		{
			auto checked = 0u, outside = 0u, outside_direct = 0u;
			for (auto ci = 0; ci < w->dpvsPlanes.cellCount; ci++)
			{
				const auto tree = w->aabbTrees[ci].aabbTree;
				for (auto ti = 0; ti < w->aabbTreeCounts[ci].aabbTreeCount; ti++)
				{
					const auto& t = tree[ti];
					if (t.childCount) continue;
					for (auto k = 0u; k < t.surfaceCount; k++)
					{
						const auto inside = [&](const unsigned surf)
						{
							const auto& b = w->dpvs.surfacesBounds[surf].bounds;
							for (auto a = 0; a < 3; a++)
								if (std::fabs(b.midPoint[a] - t.bounds.midPoint[a]) > t.bounds.halfSize[a] + 1.0f) return false;
							return true;
						};
						checked++;
						if (!inside(w->dpvs.sortedSurfIndex[t.startSurfIndex + k])) outside++;
						if (!inside(t.startSurfIndex + k)) outside_direct++;
					}
				}
			}
			fprintf(fp, "aabb leaf surfaces %u, center outside node via sortedSurfIndex %u, direct %u\n", checked, outside, outside_direct);
		}
		fprintf(fp, "draw:");
		probe_hex_h1(fp, &w->draw, sizeof(w->draw));
		for (auto i = 0; i < w->draw.lightmapCount; i++)
			fprintf(fp, "lightmap %s %s\n", w->draw.lightmaps[i].primary ? w->draw.lightmaps[i].primary->name : "null", w->draw.lightmaps[i].secondary ? w->draw.lightmaps[i].secondary->name : "null");
		fprintf(fp, "override %s %s\n", w->draw.lightmapOverridePrimary ? w->draw.lightmapOverridePrimary->name : "null", w->draw.lightmapOverrideSecondary ? w->draw.lightmapOverrideSecondary->name : "null");
		fprintf(fp, "verts:");
		probe_hex_h1(fp, w->draw.vd.vertices, 44 * 4);
		fprintf(fp, "indices:");
		probe_hex_h1(fp, w->draw.indices, 64);
		fprintf(fp, "lightgrid header:");
		probe_hex_h1(fp, &w->lightGrid, 176);
		fprintf(fp, "lightgrid trees:");
		probe_hex_h1(fp, &w->lightGrid.tree, sizeof(w->lightGrid.tree));
		fprintf(fp, "sky colors:");
		probe_hex_h1(fp, &w->lightGrid.skyLightGridColors, sizeof(w->lightGrid.skyLightGridColors));
		fprintf(fp, "default colors:");
		probe_hex_h1(fp, &w->lightGrid.defaultLightGridColors, sizeof(w->lightGrid.defaultLightGridColors));
		fprintf(fp, "cached sky entry %08X %04X avg %f %f %f missing %u\n", w->lightGrid.cachedSkyLightGridEntry.colorsIndex, w->lightGrid.cachedSkyLightGridEntry.primaryLightEnvIndex,
			w->lightGrid.cachedSkyLightEntryAverageColor[0], w->lightGrid.cachedSkyLightEntryAverageColor[1], w->lightGrid.cachedSkyLightEntryAverageColor[2], w->lightGrid.missingGridColorIndex);
		if (w->lightGrid.stageLightingContrastGain) fprintf(fp, "stage gain %f\n", w->lightGrid.stageLightingContrastGain[0]);
		if (w->lightGrid.colors) { fprintf(fp, "colors[0]:"); probe_hex_h1(fp, w->lightGrid.colors, 336); }
		fprintf(fp, "lightGridEntries %u colors %u\n", w->lightGrid.entryCount, w->lightGrid.colorCount);
		fprintf(fp, "models %d:", w->modelCount);
		probe_hex_h1(fp, w->models, 96 * std::min(w->modelCount, 3));
		fprintf(fp, "after models:");
		probe_hex_h1(fp, &w->bounds, 1864 - 1528);
		fprintf(fp, "shadowGeom %p opt %p lightRegion %p primaryLightCount %u\n", w->shadowGeom, w->shadowGeomOptimized, w->lightRegion, w->primaryLightCount);
		if (w->shadowGeom) for (auto i = 0u; i < w->primaryLightCount; i++) fprintf(fp, "  sg %u: %u %u\n", i, w->shadowGeom[i].surfaceCount, w->shadowGeom[i].smodelCount);
		if (w->shadowGeomOptimized) for (auto i = 0u; i < w->primaryLightCount; i++) fprintf(fp, "  sgo %u: %u %u\n", i, w->shadowGeomOptimized[i].surfaceCount, w->shadowGeomOptimized[i].smodelCount);
		if (w->lightRegion) for (auto i = 0u; i < w->primaryLightCount; i++) fprintf(fp, "  lr %u: %u\n", i, w->lightRegion[i].hullCount);
		fprintf(fp, "dpvs:");
		probe_hex_h1(fp, &w->dpvs, sizeof(w->dpvs));
		fprintf(fp, "lodData:");
		probe_hex_h1(fp, w->dpvs.lodData, 4 * 32);
		fprintf(fp, "unk1:");
		probe_hex_h1(fp, w->dpvs.unknownSModelVisData1, 4 * 32);
		fprintf(fp, "unk2:");
		probe_hex_h1(fp, w->dpvs.unknownSModelVisData2, 4 * 32);
		fprintf(fp, "sortedSurfIndex:");
		probe_hex_h1(fp, w->dpvs.sortedSurfIndex, 4 * 32);
		fprintf(fp, "surfaces:");
		probe_hex_h1(fp, w->dpvs.surfaces, 40 * 6);
		for (auto i = 0u; i < std::min(w->surfaceCount, 6u); i++) fprintf(fp, "  mat %s\n", w->dpvs.surfaces[i].material ? w->dpvs.surfaces[i].material->name : "null");
		fprintf(fp, "bounds:");
		probe_hex_h1(fp, w->dpvs.surfacesBounds, 36 * 3);
		fprintf(fp, "smodelInsts:");
		probe_hex_h1(fp, w->dpvs.smodelInsts, 36 * 3);
		fprintf(fp, "drawInsts:");
		probe_hex_h1(fp, w->dpvs.smodelDrawInsts, 80 * 6);
		std::map<unsigned, int> fl;
		for (auto i = 0u; i < w->dpvs.smodelCount; i++) fl[w->dpvs.smodelDrawInsts[i].flags]++;
		fprintf(fp, "drawinst flags:"); for (auto& [k, v] : fl) fprintf(fp, " %X:%d", k, v); fprintf(fp, "\n");
		fprintf(fp, "lighting:");
		probe_hex_h1(fp, w->dpvs.smodelLightingInsts, 24 * 6);
		std::map<unsigned, int> lf, pf;
		for (auto i = 0u; i < w->surfaceCount; i++) { lf[w->dpvs.surfaces[i].laf.fields.flags]++; pf[w->dpvs.surfaces[i].laf.fields.lightmapIndex]++; }
		fprintf(fp, "laf flags:"); for (auto& [k, v] : lf) fprintf(fp, " %X:%d", k, v);
		fprintf(fp, "\nlaf lmap:"); for (auto& [k, v] : pf) fprintf(fp, " %u:%d", k, v); fprintf(fp, "\n");
		fprintf(fp, "dpvsDyn:");
		probe_hex_h1(fp, &w->dpvsDyn, sizeof(w->dpvsDyn));
		{
			const unsigned int* rb = &w->dpvs.litOpaqueSurfsBegin;
			for (auto r = 0; r < 6; r++)
			{
				std::map<std::string, int> regions;
				for (auto i = rb[r * 2]; i < rb[r * 2 + 1]; i++)
				{
					const auto m0 = w->dpvs.surfaces[i].material; const auto m = m0 ? db_find_x_asset_header_safe(ASSET_TYPE_MATERIAL, m0->name).material : nullptr;
					regions[m ? std::to_string(m->cameraRegion) + " sk" + std::to_string(m->info.sortKey) : "null"]++;
				}
				fprintf(fp, "range %d [%X %X):", r, rb[r * 2], rb[r * 2 + 1]);
				for (auto& [k, v] : regions) fprintf(fp, " (%s)x%d", k.c_str(), v);
				fprintf(fp, "\n");
				if (rb[r * 2] < rb[r * 2 + 1]) fprintf(fp, "  first %s\n", w->dpvs.surfaces[rb[r * 2]].material ? w->dpvs.surfaces[rb[r * 2]].material->name : "null");
			}
			std::map<std::string, int> tail;
			for (auto i = w->dpvs.staticSurfaceCount; i < w->surfaceCount; i++)
			{
				const auto m0 = w->dpvs.surfaces[i].material; const auto m = m0 ? db_find_x_asset_header_safe(ASSET_TYPE_MATERIAL, m0->name).material : nullptr;
				tail[m ? std::to_string(m->cameraRegion) + " " + m->name : "null"]++;
			}
			fprintf(fp, "tail surfaces:"); for (auto& [k, v] : tail) fprintf(fp, " (%s)x%d", k.c_str(), v); fprintf(fp, "\n");
			fprintf(fp, "sortedSurfIndex identity:");
			auto ident = true; for (auto i = 0u; i < w->dpvs.staticSurfaceCount; i++) if (w->dpvs.sortedSurfIndex[i] != i) { ident = false; fprintf(fp, " first diff %u -> %u", i, w->dpvs.sortedSurfIndex[i]); break; }
			fprintf(fp, " %d\n", ident);
			std::map<unsigned, int> lr;
			for (auto i = 0u; i < w->dpvs.smodelCount; i++) lr[w->dpvs.smodelDrawInsts[i].primaryLightEnvIndex | (w->dpvs.smodelDrawInsts[i].reflectionProbeIndex << 8)]++;
			fprintf(fp, "di lightenv|probe:"); for (auto& [k, v] : lr) fprintf(fp, " %X:%d", k, v); fprintf(fp, "\n");
			fprintf(fp, "models:");
			for (auto i = 0; i < std::min(w->modelCount, 4); i++) probe_hex_h1(fp, &w->models[i], sizeof(w->models[i]));
		}
		fprintf(fp, "tail:");
		probe_hex_h1(fp, &w->mapVtxChecksum, 0xB30 - 2744);
		fprintf(fp, "build %s | %s | %s | %s\n", w->buildInfo.bspCommandline ? w->buildInfo.bspCommandline : "", w->buildInfo.lightCommandline ? w->buildInfo.lightCommandline : "", w->buildInfo.bspTimestamp ? w->buildInfo.bspTimestamp : "", w->buildInfo.lightTimestamp ? w->buildInfo.lightTimestamp : "");
		fclose(fp);
	}

	template <typename K>
	inline void probe_hist_h1(FILE* fp, const char* label, const std::map<K, int>& h, int limit = 40)
	{
		fprintf(fp, "%s (%zu):", label, h.size());
		auto n = 0;
		for (auto& [k, v] : h) { if (n++ >= limit) { fprintf(fp, " ..."); break; } fprintf(fp, " %llX:%d", static_cast<unsigned long long>(k), v); }
		fprintf(fp, "\n");
	}

	inline void probe_h1_clip_info(FILE* fp, ClipInfo* info)
	{
		fprintf(fp, "planes %d mats %u lbnodes %u leafbrushes %u aabb %d smodels %u smnodes %u sides %u edges %u brushes %u verts %u tris %d borders %d parts %d smlist %u\n",
			info->planeCount, info->numMaterials, info->bCollisionTree.leafbrushNodesCount, info->bCollisionTree.numLeafBrushes, info->pCollisionTree.aabbTreeCount,
			info->sCollisionTree.numStaticModels, info->sCollisionTree.smodelNodeCount, info->bCollisionData.numBrushSides, info->bCollisionData.numBrushEdges,
			info->bCollisionData.numBrushes, info->pCollisionData.vertCount, info->pCollisionData.triCount, info->pCollisionData.borderCount, info->pCollisionData.partitionCount,
			info->sCollisionData.numStaticModels);
		std::map<int, int> mc, mf;
		for (auto i = 0u; i < info->numMaterials; i++) { mc[info->materials[i].contents]++; mf[info->materials[i].surfaceFlags]++; }
		probe_hist_h1(fp, "mat contents", mc);
		probe_hist_h1(fp, "mat flags", mf);
		{
			std::map<int, int> pt, ps, pbad;
			for (auto i = 0; i < info->planeCount; i++)
			{
				const auto& p = info->planes[i];
				const auto bytes = reinterpret_cast<const unsigned char*>(&p);
				auto type = 3;
				for (auto a = 0; a < 3; a++) if (p.normal[a] == 1.0f) type = a;
				auto sign = 0;
				for (auto a = 0; a < 3; a++) if (p.normal[a] < 0.0f) sign |= 1 << a;
				pt[bytes[16]]++; ps[bytes[17]]++;
				pbad[(bytes[16] != type ? 1 : 0) | (bytes[17] != sign ? 2 : 0)]++;
			}
			probe_hist_h1(fp, "plane type", pt);
			probe_hist_h1(fp, "plane sign", ps);
			probe_hist_h1(fp, "plane bad(1 type 2 sign)", pbad);
		}
		for (auto i = 0u; i < std::min(info->numMaterials, 24u); i++) fprintf(fp, "  mat %s %X %X\n", info->materials[i].name, info->materials[i].surfaceFlags, info->materials[i].contents);
		std::map<int, int> lc, ax, lcont, off0, off1;
		for (auto i = 0u; i < info->bCollisionTree.leafbrushNodesCount; i++)
		{
			const auto& n = info->bCollisionTree.leafbrushNodes[i];
			lc[n.leafBrushCount]++; ax[n.axis]++; lcont[n.contents]++;
			if (n.leafBrushCount <= 0) { off0[n.data.children.childOffset[0]]++; off1[n.data.children.childOffset[1]]++; }
		}
		probe_hist_h1(fp, "lbn count", lc);
		probe_hist_h1(fp, "lbn axis", ax);
		probe_hist_h1(fp, "lbn contents", lcont);
		probe_hist_h1(fp, "lbn off0", off0, 20);
		probe_hist_h1(fp, "lbn off1", off1, 20);
		fprintf(fp, "lbn first 24:");
		for (auto i = 0u; i < std::min(info->bCollisionTree.leafbrushNodesCount, 24u); i++)
		{
			const auto& n = info->bCollisionTree.leafbrushNodes[i];
			if (n.leafBrushCount > 0) fprintf(fp, "\n  %u: axis %u count %d contents %X brushes@%lld", i, n.axis, n.leafBrushCount, n.contents, (long long)(n.data.leaf.brushes - info->bCollisionTree.leafbrushes));
			else fprintf(fp, "\n  %u: axis %u count %d contents %X dist %g range %g off %u %u", i, n.axis, n.leafBrushCount, n.contents, n.data.children.dist, n.data.children.range, n.data.children.childOffset[0], n.data.children.childOffset[1]);
		}
		fprintf(fp, "\n");
		std::map<int, int> cc, mi_leaf, mi_inner;
		int max_child_end = 0, max_part = 0;
		for (auto i = 0; i < info->pCollisionTree.aabbTreeCount; i++)
		{
			const auto& t = info->pCollisionTree.aabbTrees[i];
			cc[t.childCount]++;
			if (t.childCount) { mi_inner[t.materialIndex]++; max_child_end = std::max(max_child_end, t.u.firstChildIndex + t.childCount); }
			else { mi_leaf[t.materialIndex]++; max_part = std::max(max_part, t.u.partitionIndex); }
		}
		probe_hist_h1(fp, "aabb childCount", cc);
		probe_hist_h1(fp, "aabb inner mat", mi_inner, 10);
		probe_hist_h1(fp, "aabb leaf mat", mi_leaf, 10);
		fprintf(fp, "aabb max child end %d max part %d\n", max_child_end, max_part);
		fprintf(fp, "aabb first 16:");
		for (auto i = 0; i < std::min(info->pCollisionTree.aabbTreeCount, 16); i++)
		{
			const auto& t = info->pCollisionTree.aabbTrees[i];
			fprintf(fp, "\n  %d: mid %g %g %g half %g %g %g mat %u children %u index %d", i, t.midPoint[0], t.midPoint[1], t.midPoint[2], t.halfSize[0], t.halfSize[1], t.halfSize[2], t.materialIndex, t.childCount, t.u.firstChildIndex);
		}
		fprintf(fp, "\nsmodel nodes:");
		for (auto i = 0; i < std::min<int>(info->sCollisionTree.smodelNodeCount, 40); i++)
		{
			const auto& n = info->sCollisionTree.smodelNodes[i];
			fprintf(fp, "\n  %d: mid %g %g %g half %g %g %g first %u count %u", i, n.bounds.midPoint[0], n.bounds.midPoint[1], n.bounds.midPoint[2], n.bounds.halfSize[0], n.bounds.halfSize[1], n.bounds.halfSize[2], n.firstChild, n.childCount);
		}
		std::map<int, int> ec, gp, bcont;
		for (auto i = 0u; i < info->bCollisionData.numBrushes; i++)
		{
			const auto& b = info->bCollisionData.brushes[i];
			ec[b.edgeCount[0][0]]++; gp[b.glassPieceIndex]++;
			if (info->bCollisionData.brushContents) bcont[info->bCollisionData.brushContents[i]]++;
		}
		fprintf(fp, "\n");
		probe_hist_h1(fp, "brush edgeCount00", ec);
		probe_hist_h1(fp, "brush glass", gp, 5);
		probe_hist_h1(fp, "brush contents", bcont);
		if (info->pCollisionData.partitionCount)
		{
			fprintf(fp, "part0 tri %u border %u seg %u firstTri %d borders@%lld\n", info->pCollisionData.partitions[0].triCount, info->pCollisionData.partitions[0].borderCount,
				info->pCollisionData.partitions[0].firstVertSegment, info->pCollisionData.partitions[0].firstTri,
				info->pCollisionData.partitions[0].borders ? (long long)(info->pCollisionData.partitions[0].borders - info->pCollisionData.borders) : -1ll);
		}
	}

	inline void probe_h1_clip(clipMap_t* c)
	{
		const auto fp = fopen("probe_h1clip.txt", "wb");
		if (!fp) return;
		fprintf(fp, "clip %s nodes %u leafs %u sub %u dyn %u %u\n", c->name, c->numNodes, c->numLeafs, c->numSubModels, c->dynEntCount[0], c->dynEntCount[1]);
		probe_h1_clip_info(fp, &c->info);
		std::map<int, int> ac, bc, tc, lbn;
		for (auto i = 0u; i < c->numLeafs; i++)
		{
			const auto& l = c->leafs[i];
			ac[l.collAabbCount]++; bc[l.brushContents]++; tc[l.terrainContents]++; lbn[l.leafBrushNode]++;
		}
		probe_hist_h1(fp, "leaf aabbCount", ac);
		probe_hist_h1(fp, "leaf brushContents", bc);
		probe_hist_h1(fp, "leaf terrainContents", tc);
		probe_hist_h1(fp, "leaf lbn", lbn, 30);
		fprintf(fp, "leafs first 16:");
		for (auto i = 0u; i < std::min(c->numLeafs, 16u); i++)
		{
			const auto& l = c->leafs[i];
			fprintf(fp, "\n  %u: aabb %u+%u bc %X tc %X lbn %d mid %g %g %g half %g %g %g", i, l.firstCollAabbIndex, l.collAabbCount, l.brushContents, l.terrainContents, l.leafBrushNode,
				l.bounds.midPoint[0], l.bounds.midPoint[1], l.bounds.midPoint[2], l.bounds.halfSize[0], l.bounds.halfSize[1], l.bounds.halfSize[2]);
		}
		fprintf(fp, "\nnodes first 8:");
		for (auto i = 0u; i < std::min(c->numNodes, 8u); i++) fprintf(fp, " [%lld %d %d]", (long long)(c->nodes[i].plane - c->info.planes), c->nodes[i].children[0], c->nodes[i].children[1]);
		fprintf(fp, "\ncmodels:");
		for (auto i = 0u; i < std::min(c->numSubModels, 4u); i++)
		{
			const auto& m = c->cmodels[i];
			fprintf(fp, "\n  %u: radius %g info %s leaf aabb %u+%u bc %X tc %X lbn %d", i, m.radius, m.info == &c->info ? "main" : m.info == c->pInfo ? "pInfo" : m.info ? "other" : "null",
				m.leaf.firstCollAabbIndex, m.leaf.collAabbCount, m.leaf.brushContents, m.leaf.terrainContents, m.leaf.leafBrushNode);
		}
		fprintf(fp, "\npInfo:\n");
		
		fclose(fp);
	}

	inline void probe_h1_com(ComWorld* c)
	{
		const auto fp = fopen("probe_h1com.txt", "wb");
		if (!fp) return;
		fprintf(fp, "com %s lights %u envs %u\n", c->name, c->primaryLightCount, c->primaryLightEnvCount);
		for (auto i = 0u; i < std::min(c->primaryLightCount, 10u); i++)
		{
			const auto& l = c->primaryLights[i];
			fprintf(fp, "light %u %s type %u shadow %u pb %u exp %u state %u color %g %g %g dir %g %g %g radius %g cos %g %g %g rot %g trans %g fade %g %g bulb %g\n", i, l.defName ? l.defName : "null",
				l.type, l.canUseShadowMap, l.physicallyBased, l.exponent, l.lightingState, l.color[0], l.color[1], l.color[2], l.dir[0], l.dir[1], l.dir[2], l.radius,
				l.cosHalfFovOuter, l.cosHalfFovInner, l.cosHalfFovExpanded, l.rotationLimit, l.translationLimit, l.fadeOffset[0], l.fadeOffset[1], l.bulbRadius);
		}
		fclose(fp);
	}

	inline bool probe_tri_z(const float* a, const float* b, const float* c, float x, float y, float& z)
	{
		const auto d = (b[1] - c[1]) * (a[0] - c[0]) + (c[0] - b[0]) * (a[1] - c[1]);
		if (std::fabs(d) < 1e-6f) return false;
		const auto l1 = ((b[1] - c[1]) * (x - c[0]) + (c[0] - b[0]) * (y - c[1])) / d;
		const auto l2 = ((c[1] - a[1]) * (x - c[0]) + (a[0] - c[0]) * (y - c[1])) / d;
		const auto l3 = 1.0f - l1 - l2;
		if (l1 < 0 || l2 < 0 || l3 < 0) return false;
		z = l1 * a[2] + l2 * b[2] + l3 * c[2];
		return true;
	}

	inline int probe_partition_hits(FILE* fp, clipMap_t* c, int pi, float x, float y, const char* tag)
	{
		const auto& pd = c->info.pCollisionData;
		const auto& p = pd.partitions[pi];
		auto hits = 0;
		for (auto t = 0; t < p.triCount; t++)
		{
			const auto tri = p.firstTri + t;
			const float* v[3];
			for (auto k = 0; k < 3; k++)
			{
				const auto vi = pd.triIndices[3 * tri + k] + p.firstVertSegment * 1024u;
				v[k] = vi < pd.vertCount ? pd.verts[vi] : nullptr;
			}
			float z;
			if (v[0] && v[1] && v[2] && probe_tri_z(v[0], v[1], v[2], x, y, z))
			{
				const auto nz = (v[1][0] - v[0][0]) * (v[2][1] - v[0][1]) - (v[1][1] - v[0][1]) * (v[2][0] - v[0][0]);
				if (hits++ < 6 && fp) fprintf(fp, "  %s part %d tri %d z %g nz %g mat %d walk %X\n", tag, pi, tri, z, nz, -1,
					(c->info.pCollisionData.triEdgeIsWalkable[(3 * tri) >> 3] >> ((3 * tri) & 7)) & 7);
			}
		}
		return hits;
	}

	inline int probe_aabb(FILE* fp, clipMap_t* c, int ti, float x, float y, int depth)
	{
		const auto& t = c->info.pCollisionTree.aabbTrees[ti];
		if (std::fabs(x - t.midPoint[0]) > t.halfSize[0] || std::fabs(y - t.midPoint[1]) > t.halfSize[1]) return 0;
		if (!t.childCount) return probe_partition_hits(fp, c, t.u.partitionIndex, x, y, "tree");
		auto hits = 0;
		for (auto i = 0; i < t.childCount; i++) hits += probe_aabb(fp, c, t.u.firstChildIndex + i, x, y, depth + 1);
		return hits;
	}

	inline void probe_h1_ground(clipMap_t* c)
	{
		float x, y, zt;
		if (std::sscanf(std::getenv("ZT_PROBE_GROUND"), "%f %f %f", &x, &y, &zt) != 3) return;
		const auto fp = fopen("probe_h1ground.txt", "wb");
		if (!fp) return;
		fprintf(fp, "point %g %g %g\n", x, y, zt);
		auto brute = 0;
		for (auto pi = 0; pi < c->info.pCollisionData.partitionCount; pi++) brute += probe_partition_hits(fp, c, pi, x, y, "brute");
		fprintf(fp, "brute hits %d\n", brute);
		{
			const auto& bd = c->info.bCollisionData;
			auto brush_hits = 0;
			for (auto bi = 0u; bi < bd.numBrushes; bi++)
			{
				const auto& b = bd.brushBounds[bi];
				if (std::fabs(x - b.midPoint[0]) > b.halfSize[0] || std::fabs(y - b.midPoint[1]) > b.halfSize[1]) continue;
				auto zmin = b.midPoint[2] - b.halfSize[2], zmax = b.midPoint[2] + b.halfSize[2];
				auto inside = true;
				const auto& br = bd.brushes[bi];
				for (auto s = 0; s < br.numsides; s++)
				{
					const auto& pl = c->info.planes[br.sides[s].planeIndex];
					const auto rest = pl.dist - pl.normal[0] * x - pl.normal[1] * y;
					if (pl.normal[2] > 0.0001f) zmax = std::min(zmax, rest / pl.normal[2]);
					else if (pl.normal[2] < -0.0001f) zmin = std::max(zmin, rest / pl.normal[2]);
					else if (rest < 0) inside = false;
				}
				if (!inside || zmin > zmax) continue;
				brush_hits++;
				if (brush_hits <= 40) fprintf(fp, "  brush %u z [%g %g] contents %X sides %u bounds mid %g %g %g half %g %g %g\n", bi, zmin, zmax, bd.brushContents[bi], br.numsides,
					b.midPoint[0], b.midPoint[1], b.midPoint[2], b.halfSize[0], b.halfSize[1], b.halfSize[2]);
			}
			fprintf(fp, "brush hits %d of %u\n", brush_hits, bd.numBrushes);
		}

		const float p[3] = {x, y, zt};
		int idx = 0, steps = 0;
		while (idx >= 0 && steps++ < 4096)
		{
			const auto& n = c->nodes[idx];
			const auto d = n.plane->normal[0] * p[0] + n.plane->normal[1] * p[1] + n.plane->normal[2] * p[2] - n.plane->dist;
			idx = n.children[d >= 0 ? 0 : 1];
		}
		const auto li = -1 - idx;
		fprintf(fp, "leaf %d after %d steps\n", li, steps);
		if (li < 0 || static_cast<unsigned>(li) >= c->numLeafs) { fclose(fp); return; }
		const auto& l = c->leafs[li];
		fprintf(fp, "leaf aabb %u+%u bc %X tc %X lbn %d mid %g %g %g half %g %g %g\n", l.firstCollAabbIndex, l.collAabbCount, l.brushContents, l.terrainContents, l.leafBrushNode,
			l.bounds.midPoint[0], l.bounds.midPoint[1], l.bounds.midPoint[2], l.bounds.halfSize[0], l.bounds.halfSize[1], l.bounds.halfSize[2]);
		auto tree = 0;
		for (auto i = 0u; i < l.collAabbCount; i++) tree += probe_aabb(fp, c, l.firstCollAabbIndex + i, x, y, 0);
		fprintf(fp, "tree hits %d\n", tree);
		const auto tc = c->info.pCollisionTree.aabbTreeCount;
		std::vector<int> parent(tc, -1), leaf_of(tc, -1);
		for (auto i = 0; i < tc; i++)
		{
			const auto& t = c->info.pCollisionTree.aabbTrees[i];
			for (auto k = 0; k < t.childCount; k++) parent[t.u.firstChildIndex + k] = i;
		}
		for (auto i = 0u; i < c->numLeafs; i++) for (auto k = 0u; k < c->leafs[i].collAabbCount; k++) leaf_of[c->leafs[i].firstCollAabbIndex + k] = i;
		for (auto i = 0; i < tc; i++)
		{
			const auto& t = c->info.pCollisionTree.aabbTrees[i];
			if (t.childCount || probe_partition_hits(fp, c, t.u.partitionIndex, x, y, "find") == 0) continue;
			const auto& m = c->info.materials[t.materialIndex];
			fprintf(fp, "mat %u %s flags %X contents %X\n", t.materialIndex, m.name, m.surfaceFlags, m.contents);
			fprintf(fp, "node %d part %d mid %g %g %g half %g %g %g self leaf %d\n", i, t.u.partitionIndex, t.midPoint[0], t.midPoint[1], t.midPoint[2], t.halfSize[0], t.halfSize[1], t.halfSize[2], leaf_of[i]);
			for (auto n = parent[i], d = 0; n >= 0 && d < 40; n = parent[n], d++)
			{
				const auto& pt = c->info.pCollisionTree.aabbTrees[n];
				fprintf(fp, "  up %d mid %g %g %g half %g %g %g leaf %d\n", n, pt.midPoint[0], pt.midPoint[1], pt.midPoint[2], pt.halfSize[0], pt.halfSize[1], pt.halfSize[2], leaf_of[n]);
			}
		}
		for (auto z = zt; z > zt - 200.0f; z -= 4.0f)
		{
			const float q[3] = {x, y, z};
			int n = 0;
			while (n >= 0)
			{
				const auto& nd = c->nodes[n];
				const auto d = nd.plane->normal[0] * q[0] + nd.plane->normal[1] * q[1] + nd.plane->normal[2] * q[2] - nd.plane->dist;
				n = nd.children[d >= 0 ? 0 : 1];
			}
			const auto& ql = c->leafs[-1 - n];
			auto hits = 0;
			for (auto i = 0u; i < ql.collAabbCount; i++) hits += probe_aabb(nullptr, c, ql.firstCollAabbIndex + i, x, y, 0);
			fprintf(fp, " z %g leaf %d aabb %u+%u tc %X hits %d\n", z, -1 - n, ql.firstCollAabbIndex, ql.collAabbCount, ql.terrainContents, hits);
		}
		for (auto i = 0u; i < l.collAabbCount; i++)
		{
			const auto& t = c->info.pCollisionTree.aabbTrees[l.firstCollAabbIndex + i];
			fprintf(fp, " leafroot %u mid %g %g %g half %g %g %g children %u\n", l.firstCollAabbIndex + i, t.midPoint[0], t.midPoint[1], t.midPoint[2], t.halfSize[0], t.halfSize[1], t.halfSize[2], t.childCount);
		}
		fclose(fp);
	}

	inline void probe_h1_portals(GfxWorld* w)
	{
		const auto fp = fopen("probe_h1portals.txt", "wb");
		if (!fp) return;
		const auto cell_count = w->dpvsPlanes.cellCount;
		fprintf(fp, "world %s cells %d nodes %d planes %d\n", w->name, cell_count, w->nodeCount, w->planeCount);
		auto total = 0u, bad_index = 0u, no_reverse = 0u, plus_d = 0u, minus_d = 0u, neither = 0u;
		auto wind_pos = 0u, wind_neg = 0u;
		auto own_pos = 0u, own_neg = 0u, tgt_pos = 0u, tgt_neg = 0u, hull_bad = 0u, zero_hull = 0u, verts_small = 0u;
		std::map<int, int> close_hist;
		const auto center = [&](int c) { return w->cells[c].bounds.midPoint; };
		for (auto c = 0; c < cell_count; c++)
		{
			const auto& cell = w->cells[c];
			const auto m = cell.bounds.midPoint;
			const auto h = cell.bounds.halfSize;
			if (c < 40) fprintf(fp, "cell %d mid %.0f %.0f %.0f half %.0f %.0f %.0f portals %u probes %u refs %u trees %d\n", c, m[0], m[1], m[2], h[0], h[1], h[2], cell.portalCount, cell.reflectionProbeCount, cell.reflectionProbeReferenceCount, w->aabbTreeCounts[c].aabbTreeCount);
			for (auto pi = 0u; pi < cell.portalCount; pi++)
			{
				const auto& p = cell.portals[pi];
				total++;
				const auto n = p.plane.coeffs;
				float r_plus = 0.0f, r_minus = 0.0f;
				for (auto vi = 0; vi < p.vertexCount; vi++)
				{
					const auto v = p.vertices[vi];
					const auto dot = n[0] * v[0] + n[1] * v[1] + n[2] * v[2];
					r_plus = std::max(r_plus, std::fabs(dot + n[3]));
					r_minus = std::max(r_minus, std::fabs(dot - n[3]));
				}
				if (r_plus < 1.0f) plus_d++; else if (r_minus < 1.0f) minus_d++; else neither++;
				if (p.vertexCount < 3) verts_small++;
				else
				{
					float area[3]{};
					for (auto vi = 1; vi + 1 < p.vertexCount; vi++)
					{
						const auto a = p.vertices[0]; const auto b = p.vertices[vi]; const auto cc = p.vertices[vi + 1];
						const float e1[3]{b[0] - a[0], b[1] - a[1], b[2] - a[2]}; const float e2[3]{cc[0] - a[0], cc[1] - a[1], cc[2] - a[2]};
						area[0] += e1[1] * e2[2] - e1[2] * e2[1]; area[1] += e1[2] * e2[0] - e1[0] * e2[2]; area[2] += e1[0] * e2[1] - e1[1] * e2[0];
					}
					const auto wn = area[0] * n[0] + area[1] * n[1] + area[2] * n[2];
					(wn >= 0 ? wind_pos : wind_neg)++;
				}
				const auto side = [&](const float* x) { return n[0] * x[0] + n[1] * x[1] + n[2] * x[2] + (r_plus < 1.0f ? n[3] : -n[3]); };
				const auto s_own = side(m);
				(s_own >= 0 ? own_pos : own_neg)++;
				auto s_tgt = 0.0f;
				if (p.cellIndex >= cell_count) bad_index++;
				else
				{
					s_tgt = side(center(p.cellIndex));
					(s_tgt >= 0 ? tgt_pos : tgt_neg)++;
					const auto& t = w->cells[p.cellIndex];
					auto found = false;
					for (auto k = 0u; k < t.portalCount; k++) if (t.portals[k].cellIndex == c) found = true;
					if (!found) no_reverse++;
				}
				const auto a0 = p.hullAxis[0];
				const auto a1 = p.hullAxis[1];
				const auto len0 = std::sqrt(a0[0] * a0[0] + a0[1] * a0[1] + a0[2] * a0[2]);
				const auto len1 = std::sqrt(a1[0] * a1[0] + a1[1] * a1[1] + a1[2] * a1[2]);
				const auto dn0 = a0[0] * n[0] + a0[1] * n[1] + a0[2] * n[2];
				const auto dn1 = a1[0] * n[0] + a1[1] * n[1] + a1[2] * n[2];
				const auto d01 = a0[0] * a1[0] + a0[1] * a1[1] + a0[2] * a1[2];
				if (len0 == 0.0f && len1 == 0.0f) zero_hull++;
				else if (std::fabs(len0 - 1) > 0.01f || std::fabs(len1 - 1) > 0.01f || std::fabs(dn0) > 0.01f || std::fabs(dn1) > 0.01f || std::fabs(d01) > 0.01f) hull_bad++;
				close_hist[p.closeDistance]++;
				if (total <= 60)
				{
					fprintf(fp, "  portal %u of cell %d -> %u verts %u close %u plane %.3f %.3f %.3f %.1f r+ %.2f r- %.2f own %.1f tgt %.1f hull |%.3f| |%.3f| n.a %.3f %.3f a.a %.3f writable",
						pi, c, p.cellIndex, p.vertexCount, p.closeDistance, n[0], n[1], n[2], n[3], r_plus, r_minus, s_own, s_tgt, len0, len1, dn0, dn1, d01);
					const auto wb = reinterpret_cast<const std::uint32_t*>(&p.writable);
					for (auto k = 0; k < 6; k++) fprintf(fp, " %08X", wb[k]);
					fprintf(fp, "\n");
				}
			}
		}
		fprintf(fp, "\ntotal %u bad_index %u no_reverse %u plane+d %u plane-d %u neither %u verts<3 %u\n", total, bad_index, no_reverse, plus_d, minus_d, neither, verts_small);
		fprintf(fp, "own center side +%u -%u, target center side +%u -%u, hull bad %u zero %u, winding +%u -%u\n", own_pos, own_neg, tgt_pos, tgt_neg, hull_bad, zero_hull, wind_pos, wind_neg);
		fprintf(fp, "closeDistance:");
		auto shown = 0;
		for (const auto& [k, v] : close_hist) { if (shown++ < 30) fprintf(fp, " %d:%d", k, v); }
		fprintf(fp, "\nanchors %u\n", w->portalDistanceAnchorCount);
		for (auto i = 0u; i < std::min(w->portalDistanceAnchorCount, 8u); i++)
		{
			const auto a = w->portalDistanceAnchorsAndCloseDistSquared[i];
			fprintf(fp, "  anchor %u %.1f %.1f %.1f %.1f\n", i, a[0], a[1], a[2], a[3]);
		}

		const auto cell_for_point = [&](const float* p) -> int
		{
			const auto cell_count1 = static_cast<int>(cell_count) + 1;
			auto node = w->dpvsPlanes.nodes;
			auto v = static_cast<int>(node[0]);
			while (v - cell_count1 >= 0)
			{
				const auto& plane = w->dpvsPlanes.planes[v - cell_count1];
				const auto d = p[0] * plane.normal[0] + p[1] * plane.normal[1] + p[2] * plane.normal[2] - plane.dist;
				const auto side = d <= 0.0f ? 1 : 0;
				node = node + side * (static_cast<int>(node[1]) - 2) + 2;
				v = node[0];
			}
			return v - 1;
		};
		float lo[3] = {1e30f, 1e30f, 1e30f}, hi[3] = {-1e30f, -1e30f, -1e30f};
		for (auto c = 0; c < cell_count; c++)
		{
			for (auto k = 0; k < 3; k++)
			{
				lo[k] = std::min(lo[k], w->cells[c].bounds.midPoint[k] - w->cells[c].bounds.halfSize[k]);
				hi[k] = std::max(hi[k], w->cells[c].bounds.midPoint[k] + w->cells[c].bounds.halfSize[k]);
			}
		}
		fprintf(fp, "cell union %.0f %.0f %.0f .. %.0f %.0f %.0f\n", lo[0], lo[1], lo[2], hi[0], hi[1], hi[2]);
		const auto inside = [&](int c, const float* p, float eps)
		{
			const auto& b = w->cells[c].bounds;
			for (auto k = 0; k < 3; k++) if (std::fabs(p[k] - b.midPoint[k]) > b.halfSize[k] + eps) return false;
			return true;
		};
		auto n_none = 0u, n_ok = 0u, n_out = 0u, n_none_in_some = 0u, n_bad_cell = 0u, n_multi = 0u;
		float worst = 0.0f;
		std::map<int, int> out_by_cell;
		constexpr auto steps = 40;
		for (auto ix = 0; ix < steps; ix++) for (auto iy = 0; iy < steps; iy++) for (auto iz = 0; iz < steps; iz++)
		{
			const float p[3] = {lo[0] + (hi[0] - lo[0]) * (ix + 0.5f) / steps, lo[1] + (hi[1] - lo[1]) * (iy + 0.5f) / steps, lo[2] + (hi[2] - lo[2]) * (iz + 0.5f) / steps};
			const auto c = cell_for_point(p);
			auto containing = 0;
			for (auto k = 0; k < cell_count; k++) if (inside(k, p, 0.0f)) containing++;
			if (containing > 1) n_multi++;
			if (c < 0)
			{
				n_none++;
				if (containing) n_none_in_some++;
				continue;
			}
			if (c >= cell_count) { n_bad_cell++; continue; }
			if (inside(c, p, 1.0f)) { n_ok++; continue; }
			n_out++;
			out_by_cell[c]++;
			const auto& b = w->cells[c].bounds;
			for (auto k = 0; k < 3; k++) worst = std::max(worst, std::fabs(p[k] - b.midPoint[k]) - b.halfSize[k]);
		}
		fprintf(fp, "cellForPoint grid %d^3: none %u (inside some cell bounds %u) ok %u outside own bounds %u (worst %.0f) bad %u multi %u\n", steps, n_none, n_none_in_some, n_ok, n_out, worst, n_bad_cell, n_multi);
		for (const auto& [c, v] : out_by_cell) fprintf(fp, "  out cell %d: %d\n", c, v);

		for (auto c = 0; c < cell_count; c++)
		{
			const auto& b = w->cells[c].bounds;
			auto self = 0, overlap = 0, wrong = 0, none = 0;
			std::map<int, int> wrong_cells;
			constexpr auto n = 12;
			for (auto ix = 0; ix < n; ix++) for (auto iy = 0; iy < n; iy++) for (auto iz = 0; iz < n; iz++)
			{
				const float p[3] = {b.midPoint[0] + b.halfSize[0] * (2.0f * (ix + 0.5f) / n - 1.0f), b.midPoint[1] + b.halfSize[1] * (2.0f * (iy + 0.5f) / n - 1.0f), b.midPoint[2] + b.halfSize[2] * (2.0f * (iz + 0.5f) / n - 1.0f)};
				const auto r = cell_for_point(p);
				if (r == c) self++;
				else if (r < 0) none++;
				else if (inside(r, p, 1.0f)) { overlap++; wrong_cells[r]++; }
				else wrong++;
			}
			fprintf(fp, "cell %d half %.0f %.0f %.0f: self %d overlap %d wrong %d none %d |", c, b.halfSize[0], b.halfSize[1], b.halfSize[2], self, overlap, wrong, none);
			auto shown_cells = 0;
			for (const auto& [k, v] : wrong_cells) if (shown_cells++ < 8) fprintf(fp, " %d:%d", k, v);
			fprintf(fp, "\n");
		}
		fclose(fp);
	}
inline void probe_h1_vcol(XModelSurfs* asset)
	{
		static FILE* fp = std::fopen("probe_h1_vcol.txt", "w");
		for (auto s = 0; s < asset->numsurfs; s++)
		{
			const auto surf = &asset->surfs[s];
			if (!surf->verts0.packedVerts0) continue;
			double sum[4]{}; int mn[4]{255,255,255,255}; int mx[4]{};
			for (auto i = 0; i < surf->vertCount; i++)
			{
				const auto c = surf->verts0.packedVerts0[i].color.packed;
				for (auto k = 0; k < 4; k++) { const int b = (c >> (k * 8)) & 0xFF; sum[k] += b; mn[k] = std::min(mn[k], b); mx[k] = std::max(mx[k], b); }
			}
			const auto n = std::max(1, static_cast<int>(surf->vertCount));
			std::fprintf(fp, "%s flags %04X verts %d avg %.0f %.0f %.0f %.0f min %d %d %d %d max %d %d %d %d\n", asset->name, surf->flags, surf->vertCount,
				sum[0] / n, sum[1] / n, sum[2] / n, sum[3] / n, mn[0], mn[1], mn[2], mn[3], mx[0], mx[1], mx[2], mx[3]);
		}
		std::fflush(fp);
	}
inline void probe_h1_world_vcol(GfxWorld* asset)
	{
		FILE* fp = std::fopen("probe_h1_world_vcol.txt", "w");
		std::map<std::uint32_t, int> counts;
		for (auto i = 0u; i < asset->draw.vertexCount; i++) counts[asset->draw.vd.vertices[i].color.packed]++;
		std::vector<std::pair<int, std::uint32_t>> sorted;
		for (const auto& [c, n] : counts) sorted.emplace_back(n, c);
		std::sort(sorted.rbegin(), sorted.rend());
		std::fprintf(fp, "verts %u distinct %zu:", asset->draw.vertexCount, sorted.size());
		for (auto k = 0u; k < std::min<std::size_t>(40, sorted.size()); k++) std::fprintf(fp, " %08X:%d", sorted[k].second, sorted[k].first);
		std::fprintf(fp, "\n");
		std::fclose(fp);
	}
inline void probe_h1_skies(GfxWorld* asset)
	{
		FILE* fp = std::fopen("probe_h1_skies.txt", "w");
		const auto& d = asset->dpvs;
		std::fprintf(fp, "static %u total %u ranges opaque %u-%u unk %u-%u decal %u-%u trans %u-%u caster %u-%u emissive %u-%u\n", d.staticSurfaceCount, asset->surfaceCount,
			d.litOpaqueSurfsBegin, d.litOpaqueSurfsEnd, d.unkSurfsBegin, d.unkSurfsEnd, d.litDecalSurfsBegin, d.litDecalSurfsEnd,
			d.litTransSurfsBegin, d.litTransSurfsEnd, d.shadowCasterSurfsBegin, d.shadowCasterSurfsEnd, d.emissiveSurfsBegin, d.emissiveSurfsEnd);
		{
			std::map<std::string, int> hist;
			for (auto i = 0u; i < d.staticSurfaceCount; i++)
			{
				const auto& sf = d.surfaces[i];
				const auto range = i < d.litOpaqueSurfsEnd ? "opaque" : i < d.litDecalSurfsEnd ? "decal" : i < d.litTransSurfsEnd ? "trans" : "other";
				char key[96];
				std::snprintf(key, sizeof(key), "%s flags %02X unused %02X%02X%02X unk %d lmap %s", range, sf.laf.fields.flags, sf.laf.fields.unused[0], sf.laf.fields.unused[1], sf.laf.fields.unused[2], sf.tris.unk, sf.laf.fields.lightmapIndex == 31 ? "31" : sf.laf.fields.lightmapIndex == 255 ? "255" : "n");
				hist[key]++;
			}
			for (const auto& [k, v] : hist) std::fprintf(fp, "laf %s: %d\n", k.data(), v);
			std::map<int, int> env, probe, lmap, senv, sprobe;
			for (auto i = 0u; i < d.staticSurfaceCount; i++)
			{
				env[d.surfaces[i].laf.fields.primaryLightEnvIndex]++;
				probe[d.surfaces[i].laf.fields.reflectionProbeIndex]++;
				lmap[d.surfaces[i].laf.fields.lightmapIndex]++;
			}
			for (auto i = 0u; i < d.smodelCount; i++)
			{
				senv[d.smodelDrawInsts[i].primaryLightEnvIndex]++;
				sprobe[d.smodelDrawInsts[i].reflectionProbeIndex]++;
			}
			std::fprintf(fp, "probes %u lightmaps %u primaryLights %u lastSun %u\n", asset->draw.reflectionProbeCount, asset->draw.lightmapCount, asset->primaryLightCount, asset->lastSunPrimaryLightIndex);
			std::map<int, int> cull, handle, sunflags, rml;
			for (auto i = 0u; i < d.smodelCount; i++)
			{
				cull[d.smodelDrawInsts[i].cullDist]++;
				handle[d.smodelDrawInsts[i].lightingHandle]++;
				sunflags[d.smodelDrawInsts[i].sunShadowFlags]++;
				rml[d.smodelDrawInsts[i].reactiveMotionCullDist]++;
			}
			probe_hist_h1(fp, "smodel cullDist", cull, 30);
			probe_hist_h1(fp, "smodel lightingHandle", handle, 10);
			probe_hist_h1(fp, "smodel sunShadowFlags", sunflags, 30);
			probe_hist_h1(fp, "smodel reactiveMotionCullDist", rml, 30);
			probe_hist_h1(fp, "surf env", env, 200);
			probe_hist_h1(fp, "surf probe", probe, 200);
			probe_hist_h1(fp, "surf lmap", lmap, 200);
			probe_hist_h1(fp, "smodel env", senv, 200);
			probe_hist_h1(fp, "smodel probe", sprobe, 200);
		}
		for (auto i = 0; i < asset->skyCount; i++)
		{
			const auto& sky = asset->skies[i];
			std::fprintf(fp, "sky %d count %d image %s bounds %.0f %.0f %.0f half %.0f %.0f %.0f\n", i, sky.skySurfCount, sky.skyImage ? sky.skyImage->name : "null",
				sky.bounds.midPoint[0], sky.bounds.midPoint[1], sky.bounds.midPoint[2], sky.bounds.halfSize[0], sky.bounds.halfSize[1], sky.bounds.halfSize[2]);
			for (auto k = 0; k < sky.skySurfCount; k++)
			{
				const auto idx = d.sortedSurfIndex[sky.skyStartSurfs[k]];
				const auto& surf = d.surfaces[idx];
				std::fprintf(fp, "  %d -> %u %s tris %u laf %016llX unk %d\n", sky.skyStartSurfs[k], idx, surf.material ? surf.material->name : "null", surf.tris.triCount, surf.laf.packed, surf.tris.unk);
				const auto pos = sky.skyStartSurfs[k];
				auto root_cells = 0, leaf_cells = 0;
				for (auto c = 0; c < asset->dpvsPlanes.cellCount; c++)
				{
					const auto trees = asset->aabbTrees[c].aabbTree;
					const auto count = asset->aabbTreeCounts[c].aabbTreeCount;
					if (!trees || !count) continue;
					if (pos >= trees[0].startSurfIndex && pos < trees[0].startSurfIndex + trees[0].surfaceCount) root_cells++;
					for (auto t = 0; t < count; t++)
					{
						if (!trees[t].childCount && pos >= trees[t].startSurfIndex && pos < trees[t].startSurfIndex + trees[t].surfaceCount) { leaf_cells++; break; }
					}
				}
				std::fprintf(fp, "    cells %d: root %d leaf %d\n", asset->dpvsPlanes.cellCount, root_cells, leaf_cells);
			}
		}
		std::fclose(fp);
	}
inline float probe_half(unsigned short h)
	{
		const auto e = (h >> 10) & 31, m = h & 1023;
		const auto v = e == 0 ? m / 1024.0f * std::pow(2.0f, -14.0f) : (1.0f + m / 1024.0f) * std::pow(2.0f, e - 15.0f);
		return (h & 0x8000) ? -v : v;
	}

	inline void probe_h1_lighting(GfxWorld* w)
	{
		const auto fp = fopen("probe_h1light.txt", "wb");
		if (!fp) return;
		const auto& g = w->lightGrid;
		const auto& lp = w->draw.lightmapParameters;
		fprintf(fp, "world %s lightmaps %d params %d %d %d %d %d probes %u\n", w->name, w->draw.lightmapCount, lp.lightmapWidthPrimary, lp.lightmapHeightPrimary,
			lp.lightmapWidthSecondary, lp.lightmapHeightSecondary, lp.lightmapModelUnitsPerTexel, w->draw.reflectionProbeCount);
		fprintf(fp, "grid regions %d lowz %d lastSun %u mins %u %u %u maxs %u %u %u entries %u colors %u missing %u table %d palette %d enc %d %d %d stages %u palEntries %u palBits %u\n",
			g.hasLightRegions, g.useSkyForLowZ, g.lastSunPrimaryLightIndex, g.mins[0], g.mins[1], g.mins[2], g.maxs[0], g.maxs[1], g.maxs[2], g.entryCount, g.colorCount,
			g.missingGridColorIndex, g.tableVersion, g.paletteVersion, g.rangeExponent8BitsEncoding, g.rangeExponent12BitsEncoding, g.rangeExponent16BitsEncoding,
			g.stageCount, g.paletteEntryCount, g.paletteBitstreamSize);
		for (auto i = 0u; i < g.stageCount; i++) fprintf(fp, "stage %u gain %g\n", i, g.stageLightingContrastGain[i]);
		fprintf(fp, "skyavg %g %g %g\n", g.cachedSkyLightEntryAverageColor[0], g.cachedSkyLightEntryAverageColor[1], g.cachedSkyLightEntryAverageColor[2]);
		for (auto i = 0; i < 56; i += 11) fprintf(fp, "sky %d %g %g %g default %g %g %g\n", i, probe_half(g.skyLightGridColors.rgb[i][0]), probe_half(g.skyLightGridColors.rgb[i][1]),
			probe_half(g.skyLightGridColors.rgb[i][2]), probe_half(g.defaultLightGridColors.rgb[i][0]), probe_half(g.defaultLightGridColors.rgb[i][1]), probe_half(g.defaultLightGridColors.rgb[i][2]));
		for (auto t = 0; t < 3; t++) fprintf(fp, "tree %d state %u depth %u nodes %d leafs %d min %d %d %d max %d %d %d\n", t, g.tree[t].index, g.tree[t].maxDepth, g.tree[t].nodeCount,
			g.tree[t].leafCount, g.tree[t].coordMinGridSpace[0], g.tree[t].coordMinGridSpace[1], g.tree[t].coordMinGridSpace[2], g.tree[t].coordMaxGridSpace[0],
			g.tree[t].coordMaxGridSpace[1], g.tree[t].coordMaxGridSpace[2]);
		if (g.colors) for (auto c = 0u; c < std::min(g.colorCount, 4u); c++) fprintf(fp, "color %u %u %u %u\n", c, g.colors[c].rgb[0][0], g.colors[c].rgb[0][1], g.colors[c].rgb[0][2]);
		std::map<unsigned, unsigned> flag_counts;
		for (auto i = 0u; i < w->dpvs.smodelCount; i++) flag_counts[w->dpvs.smodelDrawInsts[i].flags & 0x1E0]++;
		for (const auto& [f, c] : flag_counts) fprintf(fp, "smodel flags %X count %u\n", f, c);
		auto shown = 0;
		for (auto i = 0u; i < w->dpvs.smodelCount && shown < 12; i++)
		{
			const auto& d = w->dpvs.smodelDrawInsts[i];
			const auto& l = w->dpvs.smodelLightingInsts[i];
			if (d.flags & 0x20)
			{
				shown++;
				const auto& a = l.ambientLightingInfo;
				fprintf(fp, "ground %u %s ground %g %g %g %g colorIndex %u weight %g env %u\n", i, d.model ? d.model->name : "null", probe_half(a.groundLighting.array[0]),
					probe_half(a.groundLighting.array[1]), probe_half(a.groundLighting.array[2]), probe_half(a.groundLighting.array[3]), a.colorIndex, a.primaryLightWeight, d.primaryLightEnvIndex);
			}
		}
		shown = 0;
		for (auto i = 0u; i < w->dpvs.smodelCount && shown < 6; i++)
		{
			const auto& d = w->dpvs.smodelDrawInsts[i];
			if (d.flags & 0x40)
			{
				shown++;
				const auto& a = w->dpvs.smodelLightingInsts[i].ambientLightingInfo;
				fprintf(fp, "grid %u %s raw %llX colorIndex %u weight %g\n", i, d.model ? d.model->name : "null", a.groundLighting.packed, a.colorIndex, a.primaryLightWeight);
			}
		}
		shown = 0;
		for (auto i = 0u; i < w->dpvs.smodelCount && shown < 6; i++)
		{
			const auto& d = w->dpvs.smodelDrawInsts[i];
			if (d.flags & 0x100)
			{
				shown++;
				const auto& m = w->dpvs.smodelLightingInsts[i].modelLightmapInfo;
				fprintf(fp, "lmap %u %s off %g %g scale %g %g index %u\n", i, d.model ? d.model->name : "null", m.offset[0], m.offset[1], m.scale[0], m.scale[1], m.lightmapIndex);
			}
		}
		fprintf(fp, "sun valid %d sprite %s flare %s\n", w->sun.hasValidData, w->sun.spriteMaterial ? w->sun.spriteMaterial->name : "null", w->sun.flareMaterial ? w->sun.flareMaterial->name : "null");
		std::fclose(fp);
	}

	inline void probe_h1_smodel_bounds(GfxWorld* w)
	{
		const auto fp = fopen("probe_h1_smodel_bounds.txt", "wb");
		for (auto i = 0u; i < w->dpvs.smodelCount; i++)
		{
			const auto& d = w->dpvs.smodelDrawInsts[i];
			const auto& s = w->dpvs.smodelInsts[i];
			fprintf(fp, "%5u %-40s o(%.1f %.1f %.1f) cull %u b(%.1f %.1f %.1f | %.1f %.1f %.1f | %.1f %.1f %.1f)\n", i, d.model ? d.model->name : "null",
				d.placement.origin[0], d.placement.origin[1], d.placement.origin[2], d.cullDist,
				s.mins[0], s.mins[1], s.mins[2], s.maxs[0], s.maxs[1], s.maxs[2], s.lightingOrigin[0], s.lightingOrigin[1], s.lightingOrigin[2]);
			fprintf(fp, "      fl %x lh %u env %u rmcd %u rmlod %u probe %u skin %u sun %u scale %g\n", d.flags, d.lightingHandle, d.primaryLightEnvIndex, d.reactiveMotionCullDist, d.reactiveMotionLOD, d.reflectionProbeIndex, d.firstMtlSkinIndex, d.sunShadowFlags, d.placement.scale);
		}
		fclose(fp);
	}
	inline bool probe_inside(const Bounds& outer, const float* p, float slack)
	{
		for (auto k = 0; k < 3; k++) if (std::fabs(p[k] - outer.midPoint[k]) > outer.halfSize[k] + slack) return false;
		return true;
	}

	inline void probe_h1_cellref(GfxWorld* w)
	{
		const auto fp = fopen("probe_h1_cellref.txt", "wb");
		const auto& dpvs = w->dpvs;
		const auto cell_count = w->dpvsPlanes.cellCount;
		std::vector<std::vector<int>> refs(dpvs.smodelCount);
		for (auto c = 0; c < cell_count; c++)
		{
			const auto trees = w->aabbTrees[c].aabbTree;
			for (auto t = 0; t < w->aabbTreeCounts[c].aabbTreeCount; t++)
			{
				for (auto i = 0; i < trees[t].smodelIndexCount; i++)
				{
					const auto idx = trees[t].smodelIndexes[i];
					if (idx < dpvs.smodelCount && (refs[idx].empty() || refs[idx].back() != c)) refs[idx].push_back(c);
				}
			}
		}
		auto ref_out_mm = 0u, ref_out_mh = 0u, ref_total = 0u;
		for (auto i = 0u; i < dpvs.smodelCount; i++)
		{
			const auto& s = dpvs.smodelInsts[i];
			for (auto c : refs[i])
			{
				const auto& b = w->cells[c].bounds;
				auto ov_mm = true, ov_mh = true;
				for (auto k = 0; k < 3; k++)
				{
					if (s.maxs[k] < b.midPoint[k] - b.halfSize[k] - 1 || s.mins[k] > b.midPoint[k] + b.halfSize[k] + 1) ov_mm = false;
					if (s.mins[k] + s.maxs[k] < b.midPoint[k] - b.halfSize[k] - 1 || s.mins[k] - s.maxs[k] > b.midPoint[k] + b.halfSize[k] + 1) ov_mh = false;
				}
				ref_total++;
				if (!ov_mm) ref_out_mm++;
				if (!ov_mh) ref_out_mh++;
			}
			if (i == 10129 || i == 15331) fprintf(fp, "inst %u a %.0f %.0f %.0f b %.0f %.0f %.0f l %.0f %.0f %.0f\n", i, s.mins[0], s.mins[1], s.mins[2], s.maxs[0], s.maxs[1], s.maxs[2], s.lightingOrigin[0], s.lightingOrigin[1], s.lightingOrigin[2]);
		}
		fprintf(fp, "ref pairs %u not overlapping as minmax %u as midhalf %u\n", ref_total, ref_out_mm, ref_out_mh);
		for (auto c = 0; c < cell_count; c++)
		{
			const auto& b = w->cells[c].bounds;
			fprintf(fp, "cell %d mid %.0f %.0f %.0f half %.0f %.0f %.0f portals %u trees %d\n", c, b.midPoint[0], b.midPoint[1], b.midPoint[2], b.halfSize[0], b.halfSize[1], b.halfSize[2], w->cells[c].portalCount, w->aabbTreeCounts[c].aabbTreeCount);
		}
		fclose(fp);
	}

	inline void probe_h1_vis(GfxWorld* w)
	{
		const auto fp = fopen("probe_h1_vis.txt", "wb");
		const auto& dpvs = w->dpvs;
		const auto cell_count = w->dpvsPlanes.cellCount;
		fprintf(fp, "cells %d nodes %d planes %d surfaces %u static %u smodels %u\n", cell_count, w->nodeCount, w->planeCount, w->surfaceCount, dpvs.staticSurfaceCount, dpvs.smodelCount);
		std::vector<int> smodel_refs(dpvs.smodelCount), surf_refs(w->surfaceCount), sorted_refs(w->surfaceCount);
		auto sm_out = 0u, sm_total = 0u, sorted_out = 0u, direct_out = 0u, surf_total = 0u, sm_cell_out = 0u, bad_idx = 0u;
		for (auto c = 0; c < cell_count; c++)
		{
			const auto count = w->aabbTreeCounts[c].aabbTreeCount;
			const auto trees = w->aabbTrees[c].aabbTree;
			for (auto t = 0; t < count; t++)
			{
				const auto& tree = trees[t];
				for (auto i = 0; i < tree.smodelIndexCount; i++)
				{
					const auto idx = tree.smodelIndexes[i];
					if (idx >= dpvs.smodelCount) { bad_idx++; continue; }
					sm_total++;
					smodel_refs[idx]++;
					if (!probe_inside(tree.bounds, dpvs.smodelInsts[idx].mins, 1.0f)) sm_out++;
					if (!probe_inside(w->cells[c].bounds, dpvs.smodelInsts[idx].mins, 1.0f)) sm_cell_out++;
				}
				for (auto i = 0; i < tree.surfaceCount; i++)
				{
					const auto pos = tree.startSurfIndex + i;
					if (pos >= static_cast<int>(dpvs.staticSurfaceCount)) { bad_idx++; continue; }
					surf_total++;
					const auto sorted = dpvs.sortedSurfIndex[pos];
					sorted_refs[sorted]++;
					surf_refs[pos]++;
					if (!probe_inside(tree.bounds, dpvs.surfacesBounds[sorted].bounds.midPoint, 1.0f)) sorted_out++;
					if (!probe_inside(tree.bounds, dpvs.surfacesBounds[pos].bounds.midPoint, 1.0f)) direct_out++;
				}
			}
		}
		auto sm_unref = 0u, sorted_unref = 0u, direct_unref = 0u;
		for (auto i = 0u; i < dpvs.smodelCount; i++) if (!smodel_refs[i])
		{
			sm_unref++;
			if (sm_unref < 20) fprintf(fp, "unref smodel %u %s\n", i, dpvs.smodelDrawInsts[i].model ? dpvs.smodelDrawInsts[i].model->name : "null");
		}
		for (auto i = 0u; i < dpvs.staticSurfaceCount; i++) { if (!sorted_refs[i]) sorted_unref++; if (!surf_refs[i]) direct_unref++; }
		fprintf(fp, "tree smodel refs %u outside tree %u outside cell %u unref %u bad %u\n", sm_total, sm_out, sm_cell_out, sm_unref, bad_idx);
		fprintf(fp, "tree surf refs %u outside(sorted) %u outside(direct) %u unref(sorted) %u unref(direct) %u\n", surf_total, sorted_out, direct_out, sorted_unref, direct_unref);

		{
			const char* interp_names[3] = { "byte_from_node", "index", "byte_from_base" };
			unsigned int bad_range[3]{}, child_out[3]{}, sm_not_subset[3]{}, surf_not_subset[3]{}, ok[3]{};
			auto parents = 0u, root_missing_sm = 0u, root_missing_surf = 0u, reach_missing = 0u, leaf_sm = 0u, max_children = 0u, max_depth = 0u;
			for (auto c = 0; c < cell_count; c++)
			{
				const auto count = w->aabbTreeCounts[c].aabbTreeCount;
				const auto trees = w->aabbTrees[c].aabbTree;
				if (count <= 0) continue;
				std::unordered_set<unsigned short> root_set, cell_set;
				for (auto i = 0; i < trees[0].smodelIndexCount; i++) root_set.insert(trees[0].smodelIndexes[i]);
				for (auto t = 0; t < count; t++)
				{
					const auto& node = trees[t];
					for (auto i = 0; i < node.smodelIndexCount; i++) cell_set.insert(node.smodelIndexes[i]);
					if (node.childCount == 0) { leaf_sm += node.smodelIndexCount; continue; }
					parents++;
					max_children = std::max<unsigned int>(max_children, node.childCount);
					std::unordered_set<unsigned short> parent_set(node.smodelIndexes, node.smodelIndexes + node.smodelIndexCount);
					for (auto k = 0; k < 3; k++)
					{
						const GfxAabbTree* first = nullptr;
						if (k == 0) first = reinterpret_cast<const GfxAabbTree*>(reinterpret_cast<const char*>(&node) + node.childrenOffset);
						if (k == 1) first = trees + node.childrenOffset;
						if (k == 2) first = reinterpret_cast<const GfxAabbTree*>(reinterpret_cast<const char*>(trees) + node.childrenOffset);
						const auto first_index = first - trees;
						if (first_index < 0 || first_index + node.childCount > count) { bad_range[k]++; continue; }
						auto good = true;
						for (auto i = 0; i < node.childCount; i++)
						{
							const auto& child = first[i];
							for (auto a = 0; a < 3; a++)
							{
								if (std::fabs(child.bounds.midPoint[a] - node.bounds.midPoint[a]) + child.bounds.halfSize[a] > node.bounds.halfSize[a] + 1.0f) { child_out[k]++; good = false; break; }
							}
							for (auto i2 = 0; i2 < child.smodelIndexCount; i2++) if (!parent_set.contains(child.smodelIndexes[i2])) { sm_not_subset[k]++; good = false; break; }
							if (child.startSurfIndex < node.startSurfIndex || child.startSurfIndex + child.surfaceCount > node.startSurfIndex + node.surfaceCount) { surf_not_subset[k]++; good = false; }
						}
						if (good) ok[k]++;
					}
				}
				for (const auto s : cell_set) if (!root_set.contains(s)) root_missing_sm++;
				for (auto t = 1; t < count; t++)
				{
					const auto& node = trees[t];
					if (node.startSurfIndex < trees[0].startSurfIndex || node.startSurfIndex + node.surfaceCount > trees[0].startSurfIndex + trees[0].surfaceCount) root_missing_surf++;
				}
				std::vector<char> reached(count, 0);
				std::vector<std::pair<int, unsigned int>> stack{ { 0, 0u } };
				while (!stack.empty())
				{
					const auto [t, depth] = stack.back(); stack.pop_back();
					if (t < 0 || t >= count || reached[t]) continue;
					reached[t] = 1;
					max_depth = std::max(max_depth, depth);
					const auto& node = trees[t];
					if (!node.childCount) continue;
					const auto first = reinterpret_cast<const GfxAabbTree*>(reinterpret_cast<const char*>(&node) + node.childrenOffset) - trees;
					for (auto i = 0; i < node.childCount; i++) stack.push_back({ static_cast<int>(first + i), depth + 1 });
				}
				for (auto t = 0; t < count; t++) if (!reached[t]) reach_missing++;
			}
			fprintf(fp, "aabb parents %u max children %u max depth %u leaf smodel refs %u root missing smodels %u root missing surf nodes %u unreachable nodes %u\n",
				parents, max_children, max_depth, leaf_sm, root_missing_sm, root_missing_surf, reach_missing);
			for (auto k = 0; k < 3; k++) fprintf(fp, "aabb %s ok %u bad range %u child outside %u smodel not subset %u surf not subset %u\n",
				interp_names[k], ok[k], bad_range[k], child_out[k], sm_not_subset[k], surf_not_subset[k]);
		}

		auto portal_total = 0u, off_plane = 0u, bad_cell = 0u, no_back = 0u, plane_faces_out = 0u;
		for (auto c = 0; c < cell_count; c++)
		{
			const auto& cell = w->cells[c];
			for (auto p = 0; p < cell.portalCount; p++)
			{
				const auto& portal = cell.portals[p];
				portal_total++;
				auto max_d = 0.0f;
				float centroid[3]{};
				for (auto v = 0; v < portal.vertexCount; v++)
				{
					const auto* x = portal.vertices[v];
					const auto d = x[0] * portal.plane.coeffs[0] + x[1] * portal.plane.coeffs[1] + x[2] * portal.plane.coeffs[2] + portal.plane.coeffs[3];
					max_d = std::max(max_d, std::fabs(d));
					for (auto k = 0; k < 3; k++) centroid[k] += x[k] / portal.vertexCount;
				}
				if (max_d > 1.0f) off_plane++;
				if (portal.cellIndex >= cell_count) { bad_cell++; continue; }
				const auto cd = cell.bounds.midPoint[0] * portal.plane.coeffs[0] + cell.bounds.midPoint[1] * portal.plane.coeffs[1] + cell.bounds.midPoint[2] * portal.plane.coeffs[2] + portal.plane.coeffs[3];
				const auto& target = w->cells[portal.cellIndex];
				const auto td = target.bounds.midPoint[0] * portal.plane.coeffs[0] + target.bounds.midPoint[1] * portal.plane.coeffs[1] + target.bounds.midPoint[2] * portal.plane.coeffs[2] + portal.plane.coeffs[3];
				if (td > cd) plane_faces_out++;
				auto back = false;
				for (auto q = 0; q < target.portalCount; q++) if (target.portals[q].cellIndex == c) back = true;
				if (!back) no_back++;
				if (portal_total < 12)
				{
					fprintf(fp, "portal cell %d -> %u verts %u plane %.3f %.3f %.3f %.1f maxd %.2f cd %.1f td %.1f close %u hull %.2f %.2f %.2f / %.2f %.2f %.2f\n", c, portal.cellIndex, portal.vertexCount,
						portal.plane.coeffs[0], portal.plane.coeffs[1], portal.plane.coeffs[2], portal.plane.coeffs[3], max_d, cd, td, portal.closeDistance,
						portal.hullAxis[0][0], portal.hullAxis[0][1], portal.hullAxis[0][2], portal.hullAxis[1][0], portal.hullAxis[1][1], portal.hullAxis[1][2]);
				}
			}
		}
		fprintf(fp, "portals %u off_plane %u bad_cell %u no_back %u target_on_positive_side %u\n", portal_total, off_plane, bad_cell, no_back, plane_faces_out);

		auto samples = 0u, outside = 0u, nocell = 0u;
		const auto& wb = w->bounds;
		for (auto x = 0; x < 40; x++) for (auto y = 0; y < 40; y++) for (auto z = 0; z < 10; z++)
		{
			const float p[3]{ wb.midPoint[0] + wb.halfSize[0] * (x / 19.5f - 1.0f), wb.midPoint[1] + wb.halfSize[1] * (y / 19.5f - 1.0f), wb.midPoint[2] + wb.halfSize[2] * (z / 4.5f - 1.0f) };
			auto node = w->dpvsPlanes.nodes;
			auto v = static_cast<int>(node[0]) - (cell_count + 1);
			while (v >= 0)
			{
				const auto& plane = w->dpvsPlanes.planes[v];
				const auto d = p[0] * plane.normal[0] + p[1] * plane.normal[1] + p[2] * plane.normal[2] - plane.dist;
				const auto side = d <= 0.0f ? 1 : 0;
				node += side * (node[1] - 2) + 2;
				v = static_cast<int>(node[0]) - (cell_count + 1);
			}
			const auto cell = static_cast<int>(node[0]) - 1;
			samples++;
			if (cell < 0) { nocell++; continue; }
			if (!probe_inside(w->cells[cell].bounds, p, 1.0f)) outside++;
		}
		fprintf(fp, "bsp samples %u nocell %u outside_cell_bounds %u\n", samples, nocell, outside);
		fclose(fp);
	}
}
