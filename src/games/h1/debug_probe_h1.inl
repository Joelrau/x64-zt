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
}
