		void probe_surfs(source::XModelSurfs* asset)
		{
			static FILE* fp = fopen("probe.txt", "wb");
			fprintf(fp, "surfs %s %d\n", asset->name, asset->numsurfs);
			for (auto i = 0; i < asset->numsurfs; i++)
			{
				const auto s = &asset->surfs[i];
				fprintf(fp, " surf %d flags %X verts %d tris %d rigid %d\n  ", i, s->flags, s->vertCount, s->triCount, s->rigidVertListCount);
				const auto b = reinterpret_cast<unsigned char*>(s);
				for (auto j = 0; j < 0x68; j++) fprintf(fp, "%02X%s", b[j], (j % 8 == 7) ? " " : "");
				fprintf(fp, "\n");
				if (s->shared)
				{
					const auto sb = reinterpret_cast<unsigned char*>(s->shared);
					fprintf(fp, "  shared flags %X size %X data %p:\n  ", s->shared->flags, s->shared->dataSize, s->shared->data);
					for (auto j = 0; j < 0x48; j++) fprintf(fp, "%02X%s", sb[j], (j % 8 == 7) ? " " : "");
					fprintf(fp, "\n");
					if (s->shared->data && !(s->shared->flags & 1))
					{
						const auto d = reinterpret_cast<unsigned char*>(s->shared->data);
						fprintf(fp, "  data:\n");
						for (auto j = 0u; j < std::min(s->shared->dataSize, 0x40u); j++) fprintf(fp, "%02X%s", d[j], (j % 32 == 31) ? "\n" : ((j % 4 == 3) ? " " : ""));
						fprintf(fp, "\n");
						static int bin_index = 0;
						static std::unordered_map<int, int> per_flag;
						if (per_flag[s->flags]++ < 4)
						{
							const auto bin = std::format("probe_{}_{:X}_{}_{}.bin", bin_index++, s->flags, s->vertCount, s->triCount);
							FILE* bf = fopen(bin.data(), "wb");
							fwrite(d, 1, s->shared->dataSize, bf);
							fclose(bf);
							const auto vbin = "v" + bin;
							bf = fopen(vbin.data(), "wb");
							if (s->verts) fwrite(s->verts, 16, s->vertCount, bf);
							fclose(bf);
						}
						const auto vo = (s->triCount * 6u + 7u) & ~7u;
						for (auto v = 0; v < 3; v++)
						{
							const auto vb = d + vo + v * 32;
							fprintf(fp, "  sv%d: ", v);
							for (auto j = 0; j < 32; j++) fprintf(fp, "%02X%s", vb[j], (j % 4 == 3) ? " " : "");
							const auto f = reinterpret_cast<const float*>(vb);
							fprintf(fp, " | %g %g %g %g\n", f[0], f[1], f[2], f[3]);
						}
					}
				}
				if (s->verts)
				{
					for (auto v = 0; v < 3; v++) fprintf(fp, "  v%d: %g %g %g %g\n", v, s->verts[v][0], s->verts[v][1], s->verts[v][2], s->verts[v][3]);
				}
				if (s->rigidVertLists)
				{
					for (auto r = 0; r < s->rigidVertListCount; r++)
					{
						const auto rb = reinterpret_cast<unsigned char*>(&s->rigidVertLists[r]);
						fprintf(fp, "  rigid %d: ", r);
						for (auto j = 0; j < 8; j++) fprintf(fp, "%02X", rb[j]);
						fprintf(fp, "\n");
					}
				}
			}
			fflush(fp);
		}

		void probe_techset(source::MaterialTechniqueSet* asset)
		{
			static FILE* fp = fopen("probe_tech.txt", "wb");
			static std::unordered_set<std::string> shaders;
			fprintf(fp, "techset %s flags %X wvf %d\n", asset->name, asset->flags, asset->worldVertFormat);
			for (auto t = 0; t < source::technique_count; t++)
			{
				const auto tech = asset->techniques[t];
				if (!tech) continue;
				for (auto p = 0; p < tech->hdr.passCount; p++)
				{
					const auto& pass = tech->passArray[p];
					fprintf(fp, " t%d %s vs %s decl %s", t, tech->hdr.name, pass.vertexShader ? pass.vertexShader->name : "-", pass.vertexDecl ? pass.vertexDecl->name : "-");
					if (pass.vertexDecl)
					{
						fprintf(fp, " sc %d opt %d:", pass.vertexDecl->streamCount, pass.vertexDecl->hasOptionalSource);
						for (auto r = 0; r < pass.vertexDecl->streamCount && r < 32; r++)
						{
							const auto& d = pass.vertexDecl->routing.data[r];
							fprintf(fp, " %d>%d/%X", d.source, d.dest, d.mask);
						}
					}
					fprintf(fp, "\n");
					if (pass.vertexShader && pass.vertexShader->prog.loadDef.program && shaders.insert(pass.vertexShader->name).second)
					{
						const auto bin = std::format("vs/{}.cso", pass.vertexShader->name);
						std::filesystem::create_directories("vs");
						FILE* bf = fopen(bin.data(), "wb");
						if (bf)
						{
							fwrite(pass.vertexShader->prog.loadDef.program, 1, pass.vertexShader->prog.loadDef.programSize, bf);
							fclose(bf);
						}
					}
				}
			}
			fflush(fp);
		}

		void probe_xmodel(source::XModel* asset)
		{
			static FILE* fp = fopen("probe_model.txt", "wb");
			fprintf(fp, "model %s flags %X variants %d mats %d\n  ", asset->name, asset->flags, asset->variantCount, asset->numMaterials);
			const auto mb = reinterpret_cast<unsigned char*>(asset);
			for (auto j = 0; j < 0x30; j++) fprintf(fp, "%02X%s", mb[j], (j % 8 == 7) ? " " : "");
			fprintf(fp, "\n  mats:");
			for (auto i = 0; i < asset->numMaterials; i++) fprintf(fp, " [%d %s]", asset->materialIndices ? asset->materialIndices[i] : -1, asset->materialHandles && asset->materialHandles[i] ? asset->materialHandles[i]->name : "-");
			fprintf(fp, "\n");
			if (asset->flags & 0x200)
			{
				for (auto i = 0; i < asset->variantCount; i++) fprintf(fp, "  variant %s\n", asset->variants[i].model ? asset->variants[i].model->name : "-");
				fflush(fp);
				return;
			}
			const auto b = asset->base;
			if (!b) return;
			fprintf(fp, "  base %s bones %d root %d client %d collsurfs %d\n", b->name, b->numBones, b->numRootBones, b->numClientBones, b->collSurfCount);
			const auto bb = reinterpret_cast<unsigned char*>(b);
			const auto dumprange = [&](int from, int to)
			{
				fprintf(fp, "  [%X..%X]\n", from, to);
				for (auto j = from; j < to; j += 4)
				{
					float f; memcpy(&f, bb + j, 4);
					unsigned int u; memcpy(&u, bb + j, 4);
					fprintf(fp, "   +%03X %08X %g\n", j, u, f);
				}
			};
			dumprange(0x8, 0x58);
			for (auto l = 0; l < 6; l++)
			{
				const auto& lod = b->lodInfo[l];
				const auto lb = reinterpret_cast<const unsigned char*>(&lod);
				fprintf(fp, "  lod%d dist %g surfs %s :", l, lod.dist, lod.modelSurfs ? lod.modelSurfs->name : "-");
				for (auto j = 0; j < 0x50; j++) fprintf(fp, "%02X%s", lb[j], (j % 8 == 7) ? " " : "");
				fprintf(fp, "\n");
			}
			dumprange(0x268, 0x270);
			dumprange(0x278, 0x280);
			dumprange(0x288, 0x2DA);
			dumprange(0x2F0, 0x2F0);
			dumprange(0x300, 0x308);
			dumprange(0x360, 0x378);
			if (b->collSurfs)
			{
				for (auto c = 0; c < b->collSurfCount && c < 4; c++)
				{
					const auto cb = reinterpret_cast<const unsigned char*>(b->collSurfs + c * 48);
					fprintf(fp, "  coll%d:", c);
					for (auto j = 0; j < 48; j += 4) { unsigned int u; memcpy(&u, cb + j, 4); float f; memcpy(&f, cb + j, 4); fprintf(fp, " %08X(%g)", u, f); }
					fprintf(fp, "\n");
				}
			}
			if (b->boneInfo)
			{
				const auto cb = reinterpret_cast<const unsigned char*>(b->boneInfo);
				fprintf(fp, "  boneinfo0:");
				for (auto j = 0; j < 28; j += 4) { float f; memcpy(&f, cb + j, 4); fprintf(fp, " %g", f); }
				fprintf(fp, "\n");
			}
			fflush(fp);
		}

		void probe_xmodel_base_extra(source::XModel* asset)
		{
			static FILE* fp = fopen("probe_bones.txt", "wb");
			if ((asset->flags & 0x200) || !asset->base) return;
			const auto b = asset->base;
			const auto bone_count = b->numBones + b->numClientBones;
			fprintf(fp, "model %s bones %d root %d client %d\n  parents:", asset->name, b->numBones, b->numRootBones, b->numClientBones);
			for (auto i = 0; i < bone_count - b->numRootBones && i < 40; i++) fprintf(fp, " %d", b->parentList[i]);
			fprintf(fp, "\n  names:");
			for (auto i = 0; i < bone_count && i < 40; i++) fprintf(fp, " %s", SL_ConvertToString(b->boneNames[i]));
			fprintf(fp, "\n  angles:");
			for (auto i = 0; i < bone_count - b->numRootBones && i < 4; i++) fprintf(fp, " (%d %d %d %d)", b->tagAngles[i * 4], b->tagAngles[i * 4 + 1], b->tagAngles[i * 4 + 2], b->tagAngles[i * 4 + 3]);
			fprintf(fp, "\n  pos:");
			for (auto i = 0; i < bone_count - b->numRootBones && i < 4; i++) fprintf(fp, " (%g %g %g)", b->tagPositions[i * 3], b->tagPositions[i * 3 + 1], b->tagPositions[i * 3 + 2]);
			fprintf(fp, "\n  class:");
			for (auto i = 0; i < b->numBones && i < 40; i++) fprintf(fp, " %d", b->partClassification ? b->partClassification[i] : -1);
			fprintf(fp, "\n  basemat0:");
			for (auto i = 0; i < 8 && b->baseMat; i++) { float f; memcpy(&f, b->baseMat + 32 + i * 4, 4); fprintf(fp, " %g", f); }
			fprintf(fp, "\n");
			for (auto l = 0; l < 6; l++)
			{
				const auto surfs = b->lodInfo[l].modelSurfs;
				if (!surfs) continue;
				for (auto s = 0; s < surfs->numsurfs; s++)
				{
					const auto& surf = surfs->surfs[s];
					for (auto r = 0; r < surf.rigidVertListCount; r++)
					{
						const auto rb = reinterpret_cast<const unsigned short*>(&surf.rigidVertLists[r]);
						fprintf(fp, "  lod%d surf%d rigid%d: %d %d %d %d", l, s, r, rb[0], rb[1], rb[2], rb[3]);
						if (const auto tree = surf.rigidVertLists[r].collisionTree)
						{
							const auto tf = reinterpret_cast<const float*>(tree);
							fprintf(fp, " tree %g %g %g %g %g %g n%d l%d", tf[0], tf[1], tf[2], tf[3], tf[4], tf[5], tree->nodeCount, tree->leafCount);
						}
						fprintf(fp, "\n");
					}
				}
				const auto sb = reinterpret_cast<const unsigned char*>(surfs);
				fprintf(fp, "  lod%d surfs pad:", l);
				for (auto j = 0x12; j < 0x50; j++) fprintf(fp, "%02X", sb[j]);
				fprintf(fp, "\n");
			}
			fflush(fp);
		}


		void probe_hex(FILE* fp, const void* data, const std::size_t size, const std::size_t base = 0)
		{
			const auto b = static_cast<const unsigned char*>(data);
			for (std::size_t i = 0; i < size; i += 4)
			{
				if (i % 32 == 0) fprintf(fp, "\n  %04zX:", base + i);
				std::uint32_t v{};
				std::memcpy(&v, b + i, std::min<std::size_t>(4, size - i));
				fprintf(fp, " %08X", v);
			}
			fprintf(fp, "\n");
		}

		void probe_gfxworld(source::GfxWorld* asset)
		{
			FILE* fp = fopen("probe_world.txt", "wb");
			const auto w = reinterpret_cast<unsigned char*>(asset);
			const auto at32 = [&](std::size_t o) { return *reinterpret_cast<std::uint32_t*>(w + o); };
			const auto atp = [&](std::size_t o) { return *reinterpret_cast<unsigned char**>(w + o); };
			fprintf(fp, "world %s base %s\n", asset->name, asset->baseName);
			probe_hex(fp, w, sizeof(*asset));
			const auto surf_count = at32(28);
			const auto dpvs = 3240;
			fprintf(fp, "surface count %u smodel count %u\n", surf_count, at32(dpvs));
			if (const auto s = atp(dpvs + 752)) { fprintf(fp, "surfaces:"); for (auto i = 0u; i < std::min(surf_count, 12u); i++) probe_hex(fp, s + i * 56, 56); }
			if (const auto s = atp(dpvs + 760)) { fprintf(fp, "surface bounds:"); probe_hex(fp, s, 64 * 3); }
			if (const auto s = atp(dpvs + 776)) { fprintf(fp, "smodel insts:"); probe_hex(fp, s, 36 * 3); }
			if (const auto s = atp(dpvs + 784))
			{
				fprintf(fp, "smodel draw insts:");
				for (auto i = 0u; i < std::min(at32(dpvs), 6u); i++)
				{
					probe_hex(fp, s + i * 128, 128);
					const auto m = *reinterpret_cast<source::XModel**>(s + i * 128 + 72);
					fprintf(fp, "  model %s\n", m ? m->name : "null");
				}
			}
			const auto img = [&](const char* tag, source::GfxImage* im)
			{
				if (!im) { fprintf(fp, "%s null\n", tag); return; }
				fprintf(fp, "%s %s fmt %d map %d %dx%dx%d el %d lv %d len %u sem %d cat %d flags %X\n", tag, im->name, im->imageFormat, im->mapType, im->width, im->height, im->depth, im->numElements, im->levelCount, im->dataLen1, im->semantic, im->category, im->flags);
			};
			const auto draw = 192;
			for (auto i = 0u; i < at32(draw + 56); i++) img("lightmap", reinterpret_cast<source::GfxImage**>(atp(draw + 64))[i]);
			if (atp(draw + 72)) { fprintf(fp, "lightmap info:"); probe_hex(fp, atp(draw + 72), 116 * at32(draw + 56)); }
			img("draw80", reinterpret_cast<source::GfxImage*>(atp(draw + 80)));
			for (auto i = 0u; i < at32(draw + 156); i++)
			{
				img("override0", reinterpret_cast<source::GfxImage**>(atp(draw + 160) + i * 16)[0]);
				img("override1", reinterpret_cast<source::GfxImage**>(atp(draw + 160) + i * 16)[1]);
			}
			if (const auto s = atp(dpvs + 752))
			{
				std::map<std::uint32_t, int> lm, pr;
				for (auto i = 0u; i < surf_count; i++) { lm[s[i * 56 + 40]]++; pr[s[i * 56 + 41]]++; }
				fprintf(fp, "laf lmap:"); for (auto& [k, v] : lm) fprintf(fp, " %u:%d", k, v); fprintf(fp, "\nlaf probe:");
				for (auto& [k, v] : pr) fprintf(fp, " %u:%d", k, v); fprintf(fp, "\n");
			}
			img("draw184", reinterpret_cast<source::GfxImage*>(atp(draw + 184)));
			img("draw192", reinterpret_cast<source::GfxImage*>(atp(draw + 192)));
			for (auto i = 0u; i < at32(draw + 200); i++) img("draw208", reinterpret_cast<source::GfxImage**>(atp(draw + 208))[i]);
			for (auto i = 0u; i < at32(draw); i++) img("probe", reinterpret_cast<source::GfxImage**>(atp(draw + 8))[i]);
			if (atp(draw + 16)) { fprintf(fp, "probe origins:"); probe_hex(fp, atp(draw + 16), 32 * 3); }
			for (auto i = 0u; i < at32(32); i++) { const auto sky = atp(40) + i * 56; probe_hex(fp, sky, 56); img("sky", *reinterpret_cast<source::GfxImage**>(sky + 16)); }
			img("outdoor", reinterpret_cast<source::GfxImage*>(atp(3144)));
			fprintf(fp, "draw:"); probe_hex(fp, w + draw, 800, draw);
			for (auto i = 0; i < 16; i++)
			{
				const auto tr = reinterpret_cast<source::GfxMapTrZone*>(atp(192 + 640 + i * 8));
				if (!tr) continue;
				fprintf(fp, "trzone %d %s verts %u idx %u\n", i, tr->name, tr->vertexCount, tr->indexDataSize);
				const auto& d = tr->vertices.data;
				if (d.positions) { fprintf(fp, " pos"); probe_hex(fp, d.positions, 12 * 4); }
				if (d.data0) { fprintf(fp, " d0"); probe_hex(fp, d.data0, 4 * 4); }
				if (d.data1) { fprintf(fp, " d1"); probe_hex(fp, d.data1, 4 * 4); }
				if (d.data2) { fprintf(fp, " d2"); probe_hex(fp, d.data2, 8 * 4); }
				if (d.data3) { fprintf(fp, " d3"); probe_hex(fp, d.data3, 8 * 4); }
				if (d.data4) { fprintf(fp, " d4"); probe_hex(fp, d.data4, 4 * 4); }
				if (d.data5) { fprintf(fp, " d5"); probe_hex(fp, d.data5, 4 * 4); }
				if (tr->indices.data) { fprintf(fp, " idx"); probe_hex(fp, tr->indices.data, 32); }
			}
			{
				const auto nodes = reinterpret_cast<std::uint16_t*>(atp(104));
				fprintf(fp, "nodes %u:", at32(24));
				for (auto i = 0u; nodes && i < std::min(at32(24) * 2, 96u); i++) fprintf(fp, " %u", nodes[i]);
				fprintf(fp, "\nplanes:"); if (atp(96)) probe_hex(fp, atp(96), 20 * 4);
				fprintf(fp, "cells:"); if (atp(152)) probe_hex(fp, atp(152), 56 * 2);
				fprintf(fp, "cell144:"); if (atp(144)) probe_hex(fp, atp(144), 24 * 4);
				fprintf(fp, "models %u:", at32(2824)); if (atp(2832)) probe_hex(fp, atp(2832), 112 * at32(2824));
				fprintf(fp, "block2840:"); probe_hex(fp, w + 2840, 48, 2840);
				fprintf(fp, "e2896:"); if (atp(2896)) probe_hex(fp, atp(2896), 96 * 2);
				fprintf(fp, "e2816:"); if (atp(2816)) probe_hex(fp, atp(2816), 64 * 2);
				const auto sc = at32(dpvs);
				fprintf(fp, "dpvs:"); probe_hex(fp, w + dpvs, 864, dpvs);
				if (const auto p = reinterpret_cast<std::uint16_t*>(atp(dpvs + 816))) { fprintf(fp, "dpvs816:"); for (auto i = 0u; i < 48; i++) fprintf(fp, " %u", p[i]); fprintf(fp, "\n"); }
				fprintf(fp, "dpvs792:"); if (atp(dpvs + 792)) probe_hex(fp, atp(dpvs + 792), 64);
				fprintf(fp, "lighting:"); if (atp(dpvs + 800)) probe_hex(fp, atp(dpvs + 800), 24 * 6);
				fprintf(fp, "sorted:"); if (atp(dpvs + 744)) probe_hex(fp, atp(dpvs + 744), 64);
				if (const auto di = atp(dpvs + 784))
				{
					std::map<std::uint32_t, int> fl, f34, f68, f70;
					for (auto i = 0u; i < sc; i++)
					{
						const auto e = di + i * 128;
						fl[*reinterpret_cast<std::uint16_t*>(e + 0x54)]++;
						f34[*reinterpret_cast<std::uint32_t*>(e + 0x34)]++;
						f68[*reinterpret_cast<std::uint16_t*>(e + 0x68)]++;
						f70[*reinterpret_cast<std::uint32_t*>(e + 0x70)]++;
					}
					fprintf(fp, "di flags:"); for (auto& [k, v] : fl) fprintf(fp, " %X:%d", k, v);
					fprintf(fp, "\ndi 34:"); for (auto& [k, v] : f34) fprintf(fp, " %X:%d", k, v);
					fprintf(fp, "\ndi 68:"); for (auto& [k, v] : f68) fprintf(fp, " %X:%d", k, v);
					fprintf(fp, "\ndi 70:"); for (auto& [k, v] : f70) fprintf(fp, " %X:%d", k, v);
					fprintf(fp, "\n");
				}
				if (const auto s = atp(dpvs + 752))
				{
					std::map<std::uint32_t, int> lf, s48;
					for (auto i = 0u; i < surf_count; i++) { lf[s[i * 56 + 44]]++; s48[*reinterpret_cast<std::uint32_t*>(s + i * 56 + 48)]++; }
					fprintf(fp, "laf flags:"); for (auto& [k, v] : lf) fprintf(fp, " %X:%d", k, v);
					fprintf(fp, "\nsurf48:"); for (auto& [k, v] : s48) fprintf(fp, " %X:%d", k, v);
					fprintf(fp, "\n");
					const auto sorted = reinterpret_cast<std::uint32_t*>(atp(dpvs + 744));
					for (auto r = 0; r < 8; r++)
					{
						const auto b = at32(dpvs + 8 + r * 4), e = at32(dpvs + 40 + r * 4);
						fprintf(fp, "range %d [%X %X)\n", r, b, e);
						std::map<std::string, int> names;
						for (auto i = b; i < e; i++)
						{
							const auto si = sorted ? sorted[i] : i;
							const auto m = *reinterpret_cast<source::Material**>(s + si * 56 + 32);
							std::string n = m ? m->name : "null";
							if (m && m->techniqueSet) n += std::string(" | ") + m->techniqueSet->name;
							names[n]++;
						}
						auto k = 0; for (auto& [n, v] : names) { if (k++ > 12) break; fprintf(fp, "   %s x%d\n", n.c_str(), v); }
					}
					fprintf(fp, "tail surfaces:\n");
					for (auto i = at32(dpvs + 4); i < surf_count; i++)
					{
						const auto m = *reinterpret_cast<source::Material**>(s + i * 56 + 32);
						fprintf(fp, "  %u %s", i, m ? m->name : "null"); probe_hex(fp, s + i * 56, 56);
					}
				}
				fprintf(fp, "tail:"); probe_hex(fp, w + 4104, sizeof(*asset) - 4104, 4104);
			}
			fclose(fp);
		}

		void probe_clipmap(source::clipMap_t* c)
		{
			const auto fp = fopen("probe_clip.txt", "wb");
			auto& info = c->info;
			fprintf(fp, "clip %s planes %d mats %u brushes sides %u idx %u bb %d lb %u nodes %u leafs %u sub %u smn %u smd %u/%u/%u dyn %u %u scr %u mayhem %u\n",
				c->name, info.planeCount, info.numMaterials, info.brushes.sideCount, info.brushes.indexCount, info.brushBounds.count, info.leafBrushes.count,
				c->numNodes, c->numLeafs, c->numSubModels, c->staticModelNameCount, c->staticModelData.nodeCount, c->staticModelData.leafCount, c->staticModelData.boundsCount,
				c->dynEntCount[0], c->dynEntCount[1], c->scrEntryCount, c->mayhemCount);
			fprintf(fp, "clipMap raw:"); probe_hex(fp, c, sizeof(*c));
			fprintf(fp, "mats:"); for (auto i = 0u; i < std::min(info.numMaterials, 12u); i++) { fprintf(fp, "\n  %s", info.materials[i].name); probe_hex(fp, &info.materials[i], 24); }
			fprintf(fp, "sides:"); probe_hex(fp, info.brushes.sides, 32 * std::min(info.brushes.sideCount, 8u)); fflush(fp);
			
			fprintf(fp, "bindices:"); probe_hex(fp, info.brushes.indices, 4 * std::min(info.brushes.indexCount, 32u));
			fprintf(fp, "bbounds:"); probe_hex(fp, info.brushBounds.data, 32 * std::min(info.brushBounds.count, 4));
			fprintf(fp, "leafbrushes:"); probe_hex(fp, info.leafBrushes.data, 32 * std::min<unsigned>(info.leafBrushes.count, 4u));
			fprintf(fp, "nodes:"); probe_hex(fp, c->nodes, 16 * std::min(c->numNodes, 8u));
			for (auto i = 0u; i < std::min(c->numNodes, 8u); i++) fprintf(fp, " node %u plane %lld\n", i, (long long)(c->nodes[i].plane - info.planes));
			fprintf(fp, "leafs:"); probe_hex(fp, c->leafs, 56 * std::min(c->numLeafs, 6u));
			fprintf(fp, "cmodels:"); probe_hex(fp, c->cmodels, 0x68 * std::min(c->numSubModels, 3u));
			fprintf(fp, "smodel nodes:"); probe_hex(fp, c->staticModelData.nodes, 16 * std::min(c->staticModelData.nodeCount, 6u));
			fprintf(fp, "smodel leafs:"); probe_hex(fp, c->staticModelData.leafs, 40 * std::min(c->staticModelData.leafCount, 4u));
			fprintf(fp, "smodel bounds:"); probe_hex(fp, c->staticModelData.bounds, 20 * std::min(c->staticModelData.boundsCount, 4u));
			for (auto z = 0; z < 16; z++)
			{
				const auto t = info.trZones[z];
				if (!t) continue;
				fprintf(fp, "trzone %d %s tree refs %u nodes %u mesh verts %u tris %u parts %u aabbs %u smodels %u\n", z, t->name, t->tree.nodeRefCount, t->tree.nodeCount,
					t->mesh.vertCount, t->mesh.triCount, t->mesh.partitionCount, t->mesh.aabbCount, t->staticModels.count);
				fprintf(fp, "raw:"); probe_hex(fp, t, sizeof(*t));
				fprintf(fp, "noderefs:"); probe_hex(fp, t->tree.nodeRefs, 8 * std::min(t->tree.nodeRefCount, 8u));
				fprintf(fp, "tnodes:"); probe_hex(fp, t->tree.nodes, 24 * std::min(t->tree.nodeCount, 4u));
				for (auto i = 0u; i < std::min(t->tree.nodeCount, 4u); i++) { fprintf(fp, " tnode data:"); if (t->tree.nodes[i].data) probe_hex(fp, t->tree.nodes[i].data, 8); else fprintf(fp, " null\n"); }
				fprintf(fp, "tbounds:"); probe_hex(fp, t->tree.bounds, 24 * std::min(t->tree.nodeCount, 4u));
				fprintf(fp, "tnodeData:"); probe_hex(fp, t->tree.nodeData, 8 * std::min(t->tree.nodeCount, 8u));
				fprintf(fp, "verts:"); probe_hex(fp, t->mesh.verts, 12 * 4);
				fprintf(fp, "tris:"); probe_hex(fp, t->mesh.triIndices, 6 * 8);
				fprintf(fp, "parts:"); probe_hex(fp, t->mesh.partitions, 28 * std::min(t->mesh.partitionCount, 8u));
				fprintf(fp, "aabbs:"); probe_hex(fp, t->mesh.aabbs, 16 * std::min(t->mesh.aabbCount, 8u));
				fprintf(fp, "smodels:"); probe_hex(fp, t->staticModels.models, 88 * std::min(t->staticModels.count, 3u));
				for (auto i = 0u; i < std::min(t->staticModels.count, 3u); i++) fprintf(fp, " sm %s\n", t->staticModels.models[i].model ? t->staticModels.models[i].model->name : "null");
			}
			fclose(fp);
		}

		void probe_clip_trees(source::clipMap_t* c)
		{
			const auto fp = fopen("probe_cliptrees.txt", "wb");
			auto& info = c->info;
			const auto base = reinterpret_cast<std::uint8_t*>(c);
			const auto ranges = reinterpret_cast<std::uint32_t*>(base + 0x74);
			{
				std::map<unsigned, int> b0, b1, hi;
				const auto aabb = reinterpret_cast<std::uint8_t*>(info.brushBounds.data);
				for (auto i = 0; i < info.brushBounds.count; i++)
				{
					const auto packed = *reinterpret_cast<std::uint32_t*>(aabb + i * 32 + 12);
					b0[packed & 0xFF]++; b1[(packed >> 8) & 0xFF]++; hi[packed >> 16]++;
				}
				fprintf(fp, "aabb b0:"); for (auto& [k, v] : b0) fprintf(fp, " %X:%d", k, v);
				fprintf(fp, "\naabb b1:"); auto n = 0; for (auto& [k, v] : b1) { if (n++ > 30) break; fprintf(fp, " %X:%d", k, v); }
				fprintf(fp, "\naabb hi:"); n = 0; for (auto& [k, v] : hi) { if (n++ > 30) break; fprintf(fp, " %X:%d", k, v); }
				fprintf(fp, " (%zu)\n", hi.size());
				for (auto z = 0; z < 10; z++)
				{
					const auto r = ranges + z * 7;
					unsigned lo = ~0u, mx = 0, cnt = 0, child_lo = ~0u, child_hi = 0;
					for (auto i = r[2]; i < r[3]; i++)
					{
						const auto e = aabb + i * 32;
						const auto packed = *reinterpret_cast<std::uint32_t*>(e + 12);
						const auto index = *reinterpret_cast<std::uint32_t*>(e + 28);
						if (((packed >> 8) & 0xFF) == 0) { lo = std::min(lo, index); mx = std::max(mx, index); cnt++; }
						else { child_lo = std::min(child_lo, index); child_hi = std::max(child_hi, index); }
					}
					fprintf(fp, "z%d brush [%X %X) aabb [%X %X) y [%X %X) parts %X: leafs %u index [%X..%X] children [%X..%X]\n", z, r[0], r[1], r[2], r[3], r[4], r[5], r[6], cnt, lo, mx, child_lo, child_hi);
				}
				fprintf(fp, "aabb leaf samples:\n");
				auto shown = 0;
				for (auto i = 0; i < info.brushBounds.count && shown < 12; i++)
				{
					const auto e = aabb + i * 32;
					if (((*reinterpret_cast<std::uint32_t*>(e + 12) >> 8) & 0xFF) != 0) continue;
					if (i % 997) continue;
					fprintf(fp, " %d:", i); probe_hex(fp, e, 32); shown++;
				}
				fprintf(fp, "aabb first 12:\n"); probe_hex(fp, aabb, 32 * 12);
			}
			{
				std::map<int, int> counts, axes;
				const auto nodes = reinterpret_cast<std::uint8_t*>(info.brushes.sides);
				for (auto i = 0u; i < info.brushes.sideCount; i++)
				{
					counts[*reinterpret_cast<std::int32_t*>(nodes + i * 32 + 4)]++;
					axes[*reinterpret_cast<std::int32_t*>(nodes + i * 32)]++;
				}
				fprintf(fp, "lbn counts:"); auto n = 0; for (auto& [k, v] : counts) { if (n++ > 40) break; fprintf(fp, " %d:%d", k, v); }
				fprintf(fp, "\nlbn axes:"); for (auto& [k, v] : axes) fprintf(fp, " %X:%d", k, v);
				std::uint32_t max_brush = 0;
				for (auto i = 0u; i < info.brushes.indexCount; i++) max_brush = std::max<std::uint32_t>(max_brush, info.brushes.indices[i]);
				fprintf(fp, "\nmax brush id %X\n", max_brush);
			}
			{
				unsigned max_node = 0, max_aabb = 0; std::map<unsigned, int> u34;
				std::map<std::uint64_t, int> bc, tc;
				for (auto i = 0u; i < c->numLeafs; i++)
				{
					const auto l = reinterpret_cast<std::uint8_t*>(c->leafs[i]);
					max_aabb = std::max(max_aabb, *reinterpret_cast<std::uint32_t*>(l) + *reinterpret_cast<std::uint32_t*>(l + 4));
					max_node = std::max(max_node, *reinterpret_cast<std::uint32_t*>(l + 0x30));
					u34[*reinterpret_cast<std::uint16_t*>(l + 0x34)]++;
					bc[*reinterpret_cast<std::uint64_t*>(l + 8)]++;
					tc[*reinterpret_cast<std::uint64_t*>(l + 16)]++;
				}
				fprintf(fp, "leafs max aabb end %X max lbn %X\nu34:", max_aabb, max_node);
				auto n = 0; for (auto& [k, v] : u34) { if (n++ > 20) break; fprintf(fp, " %X:%d", k, v); }
				fprintf(fp, "\nbrushContents:"); for (auto& [k, v] : bc) fprintf(fp, " %llX:%d", k, v);
				fprintf(fp, "\nterrainContents:"); for (auto& [k, v] : tc) fprintf(fp, " %llX:%d", k, v);
				fprintf(fp, "\nleafs 0..12:"); probe_hex(fp, c->leafs, 56 * 12);
			}
			fprintf(fp, "lb60 (%u):", info.leafBrushes.count); probe_hex(fp, info.leafBrushes.data, 32 * 16);
			fprintf(fp, "cmodels:"); probe_hex(fp, c->cmodels, 0x68 * std::min(c->numSubModels, 6u));
			if (c->pInfo) { fprintf(fp, "pInfo raw:"); probe_hex(fp, c->pInfo, 0x238); }
			fprintf(fp, "smodel nodes:"); probe_hex(fp, c->staticModelData.nodes, 16 * std::min(c->staticModelData.nodeCount, 46u));
			fprintf(fp, "smodel leafs:"); probe_hex(fp, c->staticModelData.leafs, 40 * std::min(c->staticModelData.leafCount, 16u));
			fprintf(fp, "smodel bounds:"); probe_hex(fp, c->staticModelData.bounds, 20 * std::min(c->staticModelData.boundsCount, 16u));
			for (auto z = 0; z < 16; z++)
			{
				const auto t = info.trZones[z];
				if (!t) continue;
				std::map<std::uint64_t, int> nd;
				for (auto i = 0u; i < t->tree.nodeCount; i++) nd[t->tree.nodeData[i]]++;
				fprintf(fp, "trzone %d brush contents:", z); for (auto& [k, v] : nd) fprintf(fp, " %llX:%d", k, v);
				std::map<unsigned, int> pm; unsigned max_border = 0, max_tri = 0;
				const auto parts = reinterpret_cast<std::uint8_t*>(t->mesh.aabbs);
				for (auto i = 0u; i < t->mesh.aabbCount; i++)
				{
					pm[*reinterpret_cast<std::uint32_t*>(parts + i * 16 + 12)]++;
					max_tri = std::max<unsigned>(max_tri, *reinterpret_cast<std::int32_t*>(parts + i * 16 + 4) + parts[i * 16]);
					const auto fb = *reinterpret_cast<std::int32_t*>(parts + i * 16 + 8);
					if (fb >= 0) max_border = std::max<unsigned>(max_border, fb + parts[i * 16 + 1]);
				}
				fprintf(fp, "\n part mats (%zu) max tri end %u/%u max border end %u/%u\n", pm.size(), max_tri, t->mesh.triCount, max_border, t->mesh.partitionCount);
			}
			fclose(fp);
		}

		void probe_comworld(source::ComWorld* c)
		{
			const auto fp = fopen("probe_com.txt", "wb");
			fprintf(fp, "com %s lights %u envs %u\nraw:", c->name, c->primaryLightCount, c->primaryLightEnvCount);
			probe_hex(fp, c, sizeof(*c));
			for (auto i = 0u; i < std::min(c->primaryLightCount, 12u); i++)
			{
				const auto l = reinterpret_cast<std::uint8_t*>(&c->primaryLights[i]);
				fprintf(fp, "light %u %s\n  bytes0-7:", i, c->primaryLights[i].defName ? c->primaryLights[i].defName : "null");
				for (auto b = 0; b < 8; b++) fprintf(fp, " %02X", l[b]);
				fprintf(fp, "\n  floats:");
				for (auto f = 8; f < 0xA0; f += 4) fprintf(fp, "%s%02X=%g", (f - 8) % 32 == 0 ? "\n   " : " ", f, *reinterpret_cast<float*>(l + f));
				fprintf(fp, "\n");
			}
			fprintf(fp, "envs:"); probe_hex(fp, c->primaryLightEnvs, 10 * std::min(c->primaryLightEnvCount, 8u));
			fclose(fp);
		}

		void probe_clip_facts(source::clipMap_t* c)
		{
			const auto fp = fopen("probe_clipfacts.txt", "wb");
			fprintf(fp, "numNodes %u nodes:", c->numNodes); probe_hex(fp, c->nodes, 16 * std::min(c->numNodes, 8u));
			std::map<int, int> hi;
			for (auto i = 0u; i < c->numNodes; i++) { const auto n = reinterpret_cast<std::uint8_t*>(&c->nodes[i]); hi[*reinterpret_cast<std::int16_t*>(n + 10)]++; hi[*reinterpret_cast<std::int16_t*>(n + 14)] += 1000000; }
			fprintf(fp, "node hi16 @10/@14:"); auto k = 0; for (auto& [a, b] : hi) { if (k++ > 20) break; fprintf(fp, " %d:%d", a, b); }
			fprintf(fp, "\nlbn 0..3:"); probe_hex(fp, c->info.brushes.sides, 32 * 4);
			fprintf(fp, "mats %u:", c->info.numMaterials); probe_hex(fp, c->info.materials, 24 * std::min(c->info.numMaterials, 6u));
			for (auto z = 0; z < 16; z++)
			{
				const auto t = c->info.trZones[z];
				if (!t) continue;
				const auto refs = reinterpret_cast<std::uint8_t*>(t->tree.nodeRefs);
				fprintf(fp, "tz%d %s slot %u refs %u nodes %u verts %u tris %u borders %u parts %u smodels %u\n", z, t->name, *reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uint8_t*>(t) + 8), t->tree.nodeRefCount, t->tree.nodeCount, t->mesh.vertCount, t->mesh.triCount, t->mesh.partitionCount, t->mesh.aabbCount, t->staticModels.count);
				for (auto i = 0u; i < std::min(t->tree.nodeCount, 3u); i++)
				{
					const auto n = reinterpret_cast<std::uint8_t*>(&t->tree.nodes[i]);
					fprintf(fp, " node %u data off %lld:", i, (long long)(reinterpret_cast<std::uint8_t*>(t->tree.nodes[i].data) - refs)); probe_hex(fp, n, 24);
				}
				fprintf(fp, " refs:"); probe_hex(fp, refs, 8 * std::min(t->tree.nodeRefCount, 8u));
				if (t->staticModels.count) { fprintf(fp, " smodel0:"); probe_hex(fp, t->staticModels.models, 0x58); }
			}
			fclose(fp);
		}

		void probe_surface_bytes(source::GfxWorld* asset)
		{
			const auto w = reinterpret_cast<std::uint8_t*>(asset);
			const auto dpvs = w + 3240;
			const auto surface_count = *reinterpret_cast<unsigned int*>(w + 28);
			const auto static_count = *reinterpret_cast<unsigned int*>(dpvs + 4);
			const auto surfaces = *reinterpret_cast<std::uint8_t**>(dpvs + 752);
			const auto fp = fopen("probe_surfbytes.txt", "wb");
			fprintf(fp, "surfaces %u static %u\n", surface_count, static_count);
			for (auto off = 0; off < 56; off++)
			{
				if (off >= 32 && off < 40) continue;
				std::map<int, int> h;
				for (auto i = 0u; i < surface_count; i++) h[surfaces[i * 56 + off]]++;
				fprintf(fp, "@%02d (%zu):", off, h.size());
				auto n = 0;
				for (auto& [k, v] : h) { if (n++ > 24) { fprintf(fp, " ..."); break; } fprintf(fp, " %X:%d", k, v); }
				fprintf(fp, "\n");
			}
			unsigned max_end = 0, max_index = 0;
			const auto indices = *reinterpret_cast<unsigned short**>(w + 192 + 784);
			for (auto i = 0u; i < surface_count; i++)
			{
				const auto s = surfaces + i * 56;
				const auto first = *reinterpret_cast<unsigned*>(s + 4);
				const auto vc = *reinterpret_cast<unsigned short*>(s + 8);
				const auto tc = *reinterpret_cast<unsigned short*>(s + 10);
				const auto base = *reinterpret_cast<unsigned*>(s + 24);
				max_end = std::max(max_end, first + vc);
				for (auto j = 0u; j < tc * 3u; j++) max_index = std::max<unsigned>(max_index, indices[base + j] + first);
			}
			fprintf(fp, "max firstVertex+count %u max abs index %u\n", max_end, max_index);
			for (auto i = 0u; i < 40; i++) { fprintf(fp, "s%u:", i * 97); probe_hex(fp, surfaces + i * 97 * 56, 56); }
			fclose(fp);
		}

		void probe_surface_env(source::GfxWorld* asset)
		{
			const auto w = reinterpret_cast<std::uint8_t*>(asset);
			const auto dpvs = w + 3240;
			const auto surface_count = *reinterpret_cast<unsigned int*>(w + 28);
			const auto surfaces = *reinterpret_cast<std::uint8_t**>(dpvs + 752);
			const auto bounds = *reinterpret_cast<float**>(dpvs + 760);
			const auto fp = fopen("probe_surfenv.txt", "wb");
			const auto lists = *reinterpret_cast<std::uint16_t**>(w + 2880);
			const auto list_size = *reinterpret_cast<std::uint32_t*>(w + 2872);
			fprintf(fp, "listsize %u\n", list_size);
			for (auto i = 0u; i < surface_count; i++)
			{
				const auto s = surfaces + i * 56;
				const auto b = bounds + i * 16;
				const auto mat = *reinterpret_cast<source::Material**>(s + 32);
				fprintf(fp, "%u %u %u %u %u %u %u %u %.1f %.1f %.1f %.1f %.1f %.1f %s\n", i, s[40], s[41], s[42], s[43], s[44], s[45], s[46],
					b[0], b[1], b[2], b[3], b[4], b[5], mat ? mat->name : "null");
				const auto off = *reinterpret_cast<std::uint16_t*>(s + 44);
				fprintf(fp, "L");
				if (off < list_size) for (auto k = 0u; k < lists[off] && off + 1 + k < list_size; k++) fprintf(fp, " %u", lists[off + 1 + k]);
				fprintf(fp, "\n");
			}
			fclose(fp);
		}

		void probe_sky_shaders(source::Material* material)
		{
			if (material->techniqueSet)
			{
				const auto fp = std::fopen("probe_mats.txt", "ab");
				std::fprintf(fp, "%s %s tex %u const %u\n", material->info.name, material->techniqueSet->name, material->textureCount, material->constantCount);
				for (auto i = 0; i < material->constantCount && material->constantTable; i++)
				{
					const auto& c = material->constantTable[i];
					std::fprintf(fp, "  %08X %.12s %f %f %f %f\n", c.nameHash, c.name, c.literal[0], c.literal[1], c.literal[2], c.literal[3]);
				}
				for (auto i = 0; i < material->textureCount && material->textureTable; i++)
				{
					std::fprintf(fp, "  tex %08X %s\n", material->textureTable[i].nameHash, material->textureTable[i].u.image ? material->textureTable[i].u.image->name : "");
				}
				std::fclose(fp);
			}

			if (!material->techniqueSet || !std::strstr(material->info.name, "hdr_sky_vlobby_hub_allies"))
			{
				return;
			}

			std::set<std::string> seen;
			for (auto t = 0u; t < std::size(material->techniqueSet->techniques); t++)
			{
				const auto technique = material->techniqueSet->techniques[t];
				if (!technique) continue;
				for (auto p = 0u; p < technique->hdr.passCount; p++)
				{
					for (const auto& [shader_name, program, size] : {
						std::tuple{technique->passArray[p].pixelShader ? technique->passArray[p].pixelShader->name : nullptr,
							technique->passArray[p].pixelShader ? technique->passArray[p].pixelShader->prog.loadDef.program : nullptr,
							technique->passArray[p].pixelShader ? technique->passArray[p].pixelShader->prog.loadDef.programSize : 0u},
						std::tuple{technique->passArray[p].vertexShader ? technique->passArray[p].vertexShader->name : nullptr,
							technique->passArray[p].vertexShader ? technique->passArray[p].vertexShader->prog.loadDef.program : nullptr,
							technique->passArray[p].vertexShader ? technique->passArray[p].vertexShader->prog.loadDef.programSize : 0u}})
					{
						if (!shader_name || !program || seen.contains(shader_name)) continue;
						seen.insert(shader_name);
						const auto path = std::format("probe_sky_{}_{}.cso", t, shader_name);
						utils::io::write_file(path, std::string(reinterpret_cast<const char*>(program), size), false);
					}
				}
			}

			utils::io::write_file("probe_sky_techset.txt", material->techniqueSet->name, false);
		}

		inline void probe_smodel_flags(source::GfxWorld* asset)
		{
			const auto w = reinterpret_cast<std::uint8_t*>(asset);
			const auto dpvs = w + 3240;
			const auto count = *reinterpret_cast<unsigned int*>(dpvs + 0);
			const auto insts = *reinterpret_cast<std::uint8_t**>(dpvs + 784);
			const auto lighting = *reinterpret_cast<std::uint8_t**>(dpvs + 800);
			const auto fp = fopen("probe_smodel_flags.txt", "wb");
			std::map<int, int> hist;
			std::map<std::string, std::map<int, int>> by_model;
			for (auto i = 0u; i < count; i++)
			{
				const auto inst = insts + 128ull * i;
				const auto flags = *reinterpret_cast<std::uint16_t*>(inst + 0x54);
				const auto model = *reinterpret_cast<source::XModel**>(inst + 0x48);
				hist[flags]++;
				by_model[model ? model->name : "null"][flags]++;
			}
			fprintf(fp, "smodels %u\n", count);
			for (const auto& [f, n] : hist) fprintf(fp, "flags %04X: %d\n", f, n);
			for (const auto& [name, h] : by_model)
			{
				fprintf(fp, "%s:", name.data());
				for (const auto& [f, n] : h) fprintf(fp, " %04X:%d", f, n);
				fprintf(fp, "\n");
			}
			auto shown = std::map<int, int>{};
			for (auto i = 0u; i < count; i++)
			{
				const auto inst = insts + 128ull * i;
				const auto flags = *reinterpret_cast<std::uint16_t*>(inst + 0x54);
				if (shown[flags]++ >= 3) continue;
				const auto model = *reinterpret_cast<source::XModel**>(inst + 0x48);
				fprintf(fp, "inst %u flags %04X %s inst:", i, flags, model ? model->name : "null");
				for (auto k = 0x40; k < 0x80; k++) fprintf(fp, "%s%02X", (k % 4) ? "" : " ", inst[k]);
				fprintf(fp, " light:");
				for (auto k = 0; k < 24; k++) fprintf(fp, "%s%02X", (k % 4) ? "" : " ", lighting[24ull * i + k]);
				fprintf(fp, "\n");
}
			fclose(fp);
		}

		inline void probe_grid_hex(FILE* fp, const char* tag, const std::uint8_t* data, std::size_t stride, std::size_t count, std::size_t shown)
		{
			fprintf(fp, "%s count %zu stride %zu\n", tag, count, stride);
			if (!data) return;
			for (auto i = 0ull; i < std::min(count, shown); i++)
			{
				const auto e = data + stride * i;
				fprintf(fp, "  %llu:", i);
				for (auto k = 0ull; k < stride; k += 4) fprintf(fp, " %08X", *reinterpret_cast<const std::uint32_t*>(e + k));
				fprintf(fp, " |f");
				for (auto k = 0ull; k + 4 <= stride; k += 4) fprintf(fp, " %g", *reinterpret_cast<const float*>(e + k));
				fprintf(fp, "\n");
			}
		}

		inline void probe_light_grid(source::GfxWorld* asset)
		{
			const auto w = reinterpret_cast<std::uint8_t*>(asset);
			const auto p = w + 992 + 1080;
			const auto fp = fopen("probe_lightgrid.txt", "wb");
			if (!fp) return;
			fprintf(fp, "probe grid header:");
			for (auto k = 0; k < 232; k += 4) fprintf(fp, "%s %08X", (k % 32) ? "" : "\n ", *reinterpret_cast<std::uint32_t*>(p + k));
			fprintf(fp, "\n");
			const auto u32 = [&](std::size_t o) { return *reinterpret_cast<std::uint32_t*>(p + o); };
			const auto ptr = [&](std::size_t o) { return *reinterpret_cast<std::uint8_t**>(p + o); };
			probe_grid_hex(fp, "@8", ptr(8), 64, u32(0), 6);
			probe_grid_hex(fp, "@40", ptr(40), 12, u32(0), 6);
			probe_grid_hex(fp, "@72", ptr(72), 92, u32(64), 4);
			probe_grid_hex(fp, "@88", ptr(88), 16, u32(80), 6);
			probe_grid_hex(fp, "@112", ptr(112), 16, u32(80), 6);
			probe_grid_hex(fp, "@136", ptr(136), 64, u32(84), 6);
			probe_grid_hex(fp, "@168", ptr(168), 4, u32(160), 16);
			probe_grid_hex(fp, "@200", ptr(200), 64, u32(192), 6);
			probe_grid_hex(fp, "@216", ptr(216), 64, u32(208), 6);
			const auto dpvs = w + 3240;
			const auto count = *reinterpret_cast<unsigned int*>(dpvs + 0);
			const auto insts = *reinterpret_cast<std::uint8_t**>(dpvs + 784);
			const auto lighting = *reinterpret_cast<std::uint8_t**>(dpvs + 800);
			auto max_index = 0u, min_index = ~0u, grid = 0u;
			std::map<int, int> diff_hist;
			for (auto i = 0u; i < count; i++)
			{
				const auto flags = *reinterpret_cast<std::uint16_t*>(insts + 128ull * i + 0x54);
				if (!(flags & 0x40)) continue;
				const auto index = *reinterpret_cast<std::uint32_t*>(lighting + 24ull * i);
				const auto id = *reinterpret_cast<std::uint16_t*>(insts + 128ull * i + 0x58);
				grid++;
				max_index = std::max(max_index, index);
				min_index = std::min(min_index, index);
				diff_hist[static_cast<int>(id) - static_cast<int>(index)]++;
			}
			fprintf(fp, "grid smodels %u index min %u max %u\n", grid, min_index, max_index);
			auto shown = 0;
			for (const auto& [d, n] : diff_hist) if (shown++ < 20) fprintf(fp, "  id-index %d: %d\n", d, n);
			fclose(fp);
		}

		inline float probe_half_s2(std::uint16_t h)
		{
			const auto e = (h >> 10) & 31, m = h & 1023;
			const auto v = e == 0 ? m / 1024.0f * std::pow(2.0f, -14.0f) : (1.0f + m / 1024.0f) * std::pow(2.0f, static_cast<float>(e) - 15.0f);
			return (h & 0x8000) ? -v : v;
		}

		inline void probe_smodel_sh(source::GfxWorld* asset)
		{
			const auto w = reinterpret_cast<std::uint8_t*>(asset);
			const auto p = w + 992 + 1080;
			const auto fp = fopen("probe_smodel_sh.txt", "wb");
			if (!fp) return;
			const auto volume_count = *reinterpret_cast<std::uint32_t*>(p + 192);
			const auto volumes = *reinterpret_cast<std::uint8_t**>(p + 200);
			const auto sh_count = *reinterpret_cast<std::uint32_t*>(p + 208);
			const auto sh = *reinterpret_cast<std::uint8_t**>(p + 216);
			std::map<std::string, int> dims;
			auto max_end = 0ull;
			for (auto i = 0u; i < volume_count; i++)
			{
				const auto v = volumes + 64ull * i;
				const auto dx = *reinterpret_cast<std::uint16_t*>(v + 4), dy = *reinterpret_cast<std::uint16_t*>(v + 6);
				const auto dz = *reinterpret_cast<std::uint32_t*>(v + 8), off = *reinterpret_cast<std::uint32_t*>(v + 12);
				dims[std::format("{}x{}x{}", dx, dy, dz)]++;
				max_end = std::max<std::uint64_t>(max_end, off + 1ull * dx * dy * dz);
			}
			fprintf(fp, "volumes %u sh %u max end %llu\n", volume_count, sh_count, max_end);
			auto shown = 0;
			for (const auto& [d, n] : dims) if (shown++ < 30) fprintf(fp, "dims %s: %d\n", d.data(), n);
			const auto dpvs = w + 3240;
			const auto count = *reinterpret_cast<unsigned int*>(dpvs + 0);
			const auto insts = *reinterpret_cast<std::uint8_t**>(dpvs + 784);
			const auto lighting = *reinterpret_cast<std::uint8_t**>(dpvs + 800);
			std::map<int, int> lum_hist;
			std::map<std::string, int> shown_models;
			for (auto i = 0u; i < count; i++)
			{
				const auto inst = insts + 128ull * i;
				const auto flags = *reinterpret_cast<std::uint16_t*>(inst + 0x54);
				if (!(flags & 0x40)) continue;
				const auto index = *reinterpret_cast<std::uint32_t*>(lighting + 24ull * i);
				if (index >= volume_count) continue;
				const auto v = volumes + 64ull * index;
				const auto off = *reinterpret_cast<std::uint32_t*>(v + 12);
				const auto c = reinterpret_cast<std::uint16_t*>(sh + 64ull * off);
				const auto r = probe_half_s2(c[0]), g = probe_half_s2(c[9]), b = probe_half_s2(c[18]);
				lum_hist[static_cast<int>(std::floor(std::log2(std::max(1e-4f, (r + g + b) / 3.0f))))]++;
				const auto model = *reinterpret_cast<source::XModel**>(inst + 0x48);
				const std::string name = model ? model->name : "null";
				const auto origin = reinterpret_cast<const float*>(inst);
				if (shown_models[name]++ < 2 && (name.find("cup") != std::string::npos || name.find("bowl") != std::string::npos || name.find("rock") != std::string::npos || name.find("mug") != std::string::npos || i < 3 || name.find("sandbag") != std::string::npos || name.find("locker") != std::string::npos))
				{
					fprintf(fp, "smodel %u %s pos %g %g %g index %u dims %u %u %u off %u sh:", i, name.data(), origin[0], origin[1], origin[2], index,
						*reinterpret_cast<std::uint16_t*>(v + 4), *reinterpret_cast<std::uint16_t*>(v + 6), *reinterpret_cast<std::uint32_t*>(v + 8), off);
					for (auto k = 0; k < 28; k++) fprintf(fp, " %.3f", probe_half_s2(c[k]));
					fprintf(fp, "\n");
				}
			}
			for (const auto& [l, n] : lum_hist) fprintf(fp, "dc log2 %d: %d\n", l, n);
			fclose(fp);
		}

		inline void probe_smodel_bounds(source::GfxWorld* asset)
		{
			const auto w = reinterpret_cast<std::uint8_t*>(asset);
			const auto dpvs = w + 3240;
			const auto count = *reinterpret_cast<unsigned int*>(dpvs + 0);
			const auto bounds = *reinterpret_cast<float**>(dpvs + 776);
			const auto insts = *reinterpret_cast<std::uint8_t**>(dpvs + 784);
			const auto fp = fopen("probe_smodel_bounds.txt", "wb");
			for (auto i = 0u; i < count && i < 4000; i += 97)
			{
				const auto inst = insts + 128ull * i;
				const auto origin = reinterpret_cast<float*>(inst);
				const auto model = *reinterpret_cast<source::XModel**>(inst + 0x48);
				const auto b = bounds + 9ull * i;
				fprintf(fp, "%5u %-40s o(%.1f %.1f %.1f) s %.2f b(%.1f %.1f %.1f | %.1f %.1f %.1f | %.1f %.1f %.1f) raw:", i, model ? model->name : "null",
					origin[0], origin[1], origin[2], origin[12], b[0], b[1], b[2], b[3], b[4], b[5], b[6], b[7], b[8]);
				for (auto o = 0x34u; o < 0x80u; o += 2)
				{
					if (o >= 0x48 && o < 0x50) continue;
					fprintf(fp, " %02X:%04X", o, *reinterpret_cast<std::uint16_t*>(inst + o));
				}
				fprintf(fp, "\n");
			}
			fclose(fp);
		}

		inline void probe_xmodel_lods(source::XModel* asset)
		{
			static FILE* fp = fopen("probe_xmodel_lods.txt", "wb");
			const auto base = asset->base;
			if (!base) return;
			const auto b = reinterpret_cast<const std::uint8_t*>(base);
			fprintf(fp, "%s flags %X lods %d radius %.1f b26a %u b26b %u f2a8 %g lodf", asset->name, asset->flags, base->numLods, base->radius, b[0x26A], b[0x26B], *reinterpret_cast<const float*>(b + 0x2A8));
			for (auto i = 0; i < 6; i++) fprintf(fp, " %g", *reinterpret_cast<const float*>(b + 0x2AC + i * 4));
			fprintf(fp, "\n");
			for (auto l = 0; l < base->numLods; l++)
			{
				const auto lb = reinterpret_cast<const std::uint8_t*>(&base->lodInfo[l]);
				fprintf(fp, "  lod%d dist %g surfs %u:", l, base->lodInfo[l].dist, base->lodInfo[l].numsurfs);
				for (auto k = 16; k < 0x50; k += 4) fprintf(fp, " %g", *reinterpret_cast<const float*>(lb + k));
				fprintf(fp, "\n");
			}
			fflush(fp);
		}
