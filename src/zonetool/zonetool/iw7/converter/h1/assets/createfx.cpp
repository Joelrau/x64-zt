#include <std_include.hpp>
#include "zonetool/iw7/converter/h1/include.hpp"
#include "createfx.hpp"
#include "fxeffectdef.hpp"

#include "zonetool/utils/gsc.hpp"

#include <utils/string.hpp>
#include <utils/compression.hpp>

// v102 fx: map-placed effects.
//   IW7 keeps them in compiled GSC: scripts/mp/maps/<map>/<map>_fx.gsc (level._effect[id] = loadfx(path)) and
//   scripts/mp/maps/<map>/gen/<map>_fx.gsc (createfx: createoneshoteffect/createloopeffect + set_origin_and_angles + .v[]).
//   We decompile those with gsc-tool (iw7 context), parse the effect table and the createfx entities, and emit
//   self-contained H1 scripts that do not depend on H1's createfx API:
//     maps/mp/<map>_fx.gsc       -> loadfx table + calls maps\createfx\<map>_fx::main()
//     maps/createfx/<map>_fx.gsc -> spawnfx/triggerfx (one-shots) and playfx loops (loop effects)
//   Exploders / sounds are skipped (they need IW7 script logic / sound aliases). Effects that are not H1 effects json
//   (IW7 particle systems = ASSET_TYPE_VFX, or effects with no convertible element) are commented out.
//   The decompiled IW7 sources are written to iw7_gsc/ for reference.

namespace zonetool::iw7
{
	namespace converter::h1
	{
		namespace createfx
		{
			namespace
			{
				struct fx_ent
				{
					std::string func;
					std::string type;
					std::string fxid;
					std::string origin;
					std::string angles;
					std::string delay;
				};

				std::vector<std::pair<std::string, std::string>> effect_table; // id -> path (in script order)
				std::unordered_set<std::string> effect_ids;
				std::vector<fx_ent> ents;
				std::unordered_set<std::string> vfx_names;
				int scripts_parsed = 0;

				std::string resolve_name(const char* raw)
				{
					std::string name = raw ? raw : "";
					if (name.starts_with(","))
					{
						name = name.substr(1);
					}
					if (!name.empty() && utils::string::is_numeric(name))
					{
						const auto token = gsc::iw7::gsc_ctx->token_name(static_cast<std::uint32_t>(std::strtoul(name.data(), nullptr, 10)));
						if (!token.empty() && !token.starts_with("_id_"))
						{
							name = token;
						}
					}
					return name;
				}

				std::string decompile(ScriptFile* asset)
				{
					std::vector<std::uint8_t> stack;
					if (asset->buffer && asset->compressedLen > 0)
					{
						try
						{
							const std::string compressed{ asset->buffer, static_cast<std::size_t>(asset->compressedLen) };
							const auto data = utils::compression::zlib::decompress(compressed);
							stack.assign(data.begin(), data.end());
						}
						catch (...)
						{
						}
						if (stack.empty() && asset->len == asset->compressedLen)
						{
							stack.assign(asset->buffer, asset->buffer + asset->len);
						}
					}
					if (!asset->bytecode || asset->bytecodeLen <= 0)
					{
						return {};
					}

					const std::vector<std::uint8_t> bytecode{ asset->bytecode, asset->bytecode + asset->bytecodeLen };
					auto& disassembler = gsc::iw7::gsc_ctx->disassembler();
					auto& decompiler = gsc::iw7::gsc_ctx->decompiler();
					const auto disasm = disassembler.disassemble(bytecode, stack);
					const auto decomp = decompiler.decompile(*disasm);
					const auto printed = gsc::iw7::gsc_ctx->printer().print(*decomp);
					return { printed.begin(), printed.end() };
				}

				std::string trim(const std::string& s)
				{
					const auto b = s.find_first_not_of(" \t\r\n");
					if (b == std::string::npos) return {};
					const auto e = s.find_last_not_of(" \t\r\n");
					return s.substr(b, e - b + 1);
				}

				std::string strip_quotes(const std::string& s)
				{
					auto t = trim(s);
					if (t.size() >= 2 && t.front() == '"' && t.back() == '"')
					{
						return t.substr(1, t.size() - 2);
					}
					return t;
				}

				void add_effect(const std::string& id, const std::string& path)
				{
					if (effect_ids.contains(id))
					{
						return;
					}
					effect_ids.insert(id);
					effect_table.emplace_back(id, path);
				}

				void parse(const std::string& text)
				{
					static const std::regex re_loadfx_assign(R"re(_effect\s*\[\s*"([^"]+)"\s*\]\s*=\s*(?:[\w\\]+::)?loadfx\s*\(\s*"([^"]+)")re", std::regex::icase);
					static const std::regex re_loadfx_any(R"re(loadfx\s*\(\s*"([^"]+)")re", std::regex::icase);
					static const std::regex re_create(R"re(^\s*(\w+)\s*=\s*([\w\\:]+)\s*\(\s*"([^"]*)"\s*\)\s*;)re", std::regex::icase);
					static const std::regex re_org_ang(R"re(^\s*(\w+)\s+[\w\\:]+\s*\(\s*(\([^()]*\))\s*,\s*(\([^()]*\))\s*\)\s*;)re", std::regex::icase);
					static const std::regex re_v(R"re(^\s*(\w+)\.v\s*\[\s*"(\w+)"\s*\]\s*=\s*(.+?)\s*;\s*$)re", std::regex::icase);

					std::unordered_map<std::string, std::size_t> var_to_ent;
					const auto ent_for = [&](const std::string& var) -> fx_ent&
					{
						const auto itr = var_to_ent.find(var);
						if (itr != var_to_ent.end())
						{
							return ents[itr->second];
						}
						ents.emplace_back();
						var_to_ent[var] = ents.size() - 1;
						return ents.back();
					};

					for (auto line : utils::string::split(text, '\n'))
					{
						if (!line.empty() && line.back() == '\r') line.pop_back();
						std::smatch m;

						if (std::regex_search(line, m, re_loadfx_assign))
						{
							add_effect(m[1].str(), m[2].str());
							continue;
						}
						if (std::regex_search(line, m, re_loadfx_any))
						{
							add_effect(m[1].str(), m[1].str());
							continue;
						}
						if (std::regex_search(line, m, re_create))
						{
							const auto func = utils::string::to_lower(m[2].str());
							if (func.find("getent") != std::string::npos || func.find("getstruct") != std::string::npos
								|| func.find("getdvar") != std::string::npos)
							{
								continue;
							}
							// a new create call starts a new entity even if the variable name is reused
							var_to_ent.erase(m[1].str());
							auto& ent = ent_for(m[1].str());
							ent.func = func;
							ent.fxid = m[3].str();
							continue;
						}
						if (std::regex_search(line, m, re_org_ang))
						{
							auto& ent = ent_for(m[1].str());
							ent.origin = m[2].str();
							ent.angles = m[3].str();
							continue;
						}
						if (std::regex_search(line, m, re_v))
						{
							auto& ent = ent_for(m[1].str());
							const auto key = utils::string::to_lower(m[2].str());
							const auto value = m[3].str();
							if (key == "origin") ent.origin = trim(value);
							else if (key == "angles") ent.angles = trim(value);
							else if (key == "fxid") ent.fxid = strip_quotes(value);
							else if (key == "delay") ent.delay = trim(value);
							else if (key == "type") ent.type = utils::string::to_lower(strip_quotes(value));
							continue;
						}
					}
				}

				std::string ent_kind(const fx_ent& ent)
				{
					if (!ent.type.empty())
					{
						if (ent.type.find("loop") != std::string::npos && ent.type.find("sound") == std::string::npos) return "loop";
						if (ent.type.find("oneshot") != std::string::npos) return "oneshot";
						if (ent.type.find("exploder") != std::string::npos) return "exploder";
						if (ent.type.find("sound") != std::string::npos) return "sound";
					}
					if (ent.func.find("sound") != std::string::npos) return "sound";
					if (ent.func.find("exploder") != std::string::npos) return "exploder";
					if (ent.func.find("loop") != std::string::npos) return "loop";
					return "oneshot";
				}

				void write_file(const std::string& path, const std::string& data)
				{
					filesystem::file file(path);
					file.open("wb");
					file.write(data);
					file.close();
				}

				// v110 fx: loadfx strings vs written effect names: exact, then normalised (lowercase, '/' separators, no ',' prefix,
				//          no extension), then by base name (only when unique)
				std::string normalize_fx_name(const std::string& raw)
				{
					auto out = utils::string::to_lower(trim(raw));
					std::replace(out.begin(), out.end(), '\\', '/');
					while (!out.empty() && (out.front() == ',' || out.front() == '/')) out.erase(out.begin());
					const auto slash = out.find_last_of('/');
					const auto dot = out.find_last_of('.');
					if (dot != std::string::npos && (slash == std::string::npos || dot > slash))
					{
						out.erase(dot);
					}
					return out;
				}

				std::string base_fx_name(const std::string& normalized)
				{
					const auto slash = normalized.find_last_of('/');
					return slash == std::string::npos ? normalized : normalized.substr(slash + 1);
				}

				struct fx_resolver
				{
					const std::unordered_set<std::string>* written = nullptr;
					std::unordered_map<std::string, std::string> by_norm;
					std::unordered_map<std::string, std::string> by_base;
					std::unordered_set<std::string> ambiguous_base;

					explicit fx_resolver(const std::unordered_set<std::string>& names) : written(&names)
					{
						for (const auto& name : names)
						{
							const auto norm = normalize_fx_name(name);
							by_norm.emplace(norm, name);
							const auto base = base_fx_name(norm);
							if (by_base.contains(base) && by_base[base] != name) ambiguous_base.insert(base);
							else by_base.emplace(base, name);
						}
						for (const auto& base : ambiguous_base) by_base.erase(base);
					}

					std::string resolve(const std::string& raw) const
					{
						if (raw.empty()) return {};
						if (written->contains(raw)) return raw;
						const auto norm = normalize_fx_name(raw);
						if (const auto itr = by_norm.find(norm); itr != by_norm.end()) return itr->second;
						if (const auto itr = by_base.find(base_fx_name(norm)); itr != by_base.end()) return itr->second;
						return {};
					}
				};

				void reset()
				{
					effect_table.clear();
					effect_ids.clear();
					ents.clear();
					vfx_names.clear();
					scripts_parsed = 0;
				}
			}

			void note_vfx(const char* name)
			{
				if (name && *name)
				{
					vfx_names.insert(name[0] == ',' ? name + 1 : name);
				}
			}

			bool is_ambient(const std::string& effect_path)
			{
				// v104 vfx: ent fxid -> loadfx path (or a path ending in /fxid when the table misses it)
				for (const auto& ent : ents)
				{
					if (ent.fxid.empty())
					{
						continue;
					}
					const auto target = normalize_fx_name(effect_path);
					const auto fxid_norm = normalize_fx_name(ent.fxid);
					if (fxid_norm == target || base_fx_name(target) == base_fx_name(fxid_norm))
					{
						return true;
					}
					for (const auto& [id, path] : effect_table)
					{
						if (id == ent.fxid && (path == effect_path || normalize_fx_name(path) == target || base_fx_name(normalize_fx_name(path)) == base_fx_name(target)))
						{
							return true;
						}
					}
				}
				return false;
			}

			void dump(ScriptFile* asset)
			{
				if (!asset || !asset->name)
				{
					return;
				}

				const auto name = resolve_name(asset->name);
				const auto lname = utils::string::to_lower(name);
				const auto& map = filesystem::get_fastfile();
				const bool numeric = utils::string::is_numeric(name);
				if (lname.find("fx") == std::string::npos && (map.empty() || lname.find(map) == std::string::npos) && !numeric)
				{
					return;
				}

				std::string text;
				try
				{
					text = decompile(asset);
				}
				catch (const std::exception& ex)
				{
					ZONETOOL_WARNING("v102 fx: could not decompile script %s: %s", name.data(), ex.what());
					return;
				}
				if (text.empty())
				{
					return;
				}

				auto out_name = name;
				if (!out_name.ends_with(".gsc")) out_name += ".gsc";
				write_file("iw7_gsc/" + out_name, text);

				const auto ltext = utils::string::to_lower(text);
				if (ltext.find("loadfx") == std::string::npos && ltext.find("fxid") == std::string::npos)
				{
					return;
				}

				const auto ents_before = ents.size();
				const auto fx_before = effect_table.size();
				parse(text);
				scripts_parsed++;
				ZONETOOL_INFO("v102 fx: parsed script %s (%zu loadfx, %zu createfx entities)", name.data(),
					effect_table.size() - fx_before, ents.size() - ents_before);
			}

			void finalize(const std::string& map)
			{
				const auto& converted = fxeffectdef::converted_effects();

				if (!map.empty())
				{
					// v102 fx: zone csv lines for iw7_port2.ps1
					std::string csv = "// v102 fx: generated by ZoneTool, append to the map zone csv\n";
					std::vector<std::string> sorted(converted.begin(), converted.end());
					std::sort(sorted.begin(), sorted.end());
					for (const auto& fx : sorted)
					{
						csv += "fx," + fx + "\n";
					}

					const bool have_scripts = !effect_table.empty() || !ents.empty();
					if (have_scripts)
					{
						csv += utils::string::va("rawfile,maps/mp/%s_fx.gsc\n", map.data());
						csv += utils::string::va("rawfile,maps/createfx/%s_fx.gsc\n", map.data());
					}
					write_file(map + "_fx_zone.csv", csv);

					if (have_scripts)
					{
						const fx_resolver resolver(converted);

						// resolve id -> path; ents may reference ids missing from the table (use a written effect matching the id)
						std::unordered_map<std::string, std::string> id_to_path(effect_table.begin(), effect_table.end());
						for (const auto& ent : ents)
						{
							if (ent.fxid.empty() || id_to_path.contains(ent.fxid))
							{
								continue;
							}
							const auto hit = resolver.resolve(ent.fxid);
							if (!hit.empty())
							{
								id_to_path[ent.fxid] = hit;
								effect_table.emplace_back(ent.fxid, hit);
							}
						}

						// v110 fx: match loadfx strings to written effect names robustly; log samples so mismatches are visible
						{
							int shown = 0;
							for (const auto& name : converted)
							{
								if (shown++ >= 5) break;
								ZONETOOL_INFO("v110 fx: written effect sample \"%s\"", name.data());
							}
							shown = 0;
							for (const auto& [id, path] : effect_table)
							{
								if (shown++ >= 8) break;
								const auto hit = resolver.resolve(path);
								ZONETOOL_INFO("v110 fx: loadfx sample id \"%s\" path \"%s\" -> \"%s\"", id.data(), path.data(), hit.empty() ? "<none>" : hit.data());
							}
							std::fflush(stdout);
						}

						std::unordered_set<std::string> usable_ids;
						std::string fx_gsc;
						fx_gsc += utils::string::va("// v102 fx: generated by ZoneTool (iw7 -> h1) from the IW7 fx scripts of %s\n", map.data());
						fx_gsc += "// effects that are IW7 particle systems (vfx) or had nothing convertible are commented out\n\n";
						fx_gsc += "main()\n{\n";
						int missing = 0;
						for (const auto& [id, path] : effect_table)
						{
							auto resolved = resolver.resolve(path);
							if (resolved.empty()) resolved = resolver.resolve(id); // v110 fx: id often is the effect base name
							if (!resolved.empty())
							{
								usable_ids.insert(id);
								fx_gsc += utils::string::va("\tlevel._effect[ \"%s\" ] = loadfx( \"%s\" );\n", id.data(), resolved.data());
							}
							else
							{
								missing++;
								const char* why = vfx_names.contains(path) ? "IW7 particle system (vfx) with no emitter that maps to H1" : "not converted / not in this zone";
								fx_gsc += utils::string::va("\t// level._effect[ \"%s\" ] = loadfx( \"%s\" ); // v102 fx: %s\n", id.data(), path.data(), why);
							}
						}
						fx_gsc += "\n\tif ( !getdvarint( \"r_reflectionProbeGenerate\" ) )\n";
						fx_gsc += utils::string::va("\t\tmaps\\createfx\\%s_fx::main();\n", map.data());
						fx_gsc += "}\n";
						write_file(utils::string::va("maps/mp/%s_fx.gsc", map.data()), fx_gsc);

						std::string cfx;
						cfx += utils::string::va("// v102 fx: generated by ZoneTool (iw7 -> h1) from the IW7 createfx of %s\n", map.data());
						cfx += "// self-contained: spawnfx/triggerfx for one-shots, playfx loops for loop effects (no H1 createfx API needed)\n\n";
						cfx += "main()\n{\n\tlevel thread v102_spawn_createfx();\n}\n\n";
						cfx += "v102_spawn_createfx()\n{\n\twait 0.05;\n";
						int placed = 0, skipped = 0;
						int skipped_kind = 0, skipped_no_origin = 0, skipped_missing = 0;
						int ent_samples = 0;
						for (const auto& ent : ents)
						{
							if (ent_samples++ < 5)
							{
								ZONETOOL_INFO("v110 fx: createfx sample func \"%s\" type \"%s\" fxid \"%s\" origin \"%s\" angles \"%s\" usable=%d", ent.func.data(),
									ent.type.data(), ent.fxid.data(), ent.origin.data(), ent.angles.data(), usable_ids.contains(ent.fxid) ? 1 : 0);
							}
							const auto kind = ent_kind(ent);
							const auto origin = ent.origin.empty() ? "( 0, 0, 0 )"s : ent.origin;
							const auto angles = ent.angles.empty() ? "( 270, 0, 0 )"s : ent.angles;
							if (kind == "exploder" || kind == "sound" || ent.fxid.empty() || ent.origin.empty())
							{
								skipped++;
								if (ent.origin.empty() && kind != "exploder" && kind != "sound") skipped_no_origin++; else skipped_kind++;
								cfx += utils::string::va("\t// v102 fx: skipped %s \"%s\" at %s\n", kind.data(), ent.fxid.data(), origin.data());
								continue;
							}
							if (!usable_ids.contains(ent.fxid))
							{
								skipped++;
								skipped_missing++;
								cfx += utils::string::va("\t// v102 fx: missing effect \"%s\" (%s at %s)\n", ent.fxid.data(), kind.data(), origin.data());
								continue;
							}
							placed++;
							if (kind == "loop")
							{
								const auto delay = ent.delay.empty() ? "0.5"s : ent.delay;
								cfx += utils::string::va("\tlevel thread v102_loop( \"%s\", %s, %s, %s );\n", ent.fxid.data(), origin.data(), angles.data(), delay.data());
							}
							else
							{
								const auto delay = ent.delay.empty() ? "-15"s : ent.delay;
								cfx += utils::string::va("\tv102_oneshot( \"%s\", %s, %s, %s );\n", ent.fxid.data(), origin.data(), angles.data(), delay.data());
							}
						}
						cfx += "}\n\n";
						cfx += "v102_oneshot( id, org, ang, delay )\n{\n";
						cfx += "\tif ( !isdefined( level._effect ) )\n\t\treturn;\n";
						cfx += "\tif ( !isdefined( level._effect[ id ] ) )\n\t\treturn;\n";
						cfx += "\tent = spawnfx( level._effect[ id ], org, anglestoforward( ang ), anglestoup( ang ) );\n";
						cfx += "\ttriggerfx( ent, delay );\n}\n\n";
						cfx += "v102_loop( id, org, ang, delay )\n{\n";
						cfx += "\tif ( !isdefined( level._effect ) )\n\t\treturn;\n";
						cfx += "\tif ( !isdefined( level._effect[ id ] ) )\n\t\treturn;\n";
						cfx += "\tif ( delay <= 0 )\n\t\tdelay = 0.5;\n";
						cfx += "\tfwd = anglestoforward( ang );\n\tup = anglestoup( ang );\n";
						cfx += "\tfor ( ;; )\n\t{\n\t\tplayfx( level._effect[ id ], org, fwd, up );\n\t\twait delay;\n\t}\n}\n";
						write_file(utils::string::va("maps/createfx/%s_fx.gsc", map.data()), cfx);

						ZONETOOL_INFO("v102 fx: %s createfx: %d scripts, %zu effects (%d missing), %zu entities (%d placed, %d skipped)",
							map.data(), scripts_parsed, effect_table.size(), missing, ents.size(), placed, skipped);
						ZONETOOL_INFO("v110 fx: skipped entities: %d exploder/sound/no fxid, %d without origin, %d effect not written",
							skipped_kind, skipped_no_origin, skipped_missing);
					}
					else
					{
						ZONETOOL_INFO("v102 fx: no IW7 fx/createfx script found for %s (%zu effects converted, %zu vfx particle systems seen)",
							map.data(), converted.size(), vfx_names.size());
					}
				}

				std::fflush(stdout);
				reset();
			}
		}
	}
}
