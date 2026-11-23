#include <std_include.hpp>
#include "converter.hpp"

#include <game.hpp>
#include <iw3/iwd.hpp>

#include <iw3_iw4/assets/xanim_parts.hpp>
#include <iw4_iw5/assets/xanim_parts.hpp>
#include <iw5_iw7/assets/xanim_parts.hpp>
#include <iw3_iw4/assets/xmodel.hpp>
#include <iw4_iw5/assets/xmodel.hpp>
#include <iw5_iw7/assets/xmodel.hpp>
#include <iw3_iw4/assets/gfx_image.hpp>
#include <iw4_iw5/assets/gfx_image.hpp>
#include <iw5_iw7/assets/gfx_image.hpp>
#include <iw3_iw4/assets/comworld.hpp>
#include <iw4_iw5/assets/comworld.hpp>
#include <iw5_iw7/assets/comworld.hpp>
#include <iw3_iw4/assets/mapents.hpp>
#include <iw4_iw5/assets/mapents.hpp>
#include <iw5_iw7/assets/mapents.hpp>
#include <iw3_iw4/assets/fx.hpp>
#include <iw4_iw5/assets/fx.hpp>
#include <iw5_iw7/assets/particle_system.hpp>
#include <iw3_iw4/assets/material.hpp>
#include <iw4_iw5/assets/material.hpp>
#include <iw5_iw7/assets/material.hpp>
#include <iw3_iw4/assets/clipmap.hpp>
#include <iw4_iw5/assets/clipmap.hpp>
#include <iw5_iw7/assets/clipmap.hpp>
#include <iw3_iw4/assets/gfxworld.hpp>
#include <iw4_iw5/assets/gfxworld.hpp>
#include <iw5_iw7/assets/gfxworld.hpp>
#include <iw7/assets/netconststrings.hpp>
#include <iw7/assets/fxworld.hpp>
#include <iw7/assets/glassworld.hpp>
#include <iw7/assets/scriptfile.hpp>

#include <utils/compression.hpp>

#include <xsk/gsc/engine/iw7.hpp>

namespace zonetool::convert::iw3_iw7
{
	namespace
	{
		template <typename S>
		S* header_as(void* header)
		{
			return static_cast<S*>(header);
		}

		void dump_level_net_const_strings()
		{
			namespace target = zonetool::iw7;

			static const char* level_tags[]
			{
				"mdl", "mat", "rmb", "rmg", "veh", "efx", "vfx", "loc", "shk", "tag", "hic", "sic",
				"nps", "mic", "wep", "hnt", "anm", "acl", "lui", "sut", "ges", "tgt", "vsn",
			};

			for (auto type = 0; type <= target::NETCONSTSTRINGTYPE_VISION; type++)
			{
				const auto name = std::format("ncs_{}_level", level_tags[type]);

				target::NetConstStrings asset{};
				asset.name = name.data();
				asset.stringType = static_cast<target::NetConstStringType>(type);
				asset.sourceType = target::NETCONSTSTRINGSOURCE_MAP;
				target::net_const_strings::dump(&asset); // empty lists: IW3 has no net const strings
			}
		}

		void dump_empty_glass_world(const char* name)
		{
			namespace target = zonetool::iw7;

			target::G_GlassData data{};
			target::GlassWorld asset{};
			asset.name = name;
			asset.g_glassData = &data;
			target::glass_world::dump(&asset); // IW3 has no breakable glass, IW7 game init reads the glass data of the map
		}

		void dump_empty_fx_world(const char* name)
		{
			zonetool::iw7::FxWorld asset{};
			asset.name = name;
			zonetool::iw7::fx_world::dump(&asset); // IW3 has no glass system, but the zone needs its fx_map
		}

		std::unordered_map<std::string, std::string> level_script_sources;
		std::unordered_map<std::string, std::string> zone_visions;
		std::string level_script_world;

		struct ported_script
		{
			std::string calls;
			std::string functions;
		};

		std::string strip_script_comments(const std::string& text)
		{
			std::string result;
			result.reserve(text.size());
			for (std::size_t i = 0; i < text.size(); i++)
			{
				if (text[i] == '"')
				{
					const auto begin = i++;
					while (i < text.size() && text[i] != '"')
					{
						i += text[i] == '\\' ? 2 : 1;
					}
					result.append(text, begin, i - begin + 1);
				}
				else if (text.compare(i, 2, "//") == 0)
				{
					i = std::min(text.find('\n', i), text.size()) - 1;
				}
				else if (text.compare(i, 2, "/*") == 0)
				{
					const auto end = text.find("*/", i + 2);
					i = end == std::string::npos ? text.size() : end + 1;
				}
				else
				{
					result.push_back(text[i]);
				}
			}

			return result;
		}

		ported_script port_level_threads(const std::string& raw)
		{
			struct function
			{
				std::string text;
				std::string body;
			};

			const auto text = strip_script_comments(raw);
			std::unordered_map<std::string, function> functions;
			const std::regex header(R"(([A-Za-z_]\w*)\s*\([^(){};]*\)\s*\{)");
			std::smatch match;
			for (auto begin = text.cbegin(); std::regex_search(begin, text.cend(), match, header);)
			{
				auto end = match[0].second;
				for (auto depth = 1; end != text.cend() && depth > 0; ++end)
				{
					if (*end == '"')
					{
						for (++end; end != text.cend() && *end != '"'; ++end)
						{
							if (*end == '\\' && end + 1 != text.cend())
							{
								++end;
							}
						}
						if (end == text.cend())
						{
							break;
						}
					}
					else if (*end == '{')
					{
						depth++;
					}
					else if (*end == '}')
					{
						depth--;
					}
				}

				functions[utils::string::to_lower(match[1].str())] = {std::string(match[0].first, end), std::string(match[0].second, end)};
				begin = end;
			}

			ported_script result;
			const auto main = functions.find("main");
			if (main == functions.end())
			{
				return result;
			}

			std::vector<std::string> pending;
			std::unordered_set<std::string> reached;
			const std::regex thread_call(R"((?:^|[;{}])\s*(?:level\s+)?thread\s+([A-Za-z_]\w*)\s*\(\s*\)\s*;)");
			for (auto begin = main->second.body.cbegin(); std::regex_search(begin, main->second.body.cend(), match, thread_call);)
			{
				const auto name = utils::string::to_lower(match[1].str());
				if (name != "main" && functions.contains(name) && reached.insert(name).second)
				{
					pending.push_back(name);
					result.calls += std::format("\n    level thread {}();", name);
				}

				begin = match[1].second;
			}

			const std::regex call(R"(([A-Za-z_]\w*)\s*\()");
			while (!pending.empty())
			{
				const auto current = std::move(pending.back());
				pending.pop_back();
				const auto& entry = functions.at(current);
				result.functions += "\n" + entry.text + "\n";
				for (auto begin = entry.body.cbegin(); std::regex_search(begin, entry.body.cend(), match, call);)
				{
					const auto name = utils::string::to_lower(match[1].str());
					if (name != "main" && functions.contains(name) && reached.insert(name).second)
					{
						pending.push_back(name);
					}

					begin = match[0].second;
				}
			}

			return result;
		}

		std::string port_level_fx(const std::string& map)
		{
			std::string body;
			std::smatch match;

			const auto fx_source = level_script_sources.find(std::format("maps/mp/{}_fx.gsc", map));
			if (fx_source != level_script_sources.end())
			{
				const auto text = strip_script_comments(fx_source->second);
				const std::regex load_fx(R"(level\._effect\s*\[\s*"([^"]+)\"\s*\]\s*=\s*loadfx\s*\(\s*"([^"]+)\"\s*\)\s*;)", std::regex_constants::icase);
				for (auto begin = text.cbegin(); std::regex_search(begin, text.cend(), match, load_fx); begin = match[0].second)
				{
					const auto effect = zonetool::iw5::converter::iw7::particle_system::asset_name(match[2].str());
					body += std::format("\n    level._effect[\"{}\"] = loadfx( \"{}\" );", match[1].str(), effect);
				}
			}

			const auto createfx_source = level_script_sources.find(std::format("maps/createfx/{}_fx.gsc", map));
			if (createfx_source != level_script_sources.end() && !body.empty())
			{
				const auto text = strip_script_comments(createfx_source->second);
				const std::regex statement(R"(ent\s*=\s*maps\\mp\\_utility::create(oneshot|loop)effect\s*\(\s*("[^"]+\")\s*\)\s*;|ent\.v\s*\[\s*("[^"]+\")\s*\]\s*=\s*([^;]+);)", std::regex_constants::icase);
				for (auto begin = text.cbegin(); std::regex_search(begin, text.cend(), match, statement); begin = match[0].second)
				{
					if (match[1].matched)
					{
						body += std::format("\n    ent = scripts\\engine\\utility::create{}effect( {} );", utils::string::to_lower(match[1].str()), match[2].str());
					}
					else
					{
						body += std::format("\n    ent.v[{}] = {};", match[3].str(), match[4].str());
					}
				}
			}

			return body;
		}

		void dump_level_script(const char* world_name)
		{
			namespace target = zonetool::iw7;

			static auto context = []
			{
				auto result = std::make_unique<xsk::gsc::iw7::context>(xsk::gsc::instance::server);
				result->init(xsk::gsc::build::prod, {});
				return result;
			}();

			const auto map = std::filesystem::path(world_name).stem().string();
			const auto name = std::format("scripts/mp/maps/{}/{}", map, map);
			const auto* trace_env = std::getenv("ZT_TRACE_SPAWN");
			const auto trace = trace_env != nullptr && trace_env[0] == '1';
			level_script_world = world_name;

			const auto build_source = [&](const ported_script& ported)
			{
				auto source = std::format(R"(main()
{{{}
    scripts\mp\load::main();
    scripts\mp\compass::setupminimap( "compass_map_{}" );
    game["attackers"] = "allies";
    game["defenders"] = "axis";
    game["allies_outfit"] = "urban";
    game["axis_outfit"] = "woodland";{}{}
}}
)", port_level_fx(utils::string::to_lower(map)), map, trace ? "\n    level thread zt_trace_connect();" : "", ported.calls);

				// ZT_TRACE_SPAWN=1 adds a trace of each player's connect, team, class and spawn steps to the
				// game console, for maps where players stay spectators without any script error.
				if (trace)
				{
					source += R"(
zt_trace_text( value )
{
    if ( !isdefined( value ) )
        return "undefined";

    return "" + value;
}

zt_trace_connect()
{
    print( "zt_trace level " + level.gametype + " teambased=" + zt_trace_text( level.teambased ) + " onlinegame=" + zt_trace_text( level.onlinegame ) + " privatematch=" + getdvarint( "xblive_privatematch" ) + " prematch=" + zt_trace_text( level.prematchperiod ) );

    for (;;)
    {
        level waittill( "connected", player );
        player thread zt_trace_notify( "joined_team" );
        player thread zt_trace_notify( "joined_spectators" );
        player thread zt_trace_notify( "mapCamera_start" );
        player thread zt_trace_notify( "attempted_spawn" );
        player thread zt_trace_notify( "started_spawnPlayer" );
        player thread zt_trace_notify( "spawned_player" );
        player thread zt_trace_notify( "okToSpawn" );
        player thread zt_trace_notify( "becameSpectator" );
        player thread zt_trace_menu();

        if ( !isai( player ) )
            player thread zt_trace_state();
    }
}

zt_trace_notify( event )
{
    self endon( "disconnect" );

    for (;;)
    {
        self waittill( event );
        print( "zt_trace " + gettime() + " " + self.name + " notify " + event );
    }
}

zt_trace_menu()
{
    self endon( "disconnect" );

    for (;;)
    {
        self waittill( "luinotifyserver", channel, choice );
        print( "zt_trace " + gettime() + " " + self.name + " lui " + zt_trace_text( channel ) + " " + zt_trace_text( choice ) );
    }
}

zt_trace_state()
{
    self endon( "disconnect" );
    cameras = scripts\engine\utility::getstructarray( "camera_intro", "targetname" );
    intermissions = getentarray( "mp_global_intermission", "classname" );
    team_spawns = getspawnarray( "mp_tdm_spawn" );
    allies_starts = getspawnarray( "mp_tdm_spawn_allies_start" );
    axis_starts = getspawnarray( "mp_tdm_spawn_axis_start" );
    dm_spawns = getspawnarray( "mp_dm_spawn" );
    print( "zt_trace " + gettime() + " " + self.name + " connect camera_intro=" + cameras.size + " intermission=" + intermissions.size + " tdm=" + team_spawns.size + " allies_start=" + allies_starts.size + " axis_start=" + axis_starts.size + " dm=" + dm_spawns.size );
    last = "";

    for ( i = 0; i < 2400; i++ )
    {
        spawn_count = "undefined";

        if ( isdefined( level.spawnpoints ) )
            spawn_count = "" + level.spawnpoints.size;

        state = "session=" + zt_trace_text( self.sessionstate ) + " team=" + zt_trace_text( self.team ) + " sessionteam=" + zt_trace_text( self.sessionteam ) + " pers_team=" + zt_trace_text( self.pers["team"] ) + " class=" + zt_trace_text( self.pers["class"] ) + " waitclass=" + zt_trace_text( self.waitingtoselectclass ) + " waitspawn=" + zt_trace_text( self.waitingtospawn ) + " hasspawned=" + zt_trace_text( self.hasspawned ) + " game=" + zt_trace_text( game["state"] ) + " grace=" + zt_trace_text( level.ingraceperiod ) + " spawnpoints=" + spawn_count + " prematch_done=" + zt_trace_text( scripts\mp\utility::gameflag( "prematch_done" ) ) + " prematchlook=" + zt_trace_text( self.prematchlook ) + " frozen=" + zt_trace_text( self.controlsfrozen ) + " freezestack=" + zt_trace_text( self.pers["controllerFreezeStack"] ) + " health=" + zt_trace_text( self.health ) + " onground=" + zt_trace_text( self isonground() ) + " pos=" + int( self.origin[0] ) + "," + int( self.origin[1] ) + "," + int( self.origin[2] );

        if ( state != last )
            print( "zt_trace " + gettime() + " " + self.name + " " + state );

        last = state;
        wait 0.25;
    }
}
)";
				}

				return source + ported.functions;
			};

			const auto compile = [&](const std::string& source)
			{
				std::vector<std::uint8_t> data(source.begin(), source.end());
				return context->compiler().compile(name + ".gsc", data);
			};

			const auto assembly = [&]
			{
				const auto level_source = level_script_sources.find(std::format("maps/mp/{}.gsc", utils::string::to_lower(map)));
				if (level_source != level_script_sources.end())
				{
					const auto ported = port_level_threads(level_source->second);
					if (!ported.calls.empty())
					{
						try
						{
							auto result = compile(build_source(ported));
							ZONETOOL_INFO("Level script \"%s\": carried over the threads of the IW3 script", name.data());
							return result;
						}
						catch (const std::exception& error)
						{
							ZONETOOL_WARNING("Level script \"%s\": IW3 threads left out, they do not compile for IW7 (%s)",
								name.data(), error.what());
						}
					}
				}

				return compile(build_source({}));
			}();
			const auto [bytecode, stack, devmap] = context->assembler().assemble(*assembly);
			const auto buffer = utils::compression::zlib::compress(std::string(reinterpret_cast<const char*>(stack.data), stack.size));

			target::ScriptFile script{};
			script.name = name.data();
			script.compressedLen = static_cast<int>(buffer.size());
			script.len = static_cast<int>(stack.size);
			script.bytecodeLen = static_cast<int>(bytecode.size);
			script.buffer = buffer.data();
			script.bytecode = reinterpret_cast<char*>(const_cast<std::uint8_t*>(bytecode.data));
			target::scriptfile::dump(&script); // IW3 level scripts are source gsc, IW7 game init needs a compiled level script
		}

		// The whole IW7 vision schema is written: CoD4 visions share none of its keys, and some engine
		// defaults are unsafe (sunshadowSampleSizeNear 0.25 drops distant shadows to the lightmap).
		struct vision_key
		{
			const char* key;
			const char* value;
		};

		const vision_key vision_schema[] =
		{
			{"colorizationSetCurveLowMin", "0"},
			{"colorizationSetCurveLowMax", "1"},
			{"colorizationSetCurveHighMin", "1"},
			{"colorizationSetCurveHighMax", "0"},
			{"colorizationSetCurveMidMidpoint", "0.5"},
			{"colorizationSetCurveMidRange", "0"},
			{"colorizationSetCurveMidFalloff", "0.5"},
			{"lmhLowScale", "1 1 1"},
			{"lmhLowBalance", "0 0 0"},
			{"lmhLowSaturation", "0"},
			{"lmhMidScale", "1 1 1"},
			{"lmhMidBalance", "0 0 0"},
			{"lmhMidSaturation", "0"},
			{"lmhHighScale", "1 1 1"},
			{"lmhHighBalance", "0 0 0"},
			{"lmhHighSaturation", "0"},
			{"lmhBalancePreserveLum", "1"},
			{"levelsInMin", "0"},
			{"levelsInGamma", "1"},
			{"levelsInMax", "255"},
			{"levelsOutMin", "0"},
			{"levelsOutMax", "255"},
			{"r_primaryLightTweakDiffuseStrength", "1"},
			{"r_primaryLightTweakSpecularStrength", "1"},
			{"r_charLightAmbient", "1 1 1"},
			{"r_volumeLightScatter", "0"},
			{"r_volumeLightScatterLinearAtten", "1"},
			{"r_volumeLightScatterQuadraticAtten", "0"},
			{"r_volumeLightScatterAngularAtten", "0.25"},
			{"r_volumeLightScatterDepthAttenNear", "0"},
			{"r_volumeLightScatterDepthAttenFar", "0"},
			{"r_volumeLightScatterBackgroundDistance", "100000"},
			{"r_volumeLightScatterColor", "0.2 0.2 0.2"},
			{"r_ssaoStrength", "1"},
			{"r_mdaoCullDistance", "1000"},
			{"r_mdaoFadeoutDistance", "900"},
			{"r_mdaoBoneSizeThreshold", "150"},
			{"r_glare_normalStart", "0.125"},
			{"r_glare_normalEnd", "0.75"},
			{"r_glare_normalPower", "1"},
			{"r_glare_normalAmount", "0"},
			{"r_glare_mirrorStart", "0.125"},
			{"r_glare_mirrorEnd", "0.75"},
			{"r_glare_mirrorPower", "1"},
			{"r_glare_mirrorAmount", "0"},
			{"r_rimLightScale", "8"},
			{"r_rimLightStartDistance", "100"},
			{"r_rimLightDistance", "900"},
			{"r_rimLightFill", "2"},
			{"r_EVCompBounds", "0"},
			{"r_EVCompGrayReference", "0.18"},
			{"r_hdrSkyIntensity", "1"},
			{"r_hdrSkyColorTint", "1 1 1"},
			{"r_skyBlendAmount", "0"},
			{"r_skyBlendFeather", "0.001"},
			{"fogDensity", "50000"},
			{"fogStartDist", "0"},
			{"fogColorIntensity", "1"},
			{"fogColor", "1 1 1"},
			{"fogMaxOpacity", "0"},
			{"fogSunDensity", "50000"},
			{"fogSunColorIntensity", "0"},
			{"fogSunColor", "1 1 1"},
			{"fogSunBeginFadeAngle", "0"},
			{"fogSunEndFadeAngle", "55"},
			{"fogSunDir", "0 0 -1"},
			{"fogSkyIntensity", "0"},
			{"fogSkyMinAngle", "0"},
			{"fogSkyMaxAngle", "90"},
			{"tonemapMode", "2"},
			{"tonemapAuto", "1"},
			{"tonemapAutoExposureAdjustCurve", "0"},
			{"tonemapExposure", "10"},
			{"tonemapAutoExposureAdjust", "0"},
			{"tonemapMaxExposure", "9.75"},
			{"tonemapExposureAdjust", "-0.5"},
			{"tonemapAdaptSpeed", "0.05"},
			{"tonemapBlackPoint", "0"},
			{"tonemapWhitePoint", "512.0"},
			{"tonemapCrossover", "2"},
			{"tonemapShoulder", "0.96"},
			{"tonemapToe", "0"},
			{"tonemapDarkEv", "2.84"},
			{"tonemapMidEv", "7.823"},
			{"tonemapLightEv", "12.81"},
			{"tonemapDarkExposureAdjust", "-3.17"},
			{"tonemapMidExposureAdjust", "-0.651"},
			{"tonemapLightExposureAdjust", "1.47"},
			{"tonemapMinExposureAdjust", "-3.17"},
			{"tonemapMaxExposureAdjust", "2.3"},
			{"tonemapGrainStrength", "0.07"},
			{"chromaRadius", "0"},
			{"chromaStrength", "0"},
			{"chromaAberration", "0"},
			{"r_dustHeading", "0"},
			{"r_dustPitch", "-90"},
			{"r_dustTiling", "0.1"},
			{"r_dustIntensity", "0"},
			{"r_dustPowerCurv", "1"},
			{"r_dustSmoothMin", "0.5"},
			{"r_dustSmoothMax", "1"},
			{"analogRewindAmount", "0"},
			{"analogInterferenceAmount", "0"},
			{"analogCrtEffectAmount", "0"},
			{"analogRollEffectAmount", "0"},
			{"analogChromaSeparationEffectAmount", "0"},
			{"toneWhitePoint", "1 1 1"},
			{"veilStrength", "0.06"},
			{"volumetricAirDensity", "0.02"},
			{"volumetricAbsorption", "0"},
			{"volumetricAlbedo", "1 1 1"},
			{"volumetricSunBrightness", "0"},
			{"volumetricSpotBrightness", "0"},
			{"volumetricAmbientBrightness", "0"},
			{"volumetricSunAnisotropy", "0.8"},
			{"sunshadowTransmissiveTint", "0 0 0"},
			{"sunshadowSoftness", "0"},
			{"sunshadowSampleSizeNear", "0.99"},
			{"cloudShadowScale", "1"},
			{"cloudShadowScrollRate", "0"},
			{"cloudShadowScrollHeading", "0"},
			{"cloudShadowOpacity", "0"},
			{"cloudShadowLowThreshold", "0"},
			{"cloudShadowHighThreshold", "1"},
		};

		using vision_values = std::vector<std::pair<std::string, std::string>>;

		// One "key value" or "key "value"" per line, comments and blank lines skipped.
		vision_values parse_vision(const std::string& text)
		{
			vision_values result;
			std::istringstream stream(text);
			std::string line;
			while (std::getline(stream, line))
			{
				if (!line.empty() && line.back() == '\r')
				{
					line.pop_back();
				}

				const auto key_start = line.find_first_not_of(" \t");
				if (key_start == std::string::npos || line.compare(key_start, 2, "//") == 0)
				{
					continue;
				}

				const auto key_end = line.find_first_of(" \t", key_start);
				if (key_end == std::string::npos)
				{
					continue;
				}

				const auto value_start = line.find_first_not_of(" \t", key_end);
				if (value_start == std::string::npos)
				{
					continue;
				}

				std::string value;
				if (line[value_start] == '"')
				{
					const auto close = line.find('"', value_start + 1);
					value = line.substr(value_start + 1, close == std::string::npos ? std::string::npos : close - value_start - 1);
				}
				else
				{
					const auto value_end = line.find_first_of(" \t", value_start);
					value = line.substr(value_start, value_end == std::string::npos ? std::string::npos : value_end - value_start);
				}

				result.emplace_back(line.substr(key_start, key_end - key_start), value);
			}
			return result;
		}

		std::string* find_vision_key(vision_values& values, const char* key)
		{
			for (auto& [name, value] : values)
			{
				if (_stricmp(name.data(), key) == 0)
				{
					return &value;
				}
			}
			return nullptr;
		}

		float vision_float(const std::string& text, const float fallback)
		{
			char* end = nullptr;
			const auto result = std::strtof(text.data(), &end);
			return end == text.data() ? fallback : result;
		}

		// The CoD4 film pass is that game's whole grading. The per-range tints become the lmh
		// scales and desaturation becomes lmh saturation (stock IW7 values read as percent).
		// Film contrast and brightness are left out: IW7's tonemapper already sets both.
		void map_film(vision_values& source, vision_values& result)
		{
			const auto* enable = find_vision_key(source, "r_filmEnable");
			if (!enable || vision_float(*enable, 0.0f) == 0.0f)
			{
				return;
			}

			const std::pair<const char*, const char*> tints[] =
			{
				{"r_filmDarkTint", "lmhLowScale"},
				{"r_filmMediumTint", "lmhMidScale"},
				{"r_filmLightTint", "lmhHighScale"},
			};

			for (const auto& [from, to] : tints)
			{
				if (const auto* tint = find_vision_key(source, from))
				{
					*find_vision_key(result, to) = *tint;
				}
			}

			if (const auto* desaturation = find_vision_key(source, "r_filmDesaturation"))
			{
				const auto saturation = std::format("{}", std::clamp(-100.0f * vision_float(*desaturation, 0.0f), -100.0f, 100.0f));
				*find_vision_key(result, "lmhLowSaturation") = saturation;
				*find_vision_key(result, "lmhMidSaturation") = saturation;
				*find_vision_key(result, "lmhHighSaturation") = saturation;
			}
		}

		// IW7 applies vision/<map>.vision as the level's naked vision. CoD4 keeps visions in the
		// iwd files rather than the zone, so the map's own one is read from there when it exists.
		void dump_vision(const std::string& name, const std::string& iw3_text)
		{
			auto source = parse_vision(iw3_text);

			vision_values result;
			for (const auto& entry : vision_schema)
			{
				result.emplace_back(entry.key, entry.value);
			}

			map_film(source, result);
			for (const auto& [key, value] : source)
			{
				if (auto* target = find_vision_key(result, key.data()))
				{
					*target = value;
				}
			}

			std::string text;
			for (const auto& [key, value] : result)
			{
				text += key + " \"" + value + "\"\r\n";
			}

			filesystem::file file(name);
			file.open("wb");
			file.write(text);
			file.close();
		}

		void dump_level_vision(const char* world_name)
		{
			const auto name = std::format("vision/{}.vision", utils::string::to_lower(std::filesystem::path(world_name).stem().string()));
			const auto zone_vision = zone_visions.find(name);
			dump_vision(name, zone_vision != zone_visions.end() ? zone_vision->second : zonetool::iw3::read_iwd_file(name));
		}

		void dump_asset(const std::int32_t type, void* header)
		{
			namespace source = zonetool::iw3;

			x86::allocator allocator;
			zonetool::iw5::converter::iw7::gfx_image::read_file = zonetool::iw3::read_iwd_file;

			switch (type)
			{
			case source::ASSET_TYPE_XANIMPARTS:
			{
				const auto iw4_asset = source::converter::iw4::xanim_parts::convert(header_as<source::XAnimParts>(header), allocator);
				const auto iw5_asset = zonetool::iw4::converter::iw5::xanim_parts::convert(iw4_asset);
				zonetool::iw5::converter::iw7::xanim_parts::dump(iw5_asset);
				break;
			}
			case source::ASSET_TYPE_XMODEL:
			{
				const auto iw4_asset = source::converter::iw4::xmodel::convert(header_as<source::XModel>(header), allocator);
				const auto iw5_asset = zonetool::iw4::converter::iw5::xmodel::convert(iw4_asset, allocator);
				zonetool::iw5::converter::iw7::xmodel::dump(iw5_asset);
				break;
			}
			case source::ASSET_TYPE_IMAGE:
			{
				const auto iw4_asset = source::converter::iw4::gfx_image::convert(header_as<source::GfxImage>(header), allocator);
				const auto iw5_asset = zonetool::iw4::converter::iw5::gfx_image::convert(iw4_asset);
				zonetool::iw5::converter::iw7::gfx_image::dump(iw5_asset);
				break;
			}
			case source::ASSET_TYPE_COMWORLD:
			{
				const auto iw4_asset = source::converter::iw4::comworld::convert(header_as<source::ComWorld>(header), allocator);
				const auto iw5_asset = zonetool::iw4::converter::iw5::comworld::convert(iw4_asset, allocator);
				zonetool::iw5::converter::iw7::comworld::dump(iw5_asset);
				break;
			}
			case source::ASSET_TYPE_CLIPMAP:
			case source::ASSET_TYPE_CLIPMAP_PVS: // multiplayer maps ship the same struct under this type
			{
				const auto clipmap = header_as<source::clipMap_t>(header);
				const auto iw4_asset = source::converter::iw4::clipmap::convert(clipmap, allocator);
				const auto iw5_asset = zonetool::iw4::converter::iw5::clipmap::convert(iw4_asset, allocator);
				zonetool::iw5::converter::iw7::clipmap::dump(iw5_asset);

				// The clipmap asked for bounds box dynamic bodies on clutter models with no physGeoms.
				// Those models may already be written with a static body, so write them again.
				std::unordered_set<source::XModel*> written;
				for (auto list = 0; list < 2; list++)
				{
					const auto definitions = clipmap->dynEntDefList[list].get();
					for (auto i = 0; definitions && i < clipmap->dynEntCount[list]; i++)
					{
						const auto model = definitions[i].xModel.get();
						if (!model || !model->name || model->physGeoms
							|| !zonetool::iw5::converter::iw7::xmodel::wants_dynamic_box(model->name.get(), nullptr)
							|| !written.insert(model).second)
						{
							continue;
						}

						const auto iw4_model = source::converter::iw4::xmodel::convert(model, allocator);
						const auto iw5_model = zonetool::iw4::converter::iw5::xmodel::convert(iw4_model, allocator);
						zonetool::iw5::converter::iw7::xmodel::dump(iw5_model);
					}
				}
				break;
			}
			case source::ASSET_TYPE_GFXWORLD:
			{
				const auto iw4_asset = source::converter::iw4::gfxworld::convert(header_as<source::GfxWorld>(header), allocator);
				const auto iw5_asset = zonetool::iw4::converter::iw5::gfxworld::convert(iw4_asset, allocator);
				zonetool::iw5::converter::iw7::gfxworld::dump(iw5_asset);
				dump_level_net_const_strings();
				dump_empty_glass_world(iw5_asset->name);
				dump_empty_fx_world(iw5_asset->name);
				dump_level_script(iw5_asset->name);
				dump_level_vision(iw5_asset->name);
				break;
			}
			case source::ASSET_TYPE_MAP_ENTS:
				break; // the clipmap conversion carries the full mapents
			case source::ASSET_TYPE_FX:
			{
				const auto effect = header_as<source::FxEffectDef>(header);
				const auto iw4_asset = source::converter::iw4::fx::convert(effect, allocator);
				const auto iw5_asset = zonetool::iw4::converter::iw5::fx::convert(iw4_asset, allocator);
				zonetool::iw5::converter::iw7::particle_system::dump(iw5_asset);

				// The effect asked for dynamic bodies on its physics model elements. Those models
				// may already be written with a static body, so write them again.
				const auto elements = effect->elemDefs.get();
				const auto element_count = effect->elemDefCountLooping + effect->elemDefCountOneShot + effect->elemDefCountEmission;
				std::unordered_set<source::XModel*> written;
				for (auto e = 0; elements && e < element_count; e++)
				{
					const auto& element = elements[e];
					if (element.elemType != source::FX_ELEM_TYPE_MODEL)
					{
						continue;
					}

					for (auto v = 0; v < element.visualCount; v++)
					{
						const auto visuals = element.visualCount > 1 ? element.visuals.array.get() : nullptr;
						const auto model = element.visualCount > 1 ? (visuals ? visuals[v].model.get() : nullptr) : element.visuals.instance.model.get();
						if (!model || !model->name || model->physGeoms
							|| !zonetool::iw5::converter::iw7::xmodel::wants_dynamic_box(model->name.get(), nullptr)
							|| !written.insert(model).second)
						{
							continue;
						}

						const auto iw4_model = source::converter::iw4::xmodel::convert(model, allocator);
						const auto iw5_model = zonetool::iw4::converter::iw5::xmodel::convert(iw4_model, allocator);
						zonetool::iw5::converter::iw7::xmodel::dump(iw5_model);
					}
				}
				break;
			}
			case source::ASSET_TYPE_MATERIAL:
			{
				const auto iw4_asset = source::converter::iw4::material::convert(header_as<source::Material>(header), allocator);
				const auto iw5_asset = zonetool::iw4::converter::iw5::material::convert(iw4_asset, allocator);
				zonetool::iw5::converter::iw7::material::dump(iw5_asset);
				break;
			}
			case source::ASSET_TYPE_RAWFILE:
			{
				const auto rawfile = header_as<source::RawFile>(header);
				const auto name = utils::string::to_lower(rawfile->name.get());
				if (name.ends_with(".vision") && rawfile->buffer.value)
				{
					zone_visions[name] = std::string(rawfile->buffer.get(), static_cast<std::size_t>(rawfile->len));
					dump_vision(name, zone_visions[name]);
					break;
				}

				source::rawfile::dump(rawfile);
				if (name.ends_with(".gsc") && rawfile->buffer.value)
				{
					level_script_sources[name] = std::string(rawfile->buffer.get(), static_cast<std::size_t>(rawfile->len));
					const auto world = utils::string::to_lower(std::filesystem::path(level_script_world).stem().string());
					const auto is_level_script = name == std::format("maps/mp/{}.gsc", world) || name == std::format("maps/mp/{}_fx.gsc", world)
						|| name == std::format("maps/createfx/{}_fx.gsc", world);
					if (!level_script_world.empty() && is_level_script)
					{
						dump_level_script(level_script_world.data());
					}
				}
				break;
			}
			case source::ASSET_TYPE_STRINGTABLE:
				source::string_table::dump(header_as<source::StringTable>(header));
				break;
			case source::ASSET_TYPE_LOCALIZE_ENTRY:
				source::localize::dump(header_as<source::LocalizeEntry>(header));
				break;
			default:
				break;
			}
		}
	}

	const converter& definition()
	{
		static const converter iw3_iw7
		{
			.source = "iw3",
			.target = "iw7",
			.dump_asset = dump_asset,
		};

		return iw3_iw7;
	}
}
