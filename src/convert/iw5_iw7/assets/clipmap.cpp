#include <std_include.hpp>

#include "../include.hpp"
#include "clipmap.hpp"
#include "clipmap_collision.hpp"

#include <iw7/assets/clipmap.hpp>
#include <iw7/assets/mapents.hpp>
#include <iw7/common/havok_builder.hpp>

namespace zonetool::iw5::converter::iw7
{
	namespace clipmap
	{
		namespace
		{
			namespace builder = zonetool::iw7::havok::builder;

			constexpr auto brushmodel_physics_asset = "scriptbrushmodeldummydefault";
			constexpr auto physics_asset_key = "51961"; // static canonical string id 0xCAF9, no text form
			constexpr auto trigger_physics_asset = "triggermodeldummydefault";
			constexpr auto ents_trigger_contents = 0xC7FFBFFFu; // all valid contents, stock trigger shapes carry it
			constexpr auto ents_brush_basis = 1ull << 48;
			constexpr auto ents_trigger_surface_flags = 0x40080ull; // constant on every stock trigger tag
			constexpr unsigned short reserved_dynents = 64;

			template <typename Source>
			auto raw_pointer(const Source& pointer)
			{
				return pointer.get();
			}

			zonetool::iw7::XModel* model_reference(const char* name, utils::memory::allocator& allocator)
			{
				const auto reference = allocator.manual_allocate<zonetool::iw7::XModel>(sizeof(const char*));
				reference->name = name;
				return reference;
			}

			void unit_quaternion_to_angles(const float* quat, float angles[3])
			{
				const auto x = quat[0];
				const auto y = quat[1];
				const auto z = quat[2];
				const auto w = quat[3];

				float axis[3][3];
				axis[0][0] = 1.0f - 2.0f * (y * y + z * z);
				axis[0][1] = 2.0f * (x * y + z * w);
				axis[0][2] = 2.0f * (x * z - y * w);
				axis[1][0] = 2.0f * (x * y - z * w);
				axis[1][1] = 1.0f - 2.0f * (x * x + z * z);
				axis[1][2] = 2.0f * (y * z + x * w);
				axis[2][0] = 2.0f * (x * z + y * w);
				axis[2][1] = 2.0f * (y * z - x * w);
				axis[2][2] = 1.0f - 2.0f * (x * x + y * y);

				AxisToAngles(axis, angles);
			}

			void convert_map_triggers(const MapTriggers& source, zonetool::iw7::MapTriggers& destination,
				utils::memory::allocator& allocator)
			{
				destination.count = source.count;
				destination.models = allocator.allocate_array<zonetool::iw7::TriggerModel>(source.count);
				for (unsigned int i = 0; i < source.count; i++)
				{
					destination.models[i].contents = source.models[i].contents;
					destination.models[i].hullCount = source.models[i].hullCount;
					destination.models[i].firstHull = source.models[i].firstHull;
					destination.models[i].windingCount = 0;
					destination.models[i].firstWinding = 0;
					destination.models[i].flags = 0;
					destination.models[i].physicsAsset = nullptr;
					destination.models[i].physicsShapeOverrideIdx = 0xFFFF;
				}

				static_assert(sizeof(zonetool::iw7::TriggerHull) == sizeof(TriggerHull));
				static_assert(sizeof(zonetool::iw7::TriggerSlab) == sizeof(TriggerSlab));

				destination.hullCount = source.hullCount;
				destination.hulls = reinterpret_cast<zonetool::iw7::TriggerHull*>(source.hulls.get());
				destination.slabCount = source.slabCount;
				destination.slabs = reinterpret_cast<zonetool::iw7::TriggerSlab*>(source.slabs.get());

				destination.windingCount = 0;
				destination.windings = nullptr;
				destination.windingPointCount = 0;
				destination.windingPoints = nullptr;
			}

			bool same_bounds(const Bounds& left, const Bounds& right)
			{
				constexpr auto epsilon = 0.01f;
				for (auto axis = 0; axis < 3; axis++)
				{
					if (std::fabs(left.midPoint[axis] - right.midPoint[axis]) > epsilon ||
						std::fabs(left.halfSize[axis] - right.halfSize[axis]) > epsilon)
					{
						return false;
					}
				}
				return true;
			}

			// Only an exact geometric match of exactly one trigger model turns "*N" into "?N".
			int trigger_for_cmodel(const clipMap_t* clipmap, const unsigned int cmodel)
			{
				if (!clipmap || cmodel >= clipmap->numSubModels)
				{
					return -1;
				}

				const auto& bounds = clipmap->cmodels[cmodel].bounds;
				const auto& triggers = clipmap->mapEnts->trigger;
				auto match = -1;
				for (unsigned int model = 0; model < triggers.count; model++)
				{
					const auto& trigger = triggers.models[model];
					for (unsigned int hull = trigger.firstHull; hull < trigger.firstHull + trigger.hullCount; hull++)
					{
						if (hull >= triggers.hullCount || !same_bounds(bounds, triggers.hulls[hull].bounds))
						{
							continue;
						}

						if (match != -1 && match != static_cast<int>(model))
						{
							return -1;
						}
						match = static_cast<int>(model);
					}
				}
				return match;
			}

			std::string fix_entity_model_references(const clipMap_t* clipmap, const std::string& source)
			{
				static const std::regex classname_expr(R"entity("classname"\s+"([^"]*)")entity");
				static const std::regex model_expr(R"entity("model"\s+"([*?])(\d+)")entity");

				std::string result;
				result.reserve(source.size() + 128);
				std::size_t cursor = 0;
				auto rewritten_triggers = 0;
				auto wired_triggers = 0;
				auto wired_brushmodels = 0;

				while (cursor < source.size())
				{
					const auto open = source.find('{', cursor);
					if (open == std::string::npos)
					{
						result.append(source, cursor, std::string::npos);
						break;
					}

					const auto close = source.find('}', open + 1);
					if (close == std::string::npos)
					{
						result.append(source, cursor, std::string::npos);
						break;
					}

					result.append(source, cursor, open - cursor);
					auto entity = source.substr(open, close - open + 1);
					std::smatch classname;
					std::smatch model;
					if (std::regex_search(entity, classname, classname_expr) &&
						std::regex_search(entity, model, model_expr))
					{
						const auto entity_class = classname[1].str();
						const auto model_kind = model[1].str()[0];
						const auto model_index = static_cast<unsigned int>(std::strtoul(model[2].str().c_str(), nullptr, 10));

						auto is_trigger_model = model_kind == '?';
						if (entity_class.starts_with("trigger_") && model_kind == '*')
						{
							const auto trigger = trigger_for_cmodel(clipmap, model_index);
							if (trigger >= 0)
							{
								entity.replace(static_cast<std::size_t>(model.position(2)), static_cast<std::size_t>(model.length(2)), std::to_string(trigger));
								entity[static_cast<std::size_t>(model.position(1))] = '?';
								rewritten_triggers++;
								is_trigger_model = true;
							}
						}

						const auto has_asset_key = entity.find(std::string("\n") + physics_asset_key + " ") != std::string::npos;
						if (is_trigger_model && !has_asset_key)
						{
							entity.insert(entity.size() - 1, std::string(physics_asset_key) + " \"" + trigger_physics_asset + "\"\n");
							wired_triggers++;
						}
						else if (entity_class == "script_brushmodel" && model_kind == '*' && !has_asset_key)
						{
							entity.insert(entity.size() - 1, std::string(physics_asset_key) + " \"" + brushmodel_physics_asset + "\"\n");
							wired_brushmodels++;
						}
					}

					result.append(entity);
					cursor = close + 1;
				}

				if (rewritten_triggers || wired_brushmodels || wired_triggers)
				{
					ZONETOOL_INFO("mapents: rewrote %d trigger model reference(s), wired %d script brush model and %d trigger physics asset(s)",
						rewritten_triggers, wired_brushmodels, wired_triggers);
				}

				return result;
			}

			bool is_spawn_classname(const std::string& classname)
			{
				return classname.starts_with("mp_") && classname.find("_spawn") != std::string::npos;
			}

			// IW7 reads spawn points only from the spawn list (stock entity strings have none), so
			// the mp_*_spawn* entities move out of the entity string into spawn records. A map
			// without free-for-all spawns gets its team deathmatch ones under the dm names too.
			std::string make_spawn_list(const std::string& source, zonetool::iw7::SpawnPointRecordList& list,
				utils::memory::allocator& allocator)
			{
				static const std::regex pair_expr(R"entity("([^"]*)"\s+"([^"]*)")entity");

				struct spawn_point
				{
					std::string classname;
					std::string target;
					std::string script_noteworthy;
					float origin[3]{};
					float angles[3]{};
				};

				std::vector<spawn_point> spawns;
				std::string result;
				result.reserve(source.size());
				std::size_t cursor = 0;

				while (cursor < source.size())
				{
					const auto open = source.find('{', cursor);
					const auto close = open == std::string::npos ? std::string::npos : source.find('}', open + 1);
					if (close == std::string::npos)
					{
						result.append(source, cursor, std::string::npos);
						break;
					}

					const auto entity = source.substr(open, close - open + 1);
					spawn_point spawn{};
					for (auto it = std::sregex_iterator(entity.begin(), entity.end(), pair_expr); it != std::sregex_iterator(); ++it)
					{
						const auto key = (*it)[1].str();
						const auto value = (*it)[2].str();
						if (key == "classname") spawn.classname = value;
						else if (key == "target") spawn.target = value;
						else if (key == "script_noteworthy") spawn.script_noteworthy = value;
						else if (key == "origin") std::sscanf(value.data(), "%f %f %f", &spawn.origin[0], &spawn.origin[1], &spawn.origin[2]);
						else if (key == "angles") std::sscanf(value.data(), "%f %f %f", &spawn.angles[0], &spawn.angles[1], &spawn.angles[2]);
					}

					result.append(source, cursor, open - cursor);
					cursor = close + 1;
					if (!is_spawn_classname(spawn.classname))
					{
						result.append(entity);
						continue;
					}

					spawns.emplace_back(std::move(spawn));
					if (cursor < source.size() && source[cursor] == '\n')
					{
						cursor++; // the removed entity's line break
					}
				}

				const auto has_class = [&](const std::string_view prefix)
				{
					return std::ranges::any_of(spawns, [&](const spawn_point& spawn) { return spawn.classname.starts_with(prefix); });
				};

				if (!has_class("mp_dm_spawn"))
				{
					const auto count = spawns.size();
					for (std::size_t i = 0; i < count; i++)
					{
						if (spawns[i].classname == "mp_tdm_spawn" || (spawns[i].classname.starts_with("mp_tdm_spawn_") && spawns[i].classname.ends_with("_start")))
						{
							auto copy = spawns[i];
							copy.classname = spawns[i].classname == "mp_tdm_spawn" ? "mp_dm_spawn" : "mp_dm_spawn_start";
							spawns.emplace_back(std::move(copy));
						}
					}
				}

				list.spawnsCount = static_cast<unsigned short>(spawns.size());
				list.spawns = allocator.allocate_array<zonetool::iw7::SpawnPointEntityRecord>(spawns.size());
				for (std::size_t i = 0; i < spawns.size(); i++)
				{
					auto& record = list.spawns[i];
					record.index = static_cast<unsigned short>(i);
					record.name = static_cast<zonetool::iw7::scr_string_t>(SL_GetString(spawns[i].classname.data()));
					record.target = static_cast<zonetool::iw7::scr_string_t>(SL_GetString(spawns[i].target.data()));
					record.script_noteworthy = static_cast<zonetool::iw7::scr_string_t>(SL_GetString(spawns[i].script_noteworthy.data()));
					std::memcpy(record.origin, spawns[i].origin, sizeof(record.origin));
					std::memcpy(record.angles, spawns[i].angles, sizeof(record.angles));
				}

				const auto has_intermission = source.find("\"mp_global_intermission\"") != std::string::npos;
				ZONETOOL_INFO("mapents: %zu spawn point(s) moved to the spawn list%s", spawns.size(),
					has_intermission ? "" : ", no mp_global_intermission entity");
				if (!has_class("mp_tdm_spawn"))
				{
					ZONETOOL_WARNING("mapents: no mp_tdm_spawn entities, players cannot spawn in team modes");
				}

				return result;
			}

			void audit_trigger_references(const char* entity_string, const int character_count, const unsigned int model_count)
			{
				if (!entity_string || character_count <= 0)
				{
					return;
				}

				const std::string entities{entity_string, static_cast<std::size_t>(character_count)};

				auto references = 0;
				auto out_of_range = 0;
				auto highest = -1;

				for (auto position = entities.find("\"?"); position != std::string::npos; position = entities.find("\"?", position + 2))
				{
					const auto digits = position + 2;
					auto end = digits;
					while (end < entities.size() && entities[end] >= '0' && entities[end] <= '9')
					{
						end++;
					}

					if (end == digits || end >= entities.size() || entities[end] != '"')
					{
						continue;
					}

					const auto index = std::atoi(entities.substr(digits, end - digits).data());
					references++;
					highest = std::max(highest, index);
					if (index < 0 || static_cast<unsigned int>(index) >= model_count)
					{
						out_of_range++;
					}
				}

				ZONETOOL_INFO("mapents: %u trigger models, %d referenced by entities (highest \"?%d\")", model_count, references, highest);

				if (out_of_range > 0)
				{
					ZONETOOL_WARNING("mapents: %d entity trigger reference(s) point past the end of the %u-model trigger array", out_of_range, model_count);
				}
			}

			builder::polytope to_polytope(collision::convex_hull& hull)
			{
				builder::polytope convex{};
				convex.verts = std::move(hull.verts);
				for (auto& face : hull.faces)
				{
					builder::polytope_face out{};
					std::memcpy(out.plane, face.plane, sizeof(float[4]));
					out.indices = std::move(face.indices);
					convex.faces.emplace_back(std::move(out));
				}
				return convex;
			}

			zonetool::iw7::PhysicsAsset* generate_physics_asset(const char* name, const char* body_name, utils::memory::allocator& allocator)
			{
				const auto physics_asset = allocator.allocate<zonetool::iw7::PhysicsAsset>();
				physics_asset->name = name;

				builder::physics_asset_input physics{};
				physics.body_name = body_name;

				const auto blob = builder::build_physics_asset(physics);
				if (!blob.empty())
				{
					const auto memory = allocator.allocate_array<char>(blob.size());
					std::memcpy(memory, blob.data(), blob.size());
					physics_asset->havokData = memory;
					physics_asset->havokDataSize = static_cast<unsigned int>(blob.size());
				}

				physics_asset->numRigidBodies = 1;
				physics_asset->numConstraints = 0;
				physics_asset->numSFXEventAssets = 1;
				physics_asset->sfxEventAssets = allocator.allocate_array<zonetool::iw7::PhysicsSFXEventAsset*>(1);
				physics_asset->numVFXEventAssets = 1;
				physics_asset->vfxEventAssets = allocator.allocate_array<zonetool::iw7::PhysicsVFXEventAsset*>(1);

				return physics_asset;
			}

			zonetool::iw7::ScriptableDef* generate_scriptable_def_from_dynent(const DynEntityDef* dynent, utils::memory::allocator& allocator)
			{
				const auto model_name = dynent->xModel ? dynent->xModel->name.get() : nullptr;
				const auto name = std::string(model_name ? model_name : "dynent") + "_destruct_" + std::to_string(dynent->health);

				const auto new_def = allocator.allocate<zonetool::iw7::ScriptableDef>();
				new_def->name = allocator.duplicate_string(name);
				new_def->flags = zonetool::iw7::SCRIPTABLE_DEFFLAG_HAS_HEALTH | 0x80;
				new_def->type = 0;
				new_def->nextScriptableDef = nullptr;
				new_def->numParts = 1;
				new_def->parts = allocator.allocate_array<zonetool::iw7::ScriptablePartDef>(1);
				new_def->maxNumDynEntsRequired = 0;
				new_def->partCount = 1;
				new_def->serverInstancedPartCount = 0;
				new_def->serverControlledPartCount = 0;
				new_def->maxNumDynEntPartsBase = 1;
				new_def->maxNumDynEntPartsForSpawning = 0;
				new_def->eventStreamSizeRequiredServer = 0;
				new_def->eventStreamSizeRequiredClient = 0;
				new_def->eventStreamSize = 4;
				new_def->ffMemCost = 0;
				new_def->animationTreeName = 0;
				new_def->animationTreeDef[0] = nullptr;
				new_def->animationTreeDef[1] = nullptr;
				new_def->numXModels = model_name ? 1 : 0;
				new_def->models = nullptr;

				const auto model = model_name ? model_reference(model_name, allocator) : nullptr;
				if (model)
				{
					new_def->models = allocator.allocate_array<zonetool::iw7::XModel*>(1);
					new_def->models[0] = model;
				}

				const auto part = new_def->parts;
				part->name = "";
				part->flags = 0x180;
				part->flatId = 0;
				part->serverInstanceFlatId = 0;
				part->serverControlledFlatId = 0;
				part->eventStreamBufferOffsetServer = 0;
				part->eventStreamBufferOffsetClient = 0;
				part->eventStreamSize = 0;
				part->numStates = 2;
				part->states = allocator.allocate_array<zonetool::iw7::ScriptableStateDef>(2);

				const auto healthy_state = &part->states[0];
				healthy_state->base.name = allocator.duplicate_string("healthy");
				healthy_state->base.flags = 0x80;
				healthy_state->base.numEvents = 1;
				healthy_state->base.events = allocator.allocate_array<zonetool::iw7::ScriptableEventDef>(1);
				healthy_state->type = zonetool::iw7::Scriptable_StateType_Health;
				healthy_state->data.health.base = &healthy_state->base;
				healthy_state->data.health.health = std::max(dynent->health, 1);
				healthy_state->data.health.minimumDamage = 0;
				healthy_state->data.health.damagePropagationFromParent = 1.0f;
				healthy_state->data.health.damagePropagationFromChild = 1.0f;
				healthy_state->data.health.script_id = nullptr;
				healthy_state->data.health.scrScript_id = 0;

				const auto model_event = &healthy_state->base.events[0];
				model_event->base.name = "";
				model_event->type = zonetool::iw7::Scriptable_EventType_Model;
				model_event->data.model.base = &model_event->base;
				model_event->data.model.model = model;
				model_event->data.model.dynamicSimulation = false;
				model_event->data.model.activatePhysics = false;
				model_event->data.model.hudOutlineColor = 0;
				model_event->data.model.hudOutlineActive = true;
				model_event->data.model.hudOutlineFill = false;
				model_event->data.model.neverMoves = false;

				const auto dead_state = &part->states[1];
				dead_state->base.name = allocator.duplicate_string("dead");
				dead_state->base.flags = 0;
				dead_state->base.numEvents = dynent->destroyFx ? 2 : 1;
				dead_state->base.events = allocator.allocate_array<zonetool::iw7::ScriptableEventDef>(dead_state->base.numEvents);
				dead_state->type = zonetool::iw7::Scriptable_StateType_Simple;

				dead_state->data.simple.base = allocator.allocate<zonetool::iw7::ScriptableStateBaseDef>();
				dead_state->data.simple.base->name = dead_state->base.name;
				dead_state->data.simple.base->flags = dead_state->base.flags;
				dead_state->data.simple.base->numEvents = dead_state->base.numEvents;
				dead_state->data.simple.base->events = dead_state->base.events;

				const auto hide_event = &dead_state->base.events[0];
				hide_event->base.name = "";
				hide_event->type = zonetool::iw7::Scriptable_EventType_Model;
				hide_event->data.model.base = &hide_event->base;
				hide_event->data.model.model = nullptr;
				hide_event->data.model.hudOutlineColor = 1; // byte pattern of the stock dead-state event
				hide_event->data.model.hudOutlineActive = false;
				hide_event->data.model.hudOutlineFill = false;
				hide_event->data.model.neverMoves = false;
				hide_event->data.model.dynamicSimulation = false;
				hide_event->data.model.activatePhysics = false;

				if (dynent->destroyFx)
				{
					const auto destroy_event = &dead_state->base.events[1];
					destroy_event->base.name = "";
					destroy_event->type = zonetool::iw7::Scriptable_EventType_PFX;
					destroy_event->data.particleFX.base = &destroy_event->base;
					destroy_event->data.particleFX.stateful = false;

					const auto effect = allocator.manual_allocate<zonetool::iw7::FxEffectDef>(sizeof(const char*));
					effect->name = allocator.duplicate_string(dynent->destroyFx->name.get());
					destroy_event->data.particleFX.effectDef.u.fx = effect;
					destroy_event->data.particleFX.effectDef.type = zonetool::iw7::FX_COMBINED_FX;
					destroy_event->data.particleFX.eventStreamBufferOffsetClient = 0;
				}

				return new_def;
			}

			// The world blob is built first because the ents shape list continues its shape tag table.
			zonetool::iw7::MapEnts* generate_mapents(clipMap_t* clipmap, utils::memory::allocator& allocator,
				const std::vector<builder::shape_tag>& world_tags)
			{
				const auto asset = clipmap->mapEnts.get();
				if (!asset)
				{
					ZONETOOL_ERROR("clipmap \"%s\" has no mapents, entities and triggers cannot be converted", clipmap->name.get());
					return nullptr;
				}

				const auto new_asset = allocator.allocate<zonetool::iw7::MapEnts>();
				new_asset->name = asset->name.get();

				auto entity_string = fix_entity_model_references(clipmap, std::string{asset->entityString.get(), static_cast<std::size_t>(asset->numEntityChars)});
				entity_string = make_spawn_list(entity_string, new_asset->spawnList, allocator);
				new_asset->entityString = allocator.duplicate_string(entity_string);
				new_asset->numEntityChars = static_cast<int>(entity_string.size());

				convert_map_triggers(asset->trigger, new_asset->trigger, allocator);
				convert_map_triggers(asset->clientTrigger.trigger, new_asset->clientTrigger.trigger, allocator);

				audit_trigger_references(new_asset->entityString, new_asset->numEntityChars, new_asset->trigger.count);

				const auto client_count = asset->clientTrigger.trigger.count;

				new_asset->clientTrigger.triggerStringLength = asset->clientTrigger.triggerStringLength;
				new_asset->clientTrigger.triggerString = asset->clientTrigger.triggerString.get();

				const auto filled_shorts = [&](const short value)
				{
					const auto memory = allocator.allocate_array<short>(client_count);
					std::fill(memory, memory + client_count, value);
					return memory;
				};

				new_asset->clientTrigger.triggerType = allocator.allocate_array<unsigned char>(client_count);
				new_asset->clientTrigger.visionSetTriggers = filled_shorts(-1);

				for (unsigned int i = 0; i < client_count; i++)
				{
					if ((asset->clientTrigger.triggerType[i] & CLIENT_TRIGGER_VISIONSET) != 0)
					{
						new_asset->clientTrigger.triggerType[i] |= zonetool::iw7::CLIENT_TRIGGER_VISIONSET;
						new_asset->clientTrigger.visionSetTriggers[i] = static_cast<short>(asset->clientTrigger.triggerStringOffsets[i]);
					}
				}

				new_asset->clientTrigger.origins = reinterpret_cast<float(*)[3]>(asset->clientTrigger.origins.get());
				new_asset->clientTrigger.scriptDelay = asset->clientTrigger.scriptDelay.get();
				new_asset->clientTrigger.audioTriggers = asset->clientTrigger.audioTriggers.get();
				new_asset->clientTrigger.blendLookup = filled_shorts(-1);
				new_asset->clientTrigger.npcTriggers = filled_shorts(-1);
				new_asset->clientTrigger.audioStateIds = filled_shorts(-1);
				new_asset->clientTrigger.audioRvbPanInfo = allocator.allocate_array<zonetool::iw7::CTAudRvbPanInfo>(client_count);
				new_asset->clientTrigger.transientIndex = allocator.allocate_array<short>(client_count);
				new_asset->clientTrigger.linkTo = allocator.allocate_array<zonetool::iw7::ClientEntityLinkToDef*>(client_count);

				new_asset->clientTriggerBlend.numClientTriggerBlendNodes = 0;
				new_asset->clientTriggerBlend.blendNodes = nullptr;

				new_asset->splineList.splineCount = 0;
				new_asset->splineList.splines = nullptr;

				new_asset->havokEntsShapeDataSize = 0;
				new_asset->havokEntsShapeData = nullptr;

				std::vector<unsigned short> cmodel_shape_index(clipmap->numSubModels, 0xFFFF);
				{
					builder::ents_input ents{};
					ents.world_tags = world_tags;
					if (world_tags.empty())
					{
						ZONETOOL_WARNING("mapents: no world shape tag table for \"%s\", world surfaces will resolve against the ents table", asset->name.get());
					}

					auto brush_models = collision::extract_brush_models(clipmap);
					for (auto& model : brush_models)
					{
						builder::ents_shape shape{};
						shape.contents = model.contents;
						shape.entity_contents = static_cast<unsigned int>(model.contents);
						shape.material_crc = model.material_crc;
						shape.user_data = ents_brush_basis | model.surface_flags;
						shape.name = utils::string::va("%s:brushmodel %u", asset->name.get(), model.index);

						for (auto& hull : model.hulls)
						{
							shape.convexes.emplace_back(to_polytope(hull));
						}

						if (model.index < cmodel_shape_index.size())
						{
							cmodel_shape_index[model.index] = static_cast<unsigned short>(ents.shapes.size());
						}
						ents.shapes.emplace_back(std::move(shape));
					}
					std::vector<collision::brush_model>{}.swap(brush_models);

					std::vector<unsigned short> trigger_shape_index(asset->trigger.count, 0xFFFF);
					for (unsigned int t = 0; t < asset->trigger.count; t++)
					{
						auto hulls = collision::extract_trigger_hulls(asset->trigger, t);
						if (hulls.empty())
						{
							continue;
						}

						builder::ents_shape shape{};
						shape.contents = static_cast<int>(ents_trigger_contents);
						shape.entity_contents = ents_trigger_contents;
						shape.user_data = ents_brush_basis | ents_trigger_surface_flags;
						shape.name = utils::string::va("%s:trigger %u", asset->name.get(), t);

						for (auto& hull : hulls)
						{
							shape.convexes.emplace_back(to_polytope(hull));
						}

						trigger_shape_index[t] = static_cast<unsigned short>(ents.shapes.size());
						ents.shapes.emplace_back(std::move(shape));
					}

					builder::ents_tag_merge tag_merge{};
					const auto blob = builder::build_ents_shape_list(ents, &tag_merge);

					if (!blob.empty())
					{
						ZONETOOL_INFO("mapents: havok tag table, world %zu entries, ents %zu (%zu reused from the world table, %zu appended)",
							tag_merge.prefix, tag_merge.total, tag_merge.reused, tag_merge.appended);

						const auto memory = allocator.allocate_array<char>(blob.size());
						std::memcpy(memory, blob.data(), blob.size());
						new_asset->havokEntsShapeData = memory;
						new_asset->havokEntsShapeDataSize = static_cast<unsigned int>(blob.size());
					}
					else
					{
						ZONETOOL_WARNING("mapents: no havok ents shape list generated for \"%s\", brush model entities may fault on link", asset->name.get());
						std::fill(cmodel_shape_index.begin(), cmodel_shape_index.end(), static_cast<unsigned short>(0xFFFF));
						std::fill(trigger_shape_index.begin(), trigger_shape_index.end(), static_cast<unsigned short>(0xFFFF));
					}

					zonetool::iw7::PhysicsAsset* trigger_physics = nullptr;
					auto shaped_triggers = 0;
					for (unsigned int t = 0; t < new_asset->trigger.count; t++)
					{
						if (t >= trigger_shape_index.size() || trigger_shape_index[t] == 0xFFFF)
						{
							continue;
						}

						if (!trigger_physics)
						{
							trigger_physics = generate_physics_asset(trigger_physics_asset, "triggermodeldummy", allocator);
						}

						new_asset->trigger.models[t].physicsShapeOverrideIdx = trigger_shape_index[t];
						new_asset->trigger.models[t].physicsAsset = trigger_physics;
						shaped_triggers++;
					}

					if (shaped_triggers)
					{
						ZONETOOL_INFO("mapents: %d triggers given havok shapes, backed by \"%s\"", shaped_triggers, trigger_physics_asset);
					}
				}

				const auto info = allocator.allocate<zonetool::iw7::ClipInfo>();
				info->planeCount = clipmap->info.planeCount;
				info->planes = allocator.allocate_array<zonetool::iw7::cplane_s>(info->planeCount);
				static_assert(sizeof(zonetool::iw7::cplane_s) == sizeof(cplane_s));
				for (auto i = 0; i < info->planeCount; i++)
				{
					std::memcpy(&info->planes[i], &clipmap->info.planes[i], sizeof(cplane_s));
				}

				zonetool::iw7::PhysicsAsset* brushmodel_physics = nullptr;
				{
					const auto shaped = static_cast<int>(std::count_if(cmodel_shape_index.begin(), cmodel_shape_index.end(),
						[](const unsigned short index) { return index != 0xFFFF; }));

					if (shaped > 0)
					{
						brushmodel_physics = generate_physics_asset(brushmodel_physics_asset, "scriptbrushmodeldummy", allocator);
						ZONETOOL_INFO("mapents: %d brush models given havok shapes, backed by a generated \"%s\"", shaped, brushmodel_physics_asset);
					}
				}

				new_asset->numSubModels = clipmap->numSubModels;
				new_asset->cmodels = allocator.allocate_array<zonetool::iw7::cmodel_t>(clipmap->numSubModels);
				for (unsigned int i = 0; i < clipmap->numSubModels; i++)
				{
					std::memcpy(&new_asset->cmodels[i].bounds, &clipmap->cmodels[i].bounds, sizeof(Bounds));
					new_asset->cmodels[i].radius = clipmap->cmodels[i].radius;
					new_asset->cmodels[i].info = info;

					const auto shape_index = i == 0 ? static_cast<unsigned short>(0xFFFF) : cmodel_shape_index[i];
					new_asset->cmodels[i].physicsShapeOverrideIdx = shape_index;
					new_asset->cmodels[i].physicsAsset = shape_index != 0xFFFF ? brushmodel_physics : nullptr;
					new_asset->cmodels[i].navObstacleIdx = i == 0 ? 0 : 0xFFFF;
				}

				new_asset->dynEntCount[0] = clipmap->dynEntCount[0] + reserved_dynents;
				new_asset->dynEntCount[1] = clipmap->dynEntCount[1];
				new_asset->dynEntCountTotal = new_asset->dynEntCount[0] + new_asset->dynEntCount[1];

				for (auto list = 0; list < 2; list++)
				{
					const auto count = new_asset->dynEntCount[list];
					new_asset->dynEntDefList[list] = allocator.allocate_array<zonetool::iw7::DynEntityDef>(count);
					for (auto basis = 0; basis < 2; basis++)
					{
						new_asset->dynEntPoseList[basis][list] = allocator.allocate_array<zonetool::iw7::DynEntityPose>(count);
						new_asset->dynEntClientList[basis][list] = allocator.allocate_array<zonetool::iw7::DynEntityClient>(count);
					}
					new_asset->dynEntGlobalIdList[list] = allocator.allocate_array<zonetool::iw7::DynEntityGlobalId>(new_asset->dynEntCountTotal);
				}

				struct generated_scriptable
				{
					zonetool::iw7::ScriptableDef* def;
					GfxPlacement pose;
				};
				std::vector<generated_scriptable> scriptable_defs;

				const auto copy_dynents = [&](const int list)
				{
					const auto source_count = static_cast<int>(clipmap->dynEntCount[list]);
					for (auto i = 0; i < source_count; i++)
					{
						const auto new_dynent_def = &new_asset->dynEntDefList[list][i];
						const auto dynent_def = &clipmap->dynEntDefList[list][i];

						if (dynent_def->type == DYNENT_TYPE_DESTRUCT)
						{
							const auto scriptable_def = generate_scriptable_def_from_dynent(dynent_def, allocator);
							scriptable_defs.push_back({scriptable_def, dynent_def->pose});

							new_dynent_def->instanceIndex = static_cast<short>(500 + scriptable_defs.size() - 1);
							new_dynent_def->type = zonetool::iw7::DYNENT_TYPE_HINGE;
							std::memcpy(&new_dynent_def->pose, &dynent_def->pose, sizeof(GfxPlacement));
							new_dynent_def->linkTo = nullptr;
							new_dynent_def->unk5 = 1; // ordinary static scriptable association
							new_dynent_def->unk6 = static_cast<short>(dynent_def->contents);
							new_dynent_def->spawnEnabled = true;
						}
						else
						{
							switch (dynent_def->type)
							{
							case DYNENT_TYPE_CLUTTER:
								new_dynent_def->type = zonetool::iw7::DYNENT_TYPE_CLUTTER;
								std::memcpy(&new_dynent_def->initialPose, &dynent_def->pose, sizeof(GfxPlacement));
								break;
							case DYNENT_TYPE_HINGE:
								new_dynent_def->type = zonetool::iw7::DYNENT_TYPE_HINGE;
								break;
							default:
								new_dynent_def->type = zonetool::iw7::DYNENT_TYPE_INVALID;
								break;
							}

							std::memcpy(&new_dynent_def->pose, &dynent_def->pose, sizeof(GfxPlacement));
							new_dynent_def->baseModel = dynent_def->xModel
								? model_reference(dynent_def->xModel->name.get(), allocator) : nullptr;
							new_dynent_def->brushModel = dynent_def->brushModel;
							new_dynent_def->linkTo = nullptr;
							new_dynent_def->instanceIndex = 0;
							new_dynent_def->spawnEnabled = true;
						}

						const auto source_pose = &clipmap->dynEntPoseList[list][i];
						for (auto basis = 0; basis < 2; basis++)
						{
							const auto pose = &new_asset->dynEntPoseList[basis][list][i];
							std::memcpy(&pose->pose, &source_pose->pose, sizeof(GfxPlacement));
							pose->numPoses = 1;
							pose->poses = allocator.allocate_array<zonetool::iw7::GfxPlacement>(1);
							std::memcpy(&pose->poses[0], &pose->pose, sizeof(zonetool::iw7::GfxPlacement));
							pose->radius = source_pose->radius;
							pose->detailBodyToBoneMap = basis == zonetool::iw7::DYNENT_BASIS_MODEL ? allocator.allocate_array<char>(1) : nullptr;
						}
					}
				};
				copy_dynents(0);
				copy_dynents(1);

				const auto reserved_base = clipmap->dynEntCount[0];
				for (unsigned short i = 0; i < reserved_dynents; i++)
				{
					const auto dynent = &new_asset->dynEntDefList[0][reserved_base + i];
					dynent->type = zonetool::iw7::DYNENT_TYPE_SCRIPTABLEINST;
					dynent->instanceIndex = static_cast<short>(500 + scriptable_defs.size());
					dynent->unk4 = static_cast<short>(i);
					dynent->spawnActive = true;
					dynent->unk5 = 4;
					dynent->unk6 = 0x666;
					dynent->spawnEnabled = true;
				}

				for (unsigned int i = 0; i < new_asset->dynEntCountTotal; i++)
				{
					new_asset->dynEntGlobalIdList[0][i].basis = 0;
					new_asset->dynEntGlobalIdList[0][i].id = i;
					new_asset->dynEntGlobalIdList[1][i].basis = 1;
					new_asset->dynEntGlobalIdList[1][i].id = i;
				}

				std::fill_n(&new_asset->dynEntPhysicsSetupHead[0][0], 4, static_cast<unsigned short>(0xFFFF));
				std::fill_n(&new_asset->dynEntPhysicsSetupTail[0][0], 4, static_cast<unsigned short>(0xFFFF));

				new_asset->dynEntTransientGroupCount = 0;
				new_asset->dynEntTransientGroups = nullptr;
				new_asset->unk3Count = 0;
				new_asset->unk3 = nullptr;
				new_asset->clientEntAnchorCount = 0;
				new_asset->clientEntAnchors = nullptr;

				auto& scriptable = new_asset->scriptableMapEnts;
				scriptable.totalInstanceCount = static_cast<unsigned int>(500 + scriptable_defs.size());
				scriptable.runtimeInstanceCount = 500;
				scriptable.reservedInstanceCount = 500;
				scriptable.instances = allocator.allocate_array<zonetool::iw7::ScriptableInstance>(scriptable.totalInstanceCount);

				const auto capacity = scriptable.totalInstanceCount;
				scriptable.runtimeData.partRuntimeCount = static_cast<int>(capacity);
				scriptable.runtimeData.partRuntime = allocator.allocate_array<zonetool::iw7::ScriptablePartRuntime>(capacity);
				scriptable.runtimeData.partRuntimeLocalClientCount = static_cast<int>(capacity);
				scriptable.runtimeData.partRuntimeLocalClient[0] = allocator.allocate_array<zonetool::iw7::ScriptablePartRuntime>(capacity);
				scriptable.runtimeData.partRuntimeLocalClient[1] = allocator.allocate_array<zonetool::iw7::ScriptablePartRuntime>(capacity);

				const auto write_placement = [](zonetool::iw7::ScriptableInstanceContext& context, const GfxPlacement& pose)
				{
					std::memcpy(context.origin, pose.origin, sizeof(pose.origin));
					unit_quaternion_to_angles(pose.quat, context.angles);
					std::memcpy(context.initialOrigin, pose.origin, sizeof(pose.origin));
					std::memcpy(context.initialAngles, context.angles, sizeof(context.angles));
				};

				for (unsigned int i = 0; i < scriptable_defs.size(); i++)
				{
					auto& instance = scriptable.instances[500 + i];
					const auto definition = scriptable_defs[i].def;

					zonetool::iw7::ScriptableInstanceContext* contexts[3] =
					{
						&instance.contextHeader.context,
						&instance.contextHeaderLocalClient[0].context,
						&instance.contextHeaderLocalClient[1].context,
					};

					const auto stream_size = static_cast<unsigned int>(definition->eventStreamSize);
					for (auto j = 0; j < 3; j++)
					{
						const auto size = stream_size + (j == 0 ? 8u : 16u); // headers the runtime adds to the definition stream
						contexts[j]->def = definition;
						write_placement(*contexts[j], scriptable_defs[i].pose);
						contexts[j]->eventStreamBufferSize = size;
						contexts[j]->eventStreamBuffer = allocator.allocate_array<char>(size);
					}

					std::fill_n(reinterpret_cast<int*>(instance.contextHeader.unk02), 2, -1);
					std::fill_n(reinterpret_cast<int*>(instance.contextHeaderLocalClient[0].unk02), 3, -1);
					std::fill_n(reinterpret_cast<int*>(instance.contextHeaderLocalClient[1].unk02), 3, -1);
				}

				for (auto client = 0; client < 2; client++)
				{
					scriptable.reservedDynents[client].numReservedDynents = reserved_dynents;
					scriptable.reservedDynents[client].reservedDynents =
						allocator.allocate_array<zonetool::iw7::ScriptableReservedDynent>(reserved_dynents);
				}

				new_asset->numMayhemScenes = 0;
				new_asset->mayhemScenes = nullptr;
				new_asset->spawners.spawnerCount = 0;
				new_asset->spawners.spawnerList = nullptr;
				new_asset->audioPASpeakerCount = 0;
				new_asset->audioPASpeakers = nullptr;

				return new_asset;
			}

			void build_world_shape(clipMap_t* asset, zonetool::iw7::clipMap_t* new_asset, utils::memory::allocator& allocator,
				std::vector<builder::shape_tag>& world_tags)
			{
				auto world = collision::extract_world(asset);
				if (world.triangles.empty() && world.convexes.empty())
				{
					return;
				}

				builder::mesh_input input{};
				input.convexes.reserve(world.convexes.size());
				for (auto& source : world.convexes)
				{
					builder::convex convex{};
					convex.verts = std::move(source.verts);
					convex.surface_tag = source.surface_tag;
					convex.contents = source.contents;
					convex.material_crc = source.material_crc;
					convex.user_data = source.user_data;
					input.convexes.emplace_back(std::move(convex));
				}
				input.triangles = std::move(world.triangles);

				const auto blob = builder::build_world_shape(input, &world_tags);
				if (blob.empty())
				{
					ZONETOOL_WARNING("clipmap: no havok world shape generated for \"%s\"", asset->name.get());
					return;
				}

				const auto memory = allocator.allocate_array<char>(blob.size());
				std::memcpy(memory, blob.data(), blob.size());
				new_asset->havokWorldShapeData = memory;
				new_asset->havokWorldShapeDataSize = static_cast<unsigned int>(blob.size());
			}
		}

		zonetool::iw7::clipMap_t* convert(clipMap_t* asset, utils::memory::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<zonetool::iw7::clipMap_t>();

			new_asset->name = asset->name.get();
			new_asset->isInUse = asset->isInUse;

			new_asset->info.planeCount = asset->info.planeCount;
			new_asset->info.planes = reinterpret_cast<zonetool::iw7::cplane_s*>(asset->info.planes.get());
			new_asset->pInfo = &new_asset->info;

			new_asset->numStaticModels = asset->numStaticModels;
			new_asset->staticModelList = allocator.allocate_array<zonetool::iw7::cStaticModel_s>(asset->numStaticModels);
			for (unsigned int i = 0; i < asset->numStaticModels; i++)
			{
				const auto& source = asset->staticModelList[i];
				auto& model = new_asset->staticModelList[i];

				model.xmodel = source.xmodel ? model_reference(source.xmodel->name.get(), allocator) : nullptr;
				std::memcpy(model.origin, source.origin, sizeof(model.origin));
				std::memcpy(model.invScaledAxis, source.invScaledAxis, sizeof(model.invScaledAxis));
				model.unk1 = false;
				model.unk2 = false;
				model.hasTransientModel = false;
				model.hasTransientPhysicsAsset = false;
			}

			new_asset->staticModelCollisionModelList.numModels = asset->numStaticModels;
			new_asset->staticModelCollisionModelList.staticModelIndex = allocator.allocate_array<int>(asset->numStaticModels);
			for (unsigned int i = 0; i < asset->numStaticModels; i++)
			{
				new_asset->staticModelCollisionModelList.staticModelIndex[i] = static_cast<int>(i);
			}
			new_asset->numStaticModelCollisionModelLists = 0;
			new_asset->staticModelCollisionModelLists = nullptr;

			std::vector<builder::shape_tag> world_tags;
			build_world_shape(asset, new_asset, allocator, world_tags);

			new_asset->mapEnts = generate_mapents(asset, allocator, world_tags);

			new_asset->stageCount = asset->stageCount;
			new_asset->stages = allocator.allocate_array<zonetool::iw7::Stage>(asset->stageCount);
			for (unsigned int i = 0; i < asset->stageCount; i++)
			{
				new_asset->stages[i].name = asset->stages[i].name.get();
				std::memcpy(new_asset->stages[i].origin, asset->stages[i].origin, sizeof(float[3]));
				new_asset->stages[i].triggerIndex = asset->stages[i].triggerIndex;
				new_asset->stages[i].sunPrimaryLightIndex = static_cast<char>(asset->stages[i].sunPrimaryLightIndex);
				new_asset->stages[i].entityUID = 0x3A83126F;
			}

			if (new_asset->stageCount > 1) // stage triggers do not survive the dump, so stages past the first have nothing to index
			{
				ZONETOOL_WARNING("clipmap: %u stages but no stage triggers survive the dump, clamping stageCount to 1", new_asset->stageCount);
				new_asset->stageCount = 1;
			}

			for (auto axis = 0; axis < 3; axis++)
			{
				new_asset->broadphaseMin[axis] = -131072.0f;
				new_asset->broadphaseMax[axis] = 131072.0f;
			}

			new_asset->checksum = asset->checksum;

			return new_asset;
		}

		void dump(clipMap_t* asset)
		{
			utils::memory::allocator allocator;
			const auto new_asset = convert(asset, allocator);

			if (new_asset->mapEnts)
			{
				zonetool::iw7::map_ents::dump(new_asset->mapEnts);
			}

			zonetool::iw7::clip_map::dump(new_asset);
		}
	}
}
