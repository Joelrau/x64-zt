#include <std_include.hpp>

#include "../include.hpp"
#include "mapents.hpp"
#include "havok.hpp"

#include <iw8/assets/mapents.hpp>

#include <xsk/gsc/engine/iw7.hpp>
#include <xsk/gsc/engine/iw8.hpp>

namespace zonetool::iw7::converter::iw8::mapents
{
	namespace
	{
		namespace target = zonetool::iw8;

		const xsk::gsc::iw7::context& source_tokens()
		{
			static const xsk::gsc::iw7::context context{xsk::gsc::instance::server};
			return context;
		}

		const xsk::gsc::iw8::context& target_tokens()
		{
			static const xsk::gsc::iw8::context context{xsk::gsc::instance::server};
			return context;
		}

		std::string_view next_token(const std::string_view source, std::size_t& pos)
		{
			while (pos < source.size() && std::isspace(static_cast<unsigned char>(source[pos])))
			{
				pos++;
			}

			if (pos >= source.size())
			{
				return {};
			}

			const auto begin = pos;
			if (source[pos] == '"')
			{
				const auto end = source.find('"', pos + 1);
				pos = end == std::string_view::npos ? source.size() : end + 1;
				return source.substr(begin, pos - begin);
			}

			if (source[pos] == '{' || source[pos] == '}')
			{
				return source.substr(pos++, 1);
			}

			while (pos < source.size() && !std::isspace(static_cast<unsigned char>(source[pos])) && source[pos] != '"')
			{
				pos++;
			}

			return source.substr(begin, pos - begin);
		}

		std::string_view unquote(const std::string_view token)
		{
			return token.size() >= 2 && token.front() == '"' ? token.substr(1, token.size() - 2) : token;
		}

		std::uint32_t convert_key(const std::string_view source, std::size_t& pos)
		{
			const auto key = next_token(source, pos);
			std::uint32_t id{};
			if (key.front() == '"' || std::from_chars(key.data(), key.data() + key.size(), id).ptr != key.data() + key.size())
			{
				return target_tokens().token_id(std::string(unquote(key)));
			}

			if (id == 0)
			{
				return target_tokens().token_id(std::string(unquote(next_token(source, pos))));
			}

			const auto name = source_tokens().token_name(id);
			return name.starts_with("_id_") ? 0 : target_tokens().token_id(name);
		}

		std::string convert_entity_string(const MapEnts* asset)
		{
			std::string converted;
			converted.reserve(asset->numEntityChars);

			const std::string_view source(asset->entityString, std::strlen(asset->entityString));
			auto dropped_keys = 0u;
			std::size_t pos{};
			while (true)
			{
				const auto token = next_token(source, pos);
				if (token.empty())
				{
					break;
				}

				if (token == "{")
				{
					converted.append("{");
					while (true)
					{
						auto peek = pos;
						const auto next = next_token(source, peek);
						if (next.empty() || next == "}")
						{
							pos = peek;
							break;
						}

						const auto key = convert_key(source, pos);
						const auto value = next_token(source, pos);
						if (!key)
						{
							dropped_keys++;
							continue;
						}

						converted.append(std::format(" {} \"{}\"", key, unquote(value)));
					}
					converted.append(" }\n");
				}
			}

			if (dropped_keys)
			{
				ZONETOOL_WARNING("mapents \"%s\": %u keys have no IW8 id", asset->name, dropped_keys);
			}

			return converted;
		}

		template <typename T, typename S>
		T* convert_array(const S* source, const unsigned int count, utils::memory::allocator& allocator)
		{
			auto* converted = allocator.allocate_array<T>(count);
			for (auto i = 0u; i < count; i++)
			{
				converted[i] = std::bit_cast<T>(source[i]);
			}

			return converted;
		}

		void convert_splines(const SplineRecordList& source, target::SplineRecordList& converted, utils::memory::allocator& allocator)
		{
			converted.splineCount = source.splineCount;
			converted.splines = allocator.allocate_array<target::SplinePointRecordList>(source.splineCount);
			for (auto i = 0u; i < source.splineCount; i++)
			{
				const auto& spline = source.splines[i];
				auto& result = converted.splines[i];
				result.splinePointCount = spline.splinePointCount;
				result.splineLength = spline.splineLength;
				result.splinePoints = convert_array<target::SplinePointEntityRecord>(spline.splinePoints, spline.splinePointCount, allocator);
			}
		}

		target::SpatialPartition_Population_Tree* make_leaf_tree(utils::memory::allocator& allocator)
		{
			auto* tree = allocator.allocate<target::SpatialPartition_Population_Tree>();
			tree->partitionCount = 1;
			tree->partitions = allocator.allocate<target::SpatialPartition_Population_Partition>();
			tree->extents = allocator.allocate<target::ExtentBounds>();
			return tree;
		}

		target::SpatialPartition_Population* make_empty_population(utils::memory::allocator& allocator)
		{
			auto* population = allocator.allocate<target::SpatialPartition_Population>();
			population->tree = make_leaf_tree(allocator);
			population->partitionCount = 1;
			population->buckets = allocator.allocate<target::SpatialPartition_Population_Node>();
			population->buckets->spatialNodeNext = 0xFFFFFFFF;
			return population;
		}

		void convert_submodels(const MapEnts* asset, target::MapEnts* converted, utils::memory::allocator& allocator)
		{
			converted->numSubModels = 0; // map_ents shapes are not converted
			converted->cmodels = allocator.allocate_array<target::cmodel_t>(asset->numSubModels);
			for (auto i = 0u; i < asset->numSubModels; i++)
			{
				const auto& model = asset->cmodels[i];
				auto& cmodel = converted->cmodels[i];
				cmodel.bounds = std::bit_cast<target::Bounds>(model.bounds);
				cmodel.radius = model.radius;
				cmodel.physicsShapeOverrideIdx = 0xFFFF; // map_ents shapes are not converted
				cmodel.navObstacleIdx = model.navObstacleIdx;
				cmodel.edgeFirstIndex = model.edgeFirstIndex;
			}

			const auto shape_data = havok::convert_ents_shape_list(std::string_view(asset->havokEntsShapeData, asset->havokEntsShapeDataSize));
			converted->havokEntsShapeDataSize = static_cast<unsigned int>(shape_data.size());
			converted->havokEntsShapeData = allocator.allocate_array<char>(shape_data.size());
			std::memcpy(converted->havokEntsShapeData, shape_data.data(), shape_data.size());
		}
	}

	void convert_triggers(const MapTriggers& source, target::MapTriggers& converted, utils::memory::allocator& allocator)
	{
		converted.count = source.count;
		converted.models = allocator.allocate_array<target::TriggerModel>(source.count);
		for (auto i = 0u; i < source.count; i++)
		{
			const auto& model = source.models[i];
			auto& result = converted.models[i];
			result.contents = model.contents;
			result.hullCount = model.hullCount;
			result.firstHull = model.firstHull;
			result.windingCount = model.windingCount;
			result.firstWinding = model.firstWinding;
			result.flags = model.flags;
			result.physicsShapeOverrideIdx = 0xFFFF; // map_ents shapes are not converted
		}

		converted.hullCount = source.hullCount;
		converted.hulls = convert_array<target::TriggerHull>(source.hulls, source.hullCount, allocator);
		converted.slabCount = source.slabCount;
		converted.slabs = convert_array<target::TriggerSlab>(source.slabs, source.slabCount, allocator);
		converted.windingCount = source.windingCount;
		converted.windings = convert_array<target::TriggerWinding>(source.windings, source.windingCount, allocator);
		converted.windingPointCount = source.windingPointCount;
		converted.windingPoints = convert_array<target::TriggerWindingPoint>(source.windingPoints, source.windingPointCount, allocator);
	}

	void dump(MapEnts* asset)
	{
		utils::memory::allocator allocator;

		auto* converted = allocator.allocate<target::MapEnts>();
		converted->name = asset->name;
		auto entity_string = convert_entity_string(asset);
		converted->entityString = entity_string.data();
		converted->numEntityChars = static_cast<int>(entity_string.size() + 1);

		convert_triggers(asset->trigger, converted->trigger, allocator); // TODO: client triggers are not converted

		converted->clientTriggerBlend.numClientTriggerBlendNodes = asset->clientTriggerBlend.numClientTriggerBlendNodes;
		converted->clientTriggerBlend.blendNodes = convert_array<target::ClientTriggerBlendNode>(asset->clientTriggerBlend.blendNodes,
			asset->clientTriggerBlend.numClientTriggerBlendNodes, allocator);

		converted->spawnList.spawnsCount = asset->spawnList.spawnsCount;
		converted->spawnList.spawns = convert_array<target::SpawnPointEntityRecord>(asset->spawnList.spawns, asset->spawnList.spawnsCount, allocator);
		convert_splines(asset->splineList, converted->splineList, allocator);

		convert_submodels(asset, converted, allocator); // TODO: havok ents shapes, dynents and scriptables are not converted

		for (auto& population : converted->dynEntSpatialPopulation)
		{
			population = make_empty_population(allocator);
		}

		converted->scriptableMapEnts.spatialPopulationTree = make_leaf_tree(allocator);

		target::map_ents::dump(converted);
		add_csv_row("map_ents", asset->name);
	}
}
