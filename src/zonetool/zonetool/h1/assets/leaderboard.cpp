#include <std_include.hpp>
#include "leaderboard.hpp"

namespace zonetool::h1
{
	LeaderboardDef* leaderboard::parse(const std::string& name, zone_memory* mem)
	{
		auto file = filesystem::file("leaderboard\\"s + name + ".json"s);
		file.open("rb");

		if (file.get_fp())
		{
			ZONETOOL_INFO("Parsing leaderboarddef \"%s\"...", name.data());

			const auto size = file.size();
			auto bytes = file.read_bytes(size);
			file.close();

			auto data = json::parse(bytes);

			auto asset = mem->allocate<LeaderboardDef>();
			asset->name = mem->duplicate_string(data["name"].get<std::string>());
			asset->id = data["id"].get<int>();
			asset->id = data["id2"].get<int>();
			asset->id = data["xpColId"].get<int>();
			asset->id = data["prestigeColId"].get<int>();
			asset->id = data["updateType"].get<LbUpdateType>();
			asset->id = data["trackTypes"].get<LbTrackTypes>();

			auto columns = data["columns"];
			auto columns_count = static_cast<int>(data["columns"].size());
			if (columns_count)
			{
				asset->columns = mem->allocate<LbColumnDef>(columns_count);
				asset->columnCount = columns_count;

				for (auto i = 0; i < columns_count; i++)
				{
					asset->columns[i].name = mem->duplicate_string(columns[i]["name"].get<std::string>());
					asset->columns[i].id = columns[i]["id"].get<int>();
					asset->columns[i].propertyId = columns[i]["propertyId"].get<int>();
					asset->columns[i].hidden = columns[i]["hidden"].get<bool>();
					asset->columns[i].statsGroup = columns[i]["statsGroup"].get<StatsGroup>();
					asset->columns[i].statName = mem->duplicate_string(columns[i]["statName"].get<std::string>());
					asset->columns[i].type = columns[i]["type"].get<LbColType>();
					asset->columns[i].precision = columns[i]["precision"].get<int>();
					asset->columns[i].agg = columns[i]["agg"].get<LbAggType>();
					asset->columns[i].uiCalColX = columns[i]["uiCalColX"].get<int>();
					asset->columns[i].uiCalColY = columns[i]["uiCalColY"].get<int>();
				}
			}

			asset->pad = data["pad"].get<int>();

			return asset;
		}

		return nullptr;
	}

	void leaderboard::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name_.data()).leaderboardDef;
		}
	}

	void leaderboard::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void leaderboard::load_depending(zone_base* zone)
	{
	}

	std::string leaderboard::name()
	{
		return this->name_;
	}

	std::int32_t leaderboard::type()
	{
		return ASSET_TYPE_LEADERBOARD;
	}

	void leaderboard::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->columns)
		{
			buf->align(3);

			auto* dest_columns = buf->write(data->columns, data->columnCount);
			auto* columns = data->columns;
			for (auto i = 0; i < data->columnCount; i++)
			{
				if (columns[i].name)
				{
					dest_columns[i].name = buf->write_str(columns[i].name);
				}
				if (columns[i].statName)
				{
					dest_columns[i].statName = buf->write_str(columns[i].statName);
				}
			}

			zone_buffer::clear_pointer(&dest->columns);
		}

		buf->pop_stream();
	}

	void leaderboard::dump(LeaderboardDef* asset)
	{
		const auto path = asset->name;
		auto file = filesystem::file("leaderboard\\"s + path + ".json"s);
		file.open("wb");

		ordered_json lb;
		lb["name"] = asset->name;
		lb["id"] = asset->id;
		lb["id2"] = asset->id2;

		lb["xpColId"] = asset->xpColId;
		lb["prestigeColId"] = asset->prestigeColId;

		lb["updateType"] = asset->updateType;
		lb["trackTypes"] = asset->trackTypes;

		lb["columns"] = {};
		for (auto i = 0; i < asset->columnCount; i++)
		{
			auto* col = &asset->columns[i];
			ordered_json column;
			column["name"] = col->name;
			column["id"] = col->id;
			column["propertyId"] = col->propertyId;
			column["hidden"] = col->hidden;
			column["statsGroup"] = col->statsGroup;
			column["statName"] = col->statName;
			column["type"] = col->type;
			column["precision"] = col->precision;
			column["agg"] = col->agg;
			column["uiCalColX"] = col->uiCalColX;
			column["uiCalColY"] = col->uiCalColY;

			lb["columns"][i] = column;
		}

		lb["pad"] = asset->pad;

		auto str = lb.dump(4);
		file.write(str);

		file.close();
	}
}