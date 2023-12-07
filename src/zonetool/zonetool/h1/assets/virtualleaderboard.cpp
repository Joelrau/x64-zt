#include <std_include.hpp>
#include "virtualleaderboard.hpp"

namespace zonetool::h1
{
	VirtualLeaderboardDef* virtual_leaderboard::parse(const std::string& name, zone_memory* mem)
	{
		auto file = filesystem::file("virtualleaderboard\\"s + name + ".json"s);
		file.open("rb");

		if (file.get_fp())
		{
			ZONETOOL_INFO("Parsing virtualleaderboarddef \"%s\"...", name.data());

			const auto size = file.size();
			auto bytes = file.read_bytes(size);
			file.close();

			auto data = json::parse(bytes);

			auto asset = mem->allocate<VirtualLeaderboardDef>();

			asset->name = mem->duplicate_string(data["name"].get<std::string>());
			asset->leaderboardName = mem->duplicate_string(data["leaderboardName"].get<std::string>());
			asset->id = data["id"].get<int>();
			asset->leaderboardId = data["leaderboardId"].get<int>();

			auto columns = data["columns"];
			auto columns_count = static_cast<int>(data["columns"].size());
			if (columns_count)
			{
				asset->columns = mem->allocate<VLbColumnDef>(columns_count);
				asset->columnCount = columns_count;

				for (auto i = 0; i < columns_count; i++)
				{
					asset->columns[i].name = mem->duplicate_string(columns[i]["name"].get<std::string>());
					asset->columns[i].id = columns[i]["id"].get<int>();
					asset->columns[i].unk1 = columns[i]["unk1"].get<int>();
					asset->columns[i].unk2 = columns[i]["unk2"].get<int>();
					asset->columns[i].unk3 = columns[i]["unk3"].get<int>();
				}
			}

			asset->pad = data["pad"].get<int>();

			return asset;
		}

		return nullptr;
	}

	void virtual_leaderboard::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name_.data()).virtualLeaderboard;
		}
	}

	void virtual_leaderboard::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void virtual_leaderboard::load_depending(zone_base* zone)
	{
		auto* asset = this->asset_;

		if (asset->leaderboardName)
		{
			zone->add_asset_of_type(ASSET_TYPE_LEADERBOARD, asset->leaderboardName);
		}
	}

	std::string virtual_leaderboard::name()
	{
		return this->name_;
	}

	std::int32_t virtual_leaderboard::type()
	{
		return ASSET_TYPE_VIRTUAL_LEADERBOARD;
	}

	void virtual_leaderboard::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());
		dest->leaderboardName = buf->write_str(data->leaderboardName);

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
			}

			zone_buffer::clear_pointer(&dest->columns);
		}

		buf->pop_stream();
	}

	void virtual_leaderboard::dump(VirtualLeaderboardDef* asset)
	{
		const auto path = asset->name;
		auto file = filesystem::file("virtualleaderboard\\"s + path + ".json"s);
		file.open("wb");

		ordered_json vlb;
		vlb["name"] = asset->name;
		vlb["leaderboardName"] = asset->leaderboardName;
		vlb["id"] = asset->id;
		vlb["leaderboardId"] = asset->leaderboardId;

		vlb["columns"] = {};
		for (auto i = 0; i < asset->columnCount; i++)
		{
			auto* col = &asset->columns[i];
			ordered_json column;
			column["name"] = col->name;
			column["id"] = col->id;
			column["unk1"] = col->unk1;
			column["unk2"] = col->unk2;
			column["unk3"] = col->unk3;

			vlb["columns"][i] = column;
		}

		vlb["pad"] = asset->pad;

		auto str = vlb.dump(4);
		file.write(str);

		file.close();
	}
}