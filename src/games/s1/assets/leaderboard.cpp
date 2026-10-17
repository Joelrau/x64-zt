#include <std_include.hpp>
#include "leaderboard.hpp"

#include "io/json_reflect.hpp"

namespace zonetool::s1
{
	namespace
	{
		constexpr auto column_fields = glz::tuple
		{
			&LbColumnDef::name,
			&LbColumnDef::id,
			&LbColumnDef::propertyId,
			&LbColumnDef::hidden,
			&LbColumnDef::statsGroup,
			&LbColumnDef::statName,
			&LbColumnDef::type,
			&LbColumnDef::precision,
			&LbColumnDef::agg,
			&LbColumnDef::uiCalColX,
			&LbColumnDef::uiCalColY,
		};

		constexpr auto leaderboard_fields = glz::tuple
		{
			&LeaderboardDef::name,
			&LeaderboardDef::id,
			&LeaderboardDef::updateType,
			&LeaderboardDef::trackTypes,
			json_reflect::counted(&LeaderboardDef::columns, &LeaderboardDef::columnCount, column_fields),
		};
	}

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
			json_reflect::read<leaderboard_fields>(data, *asset, mem);

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

	void leaderboard::read(zone_reader& reader, LeaderboardDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		if (const auto columns = reader.read_array(asset->columns, 3, asset->columnCount))
		{
			for (auto i = 0; i < asset->columnCount; i++)
			{
				reader.read_string(columns[i].name);
				reader.read_string(columns[i].statName);
			}
		}
		reader.pop_stream();
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

			buf->clear_pointer(&dest->columns);
		}

		buf->pop_stream();
	}

	void leaderboard::dump(LeaderboardDef* asset)
	{
		const auto path = asset->name;
		auto file = filesystem::file("leaderboard\\"s + path + ".json"s);
		file.open("wb");

		ordered_json lb;
		json_reflect::write<leaderboard_fields>(lb, *asset);

		auto str = lb.dump(4);
		file.write(str);

		file.close();
	}
}
