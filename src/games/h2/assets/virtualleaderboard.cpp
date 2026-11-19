#include <std_include.hpp>
#include "virtualleaderboard.hpp"

#include "io/json_reflect.hpp"

namespace zonetool::h2
{
	namespace
	{
		constexpr auto column_fields = glz::tuple
		{
			&VLbColumnDef::name,
			&VLbColumnDef::id,
			&VLbColumnDef::unk1,
			&VLbColumnDef::unk2,
			&VLbColumnDef::unk3,
		};

		constexpr auto virtual_leaderboard_fields = glz::tuple
		{
			&VirtualLeaderboardDef::name,
			&VirtualLeaderboardDef::leaderboardName,
			&VirtualLeaderboardDef::id,
			&VirtualLeaderboardDef::leaderboardId,
			json_reflect::counted(&VirtualLeaderboardDef::columns, &VirtualLeaderboardDef::columnCount, column_fields),
			&VirtualLeaderboardDef::pad,
		};
	}

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

			json_reflect::read<virtual_leaderboard_fields>(data, *asset, mem);

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
	}

	std::string virtual_leaderboard::name()
	{
		return this->name_;
	}

	std::int32_t virtual_leaderboard::type()
	{
		return ASSET_TYPE_VIRTUAL_LEADERBOARD;
	}

	void virtual_leaderboard::read(zone_reader& reader, VirtualLeaderboardDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_string(asset->leaderboardName);
		if (const auto columns = reader.read_array(asset->columns, 3, asset->columnCount))
		{
			for (auto i = 0; i < asset->columnCount; i++)
			{
				reader.read_string(columns[i].name);
			}
		}
		reader.pop_stream();
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

			buf->clear_pointer(&dest->columns);
		}

		buf->pop_stream();
	}

	void virtual_leaderboard::dump(VirtualLeaderboardDef* asset)
	{
		const auto path = asset->name;
		auto file = filesystem::file("virtualleaderboard\\"s + path + ".json"s);
		file.open("wb");

		ordered_json vlb;
		json_reflect::write<virtual_leaderboard_fields>(vlb, *asset);

		auto str = vlb.dump(4);
		file.write(str);

		file.close();
	}
}
