#include <std_include.hpp>
#include "impactfx.hpp"

namespace zonetool::h1
{
	namespace
	{
		std::string get_name(std::string name)
		{
			if (name.empty())
			{
				return "_";
			}
			return name;
		}
	}

	constexpr int TABLE_SIZE = 19;
	constexpr int NONFLESH_SIZE = 53;
	constexpr int FLESH_SIZE = 7;

	FxImpactTable* impact_fx::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "impactfx\\"s + get_name(name) + ".json"s;

		auto file = filesystem::file(path);
		if (!file.exists())
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing impactfx \"%s\"...", name.data());

		// parse json file
		file.open("rb");
		auto size = file.size();
		auto bytes = file.read_bytes(size);
		file.close();
		ordered_json data = json::parse(bytes);

		auto* asset = mem->allocate<FxImpactTable>();

		auto asset_name = data["name"];
		asset->name = (!asset_name.is_null()) ? mem->duplicate_string(asset_name.get<std::string>()) : nullptr;

		if (!data["table"].is_null()) 
		{
			auto tables = data["table"];
			asset->table = mem->allocate<FxImpactEntry>(TABLE_SIZE);

			for (int table_i = 0; table_i < TABLE_SIZE; table_i++) 
			{
				auto table = tables[table_i];

				for (int nonflesh_i = 0; nonflesh_i < NONFLESH_SIZE; nonflesh_i++) 
				{
					auto non_flesh = table["nonflesh"][nonflesh_i].get<std::string>();
					if (!non_flesh.empty()) 
					{
						asset->table[table_i].nonflesh[nonflesh_i] = mem->allocate<FxEffectDef>(1);
						asset->table[table_i].nonflesh[nonflesh_i]->name = mem->duplicate_string(non_flesh);
					}
					else 
					{
						asset->table[table_i].nonflesh[nonflesh_i] = nullptr;
					}
				}

				for (int flesh_i = 0; flesh_i < FLESH_SIZE; flesh_i++) 
				{
					auto flesh = table["flesh"][flesh_i].get<std::string>();
					if (!flesh.empty()) 
					{
						asset->table[table_i].flesh[flesh_i] = mem->allocate<FxEffectDef>(1);
						asset->table[table_i].flesh[flesh_i]->name = mem->duplicate_string(flesh);
					}
					else 
					{
						asset->table[table_i].flesh[flesh_i] = nullptr;
					}
				}
			}
		}

		return asset;
	}

	void impact_fx::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).impactFx;
		}
	}

	void impact_fx::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void impact_fx::load_depending(zone_base* zone)
	{
		auto* data = this->asset_;

		if (data->table)
		{
			auto data_tables = data->table;
			for (auto table_i = 0; table_i < TABLE_SIZE; table_i++)
			{
				auto* data_table = &data_tables[table_i];
				for (auto nonflesh_i = 0; nonflesh_i < NONFLESH_SIZE; nonflesh_i++)
				{
					if (data_table->nonflesh[nonflesh_i])
					{
						zone->add_asset_of_type(ASSET_TYPE_FX, data_table->nonflesh[nonflesh_i]->name);
					}
				}
				for (auto flesh_i = 0; flesh_i < FLESH_SIZE; flesh_i++)
				{
					if (data_table->flesh[flesh_i])
					{
						zone->add_asset_of_type(ASSET_TYPE_FX, data_table->flesh[flesh_i]->name);
					}
				}
			}
		}
	}

	std::string impact_fx::name()
	{
		return this->name_;
	}

	std::int32_t impact_fx::type()
	{
		return ASSET_TYPE_IMPACT_FX;
	}

	void impact_fx::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->table)
		{
			buf->align(3);
			auto data_tables = data->table;
			auto dest_tables = buf->write(data->table, TABLE_SIZE);
			for (auto table_i = 0; table_i < TABLE_SIZE; table_i++)
			{
				auto* data_table = &data_tables[table_i];
				auto* dest_table = &dest_tables[table_i];
				for (auto nonflesh_i = 0; nonflesh_i < NONFLESH_SIZE; nonflesh_i++)
				{
					if (data_table->nonflesh[nonflesh_i])
					{
						dest_table->nonflesh[nonflesh_i] = reinterpret_cast<FxEffectDef*>(
							zone->get_asset_pointer(ASSET_TYPE_FX, data_table->nonflesh[nonflesh_i]->name));
					}
				}
				for (auto flesh_i = 0; flesh_i < FLESH_SIZE; flesh_i++)
				{
					if (data_table->flesh[flesh_i])
					{
						dest_table->flesh[flesh_i] = reinterpret_cast<FxEffectDef*>(
							zone->get_asset_pointer(ASSET_TYPE_FX, data_table->flesh[flesh_i]->name));
					}
				}
			}
			buf->clear_pointer(&dest->table);
		}

		buf->pop_stream();
	}

	void impact_fx::dump(FxImpactTable* asset)
	{
		const auto path = "impactfx\\"s + get_name(asset->name) + ".json"s;
		auto file = filesystem::file(path);
		file.open("wb");

		auto* data = asset;
		ordered_json json_data;

		json_data["name"] = (data->name) ? data->name : nullptr;

		if (data->table)
		{
			auto& data_tables = data->table;
			auto& json_tables = json_data["table"];

			for (int table_i = 0; table_i < TABLE_SIZE; table_i++) 
			{
				auto& data_table = data_tables[table_i];
				auto& json_table = json_tables[table_i];

				for (int nonflesh_i = 0; nonflesh_i < NONFLESH_SIZE; nonflesh_i++) 
				{
					json_table["nonflesh"][nonflesh_i] = (data_table.nonflesh[nonflesh_i])
						? data_table.nonflesh[nonflesh_i]->name
						: "";
				}

				for (int flesh_i = 0; flesh_i < FLESH_SIZE; flesh_i++) 
				{
					json_table["flesh"][flesh_i] = (data_table.flesh[flesh_i])
						? data_table.flesh[flesh_i]->name
						: "";
				}
			}
		}
		else 
		{
			json_data["table"] = nullptr;
		}

		const auto json_str = json_data.dump(4);

		file.write(json_str.data(), json_str.size(), 1);

		file.close();
	}
}