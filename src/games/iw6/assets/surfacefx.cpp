#include <std_include.hpp>
#include "surfacefx.hpp"

#include "io/json_reflect.hpp"

namespace zonetool::iw6
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

	constexpr int TABLE_SIZE = 6;
	namespace
	{
		constexpr auto surface_entry_fields = glz::tuple
		{
			&SurfaceFxEntry::surfaceEffect,
		};
	}

	constexpr int SURFACE_EFFECT_SIZE =  31;

	SurfaceFxTable* surface_fx::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "surfacefx\\"s + get_name(name) + ".json"s;

		auto file = filesystem::file(path);
		if (!file.exists())
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing surfacefx \"%s\"...", name.data());

		// parse json file
		file.open("rb");
		auto size = file.size();
		auto bytes = file.read_bytes(size);
		file.close();
		ordered_json data = json::parse(bytes);

		auto* asset = mem->allocate<SurfaceFxTable>();

		auto asset_name = data["name"];
		asset->name = (!asset_name.is_null()) ? mem->duplicate_string(asset_name.get<std::string>()) : nullptr;

		if (!data["table"].is_null())
		{
			asset->table = mem->allocate<SurfaceFxEntry>(TABLE_SIZE);
			for (int table_i = 0; table_i < TABLE_SIZE; table_i++)
			{
				json_reflect::read<surface_entry_fields>(data["table"][table_i], asset->table[table_i], mem);
			}
		}

		return asset;
	}

	void surface_fx::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).surfaceFx;
		}
	}

	void surface_fx::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void surface_fx::load_depending(zone_base* zone)
	{
		auto* data = this->asset_;

		if (data->table)
		{
			auto data_tables = data->table;
			for (auto table_i = 0; table_i < TABLE_SIZE; table_i++)
			{
				auto* data_table = &data_tables[table_i];
				for (auto effect_i = 0; effect_i < SURFACE_EFFECT_SIZE; effect_i++)
				{
					if (data_table->surfaceEffect[effect_i])
					{
						zone->add_asset_of_type(ASSET_TYPE_FX, data_table->surfaceEffect[effect_i]->name);
					}
				}
			}
		}
	}

	std::string surface_fx::name()
	{
		return this->name_;
	}

	std::int32_t surface_fx::type()
	{
		return ASSET_TYPE_SURFACE_FX;
	}

	void surface_fx::read(zone_reader& reader, SurfaceFxTable* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		if (const auto tables = reader.read_array(asset->table, 3, TABLE_SIZE))
		{
			for (auto i = 0; i < TABLE_SIZE; i++)
			{
				for (auto& effect : tables[i].surfaceEffect)
				{
					reader.read_asset(ASSET_TYPE_FX, effect);
				}
			}
		}
		reader.pop_stream();
	}

	void surface_fx::write(zone_base* zone, zone_buffer* buf)
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
				for (auto effect_i = 0; effect_i < SURFACE_EFFECT_SIZE; effect_i++)
				{
					if (data_table->surfaceEffect[effect_i])
					{
						dest_table->surfaceEffect[effect_i] = reinterpret_cast<FxEffectDef*>(
							zone->get_asset_pointer(ASSET_TYPE_FX, data_table->surfaceEffect[effect_i]->name));
					}
				}
			}
			buf->clear_pointer(&dest->table);
		}

		buf->pop_stream();
	}

	void surface_fx::dump(SurfaceFxTable* asset)
	{
		const auto path = "surfacefx\\"s + get_name(asset->name) + ".json"s;
		auto file = filesystem::file(path);
		file.open("wb");

		auto* data = asset;
		ordered_json json_data;

		json_data["name"] = (data->name) ? data->name : nullptr;

		if (data->table)
		{
			json_reflect::write_array<surface_entry_fields>(json_data["table"], data->table, TABLE_SIZE);
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
