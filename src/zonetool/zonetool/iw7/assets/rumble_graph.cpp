#include "std_include.hpp"
#include "rumble_graph.hpp"

namespace zonetool::iw7
{
	RumbleGraph* rumble_graph::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "rumble_graph\\"s + name + ".json"s;

		auto file = filesystem::file(path);
		if (!file.exists())
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing rumble_graph \"%s\"...", name.data());

		// parse json file
		file.open("rb");
		ordered_json data = json::parse(file.read_bytes(file.size()));
		file.close();

		auto asset = mem->allocate<RumbleGraph>();

		asset->name = data["name"].is_null() ? nullptr : mem->duplicate_string(data["name"].get<std::string>());
		for (auto i = 0; i < 16; i++)
		{
			asset->knots[i][0] = data["knots"][i][0].get<float>();
			asset->knots[i][1] = data["knots"][i][0].get<float>();
		}
		asset->knotCount = data["knotCount"].get<unsigned short>();

		return asset;
	}

	void rumble_graph::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).rumbleGraph;
		}
	}

	void rumble_graph::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void rumble_graph::load_depending(zone_base* zone)
	{
	}

	std::string rumble_graph::name()
	{
		return this->name_;
	}

	std::int32_t rumble_graph::type()
	{
		return ASSET_TYPE_RUMBLE_GRAPH;
	}

	void rumble_graph::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		buf->pop_stream();
	}

	void rumble_graph::dump(RumbleGraph* asset)
	{
		const auto path = "rumble_graph\\"s + asset->name + ".json"s;
		auto file = filesystem::file(path);
		file.open("wb");

		ordered_json data;

		data["name"] = asset->name;
		for (auto i = 0; i < 16; i++)
		{
			data["knots"][i][0] = asset->knots[i][0];
			data["knots"][i][1] = asset->knots[i][1];
		}
		data["knotCount"] = asset->knotCount;

		auto str = data.dump(4);
		data.clear();
		file.write(str);
		file.close();
	}
}