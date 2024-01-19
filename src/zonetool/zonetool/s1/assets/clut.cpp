#include "std_include.hpp"
#include "clut.hpp"

namespace zonetool::s1
{
	Clut* clut::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);

		const auto path = "clut\\"s + name + ".clut"s;
		if (!read.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing clut \"%s\"...", name.data());

		const auto asset = read.read_single<Clut>();
		asset->name = read.read_string();
		asset->unk = read.read_array<char>();

		return asset;
	}

	void clut::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name_.data()).clut;
		}
	}

	void clut::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void clut::load_depending(zone_base* zone)
	{
	}

	std::string clut::name()
	{
		return this->name_;
	}

	std::int32_t clut::type()
	{
		return ASSET_TYPE_CLUT;
	}

	void clut::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		if (data->unk)
		{
			buf->align(0);
			buf->write(data->unk, 4 * data->count0 * data->count1 * data->count2);
			buf->clear_pointer(&dest->unk);
		}

		dest->name = buf->write_str(this->name());

		buf->pop_stream();
	}

	void clut::dump(Clut* asset)
	{
		const auto path = "clut\\"s + asset->name + ".clut";

		assetmanager::dumper dumper;
		if (!dumper.open(path))
		{
			return;
		}

		dumper.dump_single(asset);
		dumper.dump_string(asset->name);
		dumper.dump_array(asset->unk, 4 * asset->count0 * asset->count1 * asset->count2);

		dumper.close();
	}
}