#include "std_include.hpp"
#include "clut.hpp"

namespace zonetool::h1
{
	Clut* IClut::parse(const std::string& name, ZoneMemory* mem)
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

	void IClut::init(const std::string& name, ZoneMemory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->Alloc<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->StrDup(name);
			return;
		}

		this->asset_ = parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = DB_FindXAssetHeader_Safe(XAssetType(this->type()), this->name_.data()).clut;
		}
	}

	void IClut::prepare(ZoneBuffer* buf, ZoneMemory* mem)
	{
	}

	void IClut::load_depending(IZone* zone)
	{
	}

	std::string IClut::name()
	{
		return this->name_;
	}

	std::int32_t IClut::type()
	{
		return ASSET_TYPE_CLUT;
	}

	void IClut::write(IZone* zone, ZoneBuffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(3);

		if (data->unk)
		{
			buf->align(0);
			buf->write(data->unk, 4 * data->count0 * data->count1 * data->count2);
			ZoneBuffer::clear_pointer(&dest->unk);
		}

		dest->name = buf->write_str(this->name());

		buf->pop_stream();
	}

	void IClut::dump(Clut* asset)
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