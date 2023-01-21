#include <std_include.hpp>
#include "soundcontext.hpp"

namespace zonetool::h1
{
	SndContext* ISoundContext::parse(const std::string& name, ZoneMemory* mem)
	{
		if (name.empty())
		{
			return nullptr;
		}

		const auto path = "sndcontext\\"s + name;
		auto file = filesystem::file(path);
		if (file.exists())
		{
			auto* asset = mem->Alloc<SndContext>();
			asset->name = mem->StrDup(name);

			file.open("rb");
			auto bytes = file.read_bytes(file.size());
			memcpy(asset->__pad0, bytes.data(), bytes.size());
			file.close();

			return asset;
		}

		return nullptr;
	}

	void ISoundContext::init(const std::string& name, ZoneMemory* mem)
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
			this->asset_ = DB_FindXAssetHeader_Safe(XAssetType(this->type()), this->name().data()).sndContext;
		}
	}

	void ISoundContext::prepare(ZoneBuffer* buf, ZoneMemory* mem)
	{
	}

	void ISoundContext::load_depending(IZone* zone)
	{
	}

	std::string ISoundContext::name()
	{
		return this->name_;
	}

	std::int32_t ISoundContext::type()
	{
		return ASSET_TYPE_SOUND_CONTEXT;
	}

	void ISoundContext::write(IZone* zone, ZoneBuffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(3);

		dest->name = buf->write_str(this->name());

		buf->pop_stream();
	}

	void ISoundContext::dump(SndContext* asset)
	{
		const auto path = "sndcontext\\"s + asset->name;
		auto file = filesystem::file(path);
		file.open("wb");
		file.write(asset->__pad0, sizeof(asset->__pad0), 1);
		file.close();
	}
}