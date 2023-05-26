#include <std_include.hpp>
#include "soundcontext.hpp"

namespace zonetool::s1
{
	SndContext* sound_context::parse(const std::string& name, zone_memory* mem)
	{
		if (name.empty())
		{
			return nullptr;
		}

		const auto path = "sndcontext\\"s + name;
		auto file = filesystem::file(path);
		if (file.exists())
		{
			auto* asset = mem->allocate<SndContext>();
			asset->name = mem->duplicate_string(name);

			file.open("rb");
			auto bytes = file.read_bytes(file.size());
			memcpy(asset->__pad0, bytes.data(), bytes.size());
			file.close();

			return asset;
		}

		return nullptr;
	}

	void sound_context::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).sndContext;
		}
	}

	void sound_context::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void sound_context::load_depending(zone_base* zone)
	{
	}

	std::string sound_context::name()
	{
		return this->name_;
	}

	std::int32_t sound_context::type()
	{
		return ASSET_TYPE_SOUND_CONTEXT;
	}

	void sound_context::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		buf->pop_stream();
	}

	void sound_context::dump(SndContext* asset)
	{
		const auto path = "sndcontext\\"s + asset->name;
		auto file = filesystem::file(path);
		file.open("wb");
		file.write(asset->__pad0, sizeof(asset->__pad0), 1);
		file.close();
	}
}