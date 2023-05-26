#include <std_include.hpp>
#include "reverbcurve.hpp"

namespace zonetool::h1
{
	SndCurve* reverb_curve::parse(const std::string& name, zone_memory* mem)
	{
		return sound_curve::parse(name, "reverbsendcurve", mem);
	}

	void reverb_curve::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).reverbCurve;
		}
	}

	void reverb_curve::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void reverb_curve::load_depending(zone_base* zone)
	{
	}

	std::string reverb_curve::name()
	{
		return this->name_;
	}

	std::int32_t reverb_curve::type()
	{
		return ASSET_TYPE_REVERB_CURVE;
	}

	void reverb_curve::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->filename = buf->write_str(this->name());

		buf->pop_stream();
	}

	void reverb_curve::dump(SndCurve* asset)
	{
		sound_curve::dump(asset, "reverbsendcurve");
	}
}