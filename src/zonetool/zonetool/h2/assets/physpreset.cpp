#include "std_include.hpp"
#include "physpreset.hpp"

namespace zonetool::h2
{
	void phys_preset::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).physPreset;
	}

	void phys_preset::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void phys_preset::load_depending(zone_base* zone)
	{
	}

	std::string phys_preset::name()
	{
		return this->name_;
	}

	std::int32_t phys_preset::type()
	{
		return ASSET_TYPE_PHYSPRESET;
	}

	void phys_preset::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(3);

		dest->name = buf->write_str(this->name());

		if (data->sndAliasPrefix)
		{
			dest->sndAliasPrefix = buf->write_str(data->sndAliasPrefix);
		}

		buf->pop_stream();
	}

	void phys_preset::dump(PhysPreset* asset)
	{
	}
}