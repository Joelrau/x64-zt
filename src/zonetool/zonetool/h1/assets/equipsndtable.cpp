#include <std_include.hpp>
#include "equipsndtable.hpp"

namespace zonetool::h1
{
	EquipmentSoundTable* equip_snd_table::parse(const std::string& name, zone_memory* mem)
	{
		// equipsndtable,soundaliases/equipment_snd.def is only asset in h1 and it doesn't have anything in it.
		auto* asset = mem->allocate<EquipmentSoundTable>();
		asset->name = mem->duplicate_string(name);
		return asset;
	}

	void equip_snd_table::init(const std::string& name, zone_memory* mem)
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
			ZONETOOL_FATAL("Missing equipsndtable asset...");
		}
	}

	void equip_snd_table::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void equip_snd_table::load_depending(zone_base* zone)
	{
	}

	std::string equip_snd_table::name()
	{
		return this->name_;
	}

	std::int32_t equip_snd_table::type()
	{
		return ASSET_TYPE_EQUIPMENT_SND_TABLE;
	}

	void equip_snd_table::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		// won't bother adding this, not used.

		buf->pop_stream();
	}

	void equip_snd_table::dump(EquipmentSoundTable* asset)
	{
	}
}