#include "std_include.hpp"
#include "physconstraint.hpp"

namespace zonetool::s1
{
	void phys_constraint::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).physConstraint;
	}

	void phys_constraint::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void phys_constraint::load_depending(zone_base* zone)
	{
	}

	std::string phys_constraint::name()
	{
		return this->name_;
	}

	std::int32_t phys_constraint::type()
	{
		return ASSET_TYPE_PHYSCONSTRAINT;
	}

	void phys_constraint::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(3);

		dest->name = buf->write_str(this->name());

		buf->pop_stream();
	}

	void phys_constraint::dump(PhysConstraint* asset)
	{
	}
}