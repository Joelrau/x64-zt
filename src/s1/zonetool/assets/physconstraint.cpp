#include "std_include.hpp"
#include "physconstraint.hpp"

namespace zonetool::s1
{
	void IPhysConstraint::init(const std::string& name, ZoneMemory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->Alloc<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->StrDup(name);
			return;
		}

		this->asset_ = DB_FindXAssetHeader_Safe(XAssetType(this->type()), this->name().data()).physConstraint;
	}

	void IPhysConstraint::prepare(ZoneBuffer* buf, ZoneMemory* mem)
	{
	}

	void IPhysConstraint::load_depending(IZone* zone)
	{
	}

	std::string IPhysConstraint::name()
	{
		return this->name_;
	}

	std::int32_t IPhysConstraint::type()
	{
		return ASSET_TYPE_PHYSCONSTRAINT;
	}

	void IPhysConstraint::write(IZone* zone, ZoneBuffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(3);

		dest->name = buf->write_str(this->name());

		buf->pop_stream();
	}

	void IPhysConstraint::dump(PhysConstraint* asset)
	{
	}
}