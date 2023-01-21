#include <std_include.hpp>
#include "fxparticlesimanimation.hpp"

namespace zonetool::s1
{
	FxParticleSimAnimation* IFxParticleSimAnimation::parse(const std::string& name, ZoneMemory* mem)
	{
		assetmanager::reader read(mem);

		const auto path = "particlesimanimation\\"s + name + ".psa"s;
		if (!read.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing particlesimanimation \"%s\"...", name.data());

		auto* asset = read.read_single<FxParticleSimAnimation>();
		asset->name = read.read_string();

		if (asset->material)
		{
			asset->material = read.read_asset<Material>();
		}
		if (asset->particleData)
		{
			asset->particleData = read.read_array<FxParticleSimAnimationParticleData>();
		}
		if (asset->frames)
		{
			asset->frames = read.read_array<FxParticleSimAnimationFrame>();
		}
		if (asset->colorTable)
		{
			asset->colorTable = read.read_array<GfxColorFloat>();
		}

		read.close();

		return asset;
	}

	void IFxParticleSimAnimation::init(const std::string& name, ZoneMemory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->Alloc<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->StrDup(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = DB_FindXAssetHeader_Safe(XAssetType(this->type()), this->name().data()).particleSimAnimation;
		}
	}

	void IFxParticleSimAnimation::prepare(ZoneBuffer* buf, ZoneMemory* mem)
	{
	}

	void IFxParticleSimAnimation::load_depending(IZone* zone)
	{
		if (this->asset_->material)
		{
			zone->add_asset_of_type(ASSET_TYPE_MATERIAL, this->asset_->material->name);
		}
	}

	std::string IFxParticleSimAnimation::name()
	{
		return this->name_;
	}

	std::int32_t IFxParticleSimAnimation::type()
	{
		return ASSET_TYPE_PARTICLE_SIM_ANIMATION;
	}

	void IFxParticleSimAnimation::write(IZone* zone, ZoneBuffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(3);

		dest->name = buf->write_str(this->name());

		if (data->material)
		{
			dest->material = reinterpret_cast<Material*>(zone->get_asset_pointer(ASSET_TYPE_MATERIAL, data->material->name));
		}
		if (data->particleData)
		{
			buf->align(31);
			buf->write(data->particleData, data->header.particleDataCount);
			ZoneBuffer::clear_pointer(&dest->particleData);
		}
		if (data->frames)
		{
			buf->align(3);
			buf->write(data->frames, data->header.frameCount);
			ZoneBuffer::clear_pointer(&dest->frames);
		}
		if (data->colorTable)
		{
			buf->align(3);
			buf->write(data->colorTable, data->header.colorTableSize);
			ZoneBuffer::clear_pointer(&dest->colorTable);
		}

		buf->pop_stream();
	}

	void IFxParticleSimAnimation::dump(FxParticleSimAnimation* asset)
	{
		assetmanager::dumper dump;

		const auto path = "particlesimanimation\\"s + asset->name + ".psa"s;
		if (!dump.open(path))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);

		if (asset->material)
		{
			dump.dump_asset(asset->material);
		}
		if (asset->particleData)
		{
			dump.dump_array(asset->particleData, asset->header.particleDataCount);
		}
		if (asset->frames)
		{
			dump.dump_array(asset->frames, asset->header.frameCount);
		}
		if (asset->colorTable)
		{
			dump.dump_array(asset->colorTable, asset->header.colorTableSize);
		}

		dump.close();
	}
}