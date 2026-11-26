#include <std_include.hpp>
#include "dynamicbones.hpp"

namespace zonetool::s4
{
	namespace
	{
		template <typename Walk>
		void walk_noise(Walk walk, XAnimDynamicBoneNoiseProperties& properties)
		{
			for (auto& axis : properties.axis)
			{
				walk.asset(ASSET_TYPE_XANIMCURVE, axis.curve);
			}
		}

		template <typename Walk>
		void walk_dynamic_bones(Walk walk, XAnimDynamicBones& asset)
		{
			if (const auto bones = walk.array(asset.dynamicBoneProperties, asset.numDynamicBones, 7))
			{
				for (auto i = 0u; i < asset.numDynamicBones; i++)
				{
					walk.script_string(bones[i].boneName);
					walk_noise(walk, bones[i].noise.translation);
					walk_noise(walk, bones[i].noise.rotation);
				}
			}

			if (const auto bones = walk.array(asset.collidableBones, asset.numCollidableBones, 3))
			{
				for (auto i = 0u; i < asset.numCollidableBones; i++)
				{
					walk.script_string(bones[i].boneName);
				}
			}

			walk.array(asset.collisionShapes, asset.numCollisionShapes, 3);
			walk.array(asset.customPinConstraints, asset.numCustomPinConstraints, 3);
			walk.script_strings(asset.pinConstraintBoneNames, asset.numPinConstraintBoneNames);
		}

		template <typename Walk>
		void walk_anim_curve(Walk walk, XAnimCurve& asset)
		{
			walk.array(asset.controlPoints, asset.numControlPoints, 3);
		}
	}

	void dynamic_bones::read(zone_reader& reader, XAnimDynamicBones* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		walk_dynamic_bones(read_walk{reader}, *asset);
		reader.pop_stream();
	}

	void dynamic_bones::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_dynamic_bones(write_walk{zone, buf}, *dest);
		buf->pop_stream();
	}

	void dynamic_bones::dump(XAnimDynamicBones* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("dynamicbones\\"s + asset->name + ".dynamicbonesb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_dynamic_bones(dump_walk{dump}, *asset);
		dump.close();
	}

	XAnimDynamicBones* dynamic_bones::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("dynamicbones\\"s + name + ".dynamicbonesb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing dynamicbones \"%s\"...", name.data());

		auto* asset = read.read_single<XAnimDynamicBones>();
		asset->name = read.read_string();
		walk_dynamic_bones(parse_script_walk{{read}, this->script_strings_}, *asset);
		read.close();
		return asset;
	}

	void dynamic_bones::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<XAnimDynamicBones>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void dynamic_bones::load_depending(zone_base* zone)
	{
		if (this->referenced())
		{
			return;
		}

		walk_dynamic_bones(depend_walk{zone}, *this->asset_);
	}

	void dynamic_bones::prepare(zone_buffer* buf, zone_memory* mem)
	{
		if (!this->referenced())
		{
			walk_dynamic_bones(prepare_walk{buf, this->script_strings_}, *this->asset_);
		}
	}

	std::string dynamic_bones::name()
	{
		return this->name_;
	}

	std::int32_t dynamic_bones::type()
	{
		return ASSET_TYPE_XANIM_DYNAMICBONES;
	}

	void anim_curve::read(zone_reader& reader, XAnimCurve* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		walk_anim_curve(read_walk{reader}, *asset);
		reader.pop_stream();
	}

	void anim_curve::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_anim_curve(write_walk{zone, buf}, *dest);
		buf->pop_stream();
	}

	void anim_curve::dump(XAnimCurve* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("animcurve\\"s + asset->name + ".animcurveb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_anim_curve(dump_walk{dump}, *asset);
		dump.close();
	}

	XAnimCurve* anim_curve::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("animcurve\\"s + name + ".animcurveb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing animcurve \"%s\"...", name.data());

		auto* asset = read.read_single<XAnimCurve>();
		asset->name = read.read_string();
		walk_anim_curve(parse_walk{read}, *asset);
		read.close();
		return asset;
	}

	void anim_curve::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<XAnimCurve>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	std::string anim_curve::name()
	{
		return this->name_;
	}

	std::int32_t anim_curve::type()
	{
		return ASSET_TYPE_XANIMCURVE;
	}
}
