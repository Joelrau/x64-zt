#include <std_include.hpp>
#include "proceduralbones.hpp"

namespace zonetool::s4
{
	namespace
	{
		template <typename Walk>
		void walk_procedural_bones(Walk walk, XAnimProceduralBones& asset)
		{
			if (const auto constraints = walk.array(asset.constraints, asset.numConstraints, 3))
			{
				for (auto i = 0u; i < asset.numConstraints; i++)
				{
					walk.inline_script_strings(constraints[i].sourceBoneNames, std::size(constraints[i].sourceBoneNames));
				}
			}

			walk.array(asset.unknownRecords, asset.numUnknownRecords, 3);
			walk.script_strings(asset.targetBoneNames, asset.numTargetBones);
			walk.script_strings(asset.unknownBoneNames, asset.numUnknownBoneNames);
		}
	}

	void procedural_bones::read(zone_reader& reader, XAnimProceduralBones* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		walk_procedural_bones(read_walk{reader}, *asset);
		reader.pop_stream();
	}

	void procedural_bones::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_procedural_bones(write_walk{zone, buf}, *dest);
		buf->pop_stream();
	}

	void procedural_bones::dump(XAnimProceduralBones* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("proceduralbones\\"s + asset->name + ".proceduralbonesb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_procedural_bones(dump_walk{dump}, *asset);
		dump.close();
	}

	XAnimProceduralBones* procedural_bones::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("proceduralbones\\"s + name + ".proceduralbonesb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing proceduralbones \"%s\"...", name.data());

		auto* asset = read.read_single<XAnimProceduralBones>();
		asset->name = read.read_string();
		walk_procedural_bones(parse_script_walk{{read}, this->script_strings_}, *asset);
		read.close();
		return asset;
	}

	void procedural_bones::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<XAnimProceduralBones>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void procedural_bones::prepare(zone_buffer* buf, zone_memory* mem)
	{
		if (!this->referenced())
		{
			walk_procedural_bones(prepare_walk{buf, this->script_strings_}, *this->asset_);
		}
	}

	std::string procedural_bones::name()
	{
		return this->name_;
	}

	std::int32_t procedural_bones::type()
	{
		return ASSET_TYPE_XANIM_PROCEDURALBONES;
	}
}
