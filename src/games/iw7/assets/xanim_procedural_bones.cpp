#include <std_include.hpp>
#include "xanim_procedural_bones.hpp"

namespace zonetool::iw7
{
	namespace
	{
		std::string asset_path(const std::string& name)
		{
			return "proceduralbones\\" + name;
		}

		template <typename Visitor>
		void visit_script_strings(XAnimProceduralBones* asset, Visitor&& visit)
		{
			for (auto i = 0u; i < asset->numConstraints; i++)
			{
				visit(asset->constraints[i].sourceBoneNames[0]);
				visit(asset->constraints[i].sourceBoneNames[1]);
			}

			for (auto i = 0u; i < asset->numTargetBones; i++)
			{
				visit(asset->targetBoneNames[i]);
			}

			for (auto i = 0u; i < asset->unk01_count; i++)
			{
				visit(asset->unk01[i].unk01);
			}

			for (auto i = 0u; i < asset->unk02_count; i++)
			{
				visit(asset->unk02[i].unk01);
			}
		}
	}

	XAnimProceduralBones* xanim_procedural_bones::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open(asset_path(name)))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing proceduralbones \"%s\"...", name.data());

		auto* asset = read.read_single<XAnimProceduralBones>();
		asset->name = read.read_string();
		asset->constraints = read.read_array<XAnimConstraint>();
		asset->targetBoneNames = read.read_array<scr_string_t>();
		asset->unk01 = read.read_array<unk_1453E1E68>();
		for (auto i = 0u; i < asset->unk01_count; i++)
		{
			asset->unk01[i].unk02 = read.read_string();
			asset->unk01[i].unk03 = read.read_string();
		}
		asset->unk02 = read.read_array<unk_1453E1E30>();
		asset->unk03 = read.read_array<unk_1453E1EA8>();

		visit_script_strings(asset, [&](scr_string_t&)
		{
			this->script_strings_.emplace_back(read.read_string());
		});

		read.close();
		return asset;
	}

	void xanim_procedural_bones::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<XAnimProceduralBones>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
		if (this->asset_)
		{
			return;
		}

		this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), name).proceduralBones;
		if (!this->asset_)
		{
			ZONETOOL_FATAL("Could not find proceduralbones \"%s\"", name.data());
		}

		visit_script_strings(this->asset_, [&](const scr_string_t value)
		{
			this->script_strings_.emplace_back(SL_ConvertToString(value));
		});
	}

	void xanim_procedural_bones::prepare(zone_buffer* buf, zone_memory* mem)
	{
		if (this->referenced())
		{
			return;
		}

		auto script_string = this->script_strings_.begin();
		visit_script_strings(this->asset_, [&](scr_string_t& value)
		{
			value = static_cast<scr_string_t>(buf->write_scriptstring(*script_string++));
		});
	}

	std::string xanim_procedural_bones::name()
	{
		return this->name_;
	}

	std::int32_t xanim_procedural_bones::type()
	{
		return ASSET_TYPE_XANIM_PROCEDURALBONES;
	}

	void xanim_procedural_bones::read(zone_reader& reader, XAnimProceduralBones* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto constraints = reader.read_array(asset->constraints, 3, asset->numConstraints))
		{
			for (auto i = 0u; i < asset->numConstraints; i++)
			{
				reader.read_script_strings(constraints[i].sourceBoneNames, 2);
			}
		}

		reader.read_script_string_array(asset->targetBoneNames, asset->numTargetBones);

		if (const auto entries = reader.read_array(asset->unk01, 7, asset->unk01_count))
		{
			for (auto i = 0u; i < asset->unk01_count; i++)
			{
				reader.read_script_string(entries[i].unk01);
				reader.read_string(entries[i].unk02);
				reader.read_string(entries[i].unk03);
			}
		}

		if (const auto entries = reader.read_array(asset->unk02, 3, asset->unk02_count))
		{
			for (auto i = 0u; i < asset->unk02_count; i++)
			{
				reader.read_script_string(entries[i].unk01);
			}
		}

		reader.read_array(asset->unk03, 3, asset->unk03_count);
		reader.pop_stream();
	}

	void xanim_procedural_bones::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->constraints)
		{
			buf->align(3);
			buf->write(data->constraints, data->numConstraints);
			buf->clear_pointer(&dest->constraints);
		}

		if (data->targetBoneNames)
		{
			buf->align(3);
			buf->write(data->targetBoneNames, data->numTargetBones);
			buf->clear_pointer(&dest->targetBoneNames);
		}

		if (data->unk01)
		{
			buf->align(7);
			auto* dest_entries = buf->write(data->unk01, data->unk01_count);
			for (auto i = 0u; i < data->unk01_count; i++)
			{
				if (data->unk01[i].unk02)
				{
					dest_entries[i].unk02 = buf->write_str(data->unk01[i].unk02);
				}

				if (data->unk01[i].unk03)
				{
					dest_entries[i].unk03 = buf->write_str(data->unk01[i].unk03);
				}
			}
			buf->clear_pointer(&dest->unk01);
		}

		if (data->unk02)
		{
			buf->align(3);
			buf->write(data->unk02, data->unk02_count);
			buf->clear_pointer(&dest->unk02);
		}

		if (data->unk03)
		{
			buf->align(3);
			buf->write(data->unk03, data->unk03_count);
			buf->clear_pointer(&dest->unk03);
		}

		buf->pop_stream();
	}

	void xanim_procedural_bones::dump(XAnimProceduralBones* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open(asset_path(asset->name)))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		dump.dump_array(asset->constraints, asset->numConstraints);
		dump.dump_array(asset->targetBoneNames, asset->numTargetBones);
		dump.dump_array(asset->unk01, asset->unk01_count);
		for (auto i = 0u; i < asset->unk01_count; i++)
		{
			dump.dump_string(asset->unk01[i].unk02);
			dump.dump_string(asset->unk01[i].unk03);
		}
		dump.dump_array(asset->unk02, asset->unk02_count);
		dump.dump_array(asset->unk03, asset->unk03_count);

		visit_script_strings(asset, [&](const scr_string_t value)
		{
			dump.dump_string(SL_ConvertToString(value));
		});

		dump.close();
	}
}
