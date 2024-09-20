#include "std_include.hpp"
#include "skeletonscript.hpp"

namespace zonetool::s1
{
	void skeleton_script::add_script_string(scr_string_t* ptr, const char* str)
	{
		for (std::uint32_t i = 0; i < this->script_strings.size(); i++)
		{
			if (this->script_strings[i].first == ptr)
			{
				return;
			}
		}
		this->script_strings.push_back(std::pair<scr_string_t*, const char*>(ptr, str));
	}

	const char* skeleton_script::get_script_string(scr_string_t* ptr)
	{
		for (std::uint32_t i = 0; i < this->script_strings.size(); i++)
		{
			if (this->script_strings[i].first == ptr)
			{
				return this->script_strings[i].second;
			}
		}
		return nullptr;
	}

	SkeletonScript* skeleton_script::parse(std::string name, zone_memory* mem)
	{
		const auto path = "skeletonscript\\"s + name;

		assetmanager::reader read(mem);
		if (!read.open(path))
		{
			return nullptr;
		}

		SkeletonScript* asset = read.read_single<SkeletonScript>();
		asset->name = read.read_string();

		asset->ikData.solverIDs = read.read_array<unsigned char>();
		asset->ikData.floatParams = read.read_array<float>();
		asset->ikData.intParams = read.read_array<int>();
		asset->ikData.stringParams = mem->allocate<scr_string_t>(asset->ikData.numStringParams);
		for (unsigned char i = 0; i < asset->ikData.numStringParams; i++)
		{
			this->add_script_string(&asset->ikData.stringParams[i], read.read_string());
		}

		asset->code = read.read_array<SkeletonScriptCode>();

		read.close();

		return asset;
	}

	void skeleton_script::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = db_find_x_asset_header_copy<SkeletonScript>(XAssetType(this->type()), this->name().data(), mem).skeletonScript;
			auto* original_strings = this->asset_->ikData.stringParams;
			this->asset_->ikData.stringParams = mem->allocate<scr_string_t>(this->asset_->ikData.numStringParams);
			for (unsigned char i = 0; i < this->asset_->ikData.numStringParams; i++)
			{
				this->add_script_string(&this->asset_->ikData.stringParams[i], SL_ConvertToString(original_strings[i]));
			}
		}
	}

	void skeleton_script::prepare(zone_buffer* buf, zone_memory* mem)
	{
		auto* data = this->asset_;

		for (unsigned char i = 0; i < data->ikData.numStringParams; i++)
		{
			data->ikData.stringParams[i] = static_cast<scr_string_t>(buf->write_scriptstring(
				this->get_script_string(&data->ikData.stringParams[i])));
		}
	}

	void skeleton_script::load_depending(zone_base* zone)
	{
	}

	std::string skeleton_script::name()
	{
		return this->name_;
	}

	std::int32_t skeleton_script::type()
	{
		return ASSET_TYPE_SKELETON_SCRIPT;
	}

	void skeleton_script::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->ikData.solverIDs)
		{
			buf->align(0);
			buf->write(data->ikData.solverIDs, data->ikData.numSolvers);
			buf->clear_pointer(&dest->ikData.solverIDs);
		}

		if (data->ikData.floatParams)
		{
			buf->align(3);
			buf->write(data->ikData.floatParams, data->ikData.numFloatParams);
			buf->clear_pointer(&dest->ikData.floatParams);
		}

		if (data->ikData.intParams)
		{
			buf->align(3);
			buf->write(data->ikData.intParams, data->ikData.numIntParams);
			buf->clear_pointer(&dest->ikData.intParams);
		}

		if (data->ikData.stringParams)
		{
			buf->align(3);
			buf->write(data->ikData.stringParams, data->ikData.numStringParams);
			buf->clear_pointer(&dest->ikData.stringParams);
		}

		buf->pop_stream();
	}

	void skeleton_script::dump(SkeletonScript* asset)
	{
		const auto path = "skeletonscript\\"s + asset->name;

		assetmanager::dumper dump;
		if (!dump.open(path))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);

		dump.dump_array(asset->ikData.solverIDs, asset->ikData.numSolvers);
		dump.dump_array(asset->ikData.floatParams, asset->ikData.numFloatParams);
		dump.dump_array(asset->ikData.intParams, asset->ikData.numIntParams);
		for (unsigned char i = 0; i < asset->ikData.numStringParams; i++)
		{
			dump.dump_string(SL_ConvertToString(asset->ikData.stringParams[i]));
		}

		dump.dump_array(asset->code, asset->codeSize);

		dump.close();
	}
}