#include "std_include.hpp"
#include "skeletonscript.hpp"

namespace zonetool::h1
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

		ZONETOOL_INFO("Parsing skeletonscript \"%s\"...", name.data());

		SkeletonScript* asset = read.read_single<SkeletonScript>();
		asset->name = read.read_string();

		asset->ikData.charData = read.read_array<char>();
		asset->ikData.floatData = read.read_array<float>();
		asset->ikData.int32Data = read.read_array<int>();
		asset->ikData.strings = mem->allocate<scr_string_t>(asset->ikData.stringsCount);
		for (unsigned char i = 0; i < asset->ikData.stringsCount; i++)
		{
			this->add_script_string(&asset->ikData.strings[i], read.read_string());
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
			auto* original_strings = this->asset_->ikData.strings;
			this->asset_->ikData.strings = mem->allocate<scr_string_t>(this->asset_->ikData.stringsCount);
			for (unsigned char i = 0; i < this->asset_->ikData.stringsCount; i++)
			{
				this->add_script_string(&this->asset_->ikData.strings[i], SL_ConvertToString(original_strings[i]));
			}
		}
	}

	void skeleton_script::prepare(zone_buffer* buf, zone_memory* mem)
	{
		auto* data = this->asset_;

		for (unsigned char i = 0; i < data->ikData.stringsCount; i++)
		{
			data->ikData.strings[i] = static_cast<scr_string_t>(buf->write_scriptstring(
				this->get_script_string(&data->ikData.strings[i])));
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
		return ASSET_TYPE_SKELETONSCRIPT;
	}

	void skeleton_script::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->ikData.charData)
		{
			buf->align(0);
			buf->write(data->ikData.charData, data->ikData.charDataLen);
			buf->clear_pointer(&dest->ikData.charData);
		}

		if (data->ikData.floatData)
		{
			buf->align(3);
			buf->write(data->ikData.floatData, data->ikData.floatDataLen);
			buf->clear_pointer(&dest->ikData.floatData);
		}

		if (data->ikData.int32Data)
		{
			buf->align(3);
			buf->write(data->ikData.int32Data, data->ikData.int32DataLen);
			buf->clear_pointer(&dest->ikData.int32Data);
		}

		if (data->ikData.strings)
		{
			buf->align(3);
			buf->write(data->ikData.strings, data->ikData.stringsCount);
			buf->clear_pointer(&dest->ikData.strings);
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

		dump.dump_array(asset->ikData.charData, asset->ikData.charDataLen);
		dump.dump_array(asset->ikData.floatData, asset->ikData.floatDataLen);
		dump.dump_array(asset->ikData.int32Data, asset->ikData.int32DataLen);
		for (unsigned char i = 0; i < asset->ikData.stringsCount; i++)
		{
			dump.dump_string(SL_ConvertToString(asset->ikData.strings[i]));
		}

		dump.dump_array(asset->code, asset->codeLen);

		dump.close();
	}
}