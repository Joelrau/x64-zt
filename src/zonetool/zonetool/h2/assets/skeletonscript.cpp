#include "std_include.hpp"
#include "skeletonscript.hpp"

namespace zonetool::h2
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

	void skeleton_script::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = db_find_x_asset_header_copy<SkeletonScript>(XAssetType(this->type()), this->name().data(), mem).skeletonScript;
		auto* original_strings = this->asset_->ikData.strings;
		this->asset_->ikData.strings = mem->allocate<scr_string_t>(this->asset_->ikData.stringsCount);
		for (unsigned char i = 0; i < this->asset_->ikData.stringsCount; i++)
		{
			this->add_script_string(&this->asset_->ikData.strings[i], SL_ConvertToString(original_strings[i]));
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

		buf->push_stream(3);

		dest->name = buf->write_str(this->name());
		/*
		if (data->ikData.charData)
		{
			buf->align(0);
			buf->write(data->ikData.charData, data->ikData.charDataLen);
			zone_buffer::clear_pointer(&dest->ikData.charData);
		}
		*/
		if (data->ikData.charData)
		{
			buf->align(0);
			buf->write(data->ikData.charData, data->ikData.charDataLen);
			zone_buffer::clear_pointer(&dest->ikData.charData);
		}

		if (data->ikData.floatData)
		{
			buf->align(3);
			buf->write(data->ikData.floatData, data->ikData.floatDataLen);
			zone_buffer::clear_pointer(&dest->ikData.floatData);
		}

		if (data->ikData.int32Data)
		{
			buf->align(3);
			buf->write(data->ikData.int32Data, data->ikData.int32DataLen);
			zone_buffer::clear_pointer(&dest->ikData.int32Data);
		}

		if (data->ikData.strings)
		{
			buf->align(3);
			buf->write(data->ikData.strings, data->ikData.stringsCount);
			zone_buffer::clear_pointer(&dest->ikData.strings);
		}

		buf->pop_stream();
	}

	void skeleton_script::dump(SkeletonScript* asset)
	{
	}
}