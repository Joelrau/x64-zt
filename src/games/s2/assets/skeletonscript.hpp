#pragma once

namespace zonetool::s2
{
	class skeleton_script : public asset_base<SkeletonScript, ASSET_TYPE_SKELETON_SCRIPT>
	{
	public:
		static void read(zone_reader& reader, SkeletonScript* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);

			auto& code = asset->code;
			reader.read_array(code.bytes, 0, code.byteCount);
			reader.read_array(code.ints, 3, code.intCount);
			reader.read_array(code.floats, 3, code.floatCount);
			reader.read_script_string_array(code.scriptStrings, code.scriptStringCount);

			reader.read_array(asset->data, 3, asset->dataCount);
			reader.pop_stream();
		}

		SkeletonScript* parse(const std::string& name, zone_memory* mem) override
		{
			assetmanager::reader read(mem);
			if (!read.open(dump_path(ASSET_TYPE_SKELETON_SCRIPT, name.data())))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing skeletonscript \"%s\"...", name.data());

			auto* asset = read.read_single<SkeletonScript>();
			asset->name = read.read_string();

			auto& code = asset->code;
			code.bytes = read.read_array<unsigned char>();
			code.ints = read.read_array<int>();
			code.floats = read.read_array<float>();
			code.scriptStrings = read_script_strings(read, mem, code.scriptStringCount);
			asset->data = read.read_array<int>();
			read.close();

			return asset;
		}

		void prepare(zone_buffer* buf, zone_memory* mem) override
		{
			const auto& code = this->asset_->code;
			this->script_string_ids_.assign(code.scriptStrings, code.scriptStrings + code.scriptStringCount);
			prepare_script_strings(buf, this->script_string_ids_.data(), this->script_string_ids_.size());
		}

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			buf->push_stream(XFILE_BLOCK_VIRTUAL);

			dest->name = this->write_name(buf);

			const auto& code = data->code;
			auto& dest_code = dest->code;
			if (code.bytes)
			{
				buf->align(0);
				buf->write(code.bytes, code.byteCount);
				buf->clear_pointer(&dest_code.bytes);
			}
			if (code.ints)
			{
				buf->align(3);
				buf->write(code.ints, code.intCount);
				buf->clear_pointer(&dest_code.ints);
			}
			if (code.floats)
			{
				buf->align(3);
				buf->write(code.floats, code.floatCount);
				buf->clear_pointer(&dest_code.floats);
			}
			if (code.scriptStrings)
			{
				buf->align(3);
				buf->write(this->script_string_ids_.data(), code.scriptStringCount);
				buf->clear_pointer(&dest_code.scriptStrings);
			}
			if (data->data)
			{
				buf->align(3);
				buf->write(data->data, data->dataCount);
				buf->clear_pointer(&dest->data);
			}

			buf->pop_stream();
		}

		static void dump(SkeletonScript* asset)
		{
			assetmanager::dumper write;
			if (!write.open(dump_path(ASSET_TYPE_SKELETON_SCRIPT, asset->name)))
			{
				return;
			}

			const auto& code = asset->code;
			write.dump_single(asset);
			write.dump_string(asset->name);
			write.dump_array(code.bytes, code.byteCount);
			write.dump_array(code.ints, code.intCount);
			write.dump_array(code.floats, code.floatCount);
			dump_script_strings(write, code.scriptStrings, code.scriptStringCount);
			write.dump_array(asset->data, asset->dataCount);
			write.close();
		}

	private:
		std::vector<scr_string_t> script_string_ids_;
	};
}
