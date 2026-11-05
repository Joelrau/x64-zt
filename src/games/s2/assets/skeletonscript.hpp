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

			auto& ik = asset->ikData;
			reader.read_array(ik.solverIDs, 0, ik.numSolvers);
			reader.read_array(ik.floatParams, 3, ik.numFloatParams);
			reader.read_array(ik.intParams, 3, ik.numIntParams);
			reader.read_script_string_array(ik.stringParams, ik.numStringParams);

			reader.read_array(asset->code, 3, asset->codeSize);
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

			auto& ik = asset->ikData;
			ik.solverIDs = read.read_array<unsigned char>();
			ik.floatParams = read.read_array<float>();
			ik.intParams = read.read_array<int>();
			ik.stringParams = read_script_strings(read, mem, ik.numStringParams);
			asset->code = read.read_array<SkeletonScriptCode>();
			read.close();

			return asset;
		}

		void prepare(zone_buffer* buf, zone_memory* mem) override
		{
			const auto& ik = this->asset_->ikData;
			this->script_string_ids_.assign(ik.stringParams, ik.stringParams + ik.numStringParams);
			prepare_script_strings(buf, this->script_string_ids_.data(), this->script_string_ids_.size());
		}

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			buf->push_stream(XFILE_BLOCK_VIRTUAL);

			dest->name = this->write_name(buf);

			const auto& ik = data->ikData;
			auto& dest_ik = dest->ikData;
			if (ik.solverIDs)
			{
				buf->align(0);
				buf->write(ik.solverIDs, ik.numSolvers);
				buf->clear_pointer(&dest_ik.solverIDs);
			}
			if (ik.floatParams)
			{
				buf->align(3);
				buf->write(ik.floatParams, ik.numFloatParams);
				buf->clear_pointer(&dest_ik.floatParams);
			}
			if (ik.intParams)
			{
				buf->align(3);
				buf->write(ik.intParams, ik.numIntParams);
				buf->clear_pointer(&dest_ik.intParams);
			}
			if (ik.stringParams)
			{
				buf->align(3);
				buf->write(this->script_string_ids_.data(), ik.numStringParams);
				buf->clear_pointer(&dest_ik.stringParams);
			}
			if (data->code)
			{
				buf->align(3);
				buf->write(data->code, data->codeSize);
				buf->clear_pointer(&dest->code);
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

			const auto& ik = asset->ikData;
			write.dump_single(asset);
			write.dump_string(asset->name);
			write.dump_array(ik.solverIDs, ik.numSolvers);
			write.dump_array(ik.floatParams, ik.numFloatParams);
			write.dump_array(ik.intParams, ik.numIntParams);
			dump_script_strings(write, ik.stringParams, ik.numStringParams);
			write.dump_array(asset->code, asset->codeSize);
			write.close();
		}

	private:
		std::vector<scr_string_t> script_string_ids_;
	};
}
