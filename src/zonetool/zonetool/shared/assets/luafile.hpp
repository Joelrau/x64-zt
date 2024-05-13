#pragma once

namespace zonetool
{
	template <ASSET_TEMPLATE>
	class lua_file : public asset_interface
	{
	private:
		std::string name_;
		S* asset_ = nullptr;

	public:
		S* parse(const std::string& name, zone_memory* mem)
		{
			auto file = filesystem::file(name);
			file.open("rb");

			if (file.get_fp())
			{
				ZONETOOL_INFO("Parsing luafile \"%s\"...", name.data());

				const auto size = file.size();
				auto data = file.read_bytes(size);

				auto* luafile = mem->allocate<S>();
				luafile->name = mem->duplicate_string(name);

				luafile->len = static_cast<int>(size);
				luafile->buffer = mem->allocate<char>(luafile->len);
				memcpy(
					const_cast<char*>(luafile->buffer),
					data.data(),
					data.size());

				file.close();

				return luafile;
			}

			return nullptr;
		}

		void init(const std::string& name, zone_memory* mem) override
		{
			this->name_ = name;

			if (this->referenced())
			{
				this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
				this->asset_->name = mem->duplicate_string(name);
				return;
			}

			this->asset_ = parse(name, mem);
			if (!this->asset_)
			{
				this->asset_ = db_find_x_asset_header_safe<H, E>(this->type(), this->name().data()).luaFile;
			}
		}

		void prepare(zone_buffer* buf, zone_memory* mem) override
		{
		}

		void load_depending(zone_base* zone) override
		{
		}

		void* pointer() override { return asset_; }

		bool referenced() override { return name_.starts_with(","); }

		std::string name() override
		{
			return this->name_;
		}

		std::int32_t type() override
		{
			return Type;
		}

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto* data = this->asset_;
			auto* dest = buf->write<S>(data);

			buf->push_stream(Streams::XFILE_BLOCK_VIRTUAL);

			dest->name = buf->write_str(this->name());

			if (data->buffer)
			{
				buf->align(15);
				buf->write(data->buffer, data->len);
				buf->clear_pointer(&dest->buffer);
			}

			buf->pop_stream();
		}

		template <typename S>
		static void dump(S* asset)
		{
			auto file = filesystem::file(asset->name);
			file.open("wb");

			if (asset->len > 0)
			{
				file.write(asset->buffer, asset->len, 1);
			}

			file.close();
		}
	};
}