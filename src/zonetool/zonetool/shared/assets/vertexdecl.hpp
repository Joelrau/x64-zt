#pragma once

namespace zonetool
{
	template <ASSET_TEMPLATE>
	class vertex_decl : public asset_interface
	{
	private:
		std::string name_;
		S* asset_ = nullptr;

	public:
		static S* parse(const std::string& name, zone_memory* mem)
		{
			const auto path = "techsets\\" + name + ".vertexdecl";

			assetmanager::reader read(mem);
			if (!read.open(path))
			{
				return nullptr;
			}

			auto* asset = read.read_single<S>();
			asset->name = read.read_string();
			read.close();

			return asset;
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

			this->asset_ = this->parse(name, mem);
			if (!this->asset_)
			{
				this->asset_ = db_find_x_asset_header_safe<H, E>(this->type(), this->name().data()).vertexDecl;

				if (DB_IsXAssetDefault(this->type(), this->name().data()))
				{
					ZONETOOL_FATAL("vertexdecl \"%s\" not found.", name.data());
				}
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
			auto data = this->asset_;
			auto dest = buf->write(data);

			buf->push_stream(3);

			dest->name = buf->write_str(this->name());

			buf->pop_stream();
		}

		template <typename S>
		static void dump(S* asset)
		{
			const auto path = "techsets\\"s + asset->name + ".vertexdecl"s;

			assetmanager::dumper write;
			if (!write.open(path))
			{
				return;
			}

			write.dump_single(asset);
			write.dump_string(asset->name);
			write.close();
		}
	};
}