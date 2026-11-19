#pragma once

namespace zonetool
{
	template <ASSET_TEMPLATE>
	class scriptfile : public asset_interface
	{
	private:
		std::string name_;
		S* asset_ = nullptr;

	public:
		S* parse(const std::string& name, zone_memory* mem)
		{
			auto file = filesystem::file(utils::string::va("%s.gscbin", name.data()));
			file.open("rb");
			if (!file.get_fp())
			{
				ZONETOOL_FATAL("Could not find scriptfile \"%s\"", name.data());
			}

			ZONETOOL_INFO("Parsing scriptfile \"%s\"...", name.data());

			auto* asset = mem->allocate<S>();

			std::string m_name;
			file.read_string(&m_name);
			asset->name = mem->duplicate_string(m_name);

			file.read(&asset->compressedLen);
			file.read(&asset->len);
			file.read(&asset->bytecodeLen);

			asset->buffer = mem->allocate<char>(asset->compressedLen);
			asset->bytecode = mem->allocate<char>(asset->bytecodeLen);
			file.read(asset->buffer, asset->compressedLen);
			file.read(asset->bytecode, asset->bytecodeLen);

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

			this->asset_ = parse(name, mem);
			if (!this->asset_)
			{
				this->asset_ = db_find_x_asset_header_safe<H, E>(this->type(), this->name().data()).scriptfile;
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

		static void read(zone_reader& reader, S* asset)
		{
			reader.push_stream(Streams::XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			reader.push_stream(Streams::XFILE_BLOCK_SCRIPT);
			reader.read_array(asset->buffer, 0, asset->compressedLen);
			reader.read_array(asset->bytecode, 0, asset->bytecodeLen);
			reader.pop_stream();
			reader.pop_stream();
		}

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto* data = this->asset_;
			auto* dest = buf->write<S>(data);

			buf->push_stream(Streams::XFILE_BLOCK_VIRTUAL);

			dest->name = buf->write_str(this->name());

			buf->push_stream(Streams::XFILE_BLOCK_SCRIPT);

			if (data->buffer)
			{
				buf->write(data->buffer, data->compressedLen);
				buf->clear_pointer(&dest->buffer);
			}

			if (data->bytecode)
			{
				buf->write(data->bytecode, data->bytecodeLen);
				buf->clear_pointer(&dest->bytecode);
			}

			buf->pop_stream();

			buf->pop_stream();
		}

		template <typename S>
		static void dump(S* asset)
		{
			auto file = filesystem::file(utils::string::va("%s.gscbin", asset->name));
			file.open("wb");

			if (!file.get_fp())
			{
				return;
			}

			file.write_string(asset->name);
			file.write(&asset->compressedLen);
			file.write(&asset->len);
			file.write(&asset->bytecodeLen);
			file.write(asset->buffer, asset->compressedLen);
			file.write(asset->bytecode, asset->bytecodeLen);

			file.close();
		}
	};

	struct map_setup_builtins
	{
		std::string ambient_play;
		bool keep_dvars;
	};

	inline std::string make_map_setup_script(const std::string& map, const std::string& source, const map_setup_builtins& builtins)
	{
		const std::regex setup_line(std::format(
			R"re(\s*(maps\\(mp|createart)\\({}\w*|_load)::main\(\s*\)|maps\\mp\\_compass::setupminimap\(.*\)|game\["(attackers|defenders)"\] = .*);\s*)re", map));
		static const std::regex dvar_line(R"re(\s*setdvar\(.*\);\s*)re");
		static const std::regex ambient_play_line(R"re(\s*(ambientplay|_func_11A)\((.*)\);\s*)re");

		std::string setup;
		std::string ambient;
		auto in_main = false;
		for (auto line : utils::string::split(source, '\n'))
		{
			if (line.ends_with('\r'))
			{
				line.pop_back();
			}

			std::smatch match;
			if (line == "main()")
			{
				in_main = true;
			}
			else if (in_main && line == "}")
			{
				in_main = false;
			}
			else if (in_main && (std::regex_match(line, setup_line) || (builtins.keep_dvars && std::regex_match(line, dvar_line))))
			{
				setup += line + "\n";
			}
			else if (std::regex_match(line, match, ambient_play_line))
			{
				ambient += std::format("    {}({});\n", builtins.ambient_play, match[2].str());
			}
		}

		return "main()\n{\n" + setup + ambient + "}\n";
	}
}
