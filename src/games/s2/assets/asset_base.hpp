#pragma once

namespace zonetool::s2
{
	inline std::string dump_path(const XAssetType type, const char* name, const std::string_view extension = ".bin")
	{
		auto file = std::string(name);
		std::ranges::replace(file, '*', '_');
		return std::string(type_to_string(type)) + "\\" + file + std::string(extension);
	}

	template <typename S>
	decltype(auto) asset_name(S* asset)
	{
		return (asset->name);
	}

	inline const char*& asset_name(WeaponCompleteDef* asset)
	{
		return asset->szInternalName;
	}

	inline const char*& asset_name(WeaponAttachment* asset)
	{
		return asset->szInternalName;
	}

	inline const char* asset_name(const WeaponCompleteDef* asset)
	{
		return asset->szInternalName;
	}

	inline const char* asset_name(const WeaponAttachment* asset)
	{
		return asset->szInternalName;
	}

	template <typename T>
	T* asset_reference(zone_base* zone, const XAssetType type, const T* asset)
	{
		return asset && asset_name(asset) ? static_cast<T*>(zone->get_asset_pointer(type, asset_name(asset))) : nullptr;
	}

	template <typename T>
	void add_dependency(zone_base* zone, const XAssetType type, const T* asset)
	{
		if (asset && asset_name(asset))
		{
			zone->add_asset_of_type(type, asset_name(asset));
		}
	}

	template <typename T>
	void add_dependencies(zone_base* zone, const XAssetType type, T* const* assets, const std::size_t count)
	{
		for (std::size_t i = 0; assets && i < count; i++)
		{
			add_dependency(zone, type, assets[i]);
		}
	}

	template <typename T>
	void write_name_reference(zone_buffer* buf, T*& dest_field, const char* name)
	{
		buf->clear_pointer(&dest_field);
		buf->align(7);
		buf->write(&buf->data_following);
		buf->write_str_raw(name);
	}

	inline void prepare_script_strings(zone_buffer* buf, scr_string_t* ids, const std::size_t count)
	{
		for (std::size_t i = 0; i < count; i++)
		{
			ids[i] = static_cast<scr_string_t>(buf->write_scriptstring(SL_ConvertToString(ids[i])));
		}
	}

	inline void dump_script_strings(assetmanager::dumper& write, const scr_string_t* ids, const std::size_t count)
	{
		for (std::size_t i = 0; i < count; i++)
		{
			write.dump_string(SL_ConvertToString(ids[i]));
		}
	}

	inline scr_string_t* read_script_strings(assetmanager::reader& read, zone_memory* mem, const std::size_t count)
	{
		const auto ids = mem->allocate<scr_string_t>(count);
		for (std::size_t i = 0; i < count; i++)
		{
			ids[i] = static_cast<scr_string_t>(SL_GetString(read.read_string()));
		}

		return ids;
	}

	inline void dump_script_string(assetmanager::dumper& write, const scr_string_t id)
	{
		write.dump_string(SL_ConvertToString(id));
	}

	inline scr_string_t parse_script_string(assetmanager::reader& read)
	{
		return static_cast<scr_string_t>(SL_GetString(read.read_string()));
	}

	inline void dump_script_string_array(assetmanager::dumper& write, const scr_string_t* ids, const std::size_t count)
	{
		write.dump_int(ids != nullptr);
		if (ids)
		{
			dump_script_strings(write, ids, count);
		}
	}

	inline scr_string_t* parse_script_string_array(assetmanager::reader& read, zone_memory* mem, const std::size_t count)
	{
		return read.read_int() ? read_script_strings(read, mem, count) : nullptr;
	}

	inline void dump_script_string(ordered_json& data, const scr_string_t id)
	{
		const auto* string = SL_ConvertToString(id);
		data = string ? ordered_json(string) : ordered_json(nullptr);
	}

	inline scr_string_t parse_script_string(const json& data)
	{
		return data.is_string() ? static_cast<scr_string_t>(SL_GetString(data.get_ref<const std::string&>().data())) : 0;
	}

	inline void dump_script_string_array(ordered_json& data, const scr_string_t* ids, const std::size_t count)
	{
		if (!ids)
		{
			data = nullptr;
			return;
		}

		for (std::size_t i = 0; i < count; i++)
		{
			dump_script_string(data[i], ids[i]);
		}
	}

	inline scr_string_t* parse_script_string_array(const json& data, zone_memory* mem, const std::size_t count)
	{
		if (!data.is_array())
		{
			return nullptr;
		}

		const auto ids = mem->allocate<scr_string_t>(count);
		for (std::size_t i = 0; i < count && i < data.size(); i++)
		{
			ids[i] = parse_script_string(data[i]);
		}

		return ids;
	}

	class script_string_preparer
	{
	public:
		explicit script_string_preparer(zone_buffer* buf)
			: buf_(buf)
		{
		}

		void operator()(scr_string_t& id)
		{
			if (this->prepared_.insert(&id).second)
			{
				id = static_cast<scr_string_t>(this->buf_->write_scriptstring(SL_ConvertToString(id)));
			}
		}

		void operator()(scr_string_t* ids, const std::size_t count)
		{
			for (std::size_t i = 0; ids && i < count; i++)
			{
				(*this)(ids[i]);
			}
		}

	private:
		zone_buffer* buf_;
		std::unordered_set<scr_string_t*> prepared_;
	};

	template <typename T>
	void write_inline(zone_buffer* buf, const std::size_t alignment, const T* data, const std::size_t count, T*& dest_field)
	{
		if (!data)
		{
			return;
		}

		buf->align(alignment);
		buf->write(const_cast<T*>(data), count);
		buf->clear_pointer(&dest_field);
	}

	template <typename T>
	void write_inline_bytes(zone_buffer* buf, const std::size_t alignment, const T* data, const std::size_t size, T*& dest_field)
	{
		if (!data)
		{
			return;
		}

		buf->align(alignment);
		buf->write_stream(data, size);
		buf->clear_pointer(&dest_field);
	}

	inline void write_string(zone_buffer* buf, const char* str, const char*& dest_field)
	{
		if (str)
		{
			dest_field = buf->write_str(str);
		}
	}

	template <typename T>
	T* write_array(zone_buffer* buf, const std::size_t alignment, T* data, const std::size_t count, T*& dest_field)
	{
		if (!data)
		{
			return nullptr;
		}

		if (auto* found = buf->find_sub_buffer(data, sizeof(T) * count))
		{
			dest_field = found;
			return nullptr;
		}

		buf->align(alignment);
		buf->insert_sub_buffer(data, count);
		auto* dest = buf->write(data, count);
		buf->clear_pointer(&dest_field);
		return dest;
	}

	template <typename T>
	void read_runtime_array(zone_reader& reader, const XFileBlock block, T*& field, const std::size_t alignment, const std::size_t count)
	{
		reader.push_stream(static_cast<std::uint8_t>(block));
		reader.read_array(field, alignment, count);
		reader.pop_stream();
	}

	template <typename T>
	void write_runtime_array(zone_buffer* buf, const XFileBlock block, const std::size_t alignment, T* data, const std::size_t count, T*& dest_field)
	{
		if (!data)
		{
			return;
		}

		buf->push_stream(static_cast<std::uint8_t>(block));
		buf->align(alignment);
		buf->write(data, count);
		buf->clear_pointer(&dest_field);
		buf->pop_stream();
	}

	template <typename T>
	void write_asset_array(zone_base* zone, zone_buffer* buf, const XAssetType type, T** assets, const std::size_t count, T**& dest_field)
	{
		if (auto* dest = write_array(buf, 7, assets, count, dest_field))
		{
			for (std::size_t i = 0; i < count; i++)
			{
				dest[i] = asset_reference(zone, type, assets[i]);
			}
		}
	}

	inline void write_sound(zone_buffer* buf, const snd_alias_list_t* sound, snd_alias_list_t*& dest_field)
	{
		if (sound)
		{
			write_name_reference(buf, dest_field, sound->name);
		}
	}

	inline void write_sound_array(zone_buffer* buf, snd_alias_list_t** sounds, const std::size_t count, snd_alias_list_t**& dest_field)
	{
		if (auto* dest = write_array(buf, 7, sounds, count, dest_field))
		{
			for (std::size_t i = 0; i < count; i++)
			{
				write_sound(buf, sounds[i], dest[i]);
			}
		}
	}

	template <typename T>
	void dump_asset_array(assetmanager::dumper& write, T** assets, const std::size_t count)
	{
		write.dump_array(assets, static_cast<std::uint32_t>(count));
		for (std::size_t i = 0; assets && i < count; i++)
		{
			write.dump_asset(assets[i]);
		}
	}

	template <typename T>
	T* keep_empty(zone_memory* mem, T*& field, T* parsed, const std::size_t count)
	{
		field = !parsed && !count && field ? mem->allocate<T>(1) : parsed; // a non-null empty array still aligns the zone stream
		return field;
	}

	template <typename T>
	T* parse_array(assetmanager::reader& read, zone_memory* mem, T*& field, const std::size_t count)
	{
		return keep_empty(mem, field, read.read_array<T>(), count);
	}

	template <typename T>
	void parse_runtime_array(zone_memory* mem, T*& field, const std::size_t count)
	{
		field = field ? mem->allocate<T>(std::max<std::size_t>(count, 1)) : nullptr;
	}

	template <typename T>
	void parse_asset_array(assetmanager::reader& read, zone_memory* mem, T**& field, const std::size_t count)
	{
		if (auto* assets = parse_array(read, mem, field, count))
		{
			for (std::size_t i = 0; i < count; i++)
			{
				assets[i] = read.read_asset<T>();
			}
		}
	}

	inline void parse_script_string_array(assetmanager::reader& read, zone_memory* mem, scr_string_t*& field, const std::size_t count)
	{
		keep_empty(mem, field, parse_script_string_array(read, mem, count), count);
	}

	template <typename S, XAssetType Type>
	class asset_base : public asset_interface
	{
	public:
		void init(const std::string& name, zone_memory* mem) override
		{
			this->name_ = name;

			if (this->referenced())
			{
				this->asset_ = mem->allocate<S>();
				asset_name(this->asset_) = mem->duplicate_string(name);
				return;
			}

			this->asset_ = this->parse(name, mem);
			if (!this->asset_)
			{
				this->asset_ = static_cast<S*>(db_find_asset(Type, name));
			}
		}

		void init(void* asset, zone_memory* mem) override
		{
			this->asset_ = static_cast<S*>(asset);
			this->name_ = asset_name(this->asset_);
		}

		void* pointer() override
		{
			return this->asset_;
		}

		bool referenced() override
		{
			return this->name_.starts_with(',');
		}

		std::string name() override
		{
			return this->name_;
		}

		std::int32_t type() override
		{
			return Type;
		}

		virtual S* parse(const std::string& name, zone_memory* mem) = 0;

	protected:
		char* write_name(zone_buffer* buf)
		{
			const auto* name = asset_name(this->asset_);
			return name && this->name_ == name ? buf->write_str(name) : buf->write_str(this->name_);
		}

		std::string name_;
		S* asset_ = nullptr;
	};
}
