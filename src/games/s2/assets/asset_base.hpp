#pragma once

namespace zonetool::s2
{
	inline std::string dump_path(const XAssetType type, const char* name, const std::string_view extension = ".bin")
	{
		// generated materials start with '*', which windows does not allow in a file name
		auto file = std::string(name);
		std::ranges::replace(file, '*', '_');
		return std::string(type_to_string(type)) + "\\" + file + std::string(extension);
	}

	template <typename T>
	T* asset_reference(zone_base* zone, const XAssetType type, const T* asset)
	{
		return asset ? static_cast<T*>(zone->get_asset_pointer(type, asset->name)) : nullptr;
	}

	template <typename T>
	void add_dependency(zone_base* zone, const XAssetType type, const T* asset)
	{
		if (asset && asset->name)
		{
			zone->add_asset_of_type(type, asset->name);
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

	// converts each field once, because parsed arrays can share memory through dump offsets
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
		// writes the parsed name pointer, so that fields that share it in the dump keep sharing it
		char* write_name(zone_buffer* buf)
		{
			const auto* name = asset_name(this->asset_);
			return name && this->name_ == name ? buf->write_str(name) : buf->write_str(this->name_);
		}

		std::string name_;
		S* asset_ = nullptr;
	};

	struct named_asset
	{
		const char* name;
	};

#define S2_ARRAY_FIELD(type, field) offsetof(type, field), sizeof(*type{}.field)

	template <typename Derived>
	class field_visitor
	{
	public:
		explicit field_visitor(void* base)
			: base_(static_cast<std::uint8_t*>(base))
		{
		}

		template <typename T>
		T& at(const std::size_t offset) const
		{
			return *reinterpret_cast<T*>(this->base_ + offset);
		}

		template <typename T>
		T& as() const
		{
			return *reinterpret_cast<T*>(this->base_);
		}

		void strings(const std::initializer_list<std::size_t> offsets) const
		{
			for (const auto offset : offsets)
			{
				this->self().string(offset);
			}
		}

		void assets(const XAssetType type, const std::size_t first, const std::size_t count) const
		{
			for (auto i = 0u; i < count; i++)
			{
				this->self().asset(type, first + i * sizeof(void*));
			}
		}

		void sounds(const std::size_t first, const std::size_t count) const
		{
			for (auto i = 0u; i < count; i++)
			{
				this->self().sound(first + i * sizeof(void*));
			}
		}

		template <typename F>
		void entries(const std::size_t offset, const std::size_t size, const std::size_t count, F&& callback) const
		{
			this->self().aligned_entries(offset, 7, size, count, callback);
		}

		void array(const std::size_t offset, const std::size_t element_size, const std::size_t alignment, const std::size_t count) const
		{
			this->self().bytes(offset, alignment, element_size * count);
		}

		void runtime_array(const std::size_t offset, const std::size_t element_size, const std::size_t alignment, const std::size_t count) const
		{
			this->self().runtime_bytes(offset, alignment, element_size * count);
		}

		void unread_bytes(const std::size_t offset, const std::size_t alignment, const std::size_t size) const
		{
			this->self().runtime_bytes(offset, alignment, size);
		}

		template <typename F>
		void in_stream(const XFileBlock, F&& callback) const
		{
			callback();
		}

		template <typename F>
		void shared_entry(const std::size_t offset, const std::size_t alignment, const std::size_t size, F&& callback) const
		{
			this->self().aligned_entries(offset, alignment, size, 1, callback);
		}

		template <typename T>
		const T* header(const XAssetType, const std::size_t) const
		{
			return nullptr;
		}

	protected:
		const Derived& self() const
		{
			return static_cast<const Derived&>(*this);
		}

		std::uint8_t* base_;
	};

	class field_reader : public field_visitor<field_reader>
	{
	public:
		field_reader(zone_reader& reader, void* base)
			: field_visitor(base)
			, reader_(reader)
		{
		}

		void string(const std::size_t offset) const
		{
			this->reader_.read_string(this->at<const char*>(offset));
		}

		void asset(const XAssetType type, const std::size_t offset) const
		{
			this->reader_.read_asset(type, this->at<void*>(offset));
		}

		void asset_array(const XAssetType type, const std::size_t offset, const std::size_t count) const
		{
			this->reader_.read_asset_array(type, this->at<void**>(offset), count);
		}

		void sound(const std::size_t offset) const
		{
			this->reader_.read_name_reference(this->at<snd_alias_list_t*>(offset));
		}

		void sound_array(const std::size_t offset, const std::size_t count) const
		{
			this->reader_.read_name_reference_array(this->at<snd_alias_list_t**>(offset), count);
		}

		void script_strings(const std::size_t offset, const std::size_t count) const
		{
			this->reader_.read_script_string_array(this->at<scr_string_t*>(offset), count);
		}

		void script_string(const std::size_t offset) const
		{
			this->reader_.read_script_string(this->at<scr_string_t>(offset));
		}

		std::uint8_t* bytes(const std::size_t offset, const std::size_t alignment, const std::size_t size) const
		{
			return this->reader_.read_array(this->at<std::uint8_t*>(offset), alignment, size);
		}

		void runtime_bytes(const std::size_t offset, const std::size_t alignment, const std::size_t size) const
		{
			this->in_stream(XFILE_BLOCK_RUNTIME, [&] { this->bytes(offset, alignment, size); });
		}

		void unread_bytes(const std::size_t offset, const std::size_t alignment, const std::size_t size) const
		{
			this->in_stream(XFILE_BLOCK_RUNTIME_UNREAD, [&] { this->bytes(offset, alignment, size); });
		}

		template <typename F>
		void aligned_entries(const std::size_t offset, const std::size_t alignment, const std::size_t size, const std::size_t count, F&& callback) const
		{
			if (const auto data = this->bytes(offset, alignment, size * count))
			{
				for (auto i = 0u; i < count; i++)
				{
					callback(field_reader(this->reader_, data + i * size), i);
				}
			}
		}

		template <typename F>
		void in_stream(const XFileBlock block, F&& callback) const
		{
			this->reader_.push_stream(static_cast<std::uint8_t>(block));
			callback();
			this->reader_.pop_stream();
		}

		template <typename F>
		void shared_entry(const std::size_t offset, const std::size_t alignment, const std::size_t size, F&& callback) const
		{
			if (const auto data = this->reader_.read_aliased_array(this->at<std::uint8_t*>(offset), alignment, size))
			{
				callback(field_reader(this->reader_, data), 0);
			}
		}

		template <typename T>
		const T* header(const XAssetType, const std::size_t offset) const
		{
			return this->at<const T*>(offset);
		}

		field_reader sub(const std::size_t offset) const
		{
			return field_reader(this->reader_, this->base_ + offset);
		}

		zone_reader& reader() const
		{
			return this->reader_;
		}

	private:
		zone_reader& reader_;
	};

	class field_writer : public field_visitor<field_writer>
	{
	public:
		field_writer(zone_base* zone, zone_buffer* buf, void* data, void* dest)
			: field_visitor(data)
			, zone_(zone)
			, buf_(buf)
			, dest_(static_cast<std::uint8_t*>(dest))
		{
		}

		template <typename T>
		T& dest_at(const std::size_t offset) const
		{
			return *reinterpret_cast<T*>(this->dest_ + offset);
		}

		void string(const std::size_t offset) const
		{
			write_string(this->buf_, this->at<const char*>(offset), this->dest_at<const char*>(offset));
		}

		void asset(const XAssetType type, const std::size_t offset) const
		{
			this->dest_at<named_asset*>(offset) = this->reference(type, this->at<named_asset*>(offset));
		}

		void asset_array(const XAssetType type, const std::size_t offset, const std::size_t count) const
		{
			auto* assets = this->at<named_asset**>(offset);
			auto* dest = reinterpret_cast<named_asset**>(this->bytes(offset, 7, count * sizeof(void*)));
			for (auto i = 0u; dest && i < count; i++)
			{
				dest[i] = this->reference(type, assets[i]);
			}
		}

		void sound(const std::size_t offset) const
		{
			if (const auto* sound = this->at<named_asset*>(offset))
			{
				write_name_reference(this->buf_, this->dest_at<named_asset*>(offset), sound->name);
			}
		}

		void sound_array(const std::size_t offset, const std::size_t count) const
		{
			auto* sounds = this->at<named_asset**>(offset);
			auto* dest = reinterpret_cast<named_asset**>(this->bytes(offset, 7, count * sizeof(void*)));
			for (auto i = 0u; dest && i < count; i++)
			{
				if (sounds[i])
				{
					write_name_reference(this->buf_, dest[i], sounds[i]->name);
				}
			}
		}

		void script_strings(const std::size_t offset, const std::size_t count) const
		{
			this->bytes(offset, 3, count * sizeof(scr_string_t));
		}

		void script_string(const std::size_t) const
		{
		}

		std::uint8_t* bytes(const std::size_t offset, const std::size_t alignment, const std::size_t size) const
		{
			auto* data = this->at<std::uint8_t*>(offset);
			if (!data)
			{
				return nullptr;
			}

			if (auto* found = this->buf_->find_sub_buffer(data, size))
			{
				this->dest_at<std::uint8_t*>(offset) = found;
				return nullptr;
			}

			this->buf_->align(alignment);
			this->buf_->insert_sub_buffer(data, 1, size);
			return this->write_inline_bytes(offset, data, size);
		}

		void runtime_bytes(const std::size_t offset, const std::size_t alignment, const std::size_t size) const
		{
			this->unloaded_bytes(XFILE_BLOCK_RUNTIME, offset, alignment, size);
		}

		void unread_bytes(const std::size_t offset, const std::size_t alignment, const std::size_t size) const
		{
			this->unloaded_bytes(XFILE_BLOCK_RUNTIME_UNREAD, offset, alignment, size);
		}

		template <typename F>
		void aligned_entries(const std::size_t offset, const std::size_t alignment, const std::size_t size, const std::size_t count, F&& callback) const
		{
			auto* data = this->at<std::uint8_t*>(offset);
			if (const auto dest = this->bytes(offset, alignment, size * count))
			{
				for (auto i = 0u; i < count; i++)
				{
					callback(field_writer(this->zone_, this->buf_, data + i * size, dest + i * size), i);
				}
			}
		}

		template <typename F>
		void in_stream(const XFileBlock block, F&& callback) const
		{
			this->buf_->push_stream(static_cast<std::uint8_t>(block));
			callback();
			this->buf_->pop_stream();
		}

		template <typename F>
		void shared_entry(const std::size_t offset, const std::size_t alignment, const std::size_t size, F&& callback) const
		{
			auto* data = this->at<std::uint8_t*>(offset);
			if (!data)
			{
				return;
			}

			auto& dest_field = this->dest_at<void*>(offset);
			auto& slots = shared_slots();
			if (const auto found = slots.find(data); found != slots.end())
			{
				dest_field = found->second;
				return;
			}

			this->buf_->align(alignment);
			slots.emplace(data, this->reserve_insert_slot());
			auto* dest = this->buf_->write(data, size);
			this->buf_->insert_pointer(&dest_field);
			callback(field_writer(this->zone_, this->buf_, data, dest), 0);
		}

		template <typename T>
		const T* header(const XAssetType type, const std::size_t offset) const
		{
			const auto* asset = this->at<named_asset*>(offset);
			if (!asset || !asset->name)
			{
				return nullptr;
			}

			if (auto* entry = this->zone_->find_asset(type, asset->name); entry && !entry->referenced())
			{
				return static_cast<const T*>(entry->pointer());
			}

			return static_cast<const T*>(db_find_asset(type, asset->name));
		}

		field_writer sub(const std::size_t offset) const
		{
			return field_writer(this->zone_, this->buf_, this->base_ + offset, this->dest_ + offset);
		}

		static std::unordered_map<const void*, void*>& shared_slots()
		{
			thread_local std::unordered_map<const void*, void*> slots;
			return slots;
		}

	private:
		std::uint8_t* write_inline_bytes(const std::size_t offset, std::uint8_t* data, const std::size_t size) const
		{
			auto* dest = this->buf_->write(data, size);
			this->buf_->clear_pointer(&this->dest_at<void*>(offset));
			return dest;
		}

		void unloaded_bytes(const XFileBlock block, const std::size_t offset, const std::size_t alignment, const std::size_t size) const
		{
			if (auto* data = this->at<std::uint8_t*>(offset))
			{
				this->buf_->push_stream(static_cast<std::uint8_t>(block));
				this->buf_->align(alignment);
				this->write_inline_bytes(offset, data, size);
				this->buf_->pop_stream();
			}
		}

		void* reserve_insert_slot() const
		{
			this->buf_->push_stream(XFILE_BLOCK_VIRTUAL);
			this->buf_->align(7);
			auto* slot = this->buf_->get_zone_pointer<void>(XFILE_BLOCK_VIRTUAL, this->buf_->stream_offset(XFILE_BLOCK_VIRTUAL));
			this->buf_->inc_stream(XFILE_BLOCK_VIRTUAL, sizeof(void*));
			this->buf_->pop_stream();
			return slot;
		}

		named_asset* reference(const XAssetType type, const named_asset* asset) const
		{
			return asset && asset->name ? static_cast<named_asset*>(this->zone_->get_asset_pointer(type, asset->name)) : nullptr;
		}

		zone_base* zone_;
		zone_buffer* buf_;
		std::uint8_t* dest_;
	};

	class field_dumper : public field_visitor<field_dumper>
	{
	public:
		field_dumper(assetmanager::dumper& write, void* data)
			: field_visitor(data)
			, write_(write)
		{
		}

		void string(const std::size_t offset) const
		{
			this->write_.dump_string(this->at<const char*>(offset));
		}

		void asset(const XAssetType, const std::size_t offset) const
		{
			this->write_.dump_asset(this->at<named_asset*>(offset));
		}

		void asset_array(const XAssetType, const std::size_t offset, const std::size_t count) const
		{
			auto* assets = this->at<named_asset**>(offset);
			this->write_.dump_array(assets, static_cast<std::uint32_t>(count));
			for (auto i = 0u; assets && i < count; i++)
			{
				this->write_.dump_asset(assets[i]);
			}
		}

		void sound(const std::size_t offset) const
		{
			this->asset(ASSET_TYPE_SOUND, offset);
		}

		void sound_array(const std::size_t offset, const std::size_t count) const
		{
			this->asset_array(ASSET_TYPE_SOUND, offset, count);
		}

		void script_strings(const std::size_t offset, const std::size_t count) const
		{
			dump_script_string_array(this->write_, this->at<scr_string_t*>(offset), count);
		}

		void script_string(const std::size_t offset) const
		{
			dump_script_string(this->write_, this->at<scr_string_t>(offset));
		}

		std::uint8_t* bytes(const std::size_t offset, const std::size_t, const std::size_t size) const
		{
			auto* data = this->at<std::uint8_t*>(offset);
			this->write_.dump_array(data, static_cast<std::uint32_t>(size));
			return data;
		}

		void runtime_bytes(const std::size_t, const std::size_t, const std::size_t) const
		{
		}

		template <typename F>
		void aligned_entries(const std::size_t offset, const std::size_t alignment, const std::size_t size, const std::size_t count, F&& callback) const
		{
			if (const auto data = this->bytes(offset, alignment, size * count))
			{
				for (auto i = 0u; i < count; i++)
				{
					callback(field_dumper(this->write_, data + i * size), i);
				}
			}
		}

		template <typename T>
		const T* header(const XAssetType, const std::size_t offset) const
		{
			return this->at<const T*>(offset);
		}

		field_dumper sub(const std::size_t offset) const
		{
			return field_dumper(this->write_, this->base_ + offset);
		}

	private:
		assetmanager::dumper& write_;
	};

	class field_parser : public field_visitor<field_parser>
	{
	public:
		field_parser(assetmanager::reader& read, zone_memory* mem, void* data)
			: field_visitor(data)
			, read_(read)
			, mem_(mem)
		{
		}

		void string(const std::size_t offset) const
		{
			this->at<const char*>(offset) = this->read_.read_string();
		}

		void asset(const XAssetType, const std::size_t offset) const
		{
			this->at<named_asset*>(offset) = this->read_.read_asset<named_asset>();
		}

		void asset_array(const XAssetType, const std::size_t offset, const std::size_t count) const
		{
			auto* assets = this->keep_empty(offset, this->read_.read_array<named_asset*>(), count);
			for (auto i = 0u; assets && i < count; i++)
			{
				assets[i] = this->read_.read_asset<named_asset>();
			}
		}

		void sound(const std::size_t offset) const
		{
			this->asset(ASSET_TYPE_SOUND, offset);
		}

		void sound_array(const std::size_t offset, const std::size_t count) const
		{
			this->asset_array(ASSET_TYPE_SOUND, offset, count);
		}

		void script_strings(const std::size_t offset, const std::size_t count) const
		{
			this->keep_empty(offset, parse_script_string_array(this->read_, this->mem_, count), count);
		}

		void script_string(const std::size_t offset) const
		{
			this->at<scr_string_t>(offset) = parse_script_string(this->read_);
		}

		std::uint8_t* bytes(const std::size_t offset, const std::size_t, const std::size_t size) const
		{
			return this->keep_empty(offset, this->read_.read_array<std::uint8_t>(), size);
		}

		void runtime_bytes(const std::size_t offset, const std::size_t, const std::size_t size) const
		{
			auto& data = this->at<std::uint8_t*>(offset);
			data = data ? this->mem_->allocate<std::uint8_t>(std::max<std::size_t>(size, 1)) : nullptr;
		}

		template <typename F>
		void aligned_entries(const std::size_t offset, const std::size_t alignment, const std::size_t size, const std::size_t count, F&& callback) const
		{
			if (const auto data = this->bytes(offset, alignment, size * count))
			{
				for (auto i = 0u; i < count; i++)
				{
					callback(field_parser(this->read_, this->mem_, data + i * size), i);
				}
			}
		}

		field_parser sub(const std::size_t offset) const
		{
			return field_parser(this->read_, this->mem_, this->base_ + offset);
		}

	private:
		// stock zones keep some zero-count pointers non-null; only the raw struct records that
		template <typename T>
		T* keep_empty(const std::size_t offset, T* parsed, const std::size_t count) const
		{
			auto& field = this->at<T*>(offset);
			field = !parsed && !count && field ? this->mem_->allocate<T>(1) : parsed;
			return field;
		}

		assetmanager::reader& read_;
		zone_memory* mem_;
	};

	class field_dependencies : public field_visitor<field_dependencies>
	{
	public:
		field_dependencies(zone_base* zone, void* data)
			: field_visitor(data)
			, zone_(zone)
		{
		}

		void string(const std::size_t) const
		{
		}

		void asset(const XAssetType type, const std::size_t offset) const
		{
			add_dependency(this->zone_, type, this->at<named_asset*>(offset));
		}

		void asset_array(const XAssetType type, const std::size_t offset, const std::size_t count) const
		{
			auto* assets = this->at<named_asset**>(offset);
			for (auto i = 0u; assets && i < count; i++)
			{
				add_dependency(this->zone_, type, assets[i]);
			}
		}

		void sound(const std::size_t offset) const
		{
			this->asset(ASSET_TYPE_SOUND, offset);
		}

		void sound_array(const std::size_t offset, const std::size_t count) const
		{
			this->asset_array(ASSET_TYPE_SOUND, offset, count);
		}

		void script_strings(const std::size_t, const std::size_t) const
		{
		}

		void script_string(const std::size_t) const
		{
		}

		std::uint8_t* bytes(const std::size_t offset, const std::size_t, const std::size_t) const
		{
			return this->at<std::uint8_t*>(offset);
		}

		void runtime_bytes(const std::size_t, const std::size_t, const std::size_t) const
		{
		}

		template <typename F>
		void aligned_entries(const std::size_t offset, const std::size_t alignment, const std::size_t size, const std::size_t count, F&& callback) const
		{
			if (const auto data = this->bytes(offset, alignment, size * count))
			{
				for (auto i = 0u; i < count; i++)
				{
					callback(field_dependencies(this->zone_, data + i * size), i);
				}
			}
		}

		field_dependencies sub(const std::size_t offset) const
		{
			return field_dependencies(this->zone_, this->base_ + offset);
		}

	private:
		zone_base* zone_;
	};

	class field_preparer : public field_visitor<field_preparer>
	{
	public:
		field_preparer(script_string_preparer& prepare, void* data)
			: field_visitor(data)
			, prepare_(prepare)
		{
		}

		void string(const std::size_t) const
		{
		}

		void asset(const XAssetType, const std::size_t) const
		{
		}

		void asset_array(const XAssetType, const std::size_t, const std::size_t) const
		{
		}

		void sound(const std::size_t) const
		{
		}

		void sound_array(const std::size_t, const std::size_t) const
		{
		}

		void script_strings(const std::size_t offset, const std::size_t count) const
		{
			this->prepare_(this->at<scr_string_t*>(offset), count);
		}

		void script_string(const std::size_t offset) const
		{
			this->prepare_(this->at<scr_string_t>(offset));
		}

		std::uint8_t* bytes(const std::size_t offset, const std::size_t, const std::size_t) const
		{
			return this->at<std::uint8_t*>(offset);
		}

		void runtime_bytes(const std::size_t, const std::size_t, const std::size_t) const
		{
		}

		template <typename F>
		void aligned_entries(const std::size_t offset, const std::size_t alignment, const std::size_t size, const std::size_t count, F&& callback) const
		{
			if (const auto data = this->bytes(offset, alignment, size * count))
			{
				for (auto i = 0u; i < count; i++)
				{
					callback(field_preparer(this->prepare_, data + i * size), i);
				}
			}
		}

		field_preparer sub(const std::size_t offset) const
		{
			return field_preparer(this->prepare_, this->base_ + offset);
		}

	private:
		script_string_preparer& prepare_;
	};

	// one layout function drives read, write, dump, parse, dependencies and script strings
	template <typename S, XAssetType Type, typename Layout>
	class field_layout_asset : public asset_base<S, Type>
	{
	public:
		static void read(zone_reader& reader, S* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			Layout::visit(field_reader(reader, asset));
			reader.pop_stream();
		}

		S* parse(const std::string& name, zone_memory* mem) override
		{
			assetmanager::reader read(mem);
			if (!read.open(dump_path(Type, name.data())))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing %s \"%s\"...", type_to_string(Type), name.data());

			auto* asset = read.read_single<S>();
			Layout::visit(field_parser(read, mem, asset));
			read.close();

			return asset;
		}

		void prepare(zone_buffer* buf, zone_memory* mem) override
		{
			script_string_preparer prepare(buf);
			Layout::visit(field_preparer(prepare, this->asset_));
		}

		void load_depending(zone_base* zone) override
		{
			Layout::visit(field_dependencies(zone, this->asset_));
		}

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			field_writer::shared_slots().clear();
			buf->push_stream(XFILE_BLOCK_VIRTUAL);
			Layout::visit(field_writer(zone, buf, data, dest));
			buf->pop_stream();
		}

		static void dump(S* asset)
		{
			assetmanager::dumper write;
			if (!write.open(dump_path(Type, asset_name(asset))))
			{
				return;
			}

			write.dump_single(asset);
			Layout::visit(field_dumper(write, asset));
			write.close();
		}
	};
}
