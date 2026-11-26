#pragma once
#include "../zone/loader.hpp"

namespace zonetool::s4
{
	template <typename T>
	void write_asset(zone_base* zone, const XAssetType type, T*& field)
	{
		zone->preload_asset_pointer(type);
		if (field)
		{
			field = static_cast<T*>(zone->get_asset_pointer(type, get_asset_name(type, field)));
		}
	}

	template <typename T>
	void write_string(zone_buffer* buf, T*& field)
	{
		if (field)
		{
			field = buf->write_str(field);
		}
	}

	template <typename T>
	T* write_array(zone_buffer* buf, T*& field, const std::size_t count, const std::uint32_t alignment)
	{
		if (!field)
		{
			return nullptr;
		}

		buf->align(alignment);
		auto* const data = buf->write(field, count);
		buf->clear_pointer(&field);
		return data;
	}

	template <typename T>
	T* write_shared_array(zone_buffer* buf, T*& field, const std::size_t count, const std::uint32_t alignment)
	{
		if (!field)
		{
			return nullptr;
		}

		if (const auto written = buf->find_sub_buffer(field))
		{
			field = written;
			return nullptr;
		}

		buf->align(alignment);
		buf->insert_sub_buffer(field, count);
		auto* const data = buf->write(field, count);
		buf->clear_pointer(&field);
		return data;
	}

	template <typename T>
	void write_asset_array(zone_base* zone, zone_buffer* buf, const XAssetType type, T**& field, const std::size_t count)
	{
		if (const auto assets = write_array(buf, field, count, 7))
		{
			for (std::size_t i = 0; i < count; i++)
			{
				write_asset(zone, type, assets[i]);
			}
		}
	}

	template <typename T>
	void add_asset(zone_base* zone, const XAssetType type, const T* asset)
	{
		if (asset)
		{
			zone->add_asset_of_type(type, get_asset_name(type, asset));
		}
	}

	template <typename T>
	void add_asset_array(zone_base* zone, const XAssetType type, T* const* assets, const std::size_t count)
	{
		if (assets)
		{
			for (std::size_t i = 0; i < count; i++)
			{
				add_asset(zone, type, assets[i]);
			}
		}
	}

	template <typename T>
	void dump_asset_array(assetmanager::dumper& dump, T** assets, const std::size_t count)
	{
		dump.dump_array(assets, static_cast<std::uint32_t>(count));
		if (assets)
		{
			for (std::size_t i = 0; i < count; i++)
			{
				dump.dump_asset(assets[i]);
			}
		}
	}

	template <typename T>
	T* parse_single(assetmanager::reader& read, T*& field)
	{
		return field = read.read_single<T>();
	}

	template <typename T>
	void parse_asset(assetmanager::reader& read, T*& field)
	{
		field = read.read_asset<std::remove_const_t<T>>();
	}

	template <typename T>
	void parse_asset_array(assetmanager::reader& read, T**& field, const std::size_t count)
	{
		if ((field = read.read_array<T*>()))
		{
			for (std::size_t i = 0; i < count; i++)
			{
				field[i] = read.read_asset<T>();
			}
		}
	}

	class script_string_map
	{
	private:
		std::unordered_map<scr_string_t*, const char*> strings_;

	public:
		void parse(assetmanager::reader& read, scr_string_t& string)
		{
			this->strings_[&string] = read.read_string();
		}

		void parse(assetmanager::reader& read, scr_string_t*& strings, const std::size_t count)
		{
			if ((strings = read.read_array<scr_string_t>()))
			{
				for (std::size_t i = 0; i < count; i++)
				{
					this->parse(read, strings[i]);
				}
			}
		}

		void prepare(zone_buffer* buf, scr_string_t& string)
		{
			string = static_cast<scr_string_t>(buf->write_scriptstring(this->strings_[&string]));
		}

		void prepare(zone_buffer* buf, scr_string_t* strings, const std::size_t count)
		{
			if (strings)
			{
				for (std::size_t i = 0; i < count; i++)
				{
					this->prepare(buf, strings[i]);
				}
			}
		}
	};

	inline void dump_script_string(assetmanager::dumper& dump, const scr_string_t string)
	{
		dump.dump_string(SL_ConvertToString(string));
	}

	inline void dump_script_strings(assetmanager::dumper& dump, scr_string_t* strings, const std::size_t count)
	{
		dump.dump_array(strings, static_cast<std::uint32_t>(count));
		if (strings)
		{
			for (std::size_t i = 0; i < count; i++)
			{
				dump_script_string(dump, strings[i]);
			}
		}
	}

	struct read_walk
	{
		zone_reader& reader;

		template <typename T>
		T* array(T*& field, const std::size_t count, const std::uint32_t alignment)
		{
			return this->reader.read_array(field, alignment, count);
		}

		template <typename T>
		void string(T*& field)
		{
			this->reader.read_string(field);
		}

		template <typename T>
		void asset(const XAssetType type, T*& field)
		{
			this->reader.read_asset(type, field);
		}

		void script_strings(scr_string_t*& field, const std::size_t count)
		{
			this->reader.read_script_string_array(field, count);
		}

		void inline_script_strings(scr_string_t* strings, const std::size_t count)
		{
			this->reader.read_script_strings(strings, count);
		}

		void script_string(scr_string_t& field)
		{
			this->reader.read_script_string(field);
		}

		unsigned int temp_count(unsigned int)
		{
			this->reader.push_stream(XFILE_BLOCK_TEMP);
			this->reader.align(3);
			const auto count = *this->reader.read<unsigned int>();
			this->reader.pop_stream();
			return count;
		}

		template <typename T>
		void keep_temp(T*& field, const std::size_t count)
		{
			if (field)
			{
				field = this->reader.duplicate(field, count);
			}
		}

		void push_stream(const XFileBlock stream)
		{
			this->reader.push_stream(stream);
		}

		void pop_stream()
		{
			this->reader.pop_stream();
		}
	};

	struct write_walk
	{
		zone_base* zone;
		zone_buffer* buf;

		template <typename T>
		T* array(T*& field, const std::size_t count, const std::uint32_t alignment)
		{
			return write_shared_array(this->buf, field, count, alignment);
		}

		template <typename T>
		void string(T*& field)
		{
			write_string(this->buf, field);
		}

		template <typename T>
		void asset(const XAssetType type, T*& field)
		{
			write_asset(this->zone, type, field);
		}

		void script_strings(scr_string_t*& field, const std::size_t count)
		{
			write_shared_array(this->buf, field, count, 3);
		}

		void inline_script_strings(scr_string_t*, std::size_t)
		{
		}

		void script_string(scr_string_t&)
		{
		}

		unsigned int temp_count(unsigned int count)
		{
			this->buf->push_stream(XFILE_BLOCK_TEMP);
			this->buf->align(3);
			this->buf->write(&count);
			this->buf->pop_stream();
			return count;
		}

		template <typename T>
		void keep_temp(T*&, std::size_t)
		{
		}

		void push_stream(const XFileBlock stream)
		{
			this->buf->push_stream(stream);
		}

		void pop_stream()
		{
			this->buf->pop_stream();
		}
	};

	struct dump_walk
	{
		assetmanager::dumper& dump;

		template <typename T>
		T* array(T*& field, const std::size_t count, std::uint32_t)
		{
			return this->dump.dump_array(field, static_cast<std::uint32_t>(count)) ? field : nullptr;
		}

		template <typename T>
		void string(T*& field)
		{
			this->dump.dump_string(field);
		}

		template <typename T>
		void asset(XAssetType, T*& field)
		{
			this->dump.dump_asset(field);
		}

		void script_strings(scr_string_t*& field, const std::size_t count)
		{
			dump_script_strings(this->dump, field, count);
		}

		void inline_script_strings(scr_string_t* strings, const std::size_t count)
		{
			for (std::size_t i = 0; i < count; i++)
			{
				dump_script_string(this->dump, strings[i]);
			}
		}

		void script_string(scr_string_t& field)
		{
			dump_script_string(this->dump, field);
		}

		unsigned int temp_count(const unsigned int count)
		{
			return count;
		}

		template <typename T>
		void keep_temp(T*&, std::size_t)
		{
		}

		void push_stream(XFileBlock)
		{
		}

		void pop_stream()
		{
		}
	};

	struct parse_walk
	{
		assetmanager::reader& read;

		template <typename T>
		T* array(T*& field, std::size_t, std::uint32_t)
		{
			auto shared = false;
			field = this->read.read_array<std::remove_const_t<T>>(nullptr, &shared);
			return shared ? nullptr : field;
		}

		template <typename T>
		void string(T*& field)
		{
			field = this->read.read_string();
		}

		template <typename T>
		void asset(XAssetType, T*& field)
		{
			parse_asset(this->read, field);
		}

		unsigned int temp_count(const unsigned int count)
		{
			return count;
		}

		template <typename T>
		void keep_temp(T*&, std::size_t)
		{
		}

		void push_stream(XFileBlock)
		{
		}

		void pop_stream()
		{
		}
	};

	struct parse_script_walk : parse_walk
	{
		script_string_map& map;

		void script_strings(scr_string_t*& field, const std::size_t count)
		{
			this->map.parse(this->read, field, count);
		}

		void inline_script_strings(scr_string_t* strings, const std::size_t count)
		{
			for (std::size_t i = 0; i < count; i++)
			{
				this->map.parse(this->read, strings[i]);
			}
		}

		void script_string(scr_string_t& field)
		{
			this->map.parse(this->read, field);
		}
	};

	struct prepare_walk
	{
		zone_buffer* buf;
		script_string_map& map;

		template <typename T>
		T* array(T*& field, std::size_t, std::uint32_t)
		{
			return field;
		}

		template <typename T>
		void string(T*&)
		{
		}

		template <typename T>
		void asset(XAssetType, T*&)
		{
		}

		void script_strings(scr_string_t*& field, const std::size_t count)
		{
			this->map.prepare(this->buf, field, count);
		}

		void inline_script_strings(scr_string_t* strings, const std::size_t count)
		{
			this->map.prepare(this->buf, strings, count);
		}

		void script_string(scr_string_t& field)
		{
			this->map.prepare(this->buf, field);
		}

		unsigned int temp_count(const unsigned int count)
		{
			return count;
		}

		template <typename T>
		void keep_temp(T*&, std::size_t)
		{
		}

		void push_stream(XFileBlock)
		{
		}

		void pop_stream()
		{
		}
	};

	struct depend_walk
	{
		zone_base* zone;

		template <typename T>
		T* array(T*& field, std::size_t, std::uint32_t)
		{
			return field;
		}

		template <typename T>
		void string(T*&)
		{
		}

		template <typename T>
		void asset(const XAssetType type, T*& field)
		{
			add_asset(this->zone, type, field);
		}

		void script_string(scr_string_t&)
		{
		}

		void script_strings(scr_string_t*&, std::size_t)
		{
		}

		void inline_script_strings(scr_string_t*, std::size_t)
		{
		}

		unsigned int temp_count(const unsigned int count)
		{
			return count;
		}

		template <typename T>
		void keep_temp(T*&, std::size_t)
		{
		}

		void push_stream(XFileBlock)
		{
		}

		void pop_stream()
		{
		}
	};

	template <typename Walk, typename T>
	void walk_asset_array(Walk& walk, const XAssetType type, T**& field, const std::size_t count)
	{
		if (const auto assets = walk.array(field, count, 7))
		{
			for (std::size_t i = 0; i < count; i++)
			{
				walk.asset(type, assets[i]);
			}
		}
	}

	template <typename Walk>
	void walk_spatial_tree(Walk walk, SpatialPartition_Tree*& field)
	{
		if (const auto tree = walk.array(field, 1, 7))
		{
			walk.array(tree->tree, tree->treeSize, 0);
			walk.array(tree->alwaysList, tree->alwaysListLength, 3);
		}
	}
}
