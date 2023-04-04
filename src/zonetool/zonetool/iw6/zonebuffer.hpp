#pragma once
#include <std_include.hpp>

#include <stack>
#include <bitset>

namespace zonetool::iw6
{
	class zone_buffer
	{
	protected:
		std::vector<std::uint8_t> m_buf;
		std::size_t m_pos;
		std::size_t m_len;

		std::uint8_t m_stream;

		std::int32_t m_shiftsize;
		std::size_t m_numstreams;

		std::vector<std::uintptr_t> m_zonestreams;
		std::stack<std::uint8_t> m_streamstack;

		std::vector<const char*> m_scriptstrings;

		// material shit
		std::vector<std::uint64_t> m_depthstencilstatebits;
		std::vector<std::array<std::uint32_t, 3>> m_blendstatebits;

		// technique shit
		std::vector<std::uint32_t> m_ppas;
		std::vector<std::uint32_t> m_poas;
		std::vector<std::uint32_t> m_sas;

		std::vector<std::uintptr_t> m_streamfiles; // XStreamFile pointer

	private:
		void write_data(const void* _data, std::size_t size, std::size_t count);
		void write_data(const void* _data, std::size_t size);

		void init_script_strings();

		std::unordered_map<std::uintptr_t, std::uintptr_t> m_zonepointers;
		
	public:
		zone_buffer();
		~zone_buffer();
		
		zone_buffer(std::vector<std::uint8_t> data);
		zone_buffer(std::size_t size);
		
		std::uintptr_t get_stream_pos()
		{
			return this->m_zonestreams[this->m_stream];
		}

		template <typename T>
		T* get_zone_pointer()
		{
			return reinterpret_cast<T*>
				(0xFDFDFDF000000000 | (static_cast<std::uint64_t>(this->m_stream) & 0x0F) << 32 | ((m_zonestreams[m_stream] + 1) & 0x0FFFFFFFFFFFFFF));
		}

		template <typename T = char>
		T* at()
		{
			return reinterpret_cast<T*>(m_buf.data() + m_pos);
		}

		template <typename T>
		bool has_pointer(T* pointer)
		{
			if (m_zonepointers.find(reinterpret_cast<std::uintptr_t>(pointer)) != m_zonepointers.end())
			{
				return true;
			}

			return false;
		}

		template <typename T>
		T* get_pointer(T* pointer)
		{
			if (m_zonepointers.find(reinterpret_cast<std::uintptr_t>(pointer)) != m_zonepointers.end())
			{
				return reinterpret_cast<T*>(m_zonepointers[reinterpret_cast<std::uintptr_t>(pointer)]);
			}

			return nullptr;
		}

		template <typename T>
		void create_pointer(T* pointer)
		{
			m_zonepointers[reinterpret_cast<std::uintptr_t>(pointer)] = reinterpret_cast<std::uintptr_t>(this->
				get_zone_pointer<T>());
		}

		// T is return type, should be a pointer always.
		template <typename T>
		T* write_s(std::uint64_t alignment, T* _data, std::size_t _count = 1, std::size_t _size = sizeof(T),
		           T** outPointer = nullptr)
		{
			if (m_stream != 2)
			{
				// if data is a nullptr, return null.
				if (!_data)
				{
					return nullptr;
				}

				// if a zonepointer is found, do not rewrite the data.
				if (m_zonepointers.find((uintptr_t)_data) != m_zonepointers.end())
				{
					auto ptr = m_zonepointers[(uintptr_t)_data];
					return reinterpret_cast<T*>(ptr);
				}
			}

			if (alignment >= sizeof(long long) - 1)
			{
				if (alignof(T) - 1 > alignment)
				{
					printf(
						"[WARNING]: You might have an alignment issue somewhere fella! AlignOf(T) is higher than the specified amount!\n");
				}

				if (alignment % 2 == 0)
				{
					printf(
						"[WARNING]: You might have an alignment issue somewhere fella! Alignment is an even number (%llu)!\n",
						alignment);
				}
			}

			// align the buffer writing.
			this->align(alignment);

			if (outPointer)
			{
				*outPointer = this->at<T>();
			}

			if (m_stream != 2)
			{
				// insert zonepointers
				for (std::size_t i = 0; i < _count; i++)
				{
					auto ptr = (uintptr_t)(this->get_zone_pointer<std::uintptr_t>());
					m_zonepointers[(uintptr_t)(void*)&_data[i]] = ptr;

					this->write<T>(&_data[i]);
				}
			}

			// return -1 (data is following)
			return reinterpret_cast<T*>(0xFDFDFDFFFFFFFFFF);
		}

		template <typename T>
		T* write_p(T* _data, std::size_t _count = 1)
		{
			if (_count > 1)
			{
				for (std::size_t i = 0; i < _count; i++)
				{
					m_zonepointers[reinterpret_cast<std::uintptr_t>(_data + sizeof(T) * i)] = reinterpret_cast<std::
						uintptr_t>(this->get_zone_pointer<std::uintptr_t>() + (sizeof(T) * i));
				}
			}
			else
			{
				m_zonepointers[reinterpret_cast<std::uintptr_t>(_data)] = reinterpret_cast<std::uintptr_t>(this->
					get_zone_pointer<std::uintptr_t>());
			}

			return write<T>(_data, _count);
		}

		void init_streams(std::size_t num_streams);

		void write_stream(const void* _data, std::size_t size, std::size_t count);
		void write_stream(const void* _data, std::size_t size);

		char* write_str(const std::string& _str);
		void write_str_raw(const std::string& _str);

		template <typename T>
		T* write(T* _data, std::size_t count = 1)
		{
			auto dest = this->at<T>();
			write_stream(_data, sizeof(T), count);
			return dest;
		}

		std::uint8_t* buffer();
		std::size_t size();
		void clear();

		void align(std::uint64_t alignment);
		void inc_stream(const std::uint8_t stream, const std::size_t size);
		void push_stream(std::uint8_t stream);
		void pop_stream();

		std::uint8_t current_stream();
		std::uint32_t current_stream_offset();
		std::uint32_t stream_offset(std::uint8_t stream);

		std::uint32_t write_scriptstring(const char* str);
		const char* get_scriptstring(std::size_t idx);
		std::size_t scriptstring_count();

		std::uint8_t write_depthstencilstatebit(std::uint64_t bits);
		std::uint64_t get_depthstencilstatebit(std::size_t idx);
		std::size_t depthstencilstatebit_count();

		std::uint8_t write_blendstatebits(std::array<std::uint32_t, 3> bits);
		std::array<std::uint32_t, 3> get_blendstatebits(std::size_t idx);
		std::size_t blendstatebits_count();

		void write_streamfile(std::uintptr_t stream);
		std::uintptr_t get_streamfile(std::size_t idx);
		std::size_t streamfile_count();

		std::uint8_t write_ppas(std::uint32_t sz);
		std::uint32_t get_ppas(std::size_t idx);
		std::size_t ppas_count();

		std::uint8_t write_poas(std::uint32_t sz);
		std::uint32_t get_poas(std::size_t idx);
		std::size_t poas_count();

		std::uint8_t write_sas(std::uint32_t sz);
		std::uint32_t get_sas(std::size_t idx);
		std::size_t sas_count();

		template <typename T>
		static void insert_pointer(T* ptr)
		{
			*reinterpret_cast<std::uintptr_t*>(ptr) = static_cast<std::uintptr_t>(0xFDFDFDFFFFFFFFFE);
		}

		template <typename T>
		static void clear_pointer(T* ptr)
		{
			*reinterpret_cast<std::uintptr_t*>(ptr) = static_cast<std::uintptr_t>(0xFDFDFDFFFFFFFFFF);
		}

		void save(const std::string& filename, bool use_zone_path = true);

		std::vector<std::uint8_t> compress_zlib(bool compress_blocks = false);
		std::vector<std::uint8_t> compress_zstd();
		std::vector<std::uint8_t> compress_lz4();
	};
}
