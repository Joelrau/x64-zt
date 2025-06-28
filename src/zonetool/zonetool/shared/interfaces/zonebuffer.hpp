#pragma once
#include <std_include.hpp>

#include "game/mode.hpp"

#include <stack>
#include <bitset>

namespace zonetool
{
	struct sub_zone_buffer
	{
		std::size_t start;
		std::size_t end;
		std::size_t ptr;
		std::uint8_t stream;
	};

	class zone_buffer
	{
	public:
		zone_buffer();
		~zone_buffer();
		
		zone_buffer(const std::vector<std::uint8_t>& data);
		zone_buffer(const std::size_t size);

		std::uint32_t zone_stream_runtime;

		std::uint64_t data_mask;

		std::uint64_t data_following;
		std::uint64_t data_offset;
		std::uint64_t data_ptr;
		std::uint64_t data_shared; // iw7
		
		void init();
		void set_fields(std::uint32_t zone_stream_runtime_, std::uint64_t data_mask_, std::uint32_t data_following_, std::uint32_t data_offset_, std::uint32_t data_ptr_, std::uint32_t data_shared_ = 0);

		std::size_t get_stream_pos()
		{
			return this->zone_streams_[this->stream_];
		}

		std::size_t create_data_ptr(const std::uint32_t data = 0x0, const std::uint8_t stream = 0xF, std::uint64_t mask = 0);

		template <typename T>
		T* get_zone_pointer(const std::uint8_t stream, const std::size_t ptr)
		{
			if (game::get_mode() == game::game_mode::iw7)
			{
				return reinterpret_cast<T*>(create_data_ptr(0x0, stream, 0x0) | ((ptr + 1) & 0x0FFFFFFFFFFFFFF));
			}

			return reinterpret_cast<T*>(create_data_ptr(0x0, stream, this->data_mask) | ((ptr + 1) & 0x0FFFFFFFFFFFFFF));
		}

		template <typename T = char>
		T* at()
		{
			return reinterpret_cast<T*>(this->buffer_.data() + this->pos_);
		}

		template <typename T>
		T* find_sub_buffer(const T* data)
		{
			const auto ptr = reinterpret_cast<std::size_t>(data);
			for (const auto& buffer : this->sub_zone_buffers_)
			{
				if (ptr >= buffer.start && ptr < buffer.end)
				{
					const auto offset = ptr - buffer.start;
					return this->get_zone_pointer<T>(buffer.stream, buffer.ptr + offset);
				}
			}

			return nullptr;
		}

		template <typename T>
		void insert_sub_buffer(const T* data, const std::size_t count, const std::size_t size = sizeof(T))
		{
			sub_zone_buffer buffer{};
			buffer.start = reinterpret_cast<std::size_t>(data);
			buffer.end = buffer.start + size * count;
			buffer.ptr = this->zone_streams_[this->stream_];
			buffer.stream = this->stream_;
			this->sub_zone_buffers_.emplace_back(buffer);
		}

		template <typename T>
		T* write_s(const std::size_t alignment, T* data, const std::size_t count = 1, 
			const std::size_t size = sizeof(T), T** out_pointer = nullptr)
		{
			if (this->stream_ != this->zone_stream_runtime)
			{
				if (!data)
				{
					return nullptr;
				}

				const auto found = this->find_sub_buffer(data);
				if (found != nullptr)
				{
					return found;
				}
			}

			if (alignment >= sizeof(long long) - 1)
			{
				if (alignof(T) - 1 > alignment)
				{
					printf(
						"[zonebuffer]: There is an alignment issue somewhere! alignof(T) is higher than the specified amount!\n");
				}

				if (alignment % 2 == 0)
				{
					printf(
						"[zonebuffer]: There is an alignment issue somewhere! alignment is an even number (%llu)!\n",
						alignment);
				}
			}

			this->align(alignment);

			if (out_pointer)
			{
				*out_pointer = this->at<T>();
			}

			if (this->stream_ != this->zone_stream_runtime)
			{
				this->insert_sub_buffer(data, count, size);
				this->write_stream(data, size, count);
			}

			return reinterpret_cast<T*>(this->data_following);
		}

		void init_streams(const std::size_t num_streams);

		void write_stream(const void* data, std::size_t size, std::size_t count);
		void write_stream(const void* data, std::size_t size);

		char* write_str(const char* _str);
		char* write_str(const std::string& _str);
		char* write_str_raw(const std::string& _str);

		template <typename T>
		T* write(T* data, const std::size_t count = 1)
		{
			const auto dest = this->at<T>();
			this->write_stream(data, sizeof(T), count);
			return dest;
		}

		std::vector<std::uint8_t>* buffer_raw();
		std::uint8_t* buffer();
		std::size_t size();
		void clear();

		void align(const std::size_t alignment);
		void inc_stream(const std::uint8_t stream, const std::size_t size);
		void push_stream(const std::uint8_t stream);
		void pop_stream();

		std::uint8_t current_stream();
		std::uint32_t current_stream_offset();
		std::uint32_t stream_offset(const std::uint8_t stream);

		std::uint32_t write_scriptstring(const char* str);
		const char* get_scriptstring(const std::size_t idx);
		std::size_t scriptstring_count();

		std::uint8_t write_depthstencilstatebit(const std::size_t bits);
		std::size_t get_depthstencilstatebit(const std::size_t idx);
		std::size_t depthstencilstatebit_count();

		std::uint8_t write_blendstatebits(const std::array<std::uint32_t, 4>& bits);
		std::array<std::uint32_t, 4> get_blendstatebits(std::size_t idx);
		std::size_t blendstatebits_count();

		void write_streamfile(const std::size_t stream);
		std::size_t get_streamfile(const std::size_t idx);
		std::size_t streamfile_count();

		std::uint8_t write_ppas(const std::uint32_t sz);
		std::uint32_t get_ppas(const std::size_t idx);
		std::size_t ppas_count();

		std::uint8_t write_poas(const std::uint32_t sz);
		std::uint32_t get_poas(const std::size_t idx);
		std::size_t poas_count();

		std::uint8_t write_sas(const std::uint32_t sz);
		std::uint32_t get_sas(const std::size_t idx);
		std::size_t sas_count();

		template <typename T>
		void insert_pointer(T* ptr)
		{
			*reinterpret_cast<std::size_t*>(ptr) = static_cast<std::size_t>(this->data_ptr);
		}

		template <typename T>
		void clear_pointer(T* ptr)
		{
			*reinterpret_cast<std::size_t*>(ptr) = static_cast<std::size_t>(this->data_following);
		}

		void save(const std::string& filename, bool use_zone_path = true);

		std::vector<std::uint8_t> compress_zlib(bool compress_blocks = false);
		std::vector<std::uint8_t> compress_zstd();
		std::vector<std::uint8_t> compress_lz4();

	private:
		std::vector<std::uint8_t> buffer_;
		std::size_t pos_;
		std::size_t length_;

		std::uint8_t stream_;

		std::int32_t shift_size_;
		std::size_t stream_count_;

		std::vector<std::size_t> zone_streams_;
		std::stack<std::uint8_t> stream_stack_;

		std::vector<const char*> script_strings_;

		std::vector<std::size_t> depth_stencil_state_bits_;
		std::vector<std::array<std::uint32_t, 4>> blend_state_bits_;

		std::vector<std::uint32_t> ppas_;
		std::vector<std::uint32_t> poas_;
		std::vector<std::uint32_t> sas_;

		std::vector<std::size_t> stream_files_;

		void write_data(const void* data, const std::size_t size, const std::size_t count);
		void write_data(const void* data, const std::size_t size);

		void init_script_strings();

		std::vector<sub_zone_buffer> sub_zone_buffers_;

	};
}
