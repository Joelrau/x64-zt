#include <std_include.hpp>
#include "zonebuffer.hpp"

#include "zonetool/utils/utils.hpp"
#include "zonetool/utils/compression.hpp"

#define ZSTD_COMPRESSION 11
#define ZLIB_COMPRESSION Z_BEST_COMPRESSION

namespace zonetool
{
	zone_buffer::zone_buffer()
	{
		this->init();

		this->pos_ = 0;
		this->length_ = MAX_ZONE_SIZE;
		this->buffer_.resize(this->length_);
	}

	zone_buffer::~zone_buffer()
	{
		this->clear();
	}

	zone_buffer::zone_buffer(const std::vector<std::uint8_t>& data)
	{
		this->init();
		this->buffer_ = data;
		this->pos_ = data.size();
		this->length_ = data.size();
	}

	zone_buffer::zone_buffer(const std::size_t size)
	{
		this->init();
		this->pos_ = 0;
		this->length_ = size;
		this->buffer_.resize(this->length_);
	}

	void zone_buffer::init()
	{
		this->clear();

		this->set_fields(2,
			0xFDFDFDF000000000,
			static_cast<std::uint32_t>(-1),
			static_cast<std::uint32_t>(-3),
			static_cast<std::uint32_t>(-2));
	}

	void zone_buffer::set_fields(std::uint32_t zone_stream_runtime_, std::uint64_t data_mask_, std::uint32_t data_following_, std::uint32_t data_offset_, std::uint32_t data_ptr_, std::uint32_t data_shared_)
	{
		this->zone_stream_runtime = zone_stream_runtime_;

		this->data_mask = data_mask_;

		this->data_following = this->create_data_ptr(data_following_);
		this->data_offset = this->create_data_ptr(data_offset_);
		this->data_ptr = this->create_data_ptr(data_ptr_);
		this->data_shared = this->create_data_ptr(data_shared_);
	}

	std::size_t zone_buffer::create_data_ptr(const std::uint32_t data, const std::uint8_t stream)
	{
		return this->data_mask | data | (static_cast<std::size_t>(stream) & 0x0F) << 32;
	}

	void zone_buffer::init_streams(const std::size_t streams)
	{
		this->stream_count_ = streams;
		this->zone_streams_.resize(streams);

		this->shift_size_ = 32;

		for (auto i = 31ull; i > 0; i--)
		{
			if (std::bitset<sizeof(std::size_t) * 8>(this->stream_count_).test(i))
			{
				this->shift_size_ -= (static_cast<std::uint32_t>(i) + 1);
				break;
			}
		}
	}

	void zone_buffer::write_data(const void* data, const std::size_t size, const std::size_t count)
	{
		if ((size * count) + this->pos_ > this->length_)
		{
			ZONETOOL_ERROR("No more space left in zone buffer."); // this->realloc(((size * count) + m_pos) - m_len);
			return;
		}

		std::memcpy(&this->buffer_[this->pos_], data, size * count);
		this->pos_ += size * count;
	}

	void zone_buffer::write_data(const void* data, const std::size_t size)
	{
		this->write_data(data, size, 1);
	}

	void zone_buffer::write_stream(const void* data, const std::size_t size, const std::size_t count)
	{
		if (this->stream_ == this->zone_stream_runtime)
		{
			if (this->stream_count_ > 0)
			{
				this->zone_streams_[this->stream_] += size * count;
			}

			return;
		}

		this->write_data(data, size, count);

		if (this->stream_count_ > 0)
		{
			this->zone_streams_[this->stream_] += size * count;
		}
	}

	void zone_buffer::write_stream(const void* data, const std::size_t size)
	{
		return this->write_stream(data, size, 1);
	}

	char* zone_buffer::write_str(const std::string& str)
	{
		this->write_stream(str.data(), str.size() + 1);
		return reinterpret_cast<char*>(this->data_following);
	}

	void zone_buffer::write_str_raw(const std::string& str)
	{
		return write_stream(str.data(), str.size() + 1);
	}

	std::vector<std::uint8_t>* zone_buffer::buffer_raw()
	{
		return &this->buffer_;
	}

	std::uint8_t* zone_buffer::buffer()
	{
		return this->buffer_.data();
	}

	std::size_t zone_buffer::size()
	{
		return this->pos_;
	}

	void zone_buffer::clear()
	{
		this->stream_ = 0;
		this->shift_size_ = 0;
		this->stream_count_ = 0;

		this->pos_ = 0;
		this->length_ = 0;

		this->sub_zone_buffers_.clear();
		this->init_script_strings();
		this->depth_stencil_state_bits_.clear();
		this->blend_state_bits_.clear();
		this->ppas_.clear();
		this->poas_.clear();
		this->sas_.clear();
		this->stream_files_.clear();

		this->buffer_.clear();
		this->buffer_.shrink_to_fit();
	}

	void zone_buffer::align(const std::size_t alignment)
	{
		if (this->stream_count_ > 0)
		{
			this->zone_streams_[this->stream_] = (~alignment & (alignment + this->zone_streams_[this->stream_]));
		}
	}

	void zone_buffer::inc_stream(const std::uint8_t stream, const std::size_t size)
	{
		this->zone_streams_[stream] += size;
	}

	void zone_buffer::push_stream(const std::uint8_t stream)
	{
		this->stream_stack_.push(this->stream_);
		this->stream_ = stream;
	}

	void zone_buffer::pop_stream()
	{
		this->stream_ = this->stream_stack_.top();
		this->stream_stack_.pop();
	}

	std::uint8_t zone_buffer::current_stream()
	{
		return this->stream_;
	}

	std::uint32_t zone_buffer::current_stream_offset()
	{
		return static_cast<std::uint32_t>(this->zone_streams_[this->stream_]);
	}

	std::uint32_t zone_buffer::stream_offset(const std::uint8_t stream)
	{
		return static_cast<std::uint32_t>(this->zone_streams_[stream]);
	}

	std::uint32_t zone_buffer::write_scriptstring(const char* str)
	{
		for (auto i = 0u; i < this->script_strings_.size(); i++)
		{
			if (this->script_strings_[i] == str || 
				str && this->script_strings_[i] && !strcmp(this->script_strings_[i], str))
			{
				return i;
			}
		}

		this->script_strings_.push_back(str);
		return static_cast<std::uint32_t>(this->script_strings_.size() - 1);
	}

	const char* zone_buffer::get_scriptstring(const std::size_t idx)
	{
		return this->script_strings_[idx];
	}

	std::size_t zone_buffer::scriptstring_count()
	{
		return this->script_strings_.size();
	}

	std::uint8_t zone_buffer::write_depthstencilstatebit(const std::size_t bits)
	{
		for (auto i = 0u; i < this->depth_stencil_state_bits_.size(); i++)
		{
			if (this->depth_stencil_state_bits_[i] == bits)
			{
				return static_cast<std::uint8_t>(i);
			}
		}

		this->depth_stencil_state_bits_.push_back(bits);
		return static_cast<std::uint8_t>(this->depth_stencil_state_bits_.size() - 1);
	}

	std::size_t zone_buffer::get_depthstencilstatebit(const std::size_t idx)
	{
		return this->depth_stencil_state_bits_[idx];
	}

	std::size_t zone_buffer::depthstencilstatebit_count()
	{
		return this->depth_stencil_state_bits_.size();
	}

	std::uint8_t zone_buffer::write_blendstatebits(const std::array<std::uint32_t, 4>& bits)
	{
		for (auto i = 0u; i < this->blend_state_bits_.size(); i++)
		{
			auto match = true;
			for (auto j = 0; j < 4; j++)
			{
				if (this->blend_state_bits_[i][j] != bits[j])
				{
					match = false;
					break;
				}
			}

			if (match)
			{
				return static_cast<std::uint8_t>(i);
			}
		}

		this->blend_state_bits_.push_back(bits);
		return static_cast<std::uint8_t>(this->blend_state_bits_.size() - 1);
	}

	std::array<std::uint32_t, 4> zone_buffer::get_blendstatebits(const std::size_t idx)
	{
		return this->blend_state_bits_[idx];
	}

	std::size_t zone_buffer::blendstatebits_count()
	{
		return this->blend_state_bits_.size();
	}

	std::uint8_t zone_buffer::write_ppas(const std::uint32_t sz)
	{
		for (auto i = 0u; i < this->ppas_.size(); i++)
		{
			if (this->ppas_[i] == sz)
			{
				return static_cast<std::uint8_t>(i);
			}
		}

		this->ppas_.push_back(sz);
		return static_cast<std::uint8_t>(this->ppas_.size() - 1);
	}

	std::uint32_t zone_buffer::get_ppas(const std::size_t idx)
	{
		return this->ppas_[idx];
	}

	std::size_t zone_buffer::ppas_count()
	{
		return this->ppas_.size();
	}

	std::uint8_t zone_buffer::write_poas(const std::uint32_t sz)
	{
		for (auto i = 0; i < this->poas_.size(); i++)
		{
			if (this->poas_[i] == sz)
			{
				return static_cast<std::uint8_t>(i);
			}
		}

		this->poas_.push_back(sz);
		return static_cast<std::uint8_t>(this->poas_.size() - 1);
	}

	std::uint32_t zone_buffer::get_poas(const std::size_t idx)
	{
		return this->poas_[idx];
	}

	std::size_t zone_buffer::poas_count()
	{
		return this->poas_.size();
	}

	std::uint8_t zone_buffer::write_sas(const std::uint32_t sz)
	{
		for (auto i = 0; i < this->sas_.size(); i++)
		{
			if (this->sas_[i] == sz)
			{
				return static_cast<std::uint8_t>(i);
			}
		}

		this->sas_.push_back(sz);
		return static_cast<std::uint8_t>(this->sas_.size() - 1);
	}

	std::uint32_t zone_buffer::get_sas(const std::size_t idx)
	{
		return this->sas_[idx];
	}

	std::size_t zone_buffer::sas_count()
	{
		return this->sas_.size();
	}

	void zone_buffer::write_streamfile(const std::size_t stream)
	{
		this->stream_files_.push_back(stream);
	}

	std::size_t zone_buffer::get_streamfile(const std::size_t idx)
	{
		return this->stream_files_[idx];
	}

	std::size_t zone_buffer::streamfile_count()
	{
		return this->stream_files_.size();
	}

	void zone_buffer::save(const std::string& filename, bool use_zone_path)
	{
		auto file = filesystem::file(filename);
		file.create_path();
		file.open("wb", false, use_zone_path);
		file.write(this->buffer_.data(), this->pos_, 1);
		file.close();
	}

	std::vector<std::uint8_t> zone_buffer::compress_zlib(bool compress_blocks)
	{
		return compression::compress_zlib(this->buffer_.data(), this->pos_, compress_blocks);
	}

	std::vector<std::uint8_t> zone_buffer::compress_zstd()
	{
		return compression::compress_zstd(this->buffer_.data(), this->pos_);
	}

	std::vector<std::uint8_t> zone_buffer::compress_lz4()
	{
		return compression::compress_lz4(this->buffer_.data(), this->pos_);
	}

	void zone_buffer::init_script_strings()
	{
		this->script_strings_.clear();
	}
}
