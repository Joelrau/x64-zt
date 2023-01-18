#include <std_include.hpp>
#include "zonebuffer.hpp"

#include "zonetool/utils/utils.hpp"
#include "zonetool/utils/compression.hpp"

#define ZSTD_COMPRESSION 11
#define ZLIB_COMPRESSION Z_BEST_COMPRESSION

namespace zonetool::h2
{
	ZoneBuffer::ZoneBuffer()
	{
		this->m_stream = 0;
		this->m_shiftsize = 0;
		this->m_numstreams = 0;

		this->m_pos = 0;
		this->m_len = MAX_ZONE_SIZE;

		this->m_zonepointers.clear();
		this->init_script_strings();
		this->m_depthstencilstatebits.clear();
		this->m_blendstatebits.clear();
		this->m_ppas.clear();
		this->m_poas.clear();
		this->m_sas.clear();
		this->m_streamfiles.clear();

		// 2gb, should be enough?
		this->m_buf.resize(this->m_len);
	}

	ZoneBuffer::~ZoneBuffer()
	{
		// clear zone pointers
		this->m_zonepointers.clear();

		// clear scriptstrings
		this->init_script_strings();

		// clear depth stencil state bits
		this->m_depthstencilstatebits.clear();

		// clear blend state bits
		this->m_blendstatebits.clear();

		// clear perPrimArgSizes
		this->m_ppas.clear();

		// clear perObjArgSizes
		this->m_poas.clear();

		// clear stableArgSizes
		this->m_sas.clear();

		// clear streamfiles
		this->m_streamfiles.clear();

		// clear zone buffer
		this->m_buf.clear();
	}

	ZoneBuffer::ZoneBuffer(std::vector<std::uint8_t> data)
	{
		this->m_stream = 0;
		this->m_shiftsize = 0;
		this->m_numstreams = 0;

		this->m_buf = data;
		this->m_pos = data.size();
		this->m_len = data.size();

		this->m_zonepointers.clear();
		this->init_script_strings();
		this->m_depthstencilstatebits.clear();
		this->m_blendstatebits.clear();
		this->m_ppas.clear();
		this->m_poas.clear();
		this->m_sas.clear();
		this->m_streamfiles.clear();
	}

	ZoneBuffer::ZoneBuffer(std::size_t size)
	{
		this->m_stream = 0;
		this->m_shiftsize = 0;
		this->m_numstreams = 0;

		this->m_pos = 0;
		this->m_len = size;
		this->m_buf.resize(this->m_len);

		this->m_zonepointers.clear();
		this->init_script_strings();
		this->m_depthstencilstatebits.clear();
		this->m_blendstatebits.clear();
		this->m_ppas.clear();
		this->m_poas.clear();
		this->m_sas.clear();
		this->m_streamfiles.clear();
	}

	void ZoneBuffer::init_streams(std::size_t streams)
	{
		this->m_numstreams = streams;
		this->m_zonestreams.resize(streams);

		this->m_shiftsize = 32;

		for (std::size_t i = 31; i > 0; i--)
		{
			if (std::bitset<sizeof(std::size_t) * 8>(this->m_numstreams).test(i))
			{
				this->m_shiftsize -= (static_cast<std::uint32_t>(i) + 1);
				break;
			}
		}
	}

	void ZoneBuffer::write_data(const void* _data, std::size_t size, std::size_t count)
	{
		// Check if we should realloc the buffer
		if ((size * count) + m_pos > m_len)
		{
			ZONETOOL_ERROR("No more space left in zone buffer."); // this->realloc(((size * count) + m_pos) - m_len);
			return;
		}

		// Copy data to buffer
		memcpy(&m_buf[m_pos], _data, size * count);
		m_pos += size * count;
	}

	void ZoneBuffer::write_data(const void* _data, std::size_t size)
	{
		write_data(_data, size, 1);
	}

	void ZoneBuffer::write_stream(const void* _data, std::size_t size, std::size_t count)
	{
		// If we're writing to stream 2...
		if (m_stream == 2) // ZONESTREAM_RUNTIME. Meaning that this data is generated when the game runs.
			// Therefore we need to alloc space, but we don't write data.
		{
			// if (m_numstreams > 0)
			{
				m_zonestreams[m_stream] += size * count;
			}

			return;
		}

		write_data(_data, size, count);

		// Update streams
		// if (m_numstreams > 0)
		{
			m_zonestreams[m_stream] += size * count;
		}
	}

	void ZoneBuffer::write_stream(const void* _data, std::size_t size)
	{
		return write_stream(_data, size, 1);
	}

	char* ZoneBuffer::write_str(const std::string& _str)
	{
		write_stream(_str.data(), _str.size() + 1);
		return reinterpret_cast<char*>(0xFDFDFDFFFFFFFFFF);
	}

	void ZoneBuffer::write_str_raw(const std::string& _str)
	{
		return write_stream(_str.data(), _str.size() + 1);
	}

	std::uint8_t* ZoneBuffer::buffer()
	{
		return m_buf.data();
	}

	std::size_t ZoneBuffer::size()
	{
		return m_pos;
	}

	void ZoneBuffer::align(std::uint64_t alignment)
	{
		// if (m_numstreams > 0)
		{
			m_zonestreams[m_stream] = (~alignment & (alignment + m_zonestreams[m_stream]));
		}
	}

	void ZoneBuffer::inc_stream(const std::uint8_t stream, const std::size_t size)
	{
		m_zonestreams[stream] += size;
	}

	void ZoneBuffer::push_stream(std::uint8_t stream)
	{
		m_streamstack.push(m_stream);
		m_stream = stream;
	}

	void ZoneBuffer::pop_stream()
	{
		m_stream = m_streamstack.top();
		m_streamstack.pop();
	}

	std::uint8_t ZoneBuffer::current_stream()
	{
		return m_stream;
	}

	std::uint32_t ZoneBuffer::current_stream_offset()
	{
		return static_cast<std::uint32_t>(m_zonestreams[m_stream]);
	}

	std::uint32_t ZoneBuffer::stream_offset(std::uint8_t stream)
	{
		return static_cast<std::uint32_t>(m_zonestreams[stream]);
	}

	std::uint32_t ZoneBuffer::write_scriptstring(const char* str)
	{
		for (std::uint32_t i = 0; i < this->m_scriptstrings.size(); i++)
		{
			if (this->m_scriptstrings[i] == str ||
				str && this->m_scriptstrings[i] && !strcmp(this->m_scriptstrings[i], str))
			{
				return i;
			}
		}
		this->m_scriptstrings.push_back(str);
		return static_cast<std::uint32_t>(this->m_scriptstrings.size() - 1);
	}

	const char* ZoneBuffer::get_scriptstring(std::size_t idx)
	{
		return this->m_scriptstrings[idx];
	}

	std::size_t ZoneBuffer::scriptstring_count()
	{
		return this->m_scriptstrings.size();
	}

	std::uint8_t ZoneBuffer::write_depthstencilstatebit(std::uint64_t bits)
	{
		for (std::uint8_t i = 0; i < this->m_depthstencilstatebits.size(); i++)
		{
			if (this->m_depthstencilstatebits[i] == bits)
			{
				return i;
			}
		}
		this->m_depthstencilstatebits.push_back(bits);
		return static_cast<std::uint8_t>(this->m_depthstencilstatebits.size() - 1);
	}

	std::uint64_t ZoneBuffer::get_depthstencilstatebit(std::size_t idx)
	{
		return this->m_depthstencilstatebits[idx];
	}

	std::size_t ZoneBuffer::depthstencilstatebit_count()
	{
		return this->m_depthstencilstatebits.size();
	}

	std::uint8_t ZoneBuffer::write_blendstatebits(std::array<std::uint32_t, 3> bits)
	{
		for (std::uint8_t i = 0; i < this->m_blendstatebits.size(); i++)
		{
			bool match = true;
			for (auto j = 0; j < 3; j++)
			{
				if (this->m_blendstatebits[i][j] != bits[j])
				{
					match = false;
					break;
				}
			}
			if (match)
			{
				return i;
			}
		}
		this->m_blendstatebits.push_back(bits);
		return static_cast<std::uint8_t>(this->m_blendstatebits.size() - 1);
	}

	std::array<std::uint32_t, 3> ZoneBuffer::get_blendstatebits(std::size_t idx)
	{
		return this->m_blendstatebits[idx];
	}

	std::size_t ZoneBuffer::blendstatebits_count()
	{
		return this->m_blendstatebits.size();
	}

	std::uint8_t ZoneBuffer::write_ppas(std::uint32_t sz)
	{
		for (std::uint8_t i = 0; i < this->m_ppas.size(); i++)
		{
			if (this->m_ppas[i] == sz)
			{
				return i;
			}
		}
		this->m_ppas.push_back(sz);
		return static_cast<std::uint8_t>(this->m_ppas.size() - 1);
	}

	std::uint32_t ZoneBuffer::get_ppas(std::size_t idx)
	{
		return this->m_ppas[idx];
	}

	std::size_t ZoneBuffer::ppas_count()
	{
		return this->m_ppas.size();
	}

	std::uint8_t ZoneBuffer::write_poas(std::uint32_t sz)
	{
		for (std::uint8_t i = 0; i < this->m_poas.size(); i++)
		{
			if (this->m_poas[i] == sz)
			{
				return i;
			}
		}
		this->m_poas.push_back(sz);
		return static_cast<std::uint8_t>(this->m_poas.size() - 1);
	}

	std::uint32_t ZoneBuffer::get_poas(std::size_t idx)
	{
		return this->m_poas[idx];
	}

	std::size_t ZoneBuffer::poas_count()
	{
		return this->m_poas.size();
	}

	std::uint8_t ZoneBuffer::write_sas(std::uint32_t sz)
	{
		for (std::uint8_t i = 0; i < this->m_sas.size(); i++)
		{
			if (this->m_sas[i] == sz)
			{
				return i;
			}
		}
		this->m_sas.push_back(sz);
		return static_cast<std::uint8_t>(this->m_sas.size() - 1);
	}

	std::uint32_t ZoneBuffer::get_sas(std::size_t idx)
	{
		return this->m_sas[idx];
	}

	std::size_t ZoneBuffer::sas_count()
	{
		return this->m_sas.size();
	}

	void ZoneBuffer::write_streamfile(std::uintptr_t stream)
	{
		this->m_streamfiles.push_back(stream);
	}

	std::uintptr_t ZoneBuffer::get_streamfile(std::size_t idx)
	{
		return this->m_streamfiles[idx];
	}

	std::size_t ZoneBuffer::streamfile_count()
	{
		return this->m_streamfiles.size();
	}

	void ZoneBuffer::save(const std::string& filename, bool use_zone_path)
	{
		auto file = filesystem::file(filename);
		file.create_path();
		file.open("wb", false, use_zone_path);
		file.write(this->m_buf.data(), this->m_pos, 1);
		file.close();
	}

	std::vector<std::uint8_t> ZoneBuffer::compress_zlib(bool compress_blocks)
	{
		return compression::compress_zlib(this->m_buf.data(), this->m_pos, compress_blocks);
	}

	std::vector<std::uint8_t> ZoneBuffer::compress_zstd()
	{
		return compression::compress_zstd(this->m_buf.data(), this->m_pos);
	}

	std::vector<std::uint8_t> ZoneBuffer::compress_lz4()
	{
		return compression::compress_lz4(this->m_buf.data(), this->m_pos);
	}

	void ZoneBuffer::init_script_strings()
	{
		this->m_scriptstrings.clear();
	}
}
