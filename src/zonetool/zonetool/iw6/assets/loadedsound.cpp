#include <std_include.hpp>
#include "loadedsound.hpp"

#include <utils/bit_buffer.hpp>

#define SIZEOF_SNDFILE_WAVE_HEADER 46

#define APPLICATION_ID "fsiz"
#define CONSTANT_BLOCKSIZE 0x400

namespace zonetool::iw6
{
	namespace
	{
		enum block_type_t
		{
			streaminfo,
			padding,
			application,
			seektable,
			vorbis_comment,
			cuesheet,
			picture,
			count
		};

		struct metadata_block_header_t
		{
			bool is_last;
			block_type_t type;
			int length;
		};

		struct metadata_block_t
		{
			metadata_block_header_t header;
			char* data;
			char* start;
		};

		metadata_block_t parse_metadata_block(char* buffer)
		{
			/*
				// https://xiph.org/flac/format.html#metadata_block_header

				bits | description
				   1   is last block
				   7   block type
				  24   block length (header not included)
			*/

			const auto header = _byteswap_ulong(*reinterpret_cast<uint32_t*>(
				reinterpret_cast<uint64_t>(buffer)));

			const auto is_last = static_cast<bool>(header >> (8 * 3 + 7));
			const auto block_type = static_cast<block_type_t>((header << 1) >> (8 * 3 + 1));
			const auto block_length = (header << 8) >> 8;

			metadata_block_t block{};
			block.header.is_last = is_last;
			block.header.type = block_type;
			block.header.length = block_length;
			block.data = buffer + 4;
			block.start = buffer;

			return block;
		}

		void parse_metadata_block(char* buffer, metadata_block_t* block)
		{
			*block = parse_metadata_block(buffer);
		}

		bool check_signature(const std::string& buffer)
		{
			static std::vector<uint8_t> signature = {'f', 'L', 'a', 'C'};

			if (buffer.size() < signature.size())
			{
				return false;
			}

			for (auto i = 0; i < signature.size(); i++)
			{
				if (buffer[i] != signature[i])
				{
					return false;
				}
			}

			return true;
		}

		bool check_signature(const char* buffer)
		{
			return check_signature(std::string(buffer, 4));
		}

		void verify_streaminfo_block(const metadata_block_t& block)
		{
			const auto minimum = _byteswap_ushort(*reinterpret_cast<uint16_t*>(block.data));
			const auto maximum = _byteswap_ushort(*reinterpret_cast<uint16_t*>(block.data + 2));

			if (maximum != CONSTANT_BLOCKSIZE || minimum != CONSTANT_BLOCKSIZE)
			{
				ZONETOOL_WARNING(
					"Stream must have a constant blocksize of 1024! (was min: %i, max: %i)",
					minimum, maximum);
			}
		}

		std::string convert_flac(std::string& data, RiffAudioInfo* info)
		{
			const auto start_pos = data.data();
			const auto end_pos = start_pos + data.size();

			if (!check_signature(start_pos))
			{
				ZONETOOL_FATAL("File is not a flac file");
			}

			auto pos = start_pos;
			pos += 4; // skip "fLaC"

			metadata_block_t block{};

			auto num_blocks = 0;
			auto has_seektable = false;
			auto has_application = false;

			while (!block.header.is_last && pos < end_pos)
			{
				parse_metadata_block(pos, &block);
				num_blocks++;

				if (!block.header.is_last)
				{
					pos = block.data + block.header.length;
				}

				if (block.header.type == block_type_t::application)
				{
					const auto application_id = std::string{block.data, 4};
					if (application_id == APPLICATION_ID)
					{
						has_application = true;
					}
				}

				if (block.header.type == block_type_t::streaminfo)
				{
					verify_streaminfo_block(block);

					utils::bit_buffer buffer({block.data, static_cast<size_t>(block.header.length)});

					info->sampleRate = buffer.read_bits(80, 20);
					info->channels = static_cast<char>(buffer.read_bits(100, 3) + 1);
					info->numBits = static_cast<char>(buffer.read_bits(103, 5)) + 1; // bps
					info->numSamples = buffer.read_bits(108, 36);
				}

				if (block.header.type == block_type_t::seektable)
				{
					has_seektable = true;
				}
			}

			const auto frame_section_size = (start_pos + data.size()) - (block.data + block.header.length);
			auto insert_pos = static_cast<size_t>(pos - start_pos);
			auto insert_header = _byteswap_ulong(0x02000008); // application block, length 8

			if (num_blocks == 1)
			{
				const auto header_ptr = reinterpret_cast<uint32_t*>(block.start);
				const auto header = _byteswap_ulong(*header_ptr);
				*header_ptr = _byteswap_ulong((header << 1) >> 1);

				insert_header = _byteswap_ulong(0x82000008);
				insert_pos = static_cast<size_t>(block.data - start_pos + block.header.length);
			}

			std::string insert_data{};

			if (!has_seektable)
			{
				const auto seektable_ = _byteswap_ulong(0x03000000);
				insert_data.append(reinterpret_cast<const char*>(&seektable_), 4);
			}

			if (!has_application)
			{
				insert_data.append(reinterpret_cast<const char*>(&insert_header), 4); // header
				insert_data.append(APPLICATION_ID); // data (application id)
				insert_data.append(reinterpret_cast<const char*>(&frame_section_size), 4); // data (frame section size)
			}

			std::string new_data = data;
			new_data.insert(insert_pos, insert_data);
			return new_data;
		}

	}

	LoadedSound* ILoadedSound::parse_flac(const std::string& name, ZoneMemory* mem)
	{
		ZONETOOL_INFO("Parsing loaded_sound \"%s\"...", name.data());

		const auto path = "loaded_sound\\"s + name + ".flac";
		filesystem::file file(path);
		file.open("rb");

		auto* result = mem->Alloc<LoadedSound>();
		result->name = mem->StrDup(name);

		const auto size = static_cast<int>(file.size());
		const auto data = mem->Alloc<char>(size);
		file.read(data, size, 1);

		result->sound.format.blockAlign = 0;
		result->sound.format.format = SND_FORMAT_FLAC;

		std::string data_str{data, static_cast<size_t>(size)};
		const auto converted = convert_flac(data_str, &result->sound.format);

		result->sound.data = mem->Alloc<char>(converted.size());
		std::memcpy(result->sound.data, converted.data(), converted.size());
		result->sound.loadedSize = static_cast<int>(converted.size());
		result->sound.format.dataByteCount = result->sound.loadedSize;

		file.close();
		return result;
	}

	LoadedSound* ILoadedSound::parse_wav(const std::string& name, ZoneMemory* mem)
	{
		ZONETOOL_INFO("Parsing loaded_sound \"%s\"...", name.data());

		const auto path = "loaded_sound\\"s + name + ".wav";
		filesystem::file file(path);
		file.open("rb");

		auto* fp = file.get_fp();
		auto* result = mem->Alloc<LoadedSound>();

		unsigned int chunkIDBuffer;
		unsigned int chunkSize;

		fread(&chunkIDBuffer, 4, 1, fp);
		if (chunkIDBuffer != 0x46464952) // RIFF
		{
			ZONETOOL_FATAL("%s: Invalid RIFF Header.", name.data());
		}

		fread(&chunkSize, 4, 1, fp);
		fread(&chunkIDBuffer, 4, 1, fp);

		if (chunkIDBuffer != 0x45564157) // WAVE
		{
			ZONETOOL_FATAL("%s: Invalid WAVE Header.", name.data());
		}

		while (!result->sound.data && !feof(fp))
		{
			fread(&chunkIDBuffer, 4, 1, fp);
			fread(&chunkSize, 4, 1, fp);
			switch (chunkIDBuffer)
			{
			case 0x20746D66: // fmt
				if (chunkSize >= 16)
				{
					short format;
					fread(&format, 2, 1, fp);
					if (format != 1 && format != 17)
					{
						ZONETOOL_FATAL("%s: Invalid wave format %i.", name.data(), format);
					}

					short numChannels;
					fread(&numChannels, 2, 1, fp);
					result->sound.format.channels = static_cast<char>(numChannels);

					int sampleRate;
					fread(&sampleRate, 4, 1, fp);
					result->sound.format.sampleRate = sampleRate;

					int byteRate;
					fread(&byteRate, 4, 1, fp);

					short blockAlign;
					fread(&blockAlign, 2, 1, fp);
					result->sound.format.blockAlign = static_cast<char>(blockAlign);

					short bitPerSample;
					fread(&bitPerSample, 2, 1, fp);
					result->sound.format.numBits = static_cast<char>(bitPerSample);

					if (chunkSize > 16)
					{
						fseek(fp, chunkSize - 16, SEEK_CUR);
					}
				}
				break;

			case 0x61746164: // data
				result->sound.data = mem->Alloc<char>(chunkSize);
				fread(result->sound.data, 1, chunkSize, fp);

				result->sound.loadedSize = chunkSize;
				result->sound.format.dataByteCount = result->sound.loadedSize;

				result->sound.format.numSamples = result->sound.format.dataByteCount / (result->sound.format.channels * result->sound.format.numBits / 8);
				break;

			default:
				if (chunkSize > 0)
				{
					fseek(fp, chunkSize, SEEK_CUR);
				}
				break;
			}
		}

		if (!result->sound.data)
		{
			ZONETOOL_FATAL("%s: Could not read sounddata.", name.data());
		}

		result->sound.format.format = SND_FORMAT_PCM;

		result->name = mem->StrDup(name);

		file.close();
		return result;
	}

	LoadedSound* ILoadedSound::parse(const std::string& name, ZoneMemory* mem)
	{
		auto path = "loaded_sound\\"s + name;
		if (filesystem::file(path).exists())
		{
			auto fpath = std::filesystem::path(path);
			if (fpath.has_extension())
			{
				auto ext = fpath.extension();
				if (ext == ".wav")
				{
					return parse_wav(name.substr(0, name.length() - 4), mem);
				}
				else if (ext == ".flac")
				{
					return parse_flac(name.substr(0, name.length() - 5), mem);
				}
			}
		}
		if (filesystem::file(path + ".wav").exists())
		{
			return parse_wav(name, mem);
		}
		else if (filesystem::file(path + ".flac").exists())
		{
			return parse_flac(name, mem);
		}

		return nullptr;
	}

	void ILoadedSound::init(const std::string& name, ZoneMemory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->Alloc<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->StrDup(name);
			return;
		}

		this->asset_ = parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = DB_FindXAssetHeader_Safe(XAssetType(this->type()), this->name_.data()).loadSnd;
		}
	}

	void ILoadedSound::prepare(ZoneBuffer* buf, ZoneMemory* mem)
	{
	}

	void ILoadedSound::load_depending(IZone* zone)
	{
	}

	std::string ILoadedSound::name()
	{
		return this->name_;
	}

	std::int32_t ILoadedSound::type()
	{
		return ASSET_TYPE_LOADED_SOUND;
	}

	void ILoadedSound::write(IZone* zone, ZoneBuffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(3);

		dest->name = buf->write_str(this->name());

		buf->push_stream(0);

		if (data->sound.data)
		{
			buf->align(0);
			buf->write(data->sound.data, data->sound.loadedSize);
			ZoneBuffer::clear_pointer(&dest->sound.data);
		}

		buf->pop_stream();

		if (data->sound.seekTable)
		{
			buf->align(0);
			buf->write(data->sound.seekTable, data->sound.seekTableSize);
			ZoneBuffer::clear_pointer(&dest->sound.seekTable);
		}

		buf->pop_stream();
	}

	void ILoadedSound::dump_data(LoadedSound* asset, char* data, size_t size, const std::string& extension)
	{
		const auto path = "loaded_sound\\"s + asset->name + extension;
		auto file = filesystem::file(path);
		file.open("wb");

		file.write(data, size, 1);

		file.close();
	}

	void ILoadedSound::dump(LoadedSound* asset)
	{
		if (asset)
		{
			if (asset->sound.loadedSize > 4)
			{
				if (!strncmp(asset->sound.data, "fLaC", 4))
				{
					// dump flac data from zone
					dump_data(asset, asset->sound.data, asset->sound.loadedSize, ".flac");
				}
				else
				{
					// dump wav data from zone
					auto file = filesystem::file(utils::string::va("loaded_sound\\%s.wav", asset->name));

					file.open("wb");

					char chunkID[] = { 'R', 'I', 'F', 'F' };
					file.write(chunkID, 4, 1);

					// ChunkSize
					int subchunk1Size = 16;
					int subchunk2Size = asset->sound.loadedSize;
					int chunkSize = 4 + (8 + subchunk1Size) + (8 + subchunk2Size);
					file.write(&chunkSize, 4, 1);

					// Format
					char format[] = { 'W', 'A', 'V', 'E' };
					file.write(format, 4, 1);

					// --- FMT SUBCHUNK
					// Subchunk1ID
					char subchunk1ID[] = { 'f', 'm', 't', ' ' };
					file.write(subchunk1ID, 4, 1);

					// Subchunk1Size
					file.write(&subchunk1Size, 4, 1);

					// AudioFormat
					short audioFormat = asset->sound.format.format;
					file.write(&audioFormat, 2, 1);

					// NumChannels
					short numChannels = asset->sound.format.channels;
					file.write(&numChannels, 2, 1);

					// SampleRate
					int sampleRate = asset->sound.format.sampleRate;
					file.write(&sampleRate, 4, 1);

					// ByteRate
					int byteRate = asset->sound.format.sampleRate * asset->sound.format.channels * asset->sound.format.numBits / 8;
					file.write(&byteRate, 4, 1);

					// BlockAlign
					short blockAlign = asset->sound.format.blockAlign;
					file.write(&blockAlign, 2, 1);

					// BitsPerSample
					short bitsPerSample = asset->sound.format.numBits;
					file.write(&bitsPerSample, 2, 1);

					// --- DATA SUBCHUNK
						// Subchunk2ID
					char subchunk2ID[] = { 'd', 'a', 't', 'a' };
					file.write(subchunk2ID, 4, 1);

					// Subchunk2Size
					file.write(&subchunk2Size, 4, 1);

					// Data
					file.write(asset->sound.data, asset->sound.format.dataByteCount, 1);

					file.close();
				}
			}
			else
			{
				// null
				dump_data(asset, nullptr, 0);
			}
		}
	}
}