#include <std_include.hpp>
#include "vcdiff.hpp"

namespace vcdiff
{
	namespace
	{
		enum instruction_type : std::uint8_t
		{
			instruction_noop,
			instruction_add,
			instruction_run,
			instruction_copy,
		};

		enum window_flags : std::uint8_t
		{
			window_source = 0x1,
			window_target = 0x2,
			window_extra_size = 0x4,
			window_checksum = 0x8,
		};

		struct instruction
		{
			instruction_type type;
			std::uint8_t size;
			std::uint8_t mode;
		};

		struct code_table_entry
		{
			instruction first;
			instruction second;
		};

		std::array<code_table_entry, 256> make_default_code_table()
		{
			std::array<code_table_entry, 256> table{};
			std::size_t index = 0;

			table[index++] = {{instruction_run, 0, 0}};
			for (std::uint8_t size = 0; size < 18; size++)
			{
				table[index++] = {{instruction_add, size, 0}};
			}

			for (std::uint8_t mode = 0; mode < 9; mode++)
			{
				table[index++] = {{instruction_copy, 0, mode}};
				for (std::uint8_t size = 4; size < 19; size++)
				{
					table[index++] = {{instruction_copy, size, mode}};
				}
			}

			for (std::uint8_t mode = 0; mode < 6; mode++)
			{
				for (std::uint8_t add_size = 1; add_size < 5; add_size++)
				{
					for (std::uint8_t copy_size = 4; copy_size < 7; copy_size++)
					{
						table[index++] = {{instruction_add, add_size, 0}, {instruction_copy, copy_size, mode}};
					}
				}
			}

			for (std::uint8_t mode = 6; mode < 9; mode++)
			{
				for (std::uint8_t add_size = 1; add_size < 5; add_size++)
				{
					table[index++] = {{instruction_add, add_size, 0}, {instruction_copy, 4, mode}};
				}
			}

			for (std::uint8_t mode = 0; mode < 9; mode++)
			{
				table[index++] = {{instruction_copy, 4, mode}, {instruction_add, 1, 0}};
			}

			return table;
		}

		const std::array<code_table_entry, 256>& default_code_table()
		{
			static const auto table = make_default_code_table();
			return table;
		}

		class section_reader
		{
		public:
			explicit section_reader(const std::string_view data)
				: data_(data)
			{
			}

			std::uint8_t read_byte()
			{
				if (this->position_ >= this->data_.size())
				{
					throw std::runtime_error("vcdiff section ends early");
				}

				return static_cast<std::uint8_t>(this->data_[this->position_++]);
			}

			std::uint64_t read_integer()
			{
				std::uint64_t value = 0;
				while (true)
				{
					const auto byte = this->read_byte();
					value = (value << 7) | (byte & 0x7F);
					if (!(byte & 0x80))
					{
						return value;
					}
				}
			}

			std::string_view read_view(const std::size_t size)
			{
				if (this->remaining() < size)
				{
					throw std::runtime_error("vcdiff section ends early");
				}

				const auto view = this->data_.substr(this->position_, size);
				this->position_ += size;
				return view;
			}

			std::size_t remaining() const
			{
				return this->data_.size() - this->position_;
			}

			std::size_t position() const
			{
				return this->position_;
			}

			void seek(const std::size_t position)
			{
				this->position_ = position;
			}

		private:
			std::string_view data_;
			std::size_t position_{};
		};

		class address_cache
		{
		public:
			std::uint64_t decode(const std::uint8_t mode, const std::uint64_t here, section_reader& addresses)
			{
				std::uint64_t address{};
				if (mode == 0)
				{
					address = addresses.read_integer();
				}
				else if (mode == 1)
				{
					address = here - addresses.read_integer();
				}
				else if (mode < 2 + near_size)
				{
					address = this->near_[mode - 2] + addresses.read_integer();
				}
				else
				{
					address = this->same_[(mode - 2 - near_size) * 256 + addresses.read_byte()];
				}

				this->near_[this->next_near_] = address;
				this->next_near_ = (this->next_near_ + 1) % near_size;
				this->same_[address % this->same_.size()] = address;
				return address;
			}

		private:
			static constexpr std::size_t near_size = 4;

			std::array<std::uint64_t, near_size> near_{};
			std::array<std::uint64_t, 3 * 256> same_{};
			std::size_t next_near_{};
		};

		void decode_window(const std::string_view source_segment, section_reader& data, section_reader& instructions,
			section_reader& addresses, std::string& target)
		{
			address_cache cache{};
			const auto target_start = target.size();

			while (instructions.remaining())
			{
				const auto& entry = default_code_table()[instructions.read_byte()];
				for (const auto& [type, table_size, mode] : {entry.first, entry.second})
				{
					if (type == instruction_noop)
					{
						continue;
					}

					const auto size = table_size ? table_size : instructions.read_integer();
					if (type == instruction_add)
					{
						target.append(data.read_view(size));
					}
					else if (type == instruction_run)
					{
						target.append(size, static_cast<char>(data.read_byte()));
					}
					else
					{
						const auto here = source_segment.size() + (target.size() - target_start);
						const auto address = cache.decode(mode, here, addresses);
						if (address < source_segment.size())
						{
							if (address + size > source_segment.size())
							{
								throw std::runtime_error("vcdiff copy passes the end of the source segment");
							}

							target.append(source_segment.substr(address, size));
						}
						else
						{
							auto position = target_start + (address - source_segment.size());
							for (std::uint64_t i = 0; i < size; i++)
							{
								target.push_back(target[position++]);
							}
						}
					}
				}
			}
		}
	}

	std::string apply(const std::string_view delta, const std::string_view source)
	{
		section_reader reader(delta);
		if (reader.read_view(4) != "\xD6\xC3\xC4\x00"sv)
		{
			throw std::runtime_error("delta has no vcdiff magic");
		}

		if (reader.read_byte() & 0xF3)
		{
			throw std::runtime_error("vcdiff secondary compression and custom code tables are not supported");
		}

		std::string target;
		while (reader.remaining())
		{
			const auto flags = reader.read_byte();
			if (flags & window_target)
			{
				throw std::runtime_error("vcdiff target segments are not supported");
			}

			std::string_view source_segment;
			if (flags & window_source)
			{
				const auto length = reader.read_integer();
				const auto position = reader.read_integer();
				if (position + length > source.size())
				{
					throw std::runtime_error("vcdiff source segment passes the end of the source");
				}

				source_segment = source.substr(position, length);

				if (flags & window_extra_size)
				{
					reader.read_integer();
				}
			}

			const auto delta_length = reader.read_integer();
			const auto delta_start = reader.position();
			const auto target_length = reader.read_integer();
			if (reader.read_byte() != 0)
			{
				throw std::runtime_error("vcdiff compressed sections are not supported");
			}

			const auto data_length = reader.read_integer();
			const auto instructions_length = reader.read_integer();
			const auto addresses_length = reader.read_integer();

			section_reader data(reader.read_view(data_length));
			section_reader instructions(reader.read_view(instructions_length));
			section_reader addresses(reader.read_view(addresses_length));
			if (reader.position() != delta_start + delta_length)
			{
				throw std::runtime_error("vcdiff window has the wrong delta length");
			}

			const auto window_start = target.size();
			decode_window(source_segment, data, instructions, addresses, target);
			if (target.size() - window_start != target_length)
			{
				throw std::runtime_error("vcdiff window has the wrong target length");
			}

			if (flags & window_checksum)
			{
				reader.read_view(sizeof(std::uint32_t));
			}
		}

		return target;
	}
}
