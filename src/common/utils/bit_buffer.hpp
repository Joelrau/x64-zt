#pragma once

#include <string>

// https://github.com/Aadeshp/BitBufferCpp/tree/master/src

namespace utils
{
    class bit_buffer 
    {
    public:
        bit_buffer(const std::string&);

        std::uint32_t read_bits(const size_t bit_index, const size_t num_bits);
        std::uint32_t read_bits(const size_t num_bits);

    private:
        std::uint32_t read_bits_internal(const std::size_t bit_index, const std::size_t num_bits, std::size_t ret) const
        {
            if (bit_index + num_bits > this->buffer_.size() * 8) 
            {
                return 0;
            }

            std::uint32_t pos = static_cast<std::uint32_t>(bit_index / 8);
            std::uint8_t bit_index_start = static_cast<std::uint8_t>(bit_index - (pos * 8));
            std::size_t bit_index_end = bit_index_start + num_bits - 1;

            if (bit_index_end >= 8) 
            {
                std::uint8_t byte = this->buffer_[pos];
                std::int32_t offset = static_cast<std::int32_t>(8 - num_bits - bit_index_start);

                if (offset < 0) 
                {
                    std::uint8_t mask = (0xFF >> bit_index_start);
                    byte &= mask;
                }
                else 
                {
                    byte >>= offset;
                }

                std::size_t bits_read = 8 - bit_index_start;
                std::size_t p = num_bits - bits_read;
                offset = 0;

                while (p < num_bits) 
                {
                    ret += static_cast<std::size_t>(((byte >> offset) & 0x01) * 
                        static_cast<std::size_t>(pow(2, static_cast<double>(p))));
                    ++p;
                    ++offset;
                }

                return read_bits_internal(bit_index + bits_read, num_bits - bits_read, ret);
            }


            std::uint8_t byte = this->buffer_[pos];
            if (bit_index_start > 0) 
            {
                std::uint8_t mask = ~(0xFF << (8 - bit_index_start));
                byte &= mask;
            }

            byte >>= (8 - num_bits - bit_index_start);
            ret += static_cast<std::uint32_t>(byte);

            return static_cast<std::uint32_t>(ret);
        }

        std::string buffer_;
        std::size_t bit_index_{};

    };

    class bit_buffer_le
    {
    public:
        bit_buffer_le(const std::string&);

        std::uint32_t read_bits(const unsigned int bit_index, const size_t num_bits);
        std::uint32_t read_bits(const unsigned int num_bits);
        std::uint32_t read_bytes(const unsigned int num_bytes);
        uint32_t total();
        void set_bit(unsigned int bit);

    private:
        void read_bits_internal(unsigned int bits, void* output)
        {
            if (bits == 0) return;
            if ((this->current_bit_ + bits) > (this->buffer_.size() * 8)) return;

            int cur_byte = this->current_bit_ >> 3;
            auto cur_out = 0;

            const char* bytes = this->buffer_.data();
            const auto output_bytes = static_cast<unsigned char*>(output);

            this->total_read_ += bits;

            while (bits > 0)
            {
                const int min_bit = (bits < 8) ? bits : 8;
                const auto this_byte = bytes[cur_byte++] & 0xFF;
                const int remain = this->current_bit_ & 7;

                if ((min_bit + remain) <= 8)
                {
                    output_bytes[cur_out] = uint8_t((0xFF >> (8 - min_bit)) & (this_byte >> remain));
                }
                else
                {
                    output_bytes[cur_out] = uint8_t(
                        (0xFF >> (8 - min_bit)) & (bytes[cur_byte] << (8 - remain)) | (this_byte >> remain));
                }

                cur_out++;
                this->current_bit_ += min_bit;
                bits -= min_bit;
            }
        }

        std::string buffer_{};
        unsigned int current_bit_ = 0;
        unsigned int total_read_ = 0;
        bool use_data_types_ = true;

    };
}
