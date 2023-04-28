#include "bit_buffer.hpp"

namespace utils
{
    // big endian
    bit_buffer::bit_buffer(const std::string& buffer) 
        : buffer_(buffer)
    {
    }

    uint32_t bit_buffer::read_bits(const size_t bit_index, const size_t num_bits) 
    {
        return this->read_bits_internal(bit_index, num_bits, 0);
    }

    uint32_t bit_buffer::read_bits(const size_t num_bits)
    {
        const auto index = this->bit_index_;
        this->bit_index_ += num_bits;
        return this->read_bits_internal(index, num_bits, 0);
    }

    // little endian
    bit_buffer_le::bit_buffer_le(const std::string& buffer)
        : buffer_(buffer)
    {
    }

    uint32_t bit_buffer_le::read_bits(const unsigned int num_bits)
    {
        uint32_t data{};
        this->read_bits_internal(num_bits, &data);
        return data;
    }

    uint32_t bit_buffer_le::read_bytes(const unsigned int num_bytes)
    {
        return this->read_bits(8 * num_bytes);
    }

    uint32_t bit_buffer_le::total()
    {
        return this->current_bit_;
    }

    void bit_buffer_le::set_bit(unsigned int bit)
    {
        this->current_bit_ = bit;
    }
}
