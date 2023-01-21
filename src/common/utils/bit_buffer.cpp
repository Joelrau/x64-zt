#include "bit_buffer.hpp"

namespace utils
{
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
}
