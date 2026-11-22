#include <std_include.hpp>
#include "x86_address_space.hpp"

namespace zonetool::x86
{
	namespace
	{
		constexpr std::size_t region_size = 0x100000000;
		constexpr std::size_t commit_granularity = 0x100000;
		constexpr std::size_t first_offset = 16;

		std::mutex allocation_mutex;
		std::size_t committed_size{};
		std::atomic_size_t used_size{first_offset};

		std::uint8_t* reserve_region()
		{
			const auto memory = VirtualAlloc(nullptr, region_size, MEM_RESERVE, PAGE_READWRITE);
			if (!memory)
			{
				throw std::runtime_error("cannot reserve 4 GB of address space for x86 data");
			}

			return static_cast<std::uint8_t*>(memory);
		}
	}

	std::uint8_t* address_space::base()
	{
		static const auto region = reserve_region();
		return region;
	}

	void* address_space::allocate(const std::size_t size, const std::size_t alignment)
	{
		std::lock_guard lock(allocation_mutex);

		const auto start = (used_size + alignment - 1) & ~(alignment - 1);
		const auto end = start + (size ? size : 1); // an empty array must still be an address inside the space
		if (end >= region_size)
		{
			throw std::runtime_error(std::format("x86 address space is full (0x{:X} bytes requested)", size));
		}

		if (end > committed_size)
		{
			const auto commit_end = (end + commit_granularity - 1) & ~(commit_granularity - 1);
			if (!VirtualAlloc(base() + committed_size, commit_end - committed_size, MEM_COMMIT, PAGE_READWRITE))
			{
				throw std::runtime_error(std::format("cannot commit 0x{:X} bytes of x86 data", commit_end - committed_size));
			}

			committed_size = commit_end;
		}

		used_size = end;
		return base() + start;
	}

	std::uint32_t address_space::offset_of(const void* address)
	{
		if (!contains(address))
		{
			throw std::runtime_error("address is not in the x86 address space");
		}

		return static_cast<std::uint32_t>(static_cast<const std::uint8_t*>(address) - base());
	}

	bool address_space::contains(const void* address)
	{
		const auto start = base() + first_offset;
		return address >= start && address < base() + used_size;
	}

	void address_space::reset()
	{
		std::lock_guard lock(allocation_mutex);

		if (committed_size)
		{
			VirtualFree(base(), committed_size, MEM_DECOMMIT);
		}

		committed_size = 0;
		used_size = first_offset;
	}
}
