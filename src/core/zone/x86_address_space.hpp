#pragma once

namespace zonetool::x86
{
	class address_space
	{
	public:
		static std::uint8_t* base();
		static void* allocate(std::size_t size, std::size_t alignment = 1);
		static std::uint32_t offset_of(const void* address);
		static bool contains(const void* address);
		static void reset();
	};

	template <typename T>
	struct pointer
	{
		std::uint32_t value;

		T* get() const
		{
			return this->value ? reinterpret_cast<T*>(address_space::base() + this->value) : nullptr;
		}

		operator T*() const
		{
			return this->get();
		}

		T* operator->() const
		{
			return this->get();
		}

		pointer& operator=(T* target)
		{
			this->value = target ? address_space::offset_of(target) : 0;
			return *this;
		}
	};

	static_assert(sizeof(pointer<void>) == 4);

	class allocator
	{
	public:
		template <typename T>
		T* allocate(const std::size_t count = 1)
		{
			return static_cast<T*>(address_space::allocate(sizeof(T) * count, alignof(T)));
		}

		template <typename T>
		T* allocate_array(const std::size_t count = 1)
		{
			return this->allocate<T>(count);
		}

		template <typename T>
		T* manual_allocate(const std::size_t size)
		{
			return static_cast<T*>(address_space::allocate(size, alignof(T)));
		}

		char* duplicate_string(const std::string_view text)
		{
			const auto copy = this->allocate<char>(text.size() + 1);
			std::memcpy(copy, text.data(), text.size());
			return copy;
		}
	};
}
