#pragma once

namespace zonetool
{
	constexpr std::size_t max_zone_size = 2ull * 1024 * 1024 * 1024;

	class zone_memory
	{
	private:
		LPVOID memory_pool_;
		std::size_t memory_size_;
		std::size_t mem_pos_;
		std::recursive_mutex mutex_;

	public:
		zone_memory(const zone_memory& mem) 
			: memory_pool_(mem.memory_pool_)
			  , memory_size_(mem.memory_size_)
			  , mem_pos_(mem.mem_pos_)
		{
		}

		zone_memory(const std::size_t& size)
		{
			this->mem_pos_ = 0;
			this->memory_size_ = size;
			this->memory_pool_ = VirtualAlloc(nullptr, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

			if (!this->memory_pool_)
			{
				throw std::runtime_error(std::format("cannot reserve 0x{:X} bytes of zone memory", size));
			}
		}

		void print_statistics()
		{
			printf("ZoneTool memory statistics: used %ub of ram (%fmb).\n", static_cast<unsigned int>(this->mem_pos_),
				static_cast<float>(this->mem_pos_) / 1024 / 1024);
		}

		void free()
		{
			std::lock_guard<std::recursive_mutex> g(this->mutex_);
			VirtualFree(this->memory_pool_, 0, MEM_RELEASE);

			this->memory_pool_ = nullptr;
			this->memory_size_ = 0;
			this->mem_pos_ = 0;
		}

		void clear()
		{
			memset(this->memory_pool_, 0, this->memory_size_);
			this->mem_pos_ = 0;
		}
		
		~zone_memory()
		{
			this->free();
		}

		char* duplicate_string(const char* name)
		{
			std::lock_guard<std::recursive_mutex> g(this->mutex_);

			auto len = strlen(name) + 1;
			auto pointer = this->manual_allocate<char>(len);
			memcpy(pointer, name, len);

			return pointer;
		}

		char* duplicate_string(const std::string& name)
		{
			std::lock_guard<std::recursive_mutex> g(this->mutex_);
			return this->duplicate_string(name.data());
		}

		template <typename T>
		T* allocate(std::size_t count = 1)
		{
			std::lock_guard<std::recursive_mutex> g(this->mutex_);
			return this->manual_allocate<T>(sizeof(T), count);
		}

		template <typename T>
		T* manual_allocate(std::size_t size, std::size_t count = 1)
		{
			std::lock_guard<std::recursive_mutex> g(this->mutex_);

			if (count <= 0)
			{
				return nullptr;
			}

			if (this->mem_pos_ + (size * count) > this->memory_size_)
			{
				throw std::runtime_error(std::format("zone memory is full (0x{:X}/0x{:X})",
					this->mem_pos_ + (size * count), this->memory_size_));
			}

			auto pointer = reinterpret_cast<char*>(this->memory_pool_) + this->mem_pos_;
			memset(pointer, 0, size * count);
			this->mem_pos_ += size * count;

			return reinterpret_cast<T*>(pointer);
		}
	};
}
