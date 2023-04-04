#pragma once

#include <mutex>
#include <minwindef.h>
#include <memoryapi.h>

#include <WinBase.h>
#include <WinUser.h>

#undef StrDup

namespace zonetool
{
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
				char buffer[256];
				_snprintf_s(buffer, sizeof buffer,
				          "ZoneTool just went out of memory, and has to be closed. Error code is %u (0x%08X).",
				          GetLastError(), GetLastError());

				MessageBoxA(nullptr, buffer, "ZoneTool: Out of Memory", NULL);
				std::exit(0);
			}
		}

		void free()
		{
			std::lock_guard<std::recursive_mutex> g(this->mutex_);
			VirtualFree(this->memory_pool_, 0, MEM_RELEASE);

			printf("ZoneTool memory statistics: used %ub of ram (%fmb).\n", static_cast<unsigned int>(this->mem_pos_),
				static_cast<float>(this->mem_pos_) / 1024 / 1024);

			this->memory_pool_ = nullptr;
			this->memory_size_ = 0;
			this->mem_pos_ = 0;
		}
		
		~zone_memory()
		{
			this->free();
		}

		char* duplicate_string(const char* name)
		{
			std::lock_guard<std::recursive_mutex> g(this->mutex_);

			// get string length
			auto len = strlen(name) + 1;
			auto pointer = this->manual_allocate<char>(len);
			memcpy(pointer, name, len);

			// return pointer
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
				char buffer[256];
				_snprintf_s(buffer, sizeof buffer,
					"ZoneTool just went out of memory, and has to be closed (%llu/%llu).",
					this->mem_pos_ + (size * count), this->memory_size_);

				MessageBoxA(nullptr, buffer, "ZoneTool: Out of Memory", NULL);
				std::exit(0);
			}

			// alloc pointer and zero it out
			auto pointer = reinterpret_cast<char*>(this->memory_pool_) + this->mem_pos_;
			memset(pointer, 0, size * count);
			this->mem_pos_ += size * count;

			// return pointer
			return reinterpret_cast<T*>(pointer);
		}
	};
}
