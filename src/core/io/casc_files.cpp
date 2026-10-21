#include <std_include.hpp>
#include "game_files.hpp"

#include <CascLib.h>

namespace zonetool::game_files
{
	namespace
	{
		class casc_file
		{
		public:
			casc_file(HANDLE storage, const std::string& name)
			{
				CascOpenFile(storage, name.data(), 0, CASC_OPEN_BY_NAME, &this->handle_);
			}

			~casc_file()
			{
				if (this->handle_)
				{
					CascCloseFile(this->handle_);
				}
			}

			casc_file(const casc_file&) = delete;
			casc_file& operator=(const casc_file&) = delete;

			explicit operator bool() const
			{
				return this->handle_ != nullptr;
			}

			std::optional<std::uint64_t> size() const
			{
				ULONGLONG size{};
				if (!CascGetFileSize64(this->handle_, &size))
				{
					return {};
				}

				return size;
			}

			bool read(const std::uint64_t offset, const std::size_t size, void* buffer) const
			{
				if (!CascSetFilePointer64(this->handle_, static_cast<LONGLONG>(offset), nullptr, FILE_BEGIN))
				{
					return false;
				}

				constexpr std::size_t chunk_size = 0x10000000;

				auto* output = static_cast<std::uint8_t*>(buffer);
				for (std::size_t done = 0; done < size;)
				{
					const auto chunk = static_cast<DWORD>(std::min(chunk_size, size - done));
					DWORD read{};
					if (!CascReadFile(this->handle_, output + done, chunk, &read) || read != chunk)
					{
						return false;
					}

					done += read;
				}

				return true;
			}

		private:
			HANDLE handle_{};
		};

		class casc_source final : public source
		{
		public:
			explicit casc_source(const std::filesystem::path& game_path)
				: game_path_(game_path)
			{
				if (!CascOpenStorage(game_path.string().data(), 0, &this->storage_))
				{
					throw std::runtime_error(std::format("cannot open CASC storage in {} (error {})", game_path.string(), GetCascError()));
				}

				CASC_FIND_DATA data{};
				const auto find = CascFindFirstFile(this->storage_, "*", &data, nullptr);
				if (!find)
				{
					return;
				}

				do
				{
					if (data.bFileAvailable)
					{
						this->names_.emplace(data.szFileName);
					}
				} while (CascFindNextFile(find, &data));

				CascFindClose(find);
			}

			~casc_source() override
			{
				CascCloseStorage(this->storage_);
			}

			std::string description() const override
			{
				return std::format("CASC storage {}", this->game_path_.string());
			}

			std::vector<std::string> list(const std::string_view extension) const override
			{
				std::vector<std::string> names;
				for (const auto& name : this->names_)
				{
					if (std::filesystem::path(name).extension() == extension)
					{
						names.emplace_back(name);
					}
				}

				return names;
			}

			bool exists(const std::string& name) const override
			{
				return this->names_.contains(normalize(name));
			}

			std::filesystem::path path(const std::string& name) const override
			{
				return normalize(name);
			}

			bool read(const std::string& name, std::string& data) const override
			{
				std::lock_guard _(this->mutex_);

				const casc_file file(this->storage_, normalize(name));
				const auto size = file ? file.size() : std::nullopt;
				if (!size)
				{
					return false;
				}

				data.resize(static_cast<std::size_t>(*size));
				return file.read(0, data.size(), data.data());
			}

			bool read(const std::string& name, const std::uint64_t offset, const std::size_t size, void* buffer) const override
			{
				std::lock_guard _(this->mutex_);

				const casc_file file(this->storage_, normalize(name));
				return file && file.read(offset, size, buffer);
			}

		private:
			static std::string normalize(std::string name)
			{
				std::ranges::replace(name, '/', '\\');
				return name;
			}

			std::filesystem::path game_path_;
			HANDLE storage_{};
			std::set<std::string> names_;
			mutable std::mutex mutex_;
		};
	}

	std::unique_ptr<source> open_casc(const std::filesystem::path& game_path)
	{
		return std::make_unique<casc_source>(game_path);
	}
}
