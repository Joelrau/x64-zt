#pragma once

namespace zonetool::iw7
{
	namespace havok
	{
		namespace xml
		{

		}

		namespace binary
		{
			constexpr auto havok_file_ext = ".hkx";

			char* parse_havok_data(std::string path, unsigned int* size, zone_memory* mem);
			void dump_havok_data(std::string path, char* data, unsigned int size);

			template <typename T>
			hkArray<T> make_array(const std::size_t count)
			{
				return {nullptr, static_cast<int>(count), static_cast<unsigned int>(count) | HK_ARRAY_DONT_DEALLOCATE};
			}

			class packfile_writer
			{
			public:
				template <typename T>
				std::size_t add_object(const T& object, const std::string& class_name)
				{
					const auto offset = this->append(&object, sizeof(T));
					this->virtual_fixups_.emplace_back(static_cast<std::uint32_t>(offset), class_name);
					return offset;
				}

				template <typename T>
				std::size_t add_array(const std::size_t array_field, const std::vector<T>& elements)
				{
					if (elements.empty())
					{
						return 0;
					}

					const auto offset = this->append(elements.data(), elements.size() * sizeof(T));
					this->add_pointer(array_field, offset);
					return offset;
				}

				void add_string(std::size_t pointer_field, const std::string& text);
				void add_pointer(std::size_t pointer_field, std::size_t target);
				void add_object_pointer(std::size_t pointer_field, std::size_t object);

				std::string finish(const std::string& root_class_name);

			private:
				std::size_t append(const void* data, std::size_t size);

				std::string data_;
				std::vector<std::pair<std::uint32_t, std::uint32_t>> local_fixups_;
				std::vector<std::pair<std::uint32_t, std::uint32_t>> global_fixups_;
				std::vector<std::pair<std::uint32_t, std::string>> virtual_fixups_;
			};
		}
	}
}
