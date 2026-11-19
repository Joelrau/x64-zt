#include <std_include.hpp>
#include "iw7.hpp"
#include "havok.hpp"

namespace zonetool::iw7
{
	namespace havok
	{
		constexpr auto havok_version = "hk_2014.2.5-r1";

		static const char* get_version()
		{
			return havok_version;
		}

		namespace xml
		{

		}

		namespace binary
		{
			constexpr auto hk_magic1 = 0x57E0E057;
			constexpr auto hk_magic2 = 0x10C0C010;

			bool validate_header(hkxHeaderData* header)
			{
				if (!header)
				{
					ZONETOOL_ERROR("Pointer is null");
					return false;
				}

				if (header->magic1 != hk_magic1 || header->magic2 != hk_magic2)
				{
					ZONETOOL_ERROR("Missing packfile magic header. Is this from a binary file?");
					return false;
				}

				if (header->pointerSize != sizeof(std::uintptr_t))
				{
					ZONETOOL_ERROR("Trying to process a binary file with a different pointer size than this platform.");
					return false;
				}

				if (header->littleEndian != true)
				{
					ZONETOOL_ERROR("Trying to process a binary file with a different endian than this platform.");
					return false;
				}

				if (header->reuseBaseClassPadding != false)
				{
					ZONETOOL_ERROR("Trying to process a binary file with a different padding optimization than this platform.");
					return false;
				}

				if (header->emptyBaseClassOptimization != true)
				{
					ZONETOOL_ERROR("Trying to process a binary file with a different empty base class optimization than this platform.");
					return false;
				}

				if (header->contentsVersion[0] != -1)
				{
					if (!strcmp(header->contentsVersion, get_version()))
					{
						return true;
					}
					else
					{
						ZONETOOL_ERROR("Packfile contents are not up to date");
						return false;
					}
				}

				ZONETOOL_ERROR("Packfile file format is too old");
				return false;
			}

			char* parse_havok_data(std::string path, unsigned int* size, zone_memory* mem)
			{
				if (!path.ends_with(havok_file_ext))
				{
					path.append(havok_file_ext);
				}

				auto file = filesystem::file(path);
				if (!file.exists())
				{
					*size = 0;
					return nullptr;
				}

				file.open("rb");
				*size = static_cast<unsigned int>(file.size());
				auto bytes = file.read_bytes(file.size());
				file.close();

				if (!validate_header(reinterpret_cast<hkxHeaderData*>(bytes.data())))
				{
					*size = 0;
					return nullptr;
				}

				auto* data = mem->allocate<char>(*size);
				memcpy(data, bytes.data(), *size);

				return data;
			}

			void dump_havok_data(std::string path, char* data, unsigned int size)
			{
				if (!path.ends_with(havok_file_ext))
				{
					path.append(binary::havok_file_ext);
				}

				if (!data || !size)
				{
					return;
				}

				if (!validate_header(reinterpret_cast<hkxHeaderData*>(data)))
				{
					__debugbreak();
				}

				auto file = filesystem::file(path);
				file.open("wb");
				file.write(data, size, 1);
				file.close();
			}

			namespace
			{
				const std::unordered_map<std::string, std::uint32_t> class_signatures =
				{
					{"hkClass", 0x33D42383},
					{"hkClassMember", 0xB0EFA719},
					{"hkClassEnum", 0x8A3609CF},
					{"hkClassEnumItem", 0xCE6F8A6C},
					{"HavokPhysicsShapeList", 0xC909A395},
					{"hknpCompressedMeshShape", 0x1318CC9F},
					{"hknpCompressedMeshShapeData", 0x54FD8D57},
				};

				void pad(std::string& buffer, const char fill)
				{
					while (buffer.size() % 16)
					{
						buffer.push_back(fill);
					}
				}

				template <typename T>
				void append_value(std::string& buffer, const T& value)
				{
					buffer.append(reinterpret_cast<const char*>(&value), sizeof(T));
				}

				hkxSectionHeader make_section_header(const char* tag, const std::size_t start, const std::size_t local,
					const std::size_t global, const std::size_t virtuals, const std::size_t end)
				{
					hkxSectionHeader header{};
					std::memset(&header, 0xFF, sizeof(header));
					std::memset(header.sectionTag, 0, sizeof(header.sectionTag));
					std::strncpy(header.sectionTag, tag, sizeof(header.sectionTag));
					header.absoluteDataStart = static_cast<int>(start);
					header.localFixupsOffset = static_cast<int>(local);
					header.globalFixupsOffset = static_cast<int>(global);
					header.virtualFixupsOffset = static_cast<int>(virtuals);
					header.exportsOffset = static_cast<int>(end);
					header.importsOffset = static_cast<int>(end);
					header.endOffset = static_cast<int>(end);
					return header;
				}
			}

			packfile_reader::packfile_reader(const std::string_view bytes)
			{
				if (bytes.size() < sizeof(hkxHeaderData))
				{
					throw std::runtime_error("havok packfile: file is too small");
				}

				const auto* header = reinterpret_cast<const hkxHeaderData*>(bytes.data());
				if (!validate_header(const_cast<hkxHeaderData*>(header)))
				{
					throw std::runtime_error("havok packfile: invalid header");
				}

				if (header->numSections < 1
					|| bytes.size() < sizeof(hkxHeaderData) + static_cast<std::size_t>(header->numSections) * sizeof(hkxSectionHeader))
				{
					throw std::runtime_error("havok packfile: bad section table");
				}

				const auto* sections = reinterpret_cast<const hkxSectionHeader*>(bytes.data() + sizeof(hkxHeaderData));
				if (header->contentsSectionIndex < 0 || header->contentsSectionIndex >= header->numSections)
				{
					throw std::runtime_error("havok packfile: bad contents section");
				}

				const auto& contents = sections[header->contentsSectionIndex];
				const auto contents_start = static_cast<std::size_t>(contents.absoluteDataStart);
				const auto contents_size = static_cast<std::size_t>(contents.endOffset);
				if (contents_start + contents_size > bytes.size())
				{
					throw std::runtime_error("havok packfile: contents are outside the file");
				}

				this->data_.assign(bytes.data() + contents_start, contents_size);
				auto* data = this->data_.data();

				const auto apply_local = [&](const std::size_t relative)
				{
					return data + relative;
				};

				auto offset = contents_start + static_cast<std::size_t>(contents.localFixupsOffset);
				const auto local_end = contents_start + static_cast<std::size_t>(contents.globalFixupsOffset);
				while (offset + 8 <= local_end)
				{
					std::uint32_t source{}, target{};
					std::memcpy(&source, bytes.data() + offset, sizeof(source));
					std::memcpy(&target, bytes.data() + offset + 4, sizeof(target));
					offset += 8;
					if (source == 0xFFFFFFFF)
					{
						break;
					}

					const auto value = reinterpret_cast<std::uintptr_t>(apply_local(target));
					std::memcpy(data + source, &value, sizeof(value));
				}

				offset = contents_start + static_cast<std::size_t>(contents.globalFixupsOffset);
				const auto global_end = contents_start + static_cast<std::size_t>(contents.virtualFixupsOffset);
				while (offset + 12 <= global_end)
				{
					std::uint32_t source{}, section{}, target{};
					std::memcpy(&source, bytes.data() + offset, sizeof(source));
					std::memcpy(&section, bytes.data() + offset + 4, sizeof(section));
					std::memcpy(&target, bytes.data() + offset + 8, sizeof(target));
					offset += 12;
					if (source == 0xFFFFFFFF)
					{
						break;
					}

					std::uintptr_t value{};
					if (section == static_cast<std::uint32_t>(header->contentsSectionIndex))
					{
						value = reinterpret_cast<std::uintptr_t>(apply_local(target));
					}

					std::memcpy(data + source, &value, sizeof(value));
				}

				if (header->contentsClassNameSectionIndex >= 0 && header->contentsClassNameSectionIndex < header->numSections)
				{
					const auto& names = sections[header->contentsClassNameSectionIndex];
					const auto name_offset = static_cast<std::size_t>(names.absoluteDataStart)
						+ static_cast<std::size_t>(header->contentsClassNameSectionOffset);
					if (name_offset < bytes.size())
					{
						this->root_class_name_ = std::string(bytes.data() + name_offset);
					}
				}
			}

			void packfile_writer::add_string(const std::size_t pointer_field, const std::string& text)
			{
				const auto offset = this->append(text.data(), text.size() + 1);
				this->add_pointer(pointer_field, offset);
			}

			void packfile_writer::add_pointer(const std::size_t pointer_field, const std::size_t target)
			{
				this->local_fixups_.emplace_back(static_cast<std::uint32_t>(pointer_field), static_cast<std::uint32_t>(target));
			}

			void packfile_writer::add_object_pointer(const std::size_t pointer_field, const std::size_t object)
			{
				this->global_fixups_.emplace_back(static_cast<std::uint32_t>(pointer_field), static_cast<std::uint32_t>(object));
			}

			std::size_t packfile_writer::append(const void* data, const std::size_t size)
			{
				pad(this->data_, 0);
				const auto offset = this->data_.size();
				this->data_.append(static_cast<const char*>(data), size);
				return offset;
			}

			std::string packfile_writer::finish(const std::string& root_class_name)
			{
				std::string class_names;
				std::unordered_map<std::string, std::uint32_t> class_name_offsets;
				const auto add_class_name = [&](const std::string& name)
				{
					if (class_name_offsets.contains(name))
					{
						return;
					}

					const auto signature = class_signatures.find(name);
					if (signature == class_signatures.end())
					{
						throw std::runtime_error(std::format("havok packfile: no class signature for {}", name));
					}

					append_value(class_names, signature->second);
					class_names.push_back(9);
					class_name_offsets[name] = static_cast<std::uint32_t>(class_names.size());
					class_names.append(name);
					class_names.push_back('\0');
				};

				for (const auto* name : {"hkClass", "hkClassMember", "hkClassEnum", "hkClassEnumItem"})
				{
					add_class_name(name);
				}

				for (const auto& [offset, name] : this->virtual_fixups_)
				{
					add_class_name(name);
				}

				pad(class_names, static_cast<char>(0xFF));
				pad(this->data_, 0);

				std::ranges::sort(this->local_fixups_);
				std::ranges::sort(this->global_fixups_);

				std::string local_fixups;
				for (const auto& [source, target] : this->local_fixups_)
				{
					append_value(local_fixups, source);
					append_value(local_fixups, target);
				}
				pad(local_fixups, static_cast<char>(0xFF));

				std::string global_fixups;
				for (const auto& [source, target] : this->global_fixups_)
				{
					append_value(global_fixups, source);
					append_value(global_fixups, 2u);
					append_value(global_fixups, target);
				}
				pad(global_fixups, static_cast<char>(0xFF));

				std::string virtual_fixups;
				for (const auto& [source, name] : this->virtual_fixups_)
				{
					append_value(virtual_fixups, source);
					append_value(virtual_fixups, 0u);
					append_value(virtual_fixups, class_name_offsets[name]);
				}
				pad(virtual_fixups, static_cast<char>(0xFF));

				hkxHeaderData header{};
				header.magic1 = hk_magic1;
				header.magic2 = hk_magic2;
				header.version = 11;
				header.pointerSize = sizeof(void*);
				header.littleEndian = true;
				header.reuseBaseClassPadding = false;
				header.emptyBaseClassOptimization = true;
				header.numSections = 3;
				header.contentsSectionIndex = 2;
				header.contentsClassNameSectionIndex = 0;
				header.contentsClassNameSectionOffset = static_cast<int>(class_name_offsets.at(root_class_name));
				std::memset(header.contentsVersion, 0xFF, sizeof(header.contentsVersion));
				std::memcpy(header.contentsVersion, havok_version, std::strlen(havok_version) + 1);
				header.maxpredicate = 21;

				const auto class_names_start = sizeof(hkxHeaderData) + 3 * sizeof(hkxSectionHeader);
				const auto data_start = class_names_start + class_names.size();
				const auto local_offset = this->data_.size();
				const auto global_offset = local_offset + local_fixups.size();
				const auto virtual_offset = global_offset + global_fixups.size();
				const auto end_offset = virtual_offset + virtual_fixups.size();

				std::string result;
				append_value(result, header);
				append_value(result, make_section_header("__classnames__", class_names_start,
					class_names.size(), class_names.size(), class_names.size(), class_names.size()));
				append_value(result, make_section_header("__types__", data_start, 0, 0, 0, 0));
				append_value(result, make_section_header("__data__", data_start, local_offset, global_offset, virtual_offset, end_offset));
				result.append(class_names);
				result.append(this->data_);
				result.append(local_fixups);
				result.append(global_fixups);
				result.append(virtual_fixups);
				return result;
			}
		}
	}
}