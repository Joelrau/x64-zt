#include <std_include.hpp>
#include "localize.hpp"

namespace zonetool::h1
{
	bool ILocalize::parse_localizedstrings_json(IZone* zone, const std::string& file_name)
	{
		const auto path = "localizedstrings\\"s + file_name + ".json";
		auto file = filesystem::file(path);
		file.open("rb");

		auto* fp = file.get_fp();

		if (fp == nullptr)
		{
			return false;
		}

		const auto size = file.size();
		const auto bytes = file.read_bytes(size);
		const auto localize = json::parse(bytes);

		ZONETOOL_INFO("Parsing localizedstrings \"%s.json\"...", file_name.data());
		if (!localize.is_object())
		{
			ZONETOOL_ERROR("Localized strings json file should be an object!");
			return false;
		}

		for (const auto& [key, value] : localize.items())
		{
			const auto value_str = value.get<std::string>();

			LocalizeEntry loc{};
			loc.name = key.data();
			loc.value = value_str.data();

			auto type = zone->get_type_by_name("localize");
			if (type == -1)
			{
				ZONETOOL_ERROR("Could not translate typename localize to an integer!");
			}
			try
			{
				zone->add_asset_of_type_by_pointer(type, &loc);
			}
			catch (const std::exception& e)
			{
				ZONETOOL_FATAL("A fatal exception occured while adding localizedstring: \"%s\" from file: \"%s\", exception was: \n%s",
					key.data(), path.data(), e.what());
			}
		}

		return true;
	}

	bool ILocalize::parse_localizedstrings_file(IZone* zone, const std::string& file_name)
	{
		const auto path = "localizedstrings\\"s + file_name + ".str";
		auto file = filesystem::file(path);
		file.open("rb");

		auto* fp = file.get_fp();
		if (fp)
		{
			ZONETOOL_INFO("Parsing localizedstrings \"%s.str\"...", file_name.data());

			auto bytes = file.read_bytes(file.size());
			std::string str = std::string(bytes.begin(), bytes.end());

			std::stringstream stringstream(str);
			std::string string;

			char* data;
			size_t size = 0;

			bool failed = false;
			std::string name;
			std::string value;
			size_t line = 0;
			size_t i = 0;
			while (getline(stringstream, string))
			{
				data = string.data();
				size = string.size();
				++line;
				if (size < 2)
					continue;
				for (i = 0; i < size; i++)
				{
					if (i + 1 < size && data[i] == '/' && data[i + 1] == '/')
						break;
					if (isspace(data[i]))
						continue;
					if (data[i] >= 'A' && data[i] <= 'Z')
					{
						if (!strncmp(&data[i], "REFERENCE", 9))
						{
							i += 9;
							while (isspace(data[i]))
							{
								if (i >= size)
								{
									failed = true;
									break;
								}
								i++;
							}
							if (failed)
							{
								break;
							}

							name.clear();
							while (!isspace(data[i]))
							{
								if (i >= size)
								{
									break;
								}
								name += data[i];
								i++;
							}
							break;
						}
						if (!strncmp(&data[i], "LANG_", 5))
						{
							i += 5;
							while (data[i] != '"')
							{
								if (i >= size)
								{
									failed = true;
									break;
								}
								i++;
							}
							if (failed)
							{
								break;
							}

							i++;
							value.clear();
							while (data[i] != '"')
							{
								if (i >= size)
								{
									failed = true;
									break;
								}
								if (data[i] == '\\' && i + 1 < size)
								{
									switch (data[i + 1])
									{
									case 'n':
										value += '\n';
										i += 2;
										break;
									case 't':
										value += '\t';
										i += 2;
										break;
									default:
										value += '\\';
										i++;
										break;
									}
								}
								else
								{
									value += data[i];
									i++;
								}
							}
							break;
						}
						if (!strncmp(&data[i], "ENDMARKER", 9))
						{
							file.close();
							return true;
						}
					}
				}
				if (failed)
				{
					ZONETOOL_WARNING("\"%s\" parse failed at line: %zu index: %zu", path.data(), line, i);
					file.close();
					return false;
				}
				if (!name.empty() && !value.empty())
				{
					LocalizeEntry loc;
					loc.name = name.data();
					loc.value = value.data();

					auto type = zone->get_type_by_name("localize");
					if (type == -1)
					{
						ZONETOOL_ERROR("Could not translate typename localize to an integer!");
					}
					try
					{
						zone->add_asset_of_type_by_pointer(type, &loc);
					}
					catch (std::exception& ex)
					{
						ZONETOOL_FATAL("A fatal exception occured while adding localizedstring: \"%s\" from file: \"%s\", exception was: \n%s",
							name.data(), path.data(), ex.what());
					}
					name.clear();
					value.clear();
				}
			}
			file.close();
			return true;
		}
		return false;
	}

	LocalizeEntry* ILocalize::parse(const std::string& name, ZoneMemory* mem)
	{
		const auto path = "localizedstrings\\"s + name;
		auto file = filesystem::file(path);
		file.open("rb");

		if (file.get_fp())
		{
			ZONETOOL_INFO("Parsing localizedstring \"%s\"...", name.data());

			auto* localized = mem->Alloc<LocalizeEntry>();
			localized->name = mem->StrDup(name);

			const auto size = file.size();
			auto data = file.read_bytes(size);
			localized->value = mem->StrDup(std::string(data.begin(), data.end()));

			file.close();

			return localized;
		}

		return nullptr;
	}

	void ILocalize::init(void* asset, ZoneMemory* mem)
	{
		if (asset)
		{
			auto* m_asset = reinterpret_cast<LocalizeEntry*>(asset);

			// allocate asset in zone memory
			this->asset_ = mem->Alloc<LocalizeEntry>();
			this->asset_->name = mem->StrDup(m_asset->name);
			this->asset_->value = mem->StrDup(m_asset->value);
		}
		else
		{
			this->asset_ = DB_FindXAssetHeader_Safe(XAssetType(this->type()), this->name_.data()).localize;
		}
		this->name_ = this->asset_->name;
	}

	void ILocalize::init(const std::string& name, ZoneMemory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->Alloc<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->StrDup(name);
			return;
		}

		this->asset_ = parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = DB_FindXAssetHeader_Safe(XAssetType(this->type()), this->name_.data()).localize;
		}
	}

	void ILocalize::prepare(ZoneBuffer* buf, ZoneMemory* mem)
	{
	}

	void ILocalize::load_depending(IZone* zone)
	{
	}

	std::string ILocalize::name()
	{
		return this->name_;
	}

	std::int32_t ILocalize::type()
	{
		return ASSET_TYPE_LOCALIZE_ENTRY;
	}

	void ILocalize::write(IZone* zone, ZoneBuffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write<LocalizeEntry>(data);

		buf->push_stream(3);

		dest->value = buf->write_str(data->value);
		dest->name = buf->write_str(this->name());

		buf->pop_stream();
	}

	void ILocalize::dump(LocalizeEntry* asset)
	{
		if (asset)
		{
			const auto path = "localizedstrings\\"s + asset->name;
			auto file = filesystem::file(path);
			file.open("wb");
			file.write(asset->value, strlen(asset->value), 1);
			file.close();
		}
	}
}