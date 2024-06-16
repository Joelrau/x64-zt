#include "std_include.hpp"
#include "physics_library.hpp"

#include "../common/havok.hpp"

namespace zonetool::iw7
{
	PhysicsLibrary* physics_library::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "physicslibrary\\"s + name;

		if (!name.ends_with(havok::havok_file_ext))
		{
			ZONETOOL_ERROR("physicslibrary \"%s\" does not end with havok file extension (%s)", name.data(), havok::havok_file_ext);
			return nullptr;
		}

		auto file = filesystem::file(name);
		if (!file.exists())
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing physicslibrary \"%s\"...", name.data());

		const auto size = file.size();
		auto bytes = file.read_bytes(size);
		file.close();

		auto* asset = mem->allocate<PhysicsLibrary>();
		asset->name = mem->duplicate_string(this->name());

		constexpr auto* str0 = "HavokPhysicsMaterialList";
		constexpr auto* str1 = "HavokPhysicsBodyQualityList";
		constexpr auto* str2 = "HavokPhysicsMotionPropertiesList";

		asset->isMaterialList = std::search(bytes.begin(), bytes.end(), str0, str0 + std::strlen(str0)) != bytes.end();
		asset->isBodyQualityList = std::search(bytes.begin(), bytes.end(), str1, str1 + std::strlen(str1)) != bytes.end();
		asset->isMotionPropertiesList = std::search(bytes.begin(), bytes.end(), str2, str2 + std::strlen(str2)) != bytes.end();
		asset->isGlobalTypeCompendium = false;

		bytes.clear();

		asset->havokData = havok::parse_havok_data(path, &asset->havokDataSize, mem);

		return asset;
	}

	void physics_library::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).physicsLibrary;
		}
	}

	void physics_library::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void physics_library::load_depending(zone_base* zone)
	{
	}

	std::string physics_library::name()
	{
		return this->name_;
	}

	std::int32_t physics_library::type()
	{
		return ASSET_TYPE_PHYSICSLIBRARY;
	}

	void physics_library::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->havokData)
		{
			buf->align(15);
			buf->write(data->havokData, data->havokDataSize);
			buf->clear_pointer(&dest->havokData);
		}

		buf->pop_stream();
	}

	void physics_library::dump(PhysicsLibrary* asset)
	{
		const auto path = "physicslibrary\\"s + asset->name;
		havok::dump_havok_data(path, asset->havokData, asset->havokDataSize);
	}
}