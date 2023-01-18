#pragma once

namespace zonetool::h1
{
	class Zone : public IZone
	{
	private:
		std::uintptr_t m_assetbase;
		std::string name_;
		std::vector<std::shared_ptr<IAsset>> m_assets;
		std::shared_ptr<ZoneMemory> m_zonemem;

	public:
		Zone(std::string name);
		~Zone();

		IAsset* find_asset(std::int32_t type, const std::string& name) override;

		void* get_asset_pointer(std::int32_t type, const std::string& name) override;

		void add_asset_of_type_by_pointer(std::int32_t type, void* pointer) override;

		void add_asset_of_type(std::int32_t type, const std::string& name) override;
		void add_asset_of_type(const std::string& type, const std::string& name) override;
		std::int32_t get_type_by_name(const std::string& type) override;

		void build(ZoneBuffer* buf) override;
	};
}
