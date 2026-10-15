#pragma once

namespace zonetool::s1
{
	class zone_interface : public zone_base
	{
	private:
		std::uintptr_t m_assetbase;
		std::string name_;
		std::vector<std::shared_ptr<asset_interface>> m_assets;
		std::shared_ptr<zone_memory> m_zonemem;

	public:
		zone_interface(std::string name);
		~zone_interface();

		asset_interface* find_asset(std::int32_t type, const std::string& name) override;

		void* get_asset_pointer(std::int32_t type, const std::string& name) override;

		void add_asset_of_type_by_pointer(std::int32_t type, void* pointer) override;

		void add_asset_of_type(std::int32_t type, const std::string& name) override;
		void add_asset_of_type(const std::string& type, const std::string& name) override;
		std::int32_t get_type_by_name(const std::string& type) override;

		void build(zone_buffer* buf) override;
	};
}
