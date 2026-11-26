#pragma once

#include <xfile/xfile.hpp>

namespace zonetool::s4
{
	class asset_reference : public asset_interface
	{
	private:
		std::string name_;
		std::int32_t type_;
		std::vector<std::uint8_t> data_;

	public:
		asset_reference(std::int32_t type, const std::string& name);

		void* pointer() override { return this->data_.data(); }
		bool referenced() override { return true; }
		std::string name() override { return this->name_; }
		std::int32_t type() override { return this->type_; }
		void write(zone_base* zone, zone_buffer* buf) override;
	};

	class zone_interface : public zone_base
	{
	private:
		std::uintptr_t m_assetbase;
		std::size_t m_write_index;
		std::string name_;
		std::vector<std::shared_ptr<asset_interface>> m_assets;
		std::shared_ptr<zone_memory> m_zonemem;
		zone_buffer* m_buffer{};
		std::size_t m_preload_depth{};
		std::array<std::uint32_t, 8> m_preload_positions{};
		std::uint64_t m_preload_walk_size{};

		void create_asset(std::int32_t type, const std::string& name);
		void push_preload();
		void pop_preload();

	public:
		zone_interface(std::string name);
		~zone_interface();

		asset_interface* find_asset(std::int32_t type, const std::string& name) override;

		void* get_asset_pointer(std::int32_t type, const std::string& name) override;

		void add_asset_of_type_by_pointer(std::int32_t type, void* pointer) override;
		void preload_asset_pointer(std::int32_t type) override;

		void add_asset_of_type(std::int32_t type, const std::string& name) override;
		void add_asset_of_type(const std::string& type, const std::string& name) override;
		void add_listed_asset(std::int32_t type, const std::string& name);
		std::int32_t get_type_by_name(const std::string& type) override;

		void build(zone_buffer* buf) override;
	};
}
