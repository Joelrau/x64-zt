#pragma once
#include "../zonetool.hpp"

namespace zonetool::h1
{
	class menu_def : public asset_interface
	{
	private:
		std::string name_;
		menuDef_t* asset_ = nullptr;

		void write_menu_supporting_data(zone_base* zone, zone_buffer* buf, ExpressionSupportingData* data);
		void write_menu_statement(zone_base* zone, zone_buffer* buf, Statement_s* data);
		void write_menu_eventhandlerset(zone_base* zone, zone_buffer* buf, MenuEventHandlerSet* data);
		void write_menu_itemkeyhandler(zone_base* zone, zone_buffer* buf, ItemKeyHandler* data);
		void write_menu_data(zone_base* zone, zone_buffer* buf, menuData_t* data);
		void write_menu_window(zone_base* zone, zone_buffer* buf, windowDef_t* data, windowDef_t* dest);
		void write_menu_item_defdata(zone_base* zone, zone_buffer* buf, itemDefData_t* data, int type);
		void write_menu_item_floatexpressions(zone_base* zone, zone_buffer* buf, ItemFloatExpression* data, int count);
		void write_menu_item(zone_base* zone, zone_buffer* buf, itemDef_t* data);

	public:
		menuDef_t* parse(std::string name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void init(void* ptr, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(menuDef_t* asset);
	};
}