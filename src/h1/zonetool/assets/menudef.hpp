#pragma once
#include "../zonetool.hpp"

namespace zonetool::h1
{
	class IMenuDef : public IAsset
	{
	private:
		std::string name_;
		menuDef_t* asset_ = nullptr;

		void write_menu_supporting_data(IZone* zone, ZoneBuffer* buf, ExpressionSupportingData* data);
		void write_menu_statement(IZone* zone, ZoneBuffer* buf, Statement_s* data);
		void write_menu_eventhandlerset(IZone* zone, ZoneBuffer* buf, MenuEventHandlerSet* data);
		void write_menu_itemkeyhandler(IZone* zone, ZoneBuffer* buf, ItemKeyHandler* data);
		void write_menu_data(IZone* zone, ZoneBuffer* buf, menuData_t* data);
		void write_menu_window(IZone* zone, ZoneBuffer* buf, windowDef_t* data, windowDef_t* dest);
		void write_menu_item_defdata(IZone* zone, ZoneBuffer* buf, itemDefData_t* data, int type);
		void write_menu_item_floatexpressions(IZone* zone, ZoneBuffer* buf, ItemFloatExpression* data, int count);
		void write_menu_item(IZone* zone, ZoneBuffer* buf, itemDef_t* data);

	public:
		menuDef_t* parse(std::string name, ZoneMemory* mem);

		void init(const std::string& name, ZoneMemory* mem) override;
		void init(void* ptr, ZoneMemory* mem) override;
		void prepare(ZoneBuffer* buf, ZoneMemory* mem) override;
		void load_depending(IZone* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		void write(IZone* zone, ZoneBuffer* buffer) override;

		static void dump(menuDef_t* asset);
	};
}