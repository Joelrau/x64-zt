#pragma once

namespace zonetool::s2
{
	class xmodel_base : public asset_base<XModelBase, ASSET_TYPE_XMODEL_BASE>
	{
	public:
		static void read(zone_reader& reader, XModelBase* asset);

		XModelBase* parse(const std::string& name, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;
		void write(zone_base* zone, zone_buffer* buf) override;
		static void dump(XModelBase* asset);
	};

	class xmodel : public asset_base<XModel, ASSET_TYPE_XMODEL>
	{
	public:
		static void read(zone_reader& reader, XModel* asset);

		XModel* parse(const std::string& name, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;
		void write(zone_base* zone, zone_buffer* buf) override;
		static void dump(XModel* asset);
	};
}
