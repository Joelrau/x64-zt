#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	template <typename Walk>
	void walk_alias_list(Walk walk, SndAliasList& list)
	{
		walk.string(list.aliasName);
		if (const auto aliases = walk.array(list.head, list.count, 7))
		{
			for (auto i = 0; i < list.count; i++)
			{
				walk.string(aliases[i].aliasName);
				walk.string(aliases[i].subtitle);
				walk.string(aliases[i].secondaryAliasName);
				walk.string(aliases[i].assetFileName);
			}
		}
	}

	class sound_globals : public asset_interface
	{
	private:
		std::string name_;
		SndGlobals* asset_ = nullptr;

	public:
		SndGlobals* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read(zone_reader& reader, SndGlobals* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(SndGlobals* asset);
	};

	class sound_bank : public asset_interface
	{
	private:
		std::string name_;
		SndBankResident* asset_ = nullptr;

	public:
		SndBankResident* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read(zone_reader& reader, SndBankResident* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(SndBankResident* asset);
	};

	class sound_bank_transient : public asset_interface
	{
	private:
		std::string name_;
		SndBankTransient* asset_ = nullptr;

	public:
		SndBankTransient* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read(zone_reader& reader, SndBankTransient* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(SndBankTransient* asset);
	};

	class sfx_package : public asset_interface
	{
	private:
		std::string name_;
		WeaponSFXPackage* asset_ = nullptr;

	public:
		WeaponSFXPackage* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read(zone_reader& reader, WeaponSFXPackage* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(WeaponSFXPackage* asset);
	};
}
