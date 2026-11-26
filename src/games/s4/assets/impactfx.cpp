#include <std_include.hpp>
#include "impactfx.hpp"

namespace zonetool::s4
{
	namespace
	{
		template <typename Walk, std::size_t Count>
		void walk_effects(Walk walk, FxCombinedDef (&effects)[Count])
		{
			for (auto& effect : effects)
			{
				walk.asset(ASSET_TYPE_VFX, effect.particleSystemDef);
			}
		}

		template <typename Walk>
		void walk_packs(Walk walk, FxImpactPack*& field)
		{
			if (const auto packs = walk.array(field, 2, 7))
			{
				for (auto i = 0; i < 2; i++)
				{
					walk_effects(walk, packs[i].nonflesh);
					walk_effects(walk, packs[i].flesh);
				}
			}
		}

		template <typename Walk>
		void walk_impact_fx(Walk walk, FxImpactTable& asset)
		{
			if (const auto entries = walk.array(asset.table, asset.impactTypeCount, 7))
			{
				for (auto i = 0u; i < asset.impactTypeCount; i++)
				{
					walk_packs(walk, entries[i].normal);
					walk_packs(walk, entries[i].exit);
				}
			}

			walk.array(asset.genericToRegistered, asset.impactTypeCount, 3);
			walk.array(asset.registeredToGeneric, IMPACT_TYPE_REGISTERED_COUNT, 3);
		}

		template <typename Walk>
		void walk_surface_fx(Walk walk, SurfaceFxTable& asset)
		{
			if (const auto entries = walk.array(asset.table, asset.numTableEntry, 7))
			{
				for (auto i = 0; i < asset.numTableEntry; i++)
				{
					walk.asset(ASSET_TYPE_VFX, entries[i].baseEffect.particleSystemDef);
					walk_effects(walk, entries[i].surfaceEffect);
				}
			}

			walk.push_stream(XFILE_BLOCK_RUNTIME);
			walk.array(asset.mapPoolBuffer, asset.mapPoolBufferSize, 3);
			walk.pop_stream();
		}
	}

	void impact_fx::read(zone_reader& reader, FxImpactTable* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		walk_impact_fx(read_walk{reader}, *asset);
		reader.pop_stream();
	}

	void impact_fx::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_impact_fx(write_walk{zone, buf}, *dest);
		buf->pop_stream();
	}

	void impact_fx::dump(FxImpactTable* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("impactfx\\"s + asset->name + ".impactfxb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_impact_fx(dump_walk{dump}, *asset);
		dump.close();
	}

	FxImpactTable* impact_fx::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("impactfx\\"s + name + ".impactfxb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing impactfx \"%s\"...", name.data());

		auto* asset = read.read_single<FxImpactTable>();
		asset->name = read.read_string();
		walk_impact_fx(parse_walk{read}, *asset);
		read.close();
		return asset;
	}

	void impact_fx::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<FxImpactTable>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void impact_fx::load_depending(zone_base* zone)
	{
		if (this->referenced())
		{
			return;
		}

		walk_impact_fx(depend_walk{zone}, *this->asset_);
	}

	std::string impact_fx::name()
	{
		return this->name_;
	}

	std::int32_t impact_fx::type()
	{
		return ASSET_TYPE_IMPACT_FX;
	}

	void surface_fx::read(zone_reader& reader, SurfaceFxTable* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		walk_surface_fx(read_walk{reader}, *asset);
		reader.pop_stream();
	}

	void surface_fx::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_surface_fx(write_walk{zone, buf}, *dest);
		buf->pop_stream();
	}

	void surface_fx::dump(SurfaceFxTable* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("surfacefx\\"s + asset->name + ".surfacefxb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_surface_fx(dump_walk{dump}, *asset);
		dump.close();
	}

	SurfaceFxTable* surface_fx::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("surfacefx\\"s + name + ".surfacefxb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing surfacefx \"%s\"...", name.data());

		auto* asset = read.read_single<SurfaceFxTable>();
		asset->name = read.read_string();
		walk_surface_fx(parse_walk{read}, *asset);
		read.close();
		return asset;
	}

	void surface_fx::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<SurfaceFxTable>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void surface_fx::load_depending(zone_base* zone)
	{
		if (this->referenced())
		{
			return;
		}

		walk_surface_fx(depend_walk{zone}, *this->asset_);
	}

	std::string surface_fx::name()
	{
		return this->name_;
	}

	std::int32_t surface_fx::type()
	{
		return ASSET_TYPE_SURFACE_FX;
	}
}
