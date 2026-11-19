#include <std_include.hpp>

#include "../include.hpp"
#include "fxworld.hpp"
#include "material.hpp"

#include <iw8/assets/fxworld.hpp>

namespace zonetool::iw7::converter::iw8::fxworld
{
	namespace
	{
		namespace target = zonetool::iw8;

		template <typename T, typename S>
		T* name_stub(const S* source, utils::memory::allocator& allocator)
		{
			if (!source)
			{
				return nullptr;
			}

			auto* stub = allocator.allocate<T>();
			stub->name = source->name;
			return stub;
		}

		target::Material* convert_material(const Material* handle, utils::memory::allocator& allocator)
		{
			if (!handle)
			{
				return nullptr;
			}

			auto* material = static_cast<Material*>(db_find_asset(ASSET_TYPE_MATERIAL, handle->name));
			if (!material)
			{
				add_csv_row("material", ","s + handle->name); // TODO: the glass material is not loaded
				return name_stub<target::Material>(handle, allocator);
			}

			material::dump(material);
			return name_stub<target::Material>(handle, allocator);
		}

		target::FxCombinedDef convert_effect(const FxCombinedDef& effect, utils::memory::allocator& allocator)
		{
			if (effect.type != FX_COMBINED_VFX)
			{
				return {};
			}

			return {name_stub<target::ParticleSystemDef>(effect.u.vfx, allocator)};
		}

		target::FxGlassDef* convert_defs(const FxGlassSystem& system, utils::memory::allocator& allocator)
		{
			auto* defs = allocator.allocate_array<target::FxGlassDef>(system.defCount);
			for (auto i = 0u; i < system.defCount; i++)
			{
				const auto& def = system.defs[i];
				auto& result = defs[i];
				result.halfThickness = def.halfThickness;
				std::memcpy(result.texVecs, def.texVecs, sizeof(result.texVecs));
				result.color = std::bit_cast<target::GfxColor>(def.color);
				result.material = convert_material(def.material, allocator);
				result.materialShattered = convert_material(def.materialShattered, allocator);
				result.physicsAsset = name_stub<target::PhysicsAsset>(def.physicsAsset, allocator);
				result.pieceBreakEffect = convert_effect(def.pieceBreakEffect, allocator);
				result.shatterEffect = convert_effect(def.shatterEffect, allocator);
				result.shatterSmallEffect = convert_effect(def.shatterSmallEffect, allocator);
				result.crackDecalEffect = convert_effect(def.crackDecalEffect, allocator);
				result.damagedSound.name = def.damagedSound;
				result.destroyedSound.name = def.destroyedSound;
				result.destroyedQuietSound.name = def.destroyedQuietSound;
				result.highMipRadiusInvSq = def.highMipRadiusInvSq;
				result.shatteredHighMipRadiusInvSq = def.shatteredHighMipRadiusInvSq;
				result.numCrackRings = def.numCrackRings;
			}

			return defs;
		}

		target::FxGlassInitPieceState* convert_init_pieces(const FxGlassSystem& system, utils::memory::allocator& allocator)
		{
			auto* pieces = allocator.allocate_array<target::FxGlassInitPieceState>(system.initPieceCount);
			for (auto i = 0u; i < system.initPieceCount; i++)
			{
				const auto& piece = system.initPieceStates[i];
				auto& result = pieces[i];
				result.frame = std::bit_cast<target::FxSpatialFrame>(piece.frame);
				result.radius = piece.radius;
				std::memcpy(result.texCoordOrigin, piece.texCoordOrigin, sizeof(result.texCoordOrigin));
				result.supportMask = piece.supportMask;
				result.areaX2 = piece.areaX2;
				result.defIndex = piece.defIndex;
				result.vertCount = piece.vertCount;
				result.fanDataCount = piece.fanDataCount;
			}

			return pieces;
		}

		void convert_system(const FxGlassSystem& system, target::FxGlassSystem& result, utils::memory::allocator& allocator)
		{
			result.pieceLimit = system.pieceLimit;
			result.pieceWordCount = system.pieceWordCount;
			result.cellCount = system.cellCount;
			result.firstFreePiece = system.firstFreePiece;
			result.geoDataLimit = system.geoDataLimit;
			result.piecePlaces = allocator.allocate_array<target::FxGlassPiecePlace>(system.pieceLimit);
			result.pieceStates = allocator.allocate_array<target::FxGlassPieceState>(system.pieceLimit);
			result.pieceDynamics = allocator.allocate_array<target::FxGlassPieceDynamics>(system.pieceLimit);
			result.geoData = allocator.allocate_array<target::FxGlassGeometryData>(system.geoDataLimit);
			result.isInUse = allocator.allocate_array<unsigned int>(system.pieceWordCount);
			result.cellBits = allocator.allocate_array<unsigned int>(system.pieceWordCount * system.cellCount);
			result.visData = allocator.allocate_array<unsigned char>((system.pieceLimit + 15) & ~15u);
			result.linkOrg = allocator.allocate_array<target::vec3_t>(system.pieceLimit);
			result.halfThickness = allocator.allocate_array<float>((system.pieceLimit + 3) & ~3u);
			result.initGeoDataCount = system.initGeoDataCount;
			result.initGeoData = reinterpret_cast<target::FxGlassGeometryData*>(system.initGeoData);
			result.initPieceCount = system.initPieceCount;
			result.initPieceStates = convert_init_pieces(system, allocator);
		}
	}

	void dump(FxWorld* asset)
	{
		utils::memory::allocator allocator;

		auto* converted = allocator.allocate<target::FxWorld>();
		converted->name = asset->name;
		converted->glassGlob.defCount = asset->glassSys.defCount;
		converted->glassGlob.defs = convert_defs(asset->glassSys, allocator);
		converted->glassGlob.distances = allocator.allocate_array<float>(asset->glassSys.defCount);
		convert_system(asset->glassSys, converted->glassSys, allocator); // TODO: no fx transient zone and no glassTransientLookup

		target::fx_world::dump(converted);
		add_csv_row("fx_map", asset->name);
	}
}
