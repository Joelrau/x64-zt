#pragma once

#include "zonetool/s1/structs.hpp"
#include "zonetool/h1/structs.hpp"

#include "zonetool/utils/utils.hpp"

namespace zonetool::s1
{
	namespace converter::h1
	{
		zonetool::h1::PathData* convert_path_data(PathData* asset, ZoneMemory* mem);
		zonetool::h1::FxEffectDef* convert_fx_effect_def(FxEffectDef* asset, ZoneMemory* mem);
		zonetool::h1::GfxWorld* convert_gfx_world(GfxWorld* asset, ZoneMemory* mem);
		zonetool::h1::MapEnts* convert_map_ents(MapEnts* asset, ZoneMemory* mem);
		zonetool::h1::Material* convert_material(Material* asset, ZoneMemory* mem);
		zonetool::h1::snd_alias_list_t* convert_sound(snd_alias_list_t* asset, ZoneMemory* mem);
		zonetool::h1::MaterialTechniqueSet* convert_techset(MaterialTechniqueSet* asset, ZoneMemory* mem);

		zonetool::h1::ComputeShader* convert_computeshader(ComputeShader* asset, ZoneMemory* mem);
		zonetool::h1::MaterialDomainShader* convert_domainshader(MaterialDomainShader* asset, ZoneMemory* mem);
		zonetool::h1::MaterialHullShader* convert_hullshader(MaterialHullShader* asset, ZoneMemory* mem);
		zonetool::h1::MaterialPixelShader* convert_pixelshader(MaterialPixelShader* asset, ZoneMemory* mem);
		zonetool::h1::MaterialVertexDeclaration* convert_vertexdecl(MaterialVertexDeclaration* asset, ZoneMemory* mem);
		zonetool::h1::MaterialVertexShader* convert_vertexshader(MaterialVertexShader* asset, ZoneMemory* mem);
	}
}