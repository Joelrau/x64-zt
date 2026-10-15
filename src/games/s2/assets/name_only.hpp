#pragma once

namespace zonetool::s2
{
	template <typename S, XAssetType Type>
	class name_only : public asset_base<S, Type>
	{
	public:
		static void read(zone_reader& reader, S* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			reader.pop_stream();
		}

		S* parse(const std::string& name, zone_memory* mem) override
		{
			assetmanager::reader read(mem);
			if (!read.open(std::string(type_to_string(Type)) + "\\" + name + ".bin"))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing %s \"%s\"...", type_to_string(Type), name.data());

			auto* asset = read.template read_single<S>();
			asset->name = read.read_string();
			read.close();

			return asset;
		}

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			buf->push_stream(XFILE_BLOCK_VIRTUAL);
			dest->name = this->write_name(buf);
			buf->pop_stream();
		}

		static void dump(S* asset)
		{
			assetmanager::dumper write;
			if (!write.open(std::string(type_to_string(Type)) + "\\" + asset->name + ".bin"))
			{
				return;
			}

			write.dump_single(asset);
			write.dump_string(asset->name);
			write.close();
		}
	};

	using snd_phys_preset = name_only<SndPhysPreset, ASSET_TYPE_SNDPHYSPRESET>;
	using phys_constraint = name_only<PhysConstraint, ASSET_TYPE_PHYSCONSTRAINT>;
	using vertex_decl = name_only<MaterialVertexDeclaration, ASSET_TYPE_VERTEXDECL>;
	using snd_curve = name_only<SndCurve, ASSET_TYPE_SOUND_CURVE>;
	using dist_curve = name_only<DistCurve, ASSET_TYPE_DIST_CURVE>;
	using reverb_curve = name_only<SndCurve, ASSET_TYPE_REVERB_CURVE>;
	using snd_context = name_only<SndContext, ASSET_TYPE_SOUND_CONTEXT>;
	using snd_alias_parameter_modifier = name_only<SndAliasParameterModifier, ASSET_TYPE_ALIAS_PARAMETER_MODIFIER>;
	using snd_alias_combat_cone = name_only<SndAliasCombatCone, ASSET_TYPE_ALIAS_COMBAT_CONE>;
	using snd_driver_globals = name_only<SndDriverGlobals, ASSET_TYPE_SNDDRIVER_GLOBALS>;
	using reverb_preset = name_only<ReverbPreset, ASSET_TYPE_REVERB_PRESET>;
}
