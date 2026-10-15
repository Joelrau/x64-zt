#include <std_include.hpp>
#include "s2.hpp"

namespace zonetool::s2
{
	namespace
	{
		void read_sound_file(zone_reader& reader, SoundFile* file)
		{
			switch (file->type)
			{
			case SAT_LOADED:
				reader.read_asset(ASSET_TYPE_LOADED_SOUND, file->u.loadSnd);
				break;
			case SAT_STREAMED:
				loaded_sound::read_stream_info(reader, file->u.streamSnd.info);
				break;
			case SAT_PRIMED:
				reader.read_asset(ASSET_TYPE_LOADED_SOUND, file->u.primedSnd.loadedPart);
				loaded_sound::read_stream_info(reader, file->u.primedSnd.streamedPart);
				break;
			default:
				break;
			}
		}

		void read_alias(zone_reader& reader, snd_alias_t* alias)
		{
			reader.read_string(alias->aliasName);
			reader.read_string(alias->subtitle);
			reader.read_string(alias->secondaryAliasName);
			reader.read_string(alias->chainAliasName);
			reader.read_string(alias->mixerGroup);
			reader.read_string(alias->squelchName);

			if (const auto file = reader.read_single(alias->soundFile, 7))
			{
				read_sound_file(reader, file);
			}

			reader.read_asset(ASSET_TYPE_SOUND_CONTEXT, alias->sndContext);
			reader.read_asset(ASSET_TYPE_SOUND_CURVE, alias->volumeFalloffCurve);
			reader.read_asset(ASSET_TYPE_DIST_CURVE, alias->distCurve);
			reader.read_asset(ASSET_TYPE_REVERB_CURVE, alias->reverbSendCurve);

			if (const auto duck = reader.read_single(alias->duck, 7))
			{
				reader.read_string(duck->name);
			}

			for (auto& modifier : alias->parameterModifiers)
			{
				reader.read_asset(ASSET_TYPE_ALIAS_PARAMETER_MODIFIER, modifier);
			}

			reader.read_asset(ASSET_TYPE_ALIAS_COMBAT_CONE, alias->combatCone);
		}
	}

	void sound::read(zone_reader& reader, snd_alias_list_t* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto aliases = reader.read_array(asset->head, 7, asset->count))
		{
			for (auto i = 0; i < asset->count; i++)
			{
				read_alias(reader, &aliases[i]);
			}
		}

		reader.read_array(asset->sortedIndices, 0, asset->sortedIndexCount);
		reader.pop_stream();
	}

	namespace
	{
		void write_stream_info(zone_buffer* buf, const LoadedSoundStreamInfo& info, LoadedSoundStreamInfo& dest)
		{
			write_string(buf, info.name, dest.name);
			write_string(buf, info.fileName, dest.fileName);
		}

		void write_sound_file(zone_base* zone, zone_buffer* buf, SoundFile* file, SoundFile* dest)
		{
			switch (file->type)
			{
			case SAT_LOADED:
				dest->u.loadSnd = asset_reference(zone, ASSET_TYPE_LOADED_SOUND, file->u.loadSnd);
				break;
			case SAT_STREAMED:
				write_stream_info(buf, file->u.streamSnd.info, dest->u.streamSnd.info);
				break;
			case SAT_PRIMED:
				dest->u.primedSnd.loadedPart = asset_reference(zone, ASSET_TYPE_LOADED_SOUND, file->u.primedSnd.loadedPart);
				write_stream_info(buf, file->u.primedSnd.streamedPart, dest->u.primedSnd.streamedPart);
				break;
			default:
				break;
			}
		}

		void write_alias(zone_base* zone, zone_buffer* buf, snd_alias_t* alias, snd_alias_t* dest)
		{
			write_string(buf, alias->aliasName, dest->aliasName);
			write_string(buf, alias->subtitle, dest->subtitle);
			write_string(buf, alias->secondaryAliasName, dest->secondaryAliasName);
			write_string(buf, alias->chainAliasName, dest->chainAliasName);
			write_string(buf, alias->mixerGroup, dest->mixerGroup);
			write_string(buf, alias->squelchName, dest->squelchName);

			SoundFile* file{};
			dest->soundFile = buf->write_s(7, alias->soundFile, 1, sizeof(SoundFile), &file);
			if (file)
			{
				write_sound_file(zone, buf, alias->soundFile, file);
			}

			dest->sndContext = asset_reference(zone, ASSET_TYPE_SOUND_CONTEXT, alias->sndContext);
			dest->volumeFalloffCurve = asset_reference(zone, ASSET_TYPE_SOUND_CURVE, alias->volumeFalloffCurve);
			dest->distCurve = asset_reference(zone, ASSET_TYPE_DIST_CURVE, alias->distCurve);
			dest->reverbSendCurve = asset_reference(zone, ASSET_TYPE_REVERB_CURVE, alias->reverbSendCurve);

			SndAliasDuck* duck{};
			dest->duck = buf->write_s(7, alias->duck, 1, sizeof(SndAliasDuck), &duck);
			if (duck)
			{
				write_string(buf, alias->duck->name, duck->name);
			}

			for (auto i = 0u; i < std::size(alias->parameterModifiers); i++)
			{
				dest->parameterModifiers[i] = asset_reference(zone, ASSET_TYPE_ALIAS_PARAMETER_MODIFIER, alias->parameterModifiers[i]);
			}

			dest->combatCone = asset_reference(zone, ASSET_TYPE_ALIAS_COMBAT_CONE, alias->combatCone);
		}

		void dump_stream_info(assetmanager::dumper& write, const LoadedSoundStreamInfo& info)
		{
			write.dump_string(info.name);
			write.dump_string(info.fileName);
		}

		void parse_stream_info(assetmanager::reader& read, LoadedSoundStreamInfo& info)
		{
			info.name = read.read_string();
			info.fileName = read.read_string();
		}

		void dump_alias(assetmanager::dumper& write, snd_alias_t* alias)
		{
			write.dump_string(alias->aliasName);
			write.dump_string(alias->subtitle);
			write.dump_string(alias->secondaryAliasName);
			write.dump_string(alias->chainAliasName);
			write.dump_string(alias->mixerGroup);
			write.dump_string(alias->squelchName);

			write.dump_single(alias->soundFile);
			if (const auto file = alias->soundFile)
			{
				switch (file->type)
				{
				case SAT_LOADED:
					write.dump_asset(file->u.loadSnd);
					break;
				case SAT_STREAMED:
					dump_stream_info(write, file->u.streamSnd.info);
					break;
				case SAT_PRIMED:
					write.dump_asset(file->u.primedSnd.loadedPart);
					dump_stream_info(write, file->u.primedSnd.streamedPart);
					break;
				default:
					break;
				}
			}

			write.dump_asset(alias->sndContext);
			write.dump_asset(alias->volumeFalloffCurve);
			write.dump_asset(alias->distCurve);
			write.dump_asset(alias->reverbSendCurve);

			write.dump_single(alias->duck);
			if (alias->duck)
			{
				write.dump_string(alias->duck->name);
			}

			for (auto* modifier : alias->parameterModifiers)
			{
				write.dump_asset(modifier);
			}

			write.dump_asset(alias->combatCone);
		}

		void parse_alias(assetmanager::reader& read, snd_alias_t* alias)
		{
			alias->aliasName = read.read_string();
			alias->subtitle = read.read_string();
			alias->secondaryAliasName = read.read_string();
			alias->chainAliasName = read.read_string();
			alias->mixerGroup = read.read_string();
			alias->squelchName = read.read_string();

			alias->soundFile = read.read_single<SoundFile>();
			if (const auto file = alias->soundFile)
			{
				switch (file->type)
				{
				case SAT_LOADED:
					file->u.loadSnd = read.read_asset<LoadedSound>();
					break;
				case SAT_STREAMED:
					parse_stream_info(read, file->u.streamSnd.info);
					break;
				case SAT_PRIMED:
					file->u.primedSnd.loadedPart = read.read_asset<LoadedSound>();
					parse_stream_info(read, file->u.primedSnd.streamedPart);
					break;
				default:
					break;
				}
			}

			alias->sndContext = read.read_asset<SndContext>();
			alias->volumeFalloffCurve = read.read_asset<SndCurve>();
			alias->distCurve = read.read_asset<DistCurve>();
			alias->reverbSendCurve = read.read_asset<SndCurve>();

			alias->duck = read.read_single<SndAliasDuck>();
			if (alias->duck)
			{
				alias->duck->name = read.read_string();
			}

			for (auto& modifier : alias->parameterModifiers)
			{
				modifier = read.read_asset<SndAliasParameterModifier>();
			}

			alias->combatCone = read.read_asset<SndAliasCombatCone>();
		}
	}

	snd_alias_list_t* sound::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open(dump_path(ASSET_TYPE_SOUND, name.data())))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing sound \"%s\"...", name.data());

		auto* asset = read.read_single<snd_alias_list_t>();
		asset->name = read.read_string();
		asset->head = read.read_array<snd_alias_t>();
		for (auto i = 0; asset->head && i < asset->count; i++)
		{
			parse_alias(read, &asset->head[i]);
		}
		asset->sortedIndices = read.read_array<unsigned short>();
		read.close();

		return asset;
	}

	void sound::load_depending(zone_base* zone)
	{
		for (auto i = 0; this->asset_->head && i < this->asset_->count; i++)
		{
			const auto& alias = this->asset_->head[i];
			if (const auto file = alias.soundFile)
			{
				if (file->type == SAT_LOADED)
				{
					add_dependency(zone, ASSET_TYPE_LOADED_SOUND, file->u.loadSnd);
				}
				else if (file->type == SAT_PRIMED)
				{
					add_dependency(zone, ASSET_TYPE_LOADED_SOUND, file->u.primedSnd.loadedPart);
				}
			}

			add_dependency(zone, ASSET_TYPE_SOUND_CONTEXT, alias.sndContext);
			add_dependency(zone, ASSET_TYPE_SOUND_CURVE, alias.volumeFalloffCurve);
			add_dependency(zone, ASSET_TYPE_DIST_CURVE, alias.distCurve);
			add_dependency(zone, ASSET_TYPE_REVERB_CURVE, alias.reverbSendCurve);
			for (const auto* modifier : alias.parameterModifiers)
			{
				add_dependency(zone, ASSET_TYPE_ALIAS_PARAMETER_MODIFIER, modifier);
			}
			add_dependency(zone, ASSET_TYPE_ALIAS_COMBAT_CONE, alias.combatCone);
		}
	}

	void sound::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = this->write_name(buf);

		if (data->head)
		{
			buf->align(7);
			auto* aliases = buf->write(data->head, data->count);
			for (auto i = 0; i < data->count; i++)
			{
				write_alias(zone, buf, &data->head[i], &aliases[i]);
			}
			buf->clear_pointer(&dest->head);
		}

		write_inline(buf, 0, data->sortedIndices, data->sortedIndexCount, dest->sortedIndices);

		buf->pop_stream();
	}

	void sound::dump(snd_alias_list_t* asset)
	{
		assetmanager::dumper write;
		if (!write.open(dump_path(ASSET_TYPE_SOUND, asset->name)))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);
		write.dump_array(asset->head, asset->count);
		for (auto i = 0; asset->head && i < asset->count; i++)
		{
			dump_alias(write, &asset->head[i]);
		}
		write.dump_array(asset->sortedIndices, asset->sortedIndexCount);
		write.close();
	}
}
