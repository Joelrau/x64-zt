#include <std_include.hpp>
#include "sound_globals.hpp"

#include "../common/sound.hpp"

namespace zonetool::iw7
{
	namespace vol_mod_info
	{
		const char* FIELDS[] =
		{
			"name",
			"value",
		};

		std::string get_path(SndGlobals* asset)
		{
			return sound_path_globals + "volumemods.csv";
		}

		void write_header(csv_writer& writer)
		{
			for (const auto& field : FIELDS)
			{
				writer.write_column(field);
			}
			writer.increase_line();
		}

		void write(snd_volmod_info_t* asset, csv_writer& writer)
		{
			writer.write_column(std::string(asset->name));
			writer.write_column(asset->value);

			writer.increase_line();
		}

		void dump(SndGlobals* asset)
		{
			csv_writer writer{};

			write_header(writer);

			for (auto i = 0u; i < asset->volmodinfoCount; i++)
			{
				write(&asset->volmodinfo[i], writer);
			}

			writer.save(get_path(asset));
		}
	}

	namespace ent_channel_info
	{
		const char* SND_ENTCHANNEL_FIELDS[] =
		{
			"name",
			"priority",
			"is3d",
			"restricted",
			"pausable",
			"maxvoices",
			"limitNew",
			"centerspeakerpan",
			"mpFarReverbBehavior",
			"spFarReverbBehavior",
			"cpFarReverbBehavior",
		};

		const char* IS3D[] =
		{
			"2d",
			"3d",
			"pa_speaker",
		};

		const char* RESTRICTED[] =
		{
			"unrestricted",
			"restricted",
		};

		const char* PAUSEABLE[] =
		{
			"nopause",
			"pause",
		};

		const char* LIMITBEHAVIOR[] =
		{
			"limitNew",
			"replaceQuieter",
			"replaceOldest",
			"replaceFarthest",
		};

		const char* FARREVERBBEHAVIOR[] =
		{
			"none",
			"1",
			"2",
			"3",
			"4",
			"5",
		};

		std::string get_path(SndGlobals* asset)
		{
			return sound_path_globals + "channels.csv";
		}

		void write_header(csv_writer& writer)
		{
			for (const auto& field : SND_ENTCHANNEL_FIELDS)
			{
				writer.write_column(field);
			}
			writer.increase_line();
		}

		void write(snd_entchannel_info_t* asset, csv_writer& writer)
		{
			writer.write_column(std::string(asset->name));
			writer.write_column(asset->priority);
			writer.write_column(IS3D[asset->spatialType]);
			writer.write_column(RESTRICTED[asset->isRestricted]);
			writer.write_column(PAUSEABLE[asset->isPausable]);
			writer.write_column(asset->maxVoices);
			writer.write_column(LIMITBEHAVIOR[asset->limitBehavior]);
			writer.write_column(FALSE_TRUE[asset->centerSpeakerPan]);
			writer.write_column(FARREVERBBEHAVIOR[asset->mpFarReverbBehavior]);
			writer.write_column(FARREVERBBEHAVIOR[asset->spFarReverbBehavior]);
			writer.write_column(FARREVERBBEHAVIOR[asset->cpFarReverbBehavior]);

			writer.increase_line();
		}

		void dump(SndGlobals* asset)
		{
			csv_writer writer{};

			write_header(writer);

			for (auto i = 0u; i < asset->entchannelCount; i++)
			{
				write(&asset->entchannelInfo[i], writer);
			}

			writer.save(get_path(asset));
		}
	}

	SndGlobals* sound_globals::parse(const std::string& name, zone_memory* mem)
	{
		return nullptr;
	}

	void sound_globals::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name_.data()).soundGlobals;
		}
	}

	void sound_globals::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void sound_globals::load_depending(zone_base* zone)
	{
	}

	std::string sound_globals::name()
	{
		return this->name_;
	}

	std::int32_t sound_globals::type()
	{
		return ASSET_TYPE_SOUND_GLOBALS;
	}

	void sound_globals::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		//

		buf->pop_stream();
	}

	void sound_globals::dump(SndGlobals* asset)
	{
		auto file = filesystem::file("soundglobals\\"s + asset->name + ".json"s);
		file.open("wb");

		ordered_json data;

		data["globalVolumeModifier"] = asset->globalVolumeModifier;
		data["binkVolumeModifier"] = asset->binkVolumeModifier;

		vol_mod_info::dump(asset);
		data["volmodinfo"] = vol_mod_info::get_path(asset);

		ent_channel_info::dump(asset);
		data["entchannelInfo"] = ent_channel_info::get_path(asset);

		// todo...

		file.write(data.dump(4));
		file.close();
	}
}