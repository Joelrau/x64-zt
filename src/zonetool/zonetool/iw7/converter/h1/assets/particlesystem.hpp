#pragma once

namespace zonetool::iw7
{
	namespace converter::h1
	{
		namespace particlesystem
		{
			using alive_set = std::unordered_set<std::string>;

			// v108 vfx: VirtualQuery based guards - IW7 particle structs are partly guessed ("maybe wrong fields"),
			//           so every pointer read from them is checked before it is dereferenced
			bool readable(const void* ptr, std::size_t size);
			const char* safe_name(const char* str);

			// v104 vfx: queues an IW7 ParticleSystemDef (ASSET_TYPE_VFX); written by fxeffectdef::flush()
			void dump(ParticleSystemDef* asset);

			// v104 vfx: queued systems (zone memory stays valid until stop_dumping)
			std::vector<ParticleSystemDef*>& queued();
			void clear_queue();

			// v104 vfx: number of emitters that map to an H1 element given the alive effect set (nullptr = no asset checks)
			int surviving_emitters(ParticleSystemDef* asset, const alive_set* alive, bool ambient);

			// v104 vfx: approximate IW7 particle system -> H1 FxEffectDef (one H1 element per usable emitter)
			zonetool::h1::FxEffectDef* convert(ParticleSystemDef* asset, utils::memory::allocator& allocator,
				const alive_set* alive, bool ambient, int* dropped_out);
		}
	}
}
