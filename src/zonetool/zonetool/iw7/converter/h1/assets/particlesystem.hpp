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

			// v109 fx: per H1 element info kept from conversion time (the IW7 asset memory is gone by stop_dumping)
			struct elem_meta
			{
				int category = 0;            // 0 looping, 1 one-shot, 2 emission
				bool burst = false;          // IW7 burst emitter (one-shot unless the system is placed by createfx)
				int ambient_interval = 0;    // looping interval (msec) used when the system is ambient
				int ambient_count = 1;       // looping count used when the system is ambient
				float life_msec = 0.0f;      // max particle life + delay, for msecLoopingLife
			};
			const char* safe_name(const char* str);

			// v109 fx: converts an IW7 ParticleSystemDef (ASSET_TYPE_VFX) at dump time; written by fxeffectdef::flush()
			void dump(ParticleSystemDef* asset);

			// v104 vfx: number of emitters that map to an H1 element given the alive effect set (nullptr = no asset checks)
			int surviving_emitters(ParticleSystemDef* asset, const alive_set* alive, bool ambient);

			// v104 vfx: approximate IW7 particle system -> H1 FxEffectDef (one H1 element per usable emitter)
			zonetool::h1::FxEffectDef* convert(ParticleSystemDef* asset, utils::memory::allocator& allocator,
				const alive_set* alive, bool ambient, int* dropped_out, std::vector<elem_meta>* meta_out = nullptr);
		}
	}
}
