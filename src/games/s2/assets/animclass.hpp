#pragma once

namespace zonetool::s2
{
	class anim_class : public asset_base<AnimationClass, ASSET_TYPE_ANIMCLASS>
	{
	public:
		static void read(zone_reader& reader, AnimationClass* asset);
		AnimationClass* parse(const std::string& name, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;
		void write(zone_base* zone, zone_buffer* buf) override;
		static void dump(AnimationClass* asset);
	};
}
