#pragma once

namespace zonetool::h1::converter::s2::sound
{
	void dump(zonetool::h1::snd_alias_list_t* asset);
	bool dump_used(const std::string& name);
	void dump_used(const zonetool::h1::snd_alias_list_t* alias, const char* what = "sound");
	zonetool::s2::snd_alias_list_t* reference(const zonetool::h1::snd_alias_list_t* alias, utils::memory::allocator& allocator);
	void dump_curve(zonetool::h1::SndCurve* asset);
	void dump_reverb_curve(zonetool::h1::SndCurve* asset);
	void clear();
}
