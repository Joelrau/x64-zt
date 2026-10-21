#pragma once

namespace zonetool::h1::converter::s2::sound
{
	void dump(zonetool::h1::snd_alias_list_t* asset);
	void dump_curve(zonetool::h1::SndCurve* asset);
	void dump_reverb_curve(zonetool::h1::SndCurve* asset);
}
