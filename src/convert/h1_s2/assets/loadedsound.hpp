#pragma once

namespace zonetool::h1::converter::s2::loaded_sound
{
	void write_audio(const std::string& name, std::string_view data);
	void dump(zonetool::h1::LoadedSound* asset);
}
