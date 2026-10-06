#include <std_include.hpp>

#include <game.hpp>

namespace zonetool::h1
{
	const game& definition()
	{
		static const game h1
		{
			.name = "h1",
			.display_name = "Call of Duty: Modern Warfare Remastered",
			.steam_folder = "Call of Duty Modern Warfare Remastered",
			.format =
			{
				.unsigned_magic = "S1ffu100",
				.signed_magic = "S1ff0100",
				.version = 66,
			},
			.block_count = 7,
		};

		return h1;
	}
}
