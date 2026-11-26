#include <std_include.hpp>
#include "postfx.hpp"

namespace zonetool::t9
{
	namespace
	{
		void read_post_fx_data(auto& reader, PostFxData& data)
		{
			if (const auto passes = reader.read_array(data.passes, 7, data.passCount))
			{
				for (auto i = 0u; i < data.passCount; i++)
				{
					for (auto& material : passes[i].materials)
					{
						reader.read_asset(ASSET_TYPE_MATERIAL, material);
					}

					reader.read_array(passes[i].params, 7, passes[i].paramCount);
				}
			}

			if (const auto groups = reader.read_array(data.groups, 7, data.groupCount))
			{
				for (auto i = 0u; i < data.groupCount; i++)
				{
					reader.read_array(groups[i].values, 3, groups[i].valueCount);
				}
			}

			reader.read_asset(ASSET_TYPE_IMAGE, data.image);
		}
	}

	void post_fx_bundle::read(auto& reader, PostFxBundle* asset)
	{
		read_post_fx_data(reader, asset->data);
	}

	void render_override_bundle::read(auto& reader, RenderOverrideBundle* asset)
	{
		for (auto& entry : asset->entries)
		{
			read_post_fx_data(reader, entry);
		}
	}
}

namespace zonetool::t9
{
	template void post_fx_bundle::read(zone_reader&, PostFxBundle*);
	template void render_override_bundle::read(zone_reader&, RenderOverrideBundle*);
	template void post_fx_bundle::read(zone_writer&, PostFxBundle*);
	template void render_override_bundle::read(zone_writer&, RenderOverrideBundle*);
}
