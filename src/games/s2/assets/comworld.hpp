#pragma once

namespace zonetool::s2
{
	struct com_world_layout
	{
		template <typename V>
		static void visit(const V& w)
		{
			w.string(offsetof(ComWorld, name));
			w.entries(offsetof(ComWorld, primaryLights), sizeof(ComPrimaryLight),
				w.template at<unsigned int>(offsetof(ComWorld, primaryLightCount)), [](const auto& light, std::size_t)
			{
				light.string(offsetof(ComPrimaryLight, defName));
			});
			w.array(S2_ARRAY_FIELD(ComWorld, primaryLightEnvs), 1, w.template at<unsigned int>(offsetof(ComWorld, primaryLightEnvCount)));
		}
	};

	class com_world : public field_layout_asset<ComWorld, ASSET_TYPE_COMWORLD, com_world_layout>
	{
	};

	struct glass_world_layout
	{
		template <typename V>
		static void visit(const V& w)
		{
			w.string(offsetof(GlassWorld, name));
			w.entries(offsetof(GlassWorld, g_glassData), sizeof(G_GlassData), 1, [](const auto& data, std::size_t)
			{
				data.array(S2_ARRAY_FIELD(G_GlassData, glassPieces), 3, data.template at<unsigned int>(offsetof(G_GlassData, pieceCount)));
				data.entries(offsetof(G_GlassData, glassNames), sizeof(GlassDef),
					data.template at<unsigned int>(offsetof(G_GlassData, glassNameCount)), [](const auto& glass, std::size_t)
				{
					glass.string(offsetof(GlassDef, name));
					glass.script_string(offsetof(GlassDef, targetName));
					glass.array(S2_ARRAY_FIELD(GlassDef, indices), 1, glass.template at<unsigned short>(offsetof(GlassDef, indexCount)));
				});
			});
		}
	};

	class glass_world : public field_layout_asset<GlassWorld, ASSET_TYPE_GLASSWORLD, glass_world_layout>
	{
	};
}
