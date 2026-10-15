#pragma once

namespace zonetool::s2
{
	struct splines_layout
	{
		template <typename V>
		static void visit(const V& w)
		{
			w.string(offsetof(Spline, name));
			w.entries(offsetof(Spline, splines), sizeof(SplineDef), w.template at<unsigned char>(offsetof(Spline, splineCount)), [](const auto& spline, std::size_t)
			{
				spline.bytes(offsetof(SplineDef, points), 3, sizeof(SplinePoint) * spline.template at<unsigned char>(offsetof(SplineDef, pointCount)));
				spline.bytes(offsetof(SplineDef, data), 0, spline.template at<unsigned int>(offsetof(SplineDef, dataSize)));
				spline.script_string(offsetof(SplineDef, name));
			});
		}
	};

	class splines : public field_layout_asset<Spline, ASSET_TYPE_SPLINES, splines_layout>
	{
	};
}
