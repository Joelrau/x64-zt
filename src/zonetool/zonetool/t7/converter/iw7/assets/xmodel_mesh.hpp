#pragma once

namespace zonetool::t7
{
	namespace converter::iw7
	{
		namespace xmodel_mesh
		{
			zonetool::iw7::XModelSurfs* convert(XModelMesh* asset, utils::memory::allocator& allocator);
			void dump(XModelMesh* asset);
		}
	}
}
