#pragma once

namespace zonetool::t7
{
	namespace converter::h1
	{
		namespace xmodel_mesh
		{
			zonetool::h1::XModelSurfs* convert(XModelMesh* asset, utils::memory::allocator& allocator);
			void dump(XModelMesh* asset);
		}
	}
}
