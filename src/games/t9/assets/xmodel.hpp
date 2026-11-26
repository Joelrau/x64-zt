#pragma once
#include "../t9.hpp"

namespace zonetool::t9
{
	std::optional<std::string> read_mesh_data(const XModelMesh* mesh);
	bool mesh_entry_fits(const XSurface* surface, const std::string& data, const XModelMeshEntry& entry);

	class xmodel
	{
	public:
		static void read(auto& reader, XModel* asset);
		static void dump(XModel* asset);

		static bool is_placeholder(const XModel* asset)
		{
			return !asset->numLods;
		}
	};

	class xmodel_mesh
	{
	public:
		static void read(auto& reader, XModelMesh* asset);

		static bool is_placeholder(const XModelMesh* asset)
		{
			return !asset->entries || !asset->surface;
		}
	};
}
