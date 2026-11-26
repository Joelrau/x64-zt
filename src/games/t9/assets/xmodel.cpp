#include <std_include.hpp>
#include "xmodel.hpp"
#include "../xpak.hpp"

#include <assets/cast_dump.hpp>
#include <utils/flags.hpp>

namespace zonetool::t9
{
	namespace
	{
		void read_surface(auto& reader, XSurface* surface)
		{
			reader.push_stream((surface->flags & XSURFACE_FLAG_STREAMED) ? XFILE_BLOCK_STREAMER : XFILE_BLOCK_MESH);
			reader.read_array(surface->meshData, 255, surface->meshDataSize);
			reader.pop_stream();

			const auto bone_count = surface->boneCounts[0] + surface->boneCounts[1];
			reader.read_array(surface->boneNames, 3, bone_count);
			reader.read_array(surface->boneRefs, 3, bone_count);
			reader.read_array(surface->boneData, 3, bone_count);
		}

		void read_surface_array(auto& reader, XSurface*& field)
		{
			if (const auto surface = reader.read_array(field, 15))
			{
				read_surface(reader, surface);
			}
		}

		void read_mesh_info(auto& reader, XModelMeshInfo* info)
		{
			reader.read_string(info->name);
			reader.read_array(info->parts, 3, info->partCount);
			reader.read_array(info->triples, 0, info->tripleCount);
			reader.read_array(info->sections, 3, info->sectionCount);
		}

		void read_material_list(auto& reader, Material**& field, const std::uint16_t count)
		{
			if (const auto materials = reader.read_array(field, 7, count))
			{
				for (auto i = 0; i < count; i++)
				{
					reader.read_asset(ASSET_TYPE_MATERIAL, materials[i]);
				}
			}
		}
	}

	void xmodel::read(auto& reader, XModel* asset)
	{
		reader.read_asset(ASSET_TYPE_XSKELETON, asset->skeleton);
		reader.read_asset(ASSET_TYPE_XCOLLISION, asset->collision);
		for (auto& mesh : asset->meshes)
		{
			reader.read_asset(ASSET_TYPE_XMODELMESH, mesh);
		}

		if (const auto sets = reader.read_array(asset->lodMaterials, 7, asset->numLods))
		{
			for (auto i = 0; i < asset->numLods; i++)
			{
				read_material_list(reader, sets[i].materials, sets[i].count);
				read_material_list(reader, sets[i].materials2, sets[i].count);
				reader.read_array(sets[i].values, 3, sets[i].count);
			}
		}

		reader.read_array(asset->extents, 3);

		if (const auto attachments = reader.read_array(asset->attachments, 7, asset->attachmentCount))
		{
			for (auto i = 0; i < asset->attachmentCount; i++)
			{
				reader.read_asset(ASSET_TYPE_XMODEL, attachments[i].model);
			}
		}
	}

	void xmodel_mesh::read(auto& reader, XModelMesh* asset)
	{
		reader.read_array(asset->extents, 3);
		read_surface_array(reader, asset->surface);

		if (const auto entries = reader.read_array(asset->entries, 7, asset->entryCount))
		{
			for (auto i = 0; i < asset->entryCount; i++)
			{
				if (const auto info = reader.read_array(entries[i].info, 7))
				{
					read_mesh_info(reader, info);
				}

				read_surface_array(reader, entries[i].surface);
				reader.read_array(entries[i].extents, 3);
			}
		}
	}

	std::optional<std::string> read_mesh_data(const XModelMesh* mesh)
	{
		const auto surface = mesh ? mesh->surface : nullptr;
		if (!surface)
		{
			return {};
		}

		if (!(surface->flags & XSURFACE_FLAG_STREAMED))
		{
			if (!surface->meshData)
			{
				return {};
			}

			return std::string(reinterpret_cast<const char*>(surface->meshData), surface->meshDataSize);
		}

		try
		{
			return xpak::read(mesh->streamKey, surface->meshDataSize);
		}
		catch (const std::exception& error)
		{
			ZONETOOL_WARNING("Mesh \"%s\" stream key 0x%llX: %s", xhash_name(mesh->name).data(), mesh->streamKey, error.what());
			return {};
		}
	}

	bool mesh_entry_fits(const XSurface* surface, const std::string& data, const XModelMeshEntry& entry)
	{
		const auto vertex_end = static_cast<std::uint64_t>(entry.vertexStart) + entry.vertCount;
		return (static_cast<std::uint64_t>(entry.indexStart) + entry.triCount * 3ull) * 2 <= surface->positionOffset
			&& surface->positionOffset + vertex_end * 12 <= data.size()
			&& surface->vertexOffset + vertex_end * sizeof(XSurfaceVertex) <= data.size();
	}

	namespace
	{
		namespace cs = zonetool::cast;

		void write_cast_mesh(cs::writer_node& model_node, const XSurface* surface, const std::string& data, const XModelMeshEntry& entry,
			const std::uint64_t material_hash, const std::string& name)
		{
			const auto indices = reinterpret_cast<const std::uint16_t*>(data.data()) + entry.indexStart;
			const auto positions = reinterpret_cast<const float(*)[3]>(data.data() + surface->positionOffset) + entry.vertexStart;
			const auto vertices = reinterpret_cast<const XSurfaceVertex*>(data.data() + surface->vertexOffset) + entry.vertexStart;

			std::vector<float> position_array, normal_array, tangent_array, uv_array;
			std::vector<std::uint32_t> color_array, face_array;
			for (auto i = 0u; i < entry.vertCount; i++)
			{
				position_array.insert(position_array.end(), positions[i], positions[i] + 3);

				float normal[3]{};
				float tangent[3]{};
				cast_dump::detail::unpack_unit_vec(vertices[i].normal, normal);
				cast_dump::detail::unpack_unit_vec(vertices[i].tangent, tangent);
				normal_array.insert(normal_array.end(), normal, normal + 3);
				tangent_array.insert(tangent_array.end(), tangent, tangent + 3);

				uv_array.push_back(half_float::half_to_float(vertices[i].texCoord[0]));
				uv_array.push_back(half_float::half_to_float(vertices[i].texCoord[1]));
				color_array.push_back(vertices[i].color);
			}

			for (auto i = 0u; i < entry.triCount; i++)
			{
				const auto face = indices + i * 3;
				face_array.insert(face_array.end(), {face[0], face[2], face[1]});
			}

			auto& mesh = model_node.add_child(cs::id_mesh);
			mesh.add_string("n", name);
			if (material_hash)
			{
				mesh.add_integer64("m", material_hash);
			}

			mesh.add_vector3_array("vp", position_array);
			mesh.add_vector3_array("vn", normal_array);
			mesh.add_vector3_array("vt", tangent_array);
			mesh.add_byte("ul", 1);
			mesh.add_vector2_array("u0", uv_array);
			mesh.add_byte("cl", 1);
			mesh.add_integer_array("c0", color_array);
			mesh.add_integer_array("f", face_array);
		}

		void dump_cast(const XModel* asset)
		{
			const auto name = xhash_name(asset->name);

			cs::writer writer;
			auto& root = writer.root();
			cast_dump::detail::add_metadata(root, name.data());

			auto lods_written = 0u;
			for (auto slot = std::size(asset->meshes); slot-- > 0;)
			{
				const auto mesh = asset->meshes[slot];
				if (!mesh || !mesh->entries || !mesh->surface)
				{
					continue;
				}

				const auto lod = lods_written;
				const auto data = read_mesh_data(mesh);
				if (!data)
				{
					ZONETOOL_WARNING("CAST XModel \"%s\": lod %u has no mesh data", name.data(), lod);
					continue;
				}

				const auto materials = slot < asset->numLods ? &asset->lodMaterials[slot] : nullptr;

				auto& model_node = root.add_child(cs::id_model);
				model_node.add_string("n", std::format("{}_lod{}", name, lod));

				for (auto i = 0u; i < mesh->entryCount; i++)
				{
					const auto& entry = mesh->entries[i];
					if (!mesh_entry_fits(mesh->surface, *data, entry))
					{
						ZONETOOL_WARNING("CAST XModel \"%s\": lod %u entry %u is out of bounds", name.data(), lod, i);
						continue;
					}

					std::uint64_t material_hash{};
					if (materials && i < materials->count && materials->materials && materials->materials[i])
					{
						auto& material_node = model_node.add_child(cs::id_material);
						material_node.add_string("n", xhash_name(materials->materials[i]->name));
						material_node.add_string("t", "pbr");
						material_hash = material_node.hash();
					}

					write_cast_mesh(model_node, mesh->surface, *data, entry, material_hash, std::format("{}_lod{}_surf{}", name, lod, i));
				}

				lods_written++;
			}

			if (!lods_written)
			{
				return;
			}

			const auto path = cast_dump::detail::cast_path("xmodel", name.data());
			if (!writer.write(path))
			{
				ZONETOOL_ERROR("CAST XModel \"%s\": cannot write \"%s\"", name.data(), path.data());
			}
		}
	}

	void xmodel::dump(XModel* asset)
	{
		if (utils::flags::has_flag("castdump"))
		{
			dump_cast(asset);
		}
	}
}

namespace zonetool::t9
{
	template void xmodel::read(zone_reader&, XModel*);
	template void xmodel_mesh::read(zone_reader&, XModelMesh*);
	template void xmodel::read(zone_writer&, XModel*);
	template void xmodel_mesh::read(zone_writer&, XModelMesh*);
}
