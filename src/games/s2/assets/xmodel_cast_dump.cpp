#include <std_include.hpp>
#include "s2.hpp"

#include <formats/cast/cast_writer.hpp>

#include <utils/string.hpp>

namespace zonetool::s2::cast_xmodel
{
	namespace
	{
		namespace cs = zonetool::cast;

		constexpr std::uint32_t bone_offset_stride = 64;
		constexpr float lod_dist_divisor = 565.087f;
		constexpr float quat_scale = 32767.0f;

		struct influence
		{
			std::uint32_t bone = 0;
			float weight = 0.0f;
		};

		struct vertex_streams
		{
			const std::uint16_t* indices;
			const float* positions;
			const std::uint32_t* colors;
			const std::uint32_t* tex_coords;
			const std::uint32_t* normals;
			const std::uint32_t* tangents;
			const XSurfaceSkinVertex* skin;
		};

		std::optional<vertex_streams> map_vertex_streams(const XSurface& surface, const std::string& data)
		{
			const auto vert_count = static_cast<std::size_t>(surface.vertCount);
			std::size_t offset = 0;

			const auto take = [&](const std::size_t size)
			{
				offset = (offset + 7) & ~7ull;
				const auto start = offset;
				offset += size;
				return data.data() + start;
			};

			vertex_streams streams{};
			streams.indices = reinterpret_cast<const std::uint16_t*>(take(surface.triCount * 6ull));
			streams.positions = reinterpret_cast<const float*>(take(vert_count * 12));
			take(vert_count * 4);
			streams.colors = reinterpret_cast<const std::uint32_t*>(take(vert_count * 4));
			streams.tex_coords = reinterpret_cast<const std::uint32_t*>(take(vert_count * 4));
			streams.normals = reinterpret_cast<const std::uint32_t*>(take(vert_count * 4));
			streams.tangents = reinterpret_cast<const std::uint32_t*>(take(vert_count * 4));

			if (surface.flags & XSURFACE_FLAG_LIGHTMAP_UV)
			{
				take(vert_count * 4);
			}

			if (surface.flags & XSURFACE_FLAG_EXTRA_STREAM)
			{
				take(vert_count * 4);
			}

			if (surface.flags & XSURFACE_FLAG_SKINNED)
			{
				streams.skin = reinterpret_cast<const XSurfaceSkinVertex*>(take(vert_count * sizeof(XSurfaceSkinVertex)));
			}

			if (offset > data.size())
			{
				return {};
			}

			return streams;
		}

		std::optional<std::string> read_vertex_data(const XSurfaceShared* shared)
		{
			if (!shared)
			{
				return {};
			}

			if (shared->flags & XSURFACE_SHARED_STREAMED)
			{
				return read_shared_stream(shared);
			}

			if (!shared->data)
			{
				return {};
			}

			return std::string(shared->data, shared->dataSize);
		}

		void unpack_unit_vec(const std::uint32_t packed, float* out)
		{
			for (auto i = 0; i < 3; i++)
			{
				out[i] = static_cast<float>((packed >> (i * 10)) & 0x3FF) / 1023.0f * 2.0f - 1.0f;
			}
		}

		void unpack_tex_coords(const std::uint32_t packed, float* out)
		{
			out[0] = half_float::half_to_float(static_cast<std::uint16_t>(packed & 0xFFFF));
			out[1] = half_float::half_to_float(static_cast<std::uint16_t>(packed >> 16));
		}

		std::vector<std::vector<influence>> read_influences(const XSurface& surface, const vertex_streams& streams)
		{
			std::vector<std::vector<influence>> result(surface.vertCount);

			if (streams.skin)
			{
				for (auto i = 0; i < surface.vertCount; i++)
				{
					const auto& skin = streams.skin[i];
					const auto count = std::clamp<std::size_t>(skin.count, 1, std::size(skin.bones));

					auto remainder = 1.0f;
					result[i].emplace_back(influence{skin.bones[0], 0.0f});
					for (std::size_t bone = 1; bone < count; bone++)
					{
						const auto weight = static_cast<float>(skin.weights[bone - 1]) / 65535.0f;
						result[i].emplace_back(influence{skin.bones[bone], weight});
						remainder -= weight;
					}
					result[i][0].weight = std::max(remainder, 0.0f);
				}

				return result;
			}

			std::size_t vertex = 0;
			for (auto list = 0; surface.rigidVertLists && list < surface.rigidVertListCount; list++)
			{
				const auto& entry = surface.rigidVertLists[list];
				const auto bone = static_cast<std::uint32_t>(entry.boneOffset / bone_offset_stride);
				for (auto i = 0; i < entry.vertCount && vertex < surface.vertCount; i++, vertex++)
				{
					result[vertex].emplace_back(influence{bone, 1.0f});
				}
			}

			for (; vertex < surface.vertCount; vertex++)
			{
				result[vertex].emplace_back(influence{0, 1.0f});
			}

			return result;
		}

		void write_skeleton(cs::writer_node& model_node, const XModelBase* base)
		{
			auto& skeleton = model_node.add_child(cs::id_skeleton);

			const auto* base_mat = reinterpret_cast<const DObjAnimMat*>(base->baseMat);
			const auto* tag_angles = reinterpret_cast<const XModelAngle*>(base->tagAngles);
			const auto* tag_positions = reinterpret_cast<const XModelTagPos*>(base->tagPositions);
			const auto bone_count = base->numBones + base->numClientBones;

			for (auto i = 0; i < bone_count; i++)
			{
				auto& bone = skeleton.add_child(cs::id_bone);

				const auto* name = base->boneNames ? SL_ConvertToString(base->boneNames[i]) : nullptr;
				bone.add_string("n", name ? name : utils::string::va("bone_%d", i));

				float position[3]{};
				float rotation[4]{0.0f, 0.0f, 0.0f, 1.0f};

				if (i < base->numRootBones || !base->parentList)
				{
					bone.add_integer32("p", 0xFFFFFFFF);
					if (base_mat)
					{
						std::memcpy(position, base_mat[i].trans, sizeof(position));
						std::memcpy(rotation, base_mat[i].quat, sizeof(rotation));
					}
				}
				else
				{
					const auto child = i - base->numRootBones;
					bone.add_integer32("p", static_cast<std::uint32_t>(i - base->parentList[child]));

					if (tag_positions && tag_angles)
					{
						position[0] = tag_positions[child].x;
						position[1] = tag_positions[child].y;
						position[2] = tag_positions[child].z;
						rotation[0] = static_cast<float>(tag_angles[child].x) / quat_scale;
						rotation[1] = static_cast<float>(tag_angles[child].y) / quat_scale;
						rotation[2] = static_cast<float>(tag_angles[child].z) / quat_scale;
						rotation[3] = static_cast<float>(tag_angles[child].base) / quat_scale;
					}
				}

				bone.add_vector3_array("lp", {position[0], position[1], position[2]});
				bone.add_vector4_array("lr", {rotation[0], rotation[1], rotation[2], rotation[3]});
			}
		}

		bool write_mesh(cs::writer_node& model_node, const XSurface& surface, const std::uint64_t material_hash, const std::string& name)
		{
			const auto data = read_vertex_data(surface.shared);
			const auto streams = data ? map_vertex_streams(surface, *data) : std::nullopt;
			if (!streams)
			{
				ZONETOOL_WARNING("CAST XModel: surface \"%s\" has no vertex data", name.data());
				return false;
			}

			auto& mesh = model_node.add_child(cs::id_mesh);
			mesh.add_string("n", name);
			if (material_hash)
			{
				mesh.add_integer64("m", material_hash);
			}

			std::vector<float> positions, normals, tangents, uvs;
			std::vector<std::uint32_t> colors;
			const auto has_colors = (surface.flags & XSURFACE_FLAG_VERTCOL_NONE) == 0;

			for (auto i = 0; i < surface.vertCount; i++)
			{
				positions.insert(positions.end(), &streams->positions[i * 3], &streams->positions[i * 3 + 3]);

				float value[3]{};
				unpack_unit_vec(streams->normals[i], value);
				normals.insert(normals.end(), std::begin(value), std::end(value));
				unpack_unit_vec(streams->tangents[i], value);
				tangents.insert(tangents.end(), std::begin(value), std::end(value));

				float coords[2]{};
				unpack_tex_coords(streams->tex_coords[i], coords);
				uvs.insert(uvs.end(), std::begin(coords), std::end(coords));

				colors.emplace_back(streams->colors[i]);
			}

			mesh.add_vector3_array("vp", positions);
			mesh.add_vector3_array("vn", normals);
			mesh.add_vector3_array("vt", tangents);
			mesh.add_byte("ul", 1);
			mesh.add_vector2_array("u0", uvs);

			if (has_colors)
			{
				mesh.add_byte("cl", 1);
				mesh.add_integer_array("c0", colors);
			}

			std::vector<std::uint32_t> faces;
			for (auto i = 0; i < surface.triCount; i++)
			{
				const auto* face = &streams->indices[i * 3];
				if (face[0] == face[1] && face[1] == face[2])
				{
					continue;
				}

				faces.insert(faces.end(), {face[0], face[2], face[1]});
			}
			mesh.add_integer_array("f", faces);

			const auto influences = read_influences(surface, *streams);
			std::size_t max_influence = 0;
			for (const auto& entry : influences)
			{
				max_influence = std::max(max_influence, entry.size());
			}

			std::vector<std::uint32_t> bones(surface.vertCount * max_influence, 0);
			std::vector<float> weights(surface.vertCount * max_influence, 0.0f);
			for (std::size_t i = 0; i < influences.size(); i++)
			{
				for (std::size_t k = 0; k < influences[i].size(); k++)
				{
					bones[i * max_influence + k] = influences[i][k].bone;
					weights[i * max_influence + k] = influences[i][k].weight;
				}
			}

			mesh.add_byte("mi", static_cast<std::uint8_t>(max_influence));
			mesh.add_integer_array("wb", bones);
			mesh.add_float_array("wv", weights);
			mesh.add_string("sm", "linear");
			return true;
		}

		float lod_distance(const XModelBase* base, const int lod)
		{
			return std::sqrt(base->lodDistSq[lod]) * lod_dist_divisor + base->radius;
		}
	}

	void dump(XModel* model)
	{
		if (!model || !model->name || (model->flags & XMODEL_FLAG_VARIANTS) || !model->base || !model->base->baseMat)
		{
			return;
		}

		const auto* base = model->base;

		try
		{
			cs::writer writer;
			auto& root = writer.root();

			auto& metadata = root.add_child(cs::id_metadata);
			metadata.add_string("a", "zonetool");
			metadata.add_string("s", model->name);
			metadata.add_string("up", "z");

			auto lods_written = 0;
			for (auto lod = 0; lod < base->numLods && lod < static_cast<int>(std::size(base->lodInfo)); lod++)
			{
				const auto& info = base->lodInfo[lod];
				if (!info.modelSurfs || !info.modelSurfs->surfs || !info.numsurfs)
				{
					continue;
				}

				auto& model_node = root.add_child(cs::id_model);
				model_node.add_string("n", utils::string::va("%s_lod%d", model->name, lod));
				if (lod + 1 < base->numLods)
				{
					model_node.add_float("ld", lod_distance(base, lod));
				}

				write_skeleton(model_node, base);

				std::vector<std::uint64_t> material_hashes(info.numsurfs, 0);
				for (auto i = 0; i < info.numsurfs; i++)
				{
					const auto index = static_cast<std::size_t>(info.surfIndex) + i;
					const auto* material = model->materialHandles && index < model->numMaterials ? model->materialHandles[index] : nullptr;
					if (!material || !material->name)
					{
						continue;
					}

					auto& material_node = model_node.add_child(cs::id_material);
					material_node.add_string("n", material->name);
					material_node.add_string("t", "pbr");
					material_hashes[i] = material_node.hash();
				}

				for (auto i = 0; i < info.numsurfs; i++)
				{
					if (!write_mesh(model_node, info.modelSurfs->surfs[i], material_hashes[i], utils::string::va("%s_lod%d_surf%d", model->name, lod, i)))
					{
						return;
					}
				}

				lods_written++;
			}

			if (!lods_written)
			{
				return;
			}

			const auto path = dump_path(ASSET_TYPE_XMODEL, model->name, ".cast");
			if (!writer.write(path))
			{
				ZONETOOL_ERROR("CAST XModel \"%s\": cannot write \"%s\"", model->name, path.data());
				return;
			}

			ZONETOOL_INFO("CAST XModel \"%s\": wrote %d lods to \"%s\"", model->name, lods_written, path.data());
		}
		catch (const std::exception& ex)
		{
			ZONETOOL_ERROR("CAST XModel \"%s\": %s", model->name, ex.what());
		}
	}
}
