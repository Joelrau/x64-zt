#include <std_include.hpp>

#include "../include.hpp"
#include "havok.hpp"

#include <formats/havok_tagfile.hpp>
#include <iw7/common/havok.hpp>

namespace zonetool::iw8::converter::iw7::havok
{
	namespace
	{
		namespace target = zonetool::iw7;
		using tag_value = ::havok::tagfile::value;
		using target::havok::binary::make_array;
		using vec3 = std::array<float, 3>;

		template <typename T>
		std::vector<T> read_integers(const tag_value& array)
		{
			std::vector<T> result(array.size());
			for (auto i = 0u; i < result.size(); i++)
			{
				result[i] = static_cast<T>(array[i].as_int());
			}

			return result;
		}

		void read_floats(const tag_value& array, float* result, const std::size_t count)
		{
			for (auto i = 0u; i < count; i++)
			{
				result[i] = static_cast<float>(array[i].as_float());
			}
		}

		target::hkVector4 read_vector(const tag_value& vector)
		{
			float components[4]{};
			read_floats(vector, components, 4);
			return {components[0], components[1], components[2], components[3]};
		}

		target::hkAabb read_aabb(const tag_value& aabb)
		{
			return {read_vector(aabb["min"]), read_vector(aabb["max"])};
		}

		vec3 to_vec3(const target::hkVector4& vector)
		{
			return {vector.x, vector.y, vector.z};
		}

		vec3 subtract(const vec3& a, const vec3& b)
		{
			return {a[0] - b[0], a[1] - b[1], a[2] - b[2]};
		}

		vec3 cross(const vec3& a, const vec3& b)
		{
			return {a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]};
		}

		float dot(const vec3& a, const vec3& b)
		{
			return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
		}

		vec3 decode_packed_vertex(const std::uint32_t packed, const float* codec_parms)
		{
			const std::uint32_t raw[3] = {packed & 0x7FF, (packed >> 11) & 0x7FF, packed >> 22};

			vec3 position{};
			for (auto i = 0; i < 3; i++)
			{
				position[i] = codec_parms[i] + static_cast<float>(raw[i]) * codec_parms[3 + i];
			}

			return position;
		}

		vec3 decode_shared_vertex(const std::uint64_t packed, const target::hkAabb& domain)
		{
			const std::uint64_t raw[3] = {packed & 0x1FFFFF, (packed >> 21) & 0x1FFFFF, packed >> 42};
			const int bits[3] = {21, 21, 22};
			const auto min = to_vec3(domain.min);
			const auto max = to_vec3(domain.max);

			vec3 position{};
			for (auto i = 0; i < 3; i++)
			{
				position[i] = min[i] + static_cast<float>(raw[i]) * (max[i] - min[i]) / static_cast<float>(1ull << bits[i]);
			}

			return position;
		}

		std::uint64_t encode_shared_vertex(const vec3& position, const target::hkAabb& domain)
		{
			const int bits[3] = {21, 21, 22};
			const int shifts[3] = {0, 21, 42};
			const auto min = to_vec3(domain.min);
			const auto max = to_vec3(domain.max);

			std::uint64_t packed = 0;
			for (auto i = 0; i < 3; i++)
			{
				const auto steps = static_cast<double>(1ull << bits[i]);
				const auto raw = std::llround((position[i] - min[i]) / (max[i] - min[i]) * steps);
				packed |= static_cast<std::uint64_t>(std::clamp<long long>(raw, 0, (1ll << bits[i]) - 1)) << shifts[i];
			}

			return packed;
		}

		std::vector<vec3> get_convex_vertices(const tag_value& instance)
		{
			float transform[16]{};
			float scale[4]{};
			read_floats(instance["transform"], transform, 16);
			read_floats(instance["scale"], scale, 4);

			const auto vertices = instance["shape"].deref()["vertices"];

			std::vector<vec3> result;
			for (auto i = 0u; i < vertices.size(); i++)
			{
				float local[4]{};
				read_floats(vertices[i], local, 4);

				vec3 world{};
				for (auto axis = 0; axis < 3; axis++)
				{
					world[axis] = transform[axis] * local[0] * scale[0] + transform[4 + axis] * local[1] * scale[1]
						+ transform[8 + axis] * local[2] * scale[2] + transform[12 + axis];
				}

				if (std::ranges::find(result, world) == result.end())
				{
					result.emplace_back(world);
				}
			}

			return result;
		}

		bool is_flat_quad(const std::array<vec3, 4>& points)
		{
			const auto normal = cross(subtract(points[1], points[0]), subtract(points[2], points[0]));
			const auto length = std::sqrt(dot(normal, normal));
			if (length == 0.0f)
			{
				return false;
			}

			return std::abs(dot(normal, subtract(points[3], points[0]))) / length < 0.0001f;
		}

		bool is_extern_shape_primitive(const target::hkcdStaticMeshTreeBasePrimitive& primitive)
		{
			const auto* indices = primitive.indices;
			return indices[0] == indices[1] && indices[1] == indices[3] && indices[2] == 4;
		}

		std::vector<unsigned char> find_leaf_nodes(const tag_value& nodes, const std::uint32_t num_primitives)
		{
			std::vector<unsigned char> leaf_nodes(num_primitives, 0xFF);
			for (auto i = 0u; i < nodes.size() && i < 0xFF; i++)
			{
				const auto data = static_cast<std::uint32_t>(nodes[i]["data"].as_int());
				if ((data & 1) == 0 && (data >> 1) < num_primitives)
				{
					leaf_nodes[data >> 1] = static_cast<unsigned char>(i);
				}
			}

			if (std::ranges::find(leaf_nodes, 0xFF) != leaf_nodes.end())
			{
				throw std::runtime_error("havok: primitive has no leaf node in its section");
			}

			return leaf_nodes;
		}

		std::uint32_t convert_contents(const std::uint32_t contents)
		{
			static const std::pair<PhysicsContents, target::PhysicsContents> contents_map[] =
			{
				{PHYSICS_CONTENTS_SOLID, target::PHYSICS_CONTENTS_SOLID},
				{PHYSICS_CONTENTS_FOLIAGE, target::PHYSICS_CONTENTS_FOLIAGE},
				{PHYSICS_CONTENTS_VEHICLETRIGGER, target::PHYSICS_CONTENTS_VEHICLETRIGGER},
				{PHYSICS_CONTENTS_GLASS, target::PHYSICS_CONTENTS_GLASS},
				{PHYSICS_CONTENTS_WATER, target::PHYSICS_CONTENTS_WATER},
				{PHYSICS_CONTENTS_MISSILECLIP, target::PHYSICS_CONTENTS_MISSILECLIP},
				{PHYSICS_CONTENTS_ITEM, target::PHYSICS_CONTENTS_ITEM},
				{PHYSICS_CONTENTS_VEHICLECLIP, target::PHYSICS_CONTENTS_VEHICLECLIP},
				{PHYSICS_CONTENTS_ITEMCLIP, target::PHYSICS_CONTENTS_ITEMCLIP},
				{PHYSICS_CONTENTS_SKY, target::PHYSICS_CONTENTS_SKY},
				{PHYSICS_CONTENTS_AINOSIGHT, target::PHYSICS_CONTENTS_AINOSIGHT},
				{PHYSICS_CONTENTS_CLIPSHOT, target::PHYSICS_CONTENTS_CLIPSHOT},
				{PHYSICS_CONTENTS_ACTOR, target::PHYSICS_CONTENTS_ACTOR},
				{PHYSICS_CONTENTS_PLAYERCLIP, target::PHYSICS_CONTENTS_PLAYERCLIP},
				{PHYSICS_CONTENTS_AICLIP, target::PHYSICS_CONTENTS_MONSTERCLIP},
				{PHYSICS_CONTENTS_SENTIENTTRIGGER, target::PHYSICS_CONTENTS_SENTIENTTRIGGER},
				{PHYSICS_CONTENTS_EXPLOSIONCLIP, target::PHYSICS_CONTENTS_EXPLOSIONCLIP},
				{PHYSICS_CONTENTS_NONSENTIENTTRIGGER, target::PHYSICS_CONTENTS_NONSENTIENTTRIGGER},
				{PHYSICS_CONTENTS_VEHICLE, target::PHYSICS_CONTENTS_VEHICLE},
				{PHYSICS_CONTENTS_MANTLE, target::PHYSICS_CONTENTS_MANTLE},
				{PHYSICS_CONTENTS_PLAYER, target::PHYSICS_CONTENTS_PLAYER},
				{PHYSICS_CONTENTS_PLAYERTRIGGER, target::PHYSICS_CONTENTS_PLAYERTRIGGER},
			};

			std::uint32_t converted = 0;
			for (const auto& [source, result] : contents_map)
			{
				if (contents & source)
				{
					converted |= result;
				}
			}

			return converted;
		}

		target::SurfaceType convert_surface_type(const SurfaceType type)
		{
			switch (type)
			{
			case SURFACE_TYPE_METAL_HELICOPTER:
				return target::SURFACE_TYPE_METAL_THIN;
			case SURFACE_TYPE_TILE:
				return target::SURFACE_TYPE_CERAMIC;
			case SURFACE_TYPE_METAL_TANK:
				return target::SURFACE_TYPE_METAL_THICK;
			case SURFACE_TYPE_BODY_ARMOR:
				return target::SURFACE_TYPE_FLESH;
			case SURFACE_TYPE_DRYWALL:
				return target::SURFACE_TYPE_PLASTER;
			case SURFACE_TYPE_MUD_RIVERBED:
				return target::SURFACE_TYPE_MUD;
			case SURFACE_TYPE_CINDERBLOCK:
				return target::SURFACE_TYPE_CONCRETE_DRY;
			default:
				return static_cast<target::SurfaceType>(type);
			}
		}

		std::uint64_t convert_user_data(const std::uint64_t user_data)
		{
			const auto surface_flags = static_cast<std::uint32_t>(user_data);
			const auto surface_type = convert_surface_type(static_cast<SurfaceType>((surface_flags >> 19) & 0x3F));
			const auto converted_flags = (surface_flags & ~(0x3Fu << 19)) | (static_cast<std::uint32_t>(surface_type) << 19);
			return (user_data & 0xFFFFFFFF00000000) | converted_flags; // TODO: the high dword is copied, its IW8 bits are not proven
		}

		std::vector<target::HavokPhysicsShapeTagData> convert_shape_tags(const tag_value& tags, std::uint32_t& world_contents)
		{
			std::vector<target::HavokPhysicsShapeTagData> converted(tags.size());
			for (auto i = 0u; i < converted.size(); i++)
			{
				const auto tag = tags[i];
				auto& result = converted[i];
				result.collisionFilterInfo = convert_contents(static_cast<std::uint32_t>(tag["collisionFilterInfo"].as_int()));
				result.materialCRC = static_cast<int>(tag["materialCRC"].as_int());
				result.materialId = static_cast<unsigned short>(tag["materialId"].as_int());
				result.userData = convert_user_data(static_cast<std::uint64_t>(tag["userData"].as_int()));
				world_contents |= result.collisionFilterInfo;
			}

			return converted;
		}

		converted_mesh convert_primitives(const tag_value& tree, const tag_value& extern_shapes, const target::hkAabb& domain)
		{
			converted_mesh mesh{};
			mesh.shared_vertices_index = read_integers<unsigned short>(tree["sharedVerticesIndex"]);
			mesh.shared_vertices = read_integers<unsigned long long>(tree["sharedVertices"]);
			const auto packed_vertices = read_integers<unsigned int>(tree["packedVertices"]);

			const auto primitives = tree["primitives"];
			mesh.primitives.resize(primitives.size());
			for (auto i = 0u; i < mesh.primitives.size(); i++)
			{
				const auto indices = primitives[i]["indices"];
				for (auto k = 0; k < 4; k++)
				{
					mesh.primitives[i].indices[k] = static_cast<unsigned char>(indices[k].as_int());
				}
			}

			const auto quad_bits = (tree["maxKeyValue"].as_int() >> 1) + 1;
			mesh.quad_is_flat.resize((quad_bits + 31) / 32);

			const auto sections = tree["sections"];
			for (auto section_index = 0u; section_index < sections.size(); section_index++)
			{
				const auto section = sections[section_index];
				const auto num_packed = static_cast<std::uint32_t>(section["numPackedVertices"].as_int());
				const auto first_packed = static_cast<std::size_t>(section["firstPackedVertexIndex"].as_int());
				const auto first_shared = static_cast<std::size_t>(section["firstSharedVertexIndex"].as_int());
				const auto first_primitive = static_cast<std::size_t>(section["firstPrimitiveIndex"].as_int());
				const auto num_primitives = static_cast<std::uint32_t>(section["numPrimitives"].as_int());
				const auto page_start = static_cast<std::size_t>(section["page"].as_int()) * 0x10000;
				const auto leaf_nodes = find_leaf_nodes(section["nodes"], num_primitives);

				float codec_parms[6]{};
				read_floats(section["codecParms"], codec_parms, 6);

				const auto get_vertex = [&](const std::uint32_t index)
				{
					if (index < num_packed)
					{
						return decode_packed_vertex(packed_vertices.at(first_packed + index), codec_parms);
					}

					const auto shared = mesh.shared_vertices_index.at(first_shared + index - num_packed);
					return decode_shared_vertex(mesh.shared_vertices.at(page_start + shared), domain);
				};

				for (auto primitive_index = 0u; primitive_index < num_primitives; primitive_index++)
				{
					auto& primitive = mesh.primitives.at(first_primitive + primitive_index);
					auto* indices = primitive.indices;

					if (is_extern_shape_primitive(primitive))
					{
						const auto record = first_shared + indices[0] - num_packed;
						const auto vertices = get_convex_vertices(extern_shapes[mesh.shared_vertices_index.at(record + 2)]);
						const auto start = mesh.shared_vertices.size() - page_start;
						if (mesh.shared_vertices.size() < page_start || start >= 0x10000 || vertices.size() >= 0x100)
						{
							throw std::runtime_error("havok: convex shape does not fit the shared vertex page");
						}

						for (const auto& vertex : vertices)
						{
							mesh.shared_vertices.emplace_back(encode_shared_vertex(vertex, domain));
						}

						mesh.shared_vertices_index[record] = static_cast<unsigned short>((vertices.size() << 8) | target::HKNP_CUSTOM_PRIMITIVE_CONVEX);
						mesh.shared_vertices_index[record + 1] = static_cast<unsigned short>(start);
						mesh.shared_vertices_index[record + 2] = 0;
						mesh.shared_vertices_index[record + 3] = 0;
						indices[1] = indices[2] = indices[3] = leaf_nodes[primitive_index];
						mesh.num_convex_shapes++;
					}
					else if (indices[2] == indices[3])
					{
						mesh.num_triangles++;
					}
					else
					{
						mesh.num_triangles += 2;
						if (is_flat_quad({get_vertex(indices[0]), get_vertex(indices[1]), get_vertex(indices[2]), get_vertex(indices[3])}))
						{
							const auto key = (section_index << 7) | primitive_index;
							mesh.quad_is_flat[key >> 5] |= 1u << (key & 31);
						}
					}
				}
			}

			return mesh;
		}

		std::vector<target::hkcdStaticMeshTreeBaseSection> convert_sections(const tag_value& sections, const std::size_t shared_vertices_index_count)
		{
			std::vector<target::hkcdStaticMeshTreeBaseSection> converted(sections.size());
			for (auto i = 0u; i < converted.size(); i++)
			{
				const auto section = sections[i];
				const auto first_shared = static_cast<std::uint32_t>(section["firstSharedVertexIndex"].as_int());
				const auto next_shared = i + 1 < converted.size()
					? static_cast<std::uint32_t>(sections[i + 1]["firstSharedVertexIndex"].as_int())
					: static_cast<std::uint32_t>(shared_vertices_index_count);
				if (next_shared - first_shared > 0xFF)
				{
					throw std::runtime_error("havok: section has more than 255 shared indices");
				}

				const auto num_packed = static_cast<std::uint32_t>(section["numPackedVertices"].as_int());
				auto& result = converted[i];
				result.nodes = make_array<target::hkcdStaticTreeCodec3Axis4>(section["nodes"].size());
				result.domain = read_aabb(section["domain"]);
				read_floats(section["codecParms"], result.codecParms, 6);
				result.firstPackedVertex = static_cast<unsigned int>(section["firstPackedVertexIndex"].as_int());
				result.sharedVertices = (first_shared << 8) | num_packed;
				result.primitives = static_cast<unsigned int>((section["firstPrimitiveIndex"].as_int() << 8) | section["numPrimitives"].as_int());
				result.dataRuns = static_cast<unsigned int>((section["firstDataRunIndex"].as_int() << 8) | section["numDataRuns"].as_int());
				result.numPackedVertices = static_cast<unsigned char>(num_packed);
				result.numSharedIndices = static_cast<unsigned char>(next_shared - first_shared);
				result.leafIndex = static_cast<unsigned short>(section["leafIndex"].as_int());
				result.page = static_cast<unsigned char>(section["page"].as_int());
				result.layerData = static_cast<unsigned char>(section["layerData"].as_int());
			}

			return converted;
		}

		std::vector<target::hkcdStaticTreeCodec3Axis4> convert_section_nodes(const tag_value& nodes)
		{
			std::vector<target::hkcdStaticTreeCodec3Axis4> converted(nodes.size());
			for (auto i = 0u; i < converted.size(); i++)
			{
				const auto node = nodes[i];
				for (auto axis = 0; axis < 3; axis++)
				{
					converted[i].xyz[axis] = static_cast<unsigned char>(node["xyz"][axis].as_int());
				}

				converted[i].data = static_cast<unsigned char>(node["data"].as_int());
			}

			return converted;
		}

		std::vector<target::hkcdStaticTreeCodec3Axis5> convert_tree_nodes(const tag_value& nodes)
		{
			std::vector<target::hkcdStaticTreeCodec3Axis5> converted(nodes.size());
			for (auto i = 0u; i < converted.size(); i++)
			{
				const auto node = nodes[i];
				for (auto axis = 0; axis < 3; axis++)
				{
					converted[i].xyz[axis] = static_cast<unsigned char>(node["xyz"][axis].as_int());
				}

				converted[i].hiData = static_cast<unsigned char>(node["hiData"].as_int());
				converted[i].loData = static_cast<unsigned char>(node["loData"].as_int());
			}

			return converted;
		}

		std::vector<target::hknpCompressedMeshShapeTreeDataRun> convert_data_runs(const tag_value& runs)
		{
			std::vector<target::hknpCompressedMeshShapeTreeDataRun> converted(runs.size());
			for (auto i = 0u; i < converted.size(); i++)
			{
				const auto run = runs[i];
				converted[i].value = static_cast<unsigned short>(run["value"].as_int());
				converted[i].index = static_cast<unsigned char>(run["index"].as_int());
				converted[i].count = static_cast<unsigned char>(run["count"].as_int());
			}

			return converted;
		}

		std::vector<target::hkcdSimdTreeNode> convert_simd_tree(const tag_value& nodes)
		{
			std::vector<target::hkcdSimdTreeNode> converted(nodes.size());
			for (auto i = 0u; i < converted.size(); i++)
			{
				const auto node = nodes[i];
				auto& result = converted[i];
				read_floats(node["lx"], result.lx, 4);
				read_floats(node["hx"], result.hx, 4);
				read_floats(node["ly"], result.ly, 4);
				read_floats(node["hy"], result.hy, 4);
				read_floats(node["lz"], result.lz, 4);
				read_floats(node["hz"], result.hz, 4);

				const auto is_leaf = node["isLeaf"].as_bool();
				const auto data = node["data"];
				for (auto slot = 0; slot < 4; slot++)
				{
					if (result.lx[slot] > result.hx[slot])
					{
						result.data[slot] = 0;
					}
					else
					{
						result.data[slot] = (static_cast<unsigned int>(data[slot].as_int()) << 1) | (is_leaf ? 1 : 0);
					}
				}
			}

			return converted;
		}

		std::size_t write_compressed_mesh(target::havok::binary::packfile_writer& writer, const tag_value& source_shape, const converted_mesh& mesh)
		{
			const auto source_data = source_shape["data"].deref();
			const auto tree = source_data["meshTree"];

			const auto triangle_is_interior = source_shape["triangleIsInterior"]["storage"];
			const auto interior_words = read_integers<unsigned int>(triangle_is_interior["words"]);

			target::hknpCompressedMeshShape shape{};
			shape.flags = target::HKNP_SHAPE_FLAG_IS_COMPOSITE;
			shape.numShapeKeyBits = static_cast<unsigned char>(tree["bitsPerKey"].as_int());
			shape.dispatchType = target::HKNP_SHAPE_DISPATCH_COMPOSITE;
			shape.edgeWeldingMap.secondaryKeyMask = 0xFFFFFFFF;
			shape.edgeWeldingMap.primaryKeyToIndex = make_array<unsigned short>(0);
			shape.edgeWeldingMap.valueAndSecondaryKeys = make_array<unsigned short>(0);
			shape.shapeTagCodecInfo = 0xFFFFFFFF;
			shape.quadIsFlat.words = make_array<unsigned int>(mesh.quad_is_flat.size());
			shape.quadIsFlat.numBits = static_cast<int>((tree["maxKeyValue"].as_int() >> 1) + 1);
			shape.triangleIsInterior.words = make_array<unsigned int>(interior_words.size());
			shape.triangleIsInterior.numBits = static_cast<int>(triangle_is_interior["numBits"].as_int());

			using shape_t = target::hknpCompressedMeshShape;
			const auto shape_offset = writer.add_object(shape, "hknpCompressedMeshShape");
			writer.add_array(shape_offset + offsetof(shape_t, quadIsFlat), mesh.quad_is_flat);
			writer.add_array(shape_offset + offsetof(shape_t, triangleIsInterior), interior_words);

			const auto tree_nodes = convert_tree_nodes(tree["nodes"]);
			const auto sections = convert_sections(tree["sections"], mesh.shared_vertices_index.size());
			const auto packed_vertices = read_integers<unsigned int>(tree["packedVertices"]);
			const auto data_runs = convert_data_runs(tree["primitiveDataRuns"]);
			const auto simd_nodes = convert_simd_tree(source_data["simdTree"]["nodes"]);

			target::hknpCompressedMeshShapeData data{};
			auto& mesh_tree = data.meshTree;
			mesh_tree.nodes = make_array<target::hkcdStaticTreeCodec3Axis5>(tree_nodes.size());
			mesh_tree.domain = read_aabb(tree["domain"]);
			mesh_tree.numPrimitiveKeys = mesh.num_triangles + mesh.num_convex_shapes;
			mesh_tree.bitsPerKey = static_cast<int>(tree["bitsPerKey"].as_int());
			mesh_tree.maxKeyValue = static_cast<unsigned int>(tree["maxKeyValue"].as_int());
			mesh_tree.sections = make_array<target::hkcdStaticMeshTreeBaseSection>(sections.size());
			mesh_tree.primitives = make_array<target::hkcdStaticMeshTreeBasePrimitive>(mesh.primitives.size());
			mesh_tree.sharedVerticesIndex = make_array<unsigned short>(mesh.shared_vertices_index.size());
			mesh_tree.packedVertices = make_array<unsigned int>(packed_vertices.size());
			mesh_tree.sharedVertices = make_array<unsigned long long>(mesh.shared_vertices.size());
			mesh_tree.primitiveDataRuns = make_array<target::hknpCompressedMeshShapeTreeDataRun>(data_runs.size());
			data.simdTree.nodes = make_array<target::hkcdSimdTreeNode>(simd_nodes.size());
			data.connectivity.headers = make_array<void>(0);
			data.connectivity.localLinks = make_array<void>(0);
			data.connectivity.globalLinks = make_array<void>(0);

			using data_t = target::hknpCompressedMeshShapeData;
			using tree_t = target::hknpCompressedMeshShapeTree;
			const auto data_offset = writer.add_object(data, "hknpCompressedMeshShapeData");
			const auto tree_offset = data_offset + offsetof(data_t, meshTree);
			writer.add_object_pointer(shape_offset + offsetof(shape_t, data), data_offset);
			writer.add_array(tree_offset + offsetof(tree_t, nodes), tree_nodes);

			const auto sections_offset = writer.add_array(tree_offset + offsetof(tree_t, sections), sections);
			const auto source_sections = tree["sections"];
			for (auto i = 0u; i < sections.size(); i++)
			{
				const auto section_field = sections_offset + i * sizeof(target::hkcdStaticMeshTreeBaseSection);
				writer.add_array(section_field + offsetof(target::hkcdStaticMeshTreeBaseSection, nodes), convert_section_nodes(source_sections[i]["nodes"]));
			}

			writer.add_array(tree_offset + offsetof(tree_t, primitives), mesh.primitives);
			writer.add_array(tree_offset + offsetof(tree_t, sharedVerticesIndex), mesh.shared_vertices_index);
			writer.add_array(tree_offset + offsetof(tree_t, packedVertices), packed_vertices);
			writer.add_array(tree_offset + offsetof(tree_t, sharedVertices), mesh.shared_vertices);
			writer.add_array(tree_offset + offsetof(tree_t, primitiveDataRuns), data_runs);
			writer.add_array(data_offset + offsetof(data_t, simdTree) + offsetof(target::hkcdSimdTree, nodes), simd_nodes);

			return shape_offset;
		}

		std::uint32_t convert_shape_contents(const std::uint32_t contents)
		{
			if (contents == 0xFFFFFFFF)
			{
				return ~static_cast<std::uint32_t>(target::PHYSICS_CONTENTS_ACTOR | target::PHYSICS_CONTENTS_DETAIL
					| target::PHYSICS_CONTENTS_STRUCTURAL | target::PHYSICS_CONTENTS_TRANSLUCENT);
			}

			return convert_contents(contents);
		}

		std::string convert_shape_list(const std::string_view tagfile, const bool is_world)
		{
			const ::havok::tagfile::file file(tagfile);
			const auto list = file.root();
			const auto shapes = list["shapes"];
			const auto count = shapes.size();

			std::vector<converted_mesh> meshes;
			for (auto i = 0u; i < count; i++)
			{
				const auto shape = shapes[i].deref();
				if (shape.type_name() != "hknpCompressedMeshShape")
				{
					throw std::runtime_error(std::format("havok: shape {} is a {}", i, shape.type_name()));
				}

				const auto tree = shape["data"].deref()["meshTree"];
				meshes.emplace_back(convert_primitives(tree, shape["externShapes"], read_aabb(tree["domain"])));
			}

			auto world_contents = static_cast<std::uint32_t>(target::PHYSICS_CONTENTS_DETAIL | target::PHYSICS_CONTENTS_TRANSLUCENT);
			const auto tags = convert_shape_tags(list["shapeTagData"], world_contents);

			std::vector<int> indices(count);
			std::vector<int> vert_counts(count);
			std::vector<int> tri_counts(count);
			std::vector<int> convex_counts(count);
			std::vector<unsigned int> contents(count);
			std::vector<target::hkVector4> min_maxes(count * 2);
			for (auto i = 0u; i < count; i++)
			{
				indices[i] = is_world ? 0 : static_cast<int>(list["shapeIndices"][i].as_int());
				vert_counts[i] = static_cast<int>(list["vertCounts"][i].as_int());
				tri_counts[i] = meshes[i].num_triangles;
				convex_counts[i] = meshes[i].num_convex_shapes;
				contents[i] = is_world ? world_contents : convert_shape_contents(static_cast<std::uint32_t>(list["shapeContents"][i].as_int()));
				min_maxes[i * 2] = read_vector(list["minMaxes"][i * 2]);
				min_maxes[i * 2 + 1] = read_vector(list["minMaxes"][i * 2 + 1]);
			}

			target::havok::binary::packfile_writer writer;

			target::HavokPhysicsShapeList shape_list{};
			shape_list.shapes = make_array<void*>(count);
			shape_list.shapeIndices = make_array<int>(count);
			shape_list.shapeNames = make_array<const char*>(count);
			shape_list.vertCounts = make_array<int>(count);
			shape_list.triCounts = make_array<int>(count);
			shape_list.minMaxes = make_array<target::hkVector4>(min_maxes.size());
			shape_list.shapeTagData = make_array<target::HavokPhysicsShapeTagData>(tags.size());
			shape_list.shapeContents = make_array<unsigned int>(count);
			shape_list.convexCounts = make_array<int>(count);

			using list_t = target::HavokPhysicsShapeList;
			const auto list_offset = writer.add_object(shape_list, "HavokPhysicsShapeList");
			const auto shape_pointers = writer.add_array(list_offset + offsetof(list_t, shapes), std::vector<void*>(count));
			writer.add_array(list_offset + offsetof(list_t, shapeIndices), indices);
			const auto name_pointers = writer.add_array(list_offset + offsetof(list_t, shapeNames), std::vector<const char*>(count));
			for (auto i = 0u; i < count; i++)
			{
				writer.add_string(name_pointers + i * sizeof(const char*), std::string(list["shapeNames"][i].string()));
			}

			writer.add_array(list_offset + offsetof(list_t, vertCounts), vert_counts);
			writer.add_array(list_offset + offsetof(list_t, triCounts), tri_counts);
			writer.add_array(list_offset + offsetof(list_t, minMaxes), min_maxes);
			writer.add_array(list_offset + offsetof(list_t, shapeTagData), tags);
			writer.add_array(list_offset + offsetof(list_t, shapeContents), contents);
			writer.add_array(list_offset + offsetof(list_t, convexCounts), convex_counts);

			for (auto i = 0u; i < count; i++)
			{
				const auto shape_offset = write_compressed_mesh(writer, shapes[i].deref(), meshes[i]);
				writer.add_object_pointer(shape_pointers + i * sizeof(void*), shape_offset);
			}

			return writer.finish("HavokPhysicsShapeList");
		}
	}

	std::string convert_world_shape(const std::string_view tagfile)
	{
		return convert_shape_list(tagfile, true);
	}

	std::string convert_ents_shapes(const std::string_view tagfile)
	{
		return convert_shape_list(tagfile, false);
	}
}
