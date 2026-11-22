#include <std_include.hpp>

#include "umbra_tome.hpp"

namespace umbra_tome
{
	namespace
	{
		unsigned int allocate(std::vector<unsigned char>& tome, const std::size_t size)
		{
			tome.resize((tome.size() + 15) & ~std::size_t(15));
			const auto offset = static_cast<unsigned int>(tome.size());
			tome.resize(tome.size() + size);
			return offset;
		}

		template <typename T>
		void write_at(std::vector<unsigned char>& data, const std::size_t offset, const T& value)
		{
			std::memcpy(data.data() + offset, &value, sizeof(T));
		}

		unsigned int crc32c(const unsigned char* data, const std::size_t size)
		{
			auto crc = 0xFFFFFFFFu;
			for (std::size_t i = 0; i < size; i++)
			{
				crc ^= data[i];
				for (auto bit = 0; bit < 8; bit++)
				{
					crc = (crc >> 1) ^ (0x82F63B78u & (0u - (crc & 1u)));
				}
			}

			return ~crc;
		}

		unsigned int get_index_width(const unsigned int count)
		{
			auto width = 1u;
			while (width < 31 && (1u << width) < count)
			{
				width++;
			}

			return width;
		}

		std::vector<unsigned char> pack_object_list(const unsigned int count, const unsigned int width)
		{
			std::vector<unsigned char> result(((static_cast<std::uint64_t>(count) * width + 31) / 32) * 4);
			auto bit_offset = std::uint64_t{};
			for (auto object = 0u; object < count; object++)
			{
				for (auto bit = 0u; bit < width; bit++)
				{
					if (object & (1u << bit))
					{
						const auto current = bit_offset + bit;
						result[static_cast<std::size_t>(current) >> 3] |= static_cast<unsigned char>(1u << (current & 7));
					}
				}

				bit_offset += width;
			}

			return result;
		}

		float get_tile_scale(const float minimum[3], const float maximum[3])
		{
			auto largest_extent = 0.0;
			for (auto i = 0; i < 3; i++)
			{
				largest_extent = std::max(largest_extent, static_cast<double>(maximum[i]) - minimum[i]);
			}

			const auto required_scale = largest_extent / 65536.0;
			auto scale = static_cast<float>(required_scale);
			if (static_cast<double>(scale) < required_scale)
			{
				scale = std::nextafter(scale, std::numeric_limits<float>::infinity());
			}

			return scale;
		}
	}

	std::vector<unsigned char> build_single_cell(const float minimum[3], const float maximum[3], const std::vector<object>& objects)
	{
		const auto object_count = static_cast<unsigned int>(objects.size());
		if (!object_count || objects.size() > 0x01000000)
		{
			return {};
		}

		const auto index_width = get_index_width(object_count);
		const auto object_list = pack_object_list(object_count, index_width);

		UmbraTomeHeader header{};
		header.version = 0xD6000014;
		header.unitsPerMeter = 128.0f;
		header.treeDepth = 33;
		header.objectCount = object_count;
		header.objectCount2 = object_count;
		header.indexWidth = index_width;
		header.cellCount = 1;
		header.tileCount = 1;
		for (auto i = 0; i < 3; i++)
		{
			header.boundsMin[i] = minimum[i];
			header.boundsMax[i] = maximum[i];
			header.rootBoundsMin[i] = minimum[i];
			header.rootBoundsMax[i] = maximum[i];
		}

		std::vector<unsigned char> tome(sizeof(header));
		std::memcpy(tome.data(), &header, sizeof(header));

		const auto top_tree = allocate(tome, sizeof(unsigned int));
		write_at(tome, top_tree, 3u);
		const auto top_map = allocate(tome, sizeof(unsigned int));
		write_at(tome, top_map, 0u);
		write_at(tome, offsetof(UmbraTomeHeader, topTree), top_tree);
		write_at(tome, offsetof(UmbraTomeHeader, topMap), top_map);

		const auto object_bounds = allocate(tome, objects.size() * 24);
		const auto object_distances = allocate(tome, objects.size() * 32);
		for (std::size_t i = 0; i < objects.size(); i++)
		{
			const auto& object = objects[i];
			const auto bounds_offset = object_bounds + i * 24;
			std::memcpy(tome.data() + bounds_offset, object.minimum, sizeof(object.minimum));
			std::memcpy(tome.data() + bounds_offset + 12, object.maximum, sizeof(object.maximum));

			const auto distance_offset = object_distances + i * 32;
			std::memcpy(tome.data() + distance_offset, object.minimum, sizeof(object.minimum));
			write_at(tome, distance_offset + 12, 0.0f);
			std::memcpy(tome.data() + distance_offset + 16, object.maximum, sizeof(object.maximum));
			write_at(tome, distance_offset + 28, std::numeric_limits<float>::infinity());
		}

		write_at(tome, offsetof(UmbraTomeHeader, objectBounds), object_bounds);
		write_at(tome, offsetof(UmbraTomeHeader, objectDistances), object_distances);

		const auto ids = allocate(tome, objects.size() * 4);
		for (std::size_t i = 0; i < objects.size(); i++)
		{
			write_at(tome, ids + i * 4, objects[i].user_id);
		}

		write_at(tome, offsetof(UmbraTomeHeader, userIDs), ids);

		const auto lists = allocate(tome, object_list.size());
		std::memcpy(tome.data() + lists, object_list.data(), object_list.size());
		write_at(tome, offsetof(UmbraTomeHeader, objectLists), lists);

		const auto cell_starts = allocate(tome, 8);
		write_at(tome, cell_starts, 0u);
		write_at(tome, cell_starts + 4, 1u);
		write_at(tome, offsetof(UmbraTomeHeader, cellStarts), cell_starts);

		const auto tile_lod_levels = allocate(tome, 4);
		write_at(tome, tile_lod_levels, 1.0f);
		write_at(tome, offsetof(UmbraTomeHeader, tileLodLevels), tile_lod_levels);

		const auto tile_offsets = allocate(tome, 4);
		write_at(tome, offsetof(UmbraTomeHeader, tileOffsets), tile_offsets);

		const auto tile_portal_expands = allocate(tome, 4);
		write_at(tome, tile_portal_expands, 0.0f);
		write_at(tome, offsetof(UmbraTomeHeader, tilePortalExpands), tile_portal_expands);

		const auto tile_offset = allocate(tome, sizeof(UmbraTomeTile));
		UmbraTomeTile tile{};
		std::memcpy(tile.boundsMin, minimum, sizeof(tile.boundsMin));
		std::memcpy(tile.boundsMax, maximum, sizeof(tile.boundsMax));
		tile.depth = 33;
		tile.unk0x34 = 1;
		tile.scale = get_tile_scale(minimum, maximum);
		tile.inverseScale = 1.0f / tile.scale;

		const auto tile_tree = allocate(tome, 4);
		write_at(tome, tile_tree, 3u);
		tile.tree = tile_tree - tile_offset;
		const auto tile_map = allocate(tome, 8);
		write_at(tome, tile_map, 0u);
		tile.map = tile_map - tile_offset;
		const auto cell_indices = allocate(tome, 8);
		write_at(tome, cell_indices, 0u);
		write_at(tome, cell_indices + 4, 0xFFFFFFFFu);
		tile.cellIndices = cell_indices - tile_offset;

		const auto cell_offset = allocate(tome, sizeof(UmbraTomeCell));
		UmbraTomeCell cell{};
		cell.objectCount = object_count;
		cell.unk0x10 = 0xFFFFFFFFu;
		cell.unk0x14 = 0x80000000u;
		cell.unk0x1C = 0xFFFFu;
		cell.unk0x20 = 0xFFFFFFFFu;
		std::memcpy(tome.data() + cell_offset, &cell, sizeof(cell));
		tile.cell = cell_offset - tile_offset;

		tome.resize((tome.size() + 15) & ~std::size_t(15));
		tile.sizeAndFlags = (static_cast<unsigned int>(tome.size()) - tile_offset) << 8 | 3u;
		std::memcpy(tome.data() + tile_offset, &tile, sizeof(tile));
		write_at(tome, tile_offsets, tile_offset);

		const auto size = static_cast<unsigned int>(tome.size());
		write_at(tome, offsetof(UmbraTomeHeader, size), size);
		write_at(tome, offsetof(UmbraTomeHeader, checksum), crc32c(tome.data() + 8, size - 8));
		return tome;
	}
}
