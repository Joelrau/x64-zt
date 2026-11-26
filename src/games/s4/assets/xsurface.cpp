#include <std_include.hpp>
#include "xsurface.hpp"

namespace zonetool::s4
{
	namespace
	{
		void read_shared(zone_reader& reader, XSurfaceShared*& field)
		{
			const auto shared = reader.read_single(field, 7);
			if (shared && !(shared->flags & XSURFACE_SHARED_STREAMED))
			{
				reader.read_array(shared->data.residentData, 15, shared->dataSize);
			}
		}

		void read_subdiv_level(zone_reader& reader, XSurfaceSubdivLevel* level)
		{
			reader.read_array(level->facePoints, 3, level->facePointBufferSize / sizeof(unsigned int));
			reader.read_array(level->edgePoints, 3, 2 * level->edgePointCount);
			reader.read_array(level->vertexPoints, 3, level->vertexPointBufferSize / sizeof(unsigned int));
			reader.read_array(level->normals, 3, level->normalBufferSize / sizeof(unsigned int));
		}

		void read_subdiv(zone_reader& reader, XSurface* surface)
		{
			if (const auto levels = reader.read_array(surface->subdivLevels, 7, surface->subdivLevelCount))
			{
				for (auto i = 0; i < surface->subdivLevelCount; i++)
				{
					read_subdiv_level(reader, &levels[i]);
				}
			}
		}

		void read_blend_shapes(zone_reader& reader, XSurface* surface)
		{
			if (const auto per_vert = reader.read_single(surface->blendShapesPerVert, 15))
			{
				reader.read_array(per_vert->perVertBlendShapeData, 7, per_vert->perVertBlendShapeDataSize / sizeof(unsigned __int64));
				reader.read_array(per_vert->surfBlendShapesData, 1, per_vert->surfBlendShapeCount);
			}

			if (const auto tangent_frame = reader.read_single(surface->blendShapesRecalcTangentFrameData, 15))
			{
				reader.read_array(tangent_frame->topologicalData, 1, tangent_frame->bufferSize / sizeof(unsigned short));
			}
		}

		void read_surface(zone_reader& reader, XSurface* surface)
		{
			read_shared(reader, surface->shared);
			reader.read_array(surface->lmapCoords, 3, surface->vertCount);
			reader.read_array(surface->rigidVertLists, 1, surface->rigidVertListCount);
			reader.read_array(surface->blendVerts, 3, surface->blendVertSize / sizeof(unsigned short));
			read_subdiv(reader, surface);
			reader.read_array(surface->childBounds, 3, surface->rigidVertListCount);
			read_blend_shapes(reader, surface);
		}
	}

	void xmodel_surfs::read(zone_reader& reader, XModelSurfs* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto surfaces = reader.read_array(asset->surfs, 15, asset->numsurfs))
		{
			for (auto i = 0; i < asset->numsurfs; i++)
			{
				read_surface(reader, &surfaces[i]);
			}
		}

		read_shared(reader, asset->shared);
		reader.pop_stream();
	}

	namespace
	{
		template <typename T>
		void write_array(zone_buffer* buf, const T* source, T*& target, const std::size_t count, const std::uint32_t alignment)
		{
			if (!source)
			{
				return;
			}

			if (const auto written = buf->find_sub_buffer(source))
			{
				target = written;
				return;
			}

			buf->align(alignment);
			buf->insert_sub_buffer(source, count);
			buf->write(source, count);
			buf->clear_pointer(&target);
		}

		bool shared_resident(const XSurfaceShared* shared)
		{
			return !(shared->flags & XSURFACE_SHARED_STREAMED);
		}

		void write_shared(zone_buffer* buf, XSurfaceShared* source, XSurfaceShared*& target)
		{
			if (!source)
			{
				return;
			}

			if (const auto written = buf->find_sub_buffer(source))
			{
				target = written;
				return;
			}

			buf->align(7);
			buf->insert_sub_buffer(source, 1);
			auto* shared = buf->write(source);
			if (shared_resident(source))
			{
				write_array(buf, source->data.residentData, shared->data.residentData, source->dataSize, 15);
			}
			buf->clear_pointer(&target);
		}

		void write_subdiv_level(zone_buffer* buf, const XSurfaceSubdivLevel* source, XSurfaceSubdivLevel* level)
		{
			write_array(buf, source->facePoints, level->facePoints, source->facePointBufferSize / sizeof(unsigned int), 3);
			write_array(buf, source->edgePoints, level->edgePoints, 2 * source->edgePointCount, 3);
			write_array(buf, source->vertexPoints, level->vertexPoints, source->vertexPointBufferSize / sizeof(unsigned int), 3);
			write_array(buf, source->normals, level->normals, source->normalBufferSize / sizeof(unsigned int), 3);
		}

		void write_subdiv(zone_buffer* buf, const XSurface* source, XSurface* surface)
		{
			if (!source->subdivLevels)
			{
				return;
			}

			buf->align(7);
			auto* levels = buf->write(source->subdivLevels, source->subdivLevelCount);
			for (auto i = 0; i < source->subdivLevelCount; i++)
			{
				write_subdiv_level(buf, &source->subdivLevels[i], &levels[i]);
			}
			buf->clear_pointer(&surface->subdivLevels);
		}

		void write_blend_shapes(zone_buffer* buf, const XSurface* source, XSurface* surface)
		{
			if (auto* per_vert = source->blendShapesPerVert)
			{
				buf->align(15);
				auto* dest = buf->write(per_vert);
				write_array(buf, per_vert->perVertBlendShapeData, dest->perVertBlendShapeData, per_vert->perVertBlendShapeDataSize / sizeof(unsigned __int64), 7);
				write_array(buf, per_vert->surfBlendShapesData, dest->surfBlendShapesData, per_vert->surfBlendShapeCount, 1);
				buf->clear_pointer(&surface->blendShapesPerVert);
			}

			if (auto* tangent_frame = source->blendShapesRecalcTangentFrameData)
			{
				buf->align(15);
				auto* dest = buf->write(tangent_frame);
				write_array(buf, tangent_frame->topologicalData, dest->topologicalData, tangent_frame->bufferSize / sizeof(unsigned short), 1);
				buf->clear_pointer(&surface->blendShapesRecalcTangentFrameData);
			}
		}

		void write_surface(zone_buffer* buf, const XSurface* source, XSurface* surface)
		{
			write_shared(buf, source->shared, surface->shared);
			write_array(buf, source->lmapCoords, surface->lmapCoords, source->vertCount, 3);
			write_array(buf, source->rigidVertLists, surface->rigidVertLists, source->rigidVertListCount, 1);
			write_array(buf, source->blendVerts, surface->blendVerts, source->blendVertSize / sizeof(unsigned short), 3);
			write_subdiv(buf, source, surface);
			write_array(buf, source->childBounds, surface->childBounds, source->rigidVertListCount, 3);
			write_blend_shapes(buf, source, surface);
		}

		void parse_shared(assetmanager::reader& read, XSurfaceShared*& shared)
		{
			if ((shared = read.read_single<XSurfaceShared>()) && shared_resident(shared))
			{
				shared->data.residentData = read.read_array<char>();
			}
		}

		void dump_shared(assetmanager::dumper& dump, const XSurfaceShared* shared)
		{
			dump.dump_single(shared);
			if (shared && shared_resident(shared))
			{
				dump.dump_array(shared->data.residentData, shared->dataSize);
			}
		}

		void parse_surface(assetmanager::reader& read, XSurface* surface)
		{
			parse_shared(read, surface->shared);
			surface->lmapCoords = read.read_array<PackedLmapCoords>();
			surface->rigidVertLists = read.read_array<XRigidVertList>();
			surface->blendVerts = read.read_array<unsigned short>();

			if ((surface->subdivLevels = read.read_array<XSurfaceSubdivLevel>()))
			{
				for (auto i = 0; i < surface->subdivLevelCount; i++)
				{
					auto* level = &surface->subdivLevels[i];
					level->facePoints = read.read_array<unsigned int>();
					level->edgePoints = read.read_array<unsigned int>();
					level->vertexPoints = read.read_array<unsigned int>();
					level->normals = read.read_array<unsigned int>();
				}
			}

			surface->childBounds = read.read_array<XSurfaceChildBounds>();

			if ((surface->blendShapesPerVert = read.read_single<BlendShapesPerVert>()))
			{
				surface->blendShapesPerVert->perVertBlendShapeData = read.read_array<unsigned __int64>();
				surface->blendShapesPerVert->surfBlendShapesData = read.read_array<unsigned short>();
			}

			if ((surface->blendShapesRecalcTangentFrameData = read.read_single<BlendShapesRecalcTangentFrameData>()))
			{
				surface->blendShapesRecalcTangentFrameData->topologicalData = read.read_array<unsigned short>();
			}
		}

		void dump_surface(assetmanager::dumper& dump, const XSurface* surface)
		{
			dump_shared(dump, surface->shared);
			dump.dump_array(surface->lmapCoords, surface->vertCount);
			dump.dump_array(surface->rigidVertLists, surface->rigidVertListCount);
			dump.dump_array(surface->blendVerts, surface->blendVertSize / sizeof(unsigned short));

			dump.dump_array(surface->subdivLevels, surface->subdivLevelCount);
			if (surface->subdivLevels)
			{
				for (auto i = 0; i < surface->subdivLevelCount; i++)
				{
					const auto* level = &surface->subdivLevels[i];
					dump.dump_array(level->facePoints, level->facePointBufferSize / sizeof(unsigned int));
					dump.dump_array(level->edgePoints, 2 * level->edgePointCount);
					dump.dump_array(level->vertexPoints, level->vertexPointBufferSize / sizeof(unsigned int));
					dump.dump_array(level->normals, level->normalBufferSize / sizeof(unsigned int));
				}
			}

			dump.dump_array(surface->childBounds, surface->rigidVertListCount);

			dump.dump_single(surface->blendShapesPerVert);
			if (const auto* per_vert = surface->blendShapesPerVert)
			{
				dump.dump_array(per_vert->perVertBlendShapeData, per_vert->perVertBlendShapeDataSize / sizeof(unsigned __int64));
				dump.dump_array(per_vert->surfBlendShapesData, per_vert->surfBlendShapeCount);
			}

			dump.dump_single(surface->blendShapesRecalcTangentFrameData);
			if (const auto* tangent_frame = surface->blendShapesRecalcTangentFrameData)
			{
				dump.dump_array(tangent_frame->topologicalData, tangent_frame->bufferSize / sizeof(unsigned short));
			}
		}
	}

	XModelSurfs* xmodel_surfs::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("xsurface\\"s + name + ".xsb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing xmodelsurfs \"%s\"...", name.data());

		auto* asset = read.read_single<XModelSurfs>();
		asset->name = read.read_string();

		if ((asset->surfs = read.read_array<XSurface>()))
		{
			for (auto i = 0; i < asset->numsurfs; i++)
			{
				parse_surface(read, &asset->surfs[i]);
			}
		}

		parse_shared(read, asset->shared);
		read.close();
		return asset;
	}

	void xmodel_surfs::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<XModelSurfs>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void xmodel_surfs::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void xmodel_surfs::load_depending(zone_base* zone)
	{
	}

	std::string xmodel_surfs::name()
	{
		return this->name_;
	}

	std::int32_t xmodel_surfs::type()
	{
		return ASSET_TYPE_XMODEL_SURFS;
	}

	void xmodel_surfs::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());

		if (data->surfs)
		{
			buf->align(15);
			auto* surfaces = buf->write(data->surfs, data->numsurfs);
			for (auto i = 0; i < data->numsurfs; i++)
			{
				write_surface(buf, &data->surfs[i], &surfaces[i]);
			}
			buf->clear_pointer(&dest->surfs);
		}

		write_shared(buf, data->shared, dest->shared);
		buf->pop_stream();
	}

	void xmodel_surfs::dump(XModelSurfs* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("xsurface\\"s + asset->name + ".xsb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);

		dump.dump_array(asset->surfs, asset->numsurfs);
		if (asset->surfs)
		{
			for (auto i = 0; i < asset->numsurfs; i++)
			{
				dump_surface(dump, &asset->surfs[i]);
			}
		}

		dump_shared(dump, asset->shared);
		dump.close();
	}
}
