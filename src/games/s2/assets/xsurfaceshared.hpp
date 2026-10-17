#pragma once

namespace zonetool::s2
{
	std::optional<std::string> read_shared_stream(const XSurfaceShared* shared);
	std::string surface_shared_key(const XSurfaceShared* shared);

	constexpr std::size_t surface_subdiv_size = 48;

	class xsurface_shared : public asset_interface
	{
	public:
		static void read(zone_reader& reader, XSurfaceShared* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);

			if (asset->flags & XSURFACE_SHARED_STREAMED)
			{
				reader.push_stream(XFILE_BLOCK_RUNTIME_UNREAD);
				reader.read_array(asset->data, 255, asset->dataSize);
				reader.pop_stream();
			}
			else
			{
				reader.read_array(asset->data, 255, asset->dataSize);
			}

			reader.read_stream(asset->subdivData, 15, 48, asset->subdivCount);
			reader.pop_stream();
		}

		void init(const std::string& name, zone_memory* mem) override;
		void init(void* asset, zone_memory* mem) override;
		void write(zone_base* zone, zone_buffer* buf) override;

		void* pointer() override { return this->asset_; }
		std::string name() override { return this->name_; }
		std::int32_t type() override { return ASSET_TYPE_XSURFACE_SHARED; }

	private:
		std::string name_;
		XSurfaceShared* asset_ = nullptr;
	};
}
