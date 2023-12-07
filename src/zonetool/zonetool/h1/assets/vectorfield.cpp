#include <std_include.hpp>
#include "vectorfield.hpp"

namespace zonetool::h1
{
	VectorField* vector_field::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);

		const auto path = "vectorfield\\"s + name;
		if (!read.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing vectorfield \"%s\"...", name.data());

		auto* asset = read.read_single<VectorField>();
		asset->name = read.read_string();

		asset->fields = read.read_array<VectorFieldSubField>();
		for (auto i = 0; i < asset->fieldsCount; i++)
		{
			asset->fields[i].linearData = read.read_array<vec4_t>();
		}

		read.close();

		return asset;
	}

	void vector_field::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).vectorField;
		}
	}

	void vector_field::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void vector_field::load_depending(zone_base* zone)
	{
	}

	std::string vector_field::name()
	{
		return this->name_;
	}

	std::int32_t vector_field::type()
	{
		return ASSET_TYPE_VECTORFIELD;
	}

	void vector_field::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->fields)
		{
			buf->align(15);

			auto* dest_fields = buf->write(data->fields, data->fieldsCount);
			auto* data_fields = data->fields;
			for (auto i = 0; i < data->fieldsCount; i++)
			{
				if (data_fields[i].linearData)
				{
					buf->align(3);
					buf->write(data_fields[i].linearData, 
						data_fields[i].numEntries[0] * data_fields[i].numEntries[1] * data_fields[i].numEntries[2]);
					zone_buffer::clear_pointer(&dest_fields[i].linearData);
				}
			}

			zone_buffer::clear_pointer(&dest->fields);
		}

		buf->pop_stream();
	}

	void vector_field::dump(VectorField* asset)
	{
		assetmanager::dumper dump;

		const auto path = "vectorfield\\"s + asset->name;
		if (!dump.open(path))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);

		dump.dump_array(asset->fields, asset->fieldsCount);
		for (auto i = 0; i < asset->fieldsCount; i++)
		{
			dump.dump_array(asset->fields[i].linearData, 
				asset->fields[i].numEntries[0] * asset->fields[i].numEntries[1] * asset->fields[i].numEntries[2]);
		}

		dump.close();
	}
}