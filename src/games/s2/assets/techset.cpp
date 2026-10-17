#include <std_include.hpp>
#include "s2.hpp"

namespace zonetool::s2
{
	namespace
	{
		void read_technique_pass(zone_reader& reader, MaterialPass* pass)
		{
			reader.read_asset(ASSET_TYPE_VERTEXSHADER, pass->vertexShader);
			reader.read_asset(ASSET_TYPE_VERTEXDECL, pass->vertexDecl);
			reader.read_asset(ASSET_TYPE_HULLSHADER, pass->hullShader);
			reader.read_asset(ASSET_TYPE_DOMAINSHADER, pass->domainShader);
			reader.read_asset(ASSET_TYPE_PIXELSHADER, pass->pixelShader);

			const auto arg_count = pass->perPrimArgCount + pass->perObjArgCount + pass->volatileArgCount + pass->stableArgCount;
			if (const auto args = reader.read_array(pass->args, 7, arg_count))
			{
				for (auto i = 0; i < arg_count; i++)
				{
					if (args[i].type == MTL_ARG_LITERAL_CONST)
					{
						reader.read_array(args[i].u.literalConst, 3, 4);
					}
				}
			}
		}

		void read_technique(zone_reader& reader, MaterialTechnique*& field)
		{
			if (!field)
			{
				return;
			}

			if (!reader.is_inline(field))
			{
				reader.resolve_pointer(field);
				return;
			}

			reader.align(7);
			const auto technique = reader.read<MaterialTechniqueHeader>();
			reader.read<MaterialPass>(technique->passCount);
			field = reinterpret_cast<MaterialTechnique*>(technique);

			for (unsigned short i = 0; i < field->hdr.passCount; i++)
			{
				read_technique_pass(reader, &field->passArray[i]);
			}

			reader.read_string(field->hdr.name);
		}
	}

	void techset::read(zone_reader& reader, MaterialTechniqueSet* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		for (auto& technique : asset->techniques)
		{
			read_technique(reader, technique);
		}
		reader.pop_stream();
	}

	namespace
	{
		constexpr std::int32_t technique_missing = -2;
		constexpr std::int32_t technique_unique = -1;

		std::size_t technique_size(const unsigned short pass_count)
		{
			return offsetof(MaterialTechnique, passArray) + pass_count * sizeof(MaterialPass);
		}

		std::size_t pass_arg_count(const MaterialPass& pass)
		{
			return pass.perPrimArgCount + pass.perObjArgCount + pass.volatileArgCount + pass.stableArgCount;
		}

		void dump_technique(assetmanager::dumper& write, const MaterialTechnique* technique, std::deque<MaterialPass>& pass_copies)
		{
			write.dump_string(technique->hdr.name);
			write.dump_int(technique->hdr.flags);
			write.dump_int(technique->hdr.passCount);

			for (auto i = 0u; i < technique->hdr.passCount; i++)
			{
				auto* pass = const_cast<MaterialPass*>(&technique->passArray[i]);

				// the buffer indices are local to the zone globals, and prepare() gets them again from the sizes.
				// the copies stay alive for the whole dump, because the dumper dedupes by address.
				auto& zone_free_pass = pass_copies.emplace_back(*pass);
				zone_free_pass.zone = 0;
				zone_free_pass.perPrimConstantBuffer = 0;
				zone_free_pass.perObjConstantBuffer = 0;
				zone_free_pass.volatileConstantBuffer = 0;
				zone_free_pass.stableConstantBuffer = 0;
				write.dump_single(&zone_free_pass);
				write.dump_asset(pass->vertexShader);
				write.dump_asset(pass->vertexDecl);
				write.dump_asset(pass->hullShader);
				write.dump_asset(pass->domainShader);
				write.dump_asset(pass->pixelShader);

				const auto arg_count = pass_arg_count(*pass);
				write.dump_array(pass->args, static_cast<std::uint32_t>(arg_count));
				for (auto arg = 0u; pass->args && arg < arg_count; arg++)
				{
					if (pass->args[arg].type == MTL_ARG_LITERAL_CONST)
					{
						write.dump_array(pass->args[arg].u.literalConst, 4);
					}
				}
			}
		}

		MaterialTechnique* parse_technique(assetmanager::reader& read, zone_memory* mem)
		{
			const auto* name = read.read_string();
			const auto flags = static_cast<unsigned short>(read.read_int());
			const auto pass_count = static_cast<unsigned short>(read.read_int());

			auto* technique = mem->manual_allocate<MaterialTechnique>(technique_size(pass_count));
			technique->hdr.name = name;
			technique->hdr.flags = flags;
			technique->hdr.passCount = pass_count;

			for (auto i = 0u; i < pass_count; i++)
			{
				auto& pass = technique->passArray[i];
				pass = *read.read_single<MaterialPass>();
				pass.vertexShader = read.read_asset<MaterialVertexShader>();
				pass.vertexDecl = read.read_asset<MaterialVertexDeclaration>();
				pass.hullShader = read.read_asset<MaterialHullShader>();
				pass.domainShader = read.read_asset<MaterialDomainShader>();
				pass.pixelShader = read.read_asset<MaterialPixelShader>();
				pass.args = read.read_array<MaterialShaderArgument>();
				for (auto arg = 0u; pass.args && arg < pass_arg_count(pass); arg++)
				{
					if (pass.args[arg].type == MTL_ARG_LITERAL_CONST)
					{
						pass.args[arg].u.literalConst = read.read_array<float>();
					}
				}
			}

			return technique;
		}

		void write_technique_pass(zone_base* zone, zone_buffer* buf, const MaterialPass& source, MaterialPass& dest)
		{
			dest.vertexShader = asset_reference(zone, ASSET_TYPE_VERTEXSHADER, source.vertexShader);
			dest.vertexDecl = asset_reference(zone, ASSET_TYPE_VERTEXDECL, source.vertexDecl);
			dest.hullShader = asset_reference(zone, ASSET_TYPE_HULLSHADER, source.hullShader);
			dest.domainShader = asset_reference(zone, ASSET_TYPE_DOMAINSHADER, source.domainShader);
			dest.pixelShader = asset_reference(zone, ASSET_TYPE_PIXELSHADER, source.pixelShader);

			const auto arg_count = pass_arg_count(source);
			if (!source.args || !arg_count)
			{
				dest.args = nullptr;
				return;
			}

			buf->align(7);
			auto* args = buf->write(source.args, arg_count);
			buf->clear_pointer(&dest.args);

			for (auto i = 0u; i < arg_count; i++)
			{
				if (args[i].type == MTL_ARG_LITERAL_CONST && source.args[i].u.literalConst)
				{
					args[i].u.literalConst = buf->write_s(3, source.args[i].u.literalConst, 4);
				}
			}
		}
	}

	MaterialTechniqueSet* techset::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open(dump_path(ASSET_TYPE_TECHNIQUE_SET, name.data())))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing techset \"%s\"...", name.data());

		auto* asset = read.read_single<MaterialTechniqueSet>();
		asset->name = read.read_string();
		for (auto& technique : asset->techniques)
		{
			const auto source = read.read_int();
			if (source == technique_missing)
			{
				technique = nullptr;
			}
			else if (source == technique_unique)
			{
				technique = parse_technique(read, mem);
			}
			else
			{
				technique = asset->techniques[source];
			}
		}
		read.close();

		return asset;
	}

	void techset::prepare(zone_buffer* buf, zone_memory* mem)
	{
		for (auto* technique : this->asset_->techniques)
		{
			for (auto i = 0u; technique && i < technique->hdr.passCount; i++)
			{
				auto& pass = technique->passArray[i];
				pass.perPrimConstantBuffer = pass.perPrimArgSize ? buf->write_ppas(pass.perPrimArgSize) : 0;
				pass.perObjConstantBuffer = pass.perObjArgSize ? buf->write_poas(pass.perObjArgSize) : 0;
				pass.volatileConstantBuffer = pass.volatileArgSize ? buf->write_vas(pass.volatileArgSize) : 0;
				pass.stableConstantBuffer = pass.stableArgSize ? buf->write_sas(pass.stableArgSize) : 0;
			}
		}
	}

	void techset::load_depending(zone_base* zone)
	{
		for (const auto* technique : this->asset_->techniques)
		{
			for (auto i = 0u; technique && i < technique->hdr.passCount; i++)
			{
				const auto& pass = technique->passArray[i];
				add_dependency(zone, ASSET_TYPE_VERTEXSHADER, pass.vertexShader);
				add_dependency(zone, ASSET_TYPE_VERTEXDECL, pass.vertexDecl);
				add_dependency(zone, ASSET_TYPE_HULLSHADER, pass.hullShader);
				add_dependency(zone, ASSET_TYPE_DOMAINSHADER, pass.domainShader);
				add_dependency(zone, ASSET_TYPE_PIXELSHADER, pass.pixelShader);
			}
		}
	}

	void techset::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(data->name);

		for (auto i = 0; i < technique_count; i++)
		{
			const auto* technique = data->techniques[i];
			if (!technique)
			{
				continue;
			}

			MaterialTechnique* written = nullptr;
			dest->techniques[i] = buf->write_s(7, const_cast<MaterialTechnique*>(technique), 1,
				technique_size(technique->hdr.passCount), &written);
			if (!written)
			{
				continue;
			}

			for (unsigned short pass = 0; pass < technique->hdr.passCount; pass++)
			{
				write_technique_pass(zone, buf, technique->passArray[pass], written->passArray[pass]);
			}

			written->hdr.name = buf->write_str(technique->hdr.name);
		}

		buf->pop_stream();
	}

	void techset::dump(MaterialTechniqueSet* asset)
	{
		assetmanager::dumper write;
		if (!write.open(dump_path(ASSET_TYPE_TECHNIQUE_SET, asset->name)))
		{
			return;
		}

		std::deque<MaterialPass> pass_copies;
		write.dump_single(asset);
		write.dump_string(asset->name);
		for (auto i = 0; i < technique_count; i++)
		{
			const auto* technique = asset->techniques[i];
			if (!technique)
			{
				write.dump_int(technique_missing);
				continue;
			}

			auto shared_with = technique_unique;
			for (auto j = 0; j < i; j++)
			{
				if (asset->techniques[j] == technique)
				{
					shared_with = j;
					break;
				}
			}

			write.dump_int(shared_with);
			if (shared_with == technique_unique)
			{
				dump_technique(write, technique, pass_copies);
			}
		}
		write.close();
	}
}
