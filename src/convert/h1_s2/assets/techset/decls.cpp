#include <std_include.hpp>

#include "../../include.hpp"
#include "../common/stock_zone.hpp"
#include "common.hpp"
#include "decls.hpp"

#include <utils/cryptography.hpp>

namespace zonetool::h1::converter::s2::techset
{
	namespace
	{
		constexpr std::array<std::uint8_t, 18> stream_slot_order{0, 1, 5, 21, 2, 3, 4, 9, 10, 16, 17, 18, 19, 20, 26, 6, 7, 29};

		std::map<std::tuple<const MaterialVertexDeclaration*, bool, bool>, zonetool::s2::MaterialVertexDeclaration*> converted_decls;
		std::unordered_map<const zonetool::s2::MaterialVertexDeclaration*, std::string> decl_routing_names;

		std::vector<std::string> parse_decl_tokens(const MaterialVertexDeclaration* decl)
		{
			const auto pieces = utils::string::split(decl->name, '.');
			std::vector<std::string> tokens;
			for (auto i = 0u; i + 1 < pieces.size(); i++)
			{
				tokens.emplace_back(i ? pieces[i].substr(1) : pieces[i]);
			}

			if (tokens.size() != decl->streamCount)
			{
				throw std::runtime_error(utils::string::va("vertexdecl \"%s\" does not name its %u streams", decl->name, decl->streamCount));
			}

			return tokens;
		}

		std::uint8_t convert_stream_source(const std::uint8_t source)
		{
			if (source <= STREAM_SRC_TANGENT)
			{
				return source;
			}

			if (source < 6u)
			{
				throw std::runtime_error(utils::string::va("stream source %u has no s2 mapping", source));
			}

			return static_cast<std::uint8_t>(source + 3u);
		}

		std::size_t stream_slot_priority(const std::uint8_t source)
		{
			const auto entry = std::ranges::find(stream_slot_order, source);
			return entry != stream_slot_order.end() ? entry - stream_slot_order.begin() : stream_slot_order.size() + source;
		}
	}

	zonetool::s2::MaterialVertexDeclaration* convert_decl(const MaterialVertexDeclaration* decl, const bool wind, const bool lightmap)
	{
		if (!decl)
		{
			return nullptr;
		}

		if (const auto entry = converted_decls.find({decl, wind, lightmap}); entry != converted_decls.end())
		{
			return entry->second;
		}

		const auto tokens = parse_decl_tokens(decl);
		std::vector<decl_stream> streams;
		for (auto i = 0u; i < decl->streamCount; i++)
		{
			const auto& routing = decl->routing.data[i];
			decl_stream stream{{convert_stream_source(routing.source), routing.dest, routing.mask}, tokens[i]};
			if (lightmap && routing.source == STREAM_SRC_TEXCOORD_0 && routing.dest == zonetool::s2::STREAM_DST_TEXCOORD_0 && (routing.mask & 0xC))
			{
				stream.routing.mask &= 0x3;
				streams.emplace_back(decl_stream{{zonetool::s2::STREAM_SRC_LIGHTMAP, zonetool::s2::STREAM_DST_TEXCOORD_15, 0x3}, "lmtf"});
			}

			streams.emplace_back(stream);
		}

		if (wind)
		{
			const auto has_normal = std::ranges::any_of(streams, [](const decl_stream& stream)
			{
				return stream.routing.source == zonetool::s2::STREAM_SRC_NORMAL;
			});

			if (!has_normal)
			{
				streams.emplace_back(decl_stream{{zonetool::s2::STREAM_SRC_NORMAL, zonetool::s2::STREAM_DST_NORMAL, 0xF}, "nn"});
			}

			streams.emplace_back(decl_stream{{zonetool::s2::STREAM_SRC_WIND, zonetool::s2::STREAM_DST_COLOR_1, 0xF}, "ec1"});
		}

		auto* result = native_allocator.allocate<zonetool::s2::MaterialVertexDeclaration>();
		if (streams.size() > std::size(result->routing.data))
		{
			throw std::runtime_error(utils::string::va("vertexdecl \"%s\" has too many streams", decl->name));
		}

		std::ranges::stable_sort(streams, {}, [](const decl_stream& stream)
		{
			return stream.routing.source;
		});

		std::vector<std::uint8_t> sources;
		for (const auto& stream : streams)
		{
			if (std::ranges::find(sources, stream.routing.source) == sources.end())
			{
				sources.emplace_back(stream.routing.source);
			}
		}

		std::ranges::stable_sort(sources, {}, stream_slot_priority);

		std::string name;
		for (auto i = 0u; i < streams.size(); i++)
		{
			auto& routing = streams[i].routing;
			routing.slot = static_cast<unsigned char>(std::ranges::find(sources, routing.source) - sources.begin());
			result->routing.data[i] = routing;
			name += utils::string::va("%s.%x.%u", streams[i].token.data(), routing.mask, routing.slot);
		}

		result->name = native_allocator.duplicate_string(name);
		decl_routing_names.emplace(result, name);
		result->streamCount = static_cast<unsigned char>(streams.size());
		result->hasOptionalSource = decl->hasOptionalSource;

		converted_decls.emplace(std::make_tuple(decl, wind, lightmap), result);
		return result;
	}

	bool has_lightmap_stream(const zonetool::s2::MaterialVertexDeclaration* decl)
	{
		return decl && std::any_of(decl->routing.data, decl->routing.data + decl->streamCount, [](const zonetool::s2::MaterialStreamRouting& routing)
		{
			return routing.source == zonetool::s2::STREAM_SRC_LIGHTMAP && routing.dest == zonetool::s2::STREAM_DST_TEXCOORD_15;
		});
	}

	bool has_vertex_lighting_stream(const zonetool::s2::MaterialVertexDeclaration* decl)
	{
		return decl && std::any_of(decl->routing.data, decl->routing.data + decl->streamCount, [](const zonetool::s2::MaterialStreamRouting& routing)
		{
			return routing.source >= zonetool::s2::STREAM_SRC_VERTEX_LIGHTING_VISIBILITY && routing.source <= zonetool::s2::STREAM_SRC_VERTEX_LIGHTING_HIGHLIGHT;
		});
	}

	void name_decls(const std::vector<zonetool::s2::MaterialTechniqueSet*>& pending_techsets, std::unordered_set<const void*>& global_shaders)
	{
		std::unordered_map<zonetool::s2::MaterialVertexDeclaration*, std::set<std::string>> vertex_shaders;
		for (const auto* asset : pending_techsets)
		{
			for (const auto* technique : asset->techniques)
			{
				for (auto i = 0u; technique && i < technique->hdr.passCount; i++)
				{
					const auto& pass = technique->passArray[i];
					if (pass.vertexDecl && pass.vertexShader && pass.vertexShader->name && !global_shaders.contains(pass.vertexDecl))
					{
						vertex_shaders[pass.vertexDecl].emplace(pass.vertexShader->name);
					}
				}
			}
		}

		for (auto& [decl, shaders] : vertex_shaders)
		{
			const auto routing = decl_routing_names.find(decl);
			if (routing == decl_routing_names.end())
			{
				continue;
			}

			if (stock_zone::is_stock_asset(zonetool::s2::ASSET_TYPE_VERTEXDECL, routing->second.data()))
			{
				decl->name = native_allocator.duplicate_string(routing->second);
				global_shaders.insert(decl);
				continue;
			}

			auto key = routing->second;
			for (const auto& shader : shaders)
			{
				key += "|" + shader;
			}

			const auto hash = utils::cryptography::jenkins_one_at_a_time::compute(key);
			decl->name = native_allocator.duplicate_string(std::format("{}_{:08x}", routing->second, hash));
		}
	}
}
