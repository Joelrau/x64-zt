#include <std_include.hpp>

#include "../../include.hpp"
#include "decls.hpp"

namespace zonetool::s2::converter::h1::techset
{
	namespace
	{
		std::unordered_map<const MaterialVertexDeclaration*, converted_decl> converted_decls;

		std::uint8_t convert_stream_source(const std::uint8_t source)
		{
			if (source <= STREAM_SRC_TANGENT)
			{
				return source;
			}

			if (source < 9u)
			{
				throw std::runtime_error(utils::string::va("stream source %u has no h1 source", source));
			}

			return static_cast<std::uint8_t>(source - 3u);
		}

		std::uint32_t texcoord_index(const MaterialStreamRouting& routing)
		{
			if (routing.dest < STREAM_DST_TEXCOORD_0)
			{
				throw std::runtime_error(utils::string::va("stream source %u goes to dest %u", routing.source, routing.dest));
			}

			return routing.dest - STREAM_DST_TEXCOORD_0;
		}

		zonetool::h1::MaterialStreamRouting& find_stream(std::vector<zonetool::h1::MaterialStreamRouting>& streams, const std::uint8_t source,
			const std::uint8_t dest, const char* what)
		{
			const auto stream = std::ranges::find_if(streams, [&](const zonetool::h1::MaterialStreamRouting& routing)
			{
				return routing.source == source && routing.dest == dest;
			});

			if (stream == streams.end())
			{
				throw std::runtime_error(utils::string::va("vertexdecl has a %s stream without its h1 base stream", what));
			}

			return *stream;
		}
	}

	const converted_decl& convert_decl(const MaterialVertexDeclaration* decl, utils::memory::allocator& allocator)
	{
		if (const auto entry = converted_decls.find(decl); entry != converted_decls.end())
		{
			return entry->second;
		}

		converted_decl result{};
		if (!decl)
		{
			return converted_decls.emplace(decl, result).first->second;
		}

		if (decl->streamCount > std::size(decl->routing.data))
		{
			throw std::runtime_error(utils::string::va("vertexdecl \"%s\" has %u streams", decl->name, decl->streamCount));
		}

		std::vector<zonetool::h1::MaterialStreamRouting> streams;
		for (auto i = 0u; i < decl->streamCount; i++)
		{
			const auto& routing = decl->routing.data[i];
			if (routing.source != STREAM_SRC_LIGHTMAP && routing.source != STREAM_SRC_TEXEL_DENSITY)
			{
				streams.emplace_back(convert_stream_source(routing.source), routing.dest, routing.mask);
			}
		}

		for (auto i = 0u; i < decl->streamCount; i++)
		{
			const auto& routing = decl->routing.data[i];
			if (routing.source == STREAM_SRC_LIGHTMAP)
			{
				const auto has_texcoord = std::ranges::any_of(streams, [](const zonetool::h1::MaterialStreamRouting& stream)
				{
					return stream.source == STREAM_SRC_TEXCOORD_0 && stream.dest == STREAM_DST_TEXCOORD_0;
				});

				if (!has_texcoord)
				{
					streams.emplace_back(STREAM_SRC_TEXCOORD_0, STREAM_DST_TEXCOORD_0, 0x3);
				}

				auto& texcoord = find_stream(streams, STREAM_SRC_TEXCOORD_0, STREAM_DST_TEXCOORD_0, "lightmap");
				if (texcoord.mask != 0x3 || routing.mask != 0x3)
				{
					throw std::runtime_error(utils::string::va("lightmap stream mask 0x%X with texcoord mask 0x%X", routing.mask, texcoord.mask));
				}

				texcoord.mask = 0xF;
				result.redirects.emplace_back(texcoord_index(routing), "TEXCOORD", 0u, 2u);
			}
			else if (routing.source == STREAM_SRC_TEXEL_DENSITY)
			{
				auto& position = find_stream(streams, STREAM_SRC_POSITION, STREAM_DST_POSITION, "texel density");
				if (routing.mask != 0x1)
				{
					throw std::runtime_error(utils::string::va("texel density mask 0x%X", routing.mask));
				}

				position.mask |= 0x8;
				result.redirects.emplace_back(texcoord_index(routing), "SV_POSITION", 0u, 3u, true);
			}
		}

		auto* converted = allocator.allocate<zonetool::h1::MaterialVertexDeclaration>();
		converted->name = allocator.duplicate_string(::game::add_source_postfix(decl->name, ::game::s2));
		converted->streamCount = static_cast<unsigned char>(streams.size());
		converted->hasOptionalSource = decl->hasOptionalSource;
		std::ranges::copy(streams, converted->routing.data);
		result.decl = converted;

		return converted_decls.emplace(decl, std::move(result)).first->second;
	}

	void clear_decls()
	{
		converted_decls.clear();
	}
}
