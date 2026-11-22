#pragma once

#pragma warning( push )
#pragma warning( disable : 4459 )
#include <DirectXTex.h>
#pragma warning( pop )

namespace image_compress
{
	std::optional<DirectX::ScratchImage> compress(const DirectX::ScratchImage& source, DXGI_FORMAT format, DirectX::TEX_COMPRESS_FLAGS flags);
}
