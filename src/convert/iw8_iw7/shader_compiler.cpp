#include <std_include.hpp>
#include "shader_compiler.hpp"

#include <d3dcompiler.h>

namespace zonetool::iw8::converter::iw7::shader_compiler
{
	namespace
	{
		pD3DCompile get_compile_function()
		{
			static const auto function = []
			{
				const auto library = LoadLibraryW(L"d3dcompiler_47.dll");
				if (!library)
				{
					throw std::runtime_error("cannot load d3dcompiler_47.dll");
				}

				return reinterpret_cast<pD3DCompile>(GetProcAddress(library, "D3DCompile"));
			}();

			if (!function)
			{
				throw std::runtime_error("d3dcompiler_47.dll has no D3DCompile");
			}

			return function;
		}

		std::string blob_string(ID3DBlob* blob)
		{
			return blob ? std::string(static_cast<const char*>(blob->GetBufferPointer()), blob->GetBufferSize()) : std::string{};
		}
	}

	std::string compile(const std::string_view source, const char* name, const char* entry, const char* profile)
	{
		ID3DBlob* code{};
		ID3DBlob* errors{};
		const auto result = get_compile_function()(source.data(), source.size(), name, nullptr, nullptr, entry, profile,
			D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &code, &errors);

		const auto error_text = blob_string(errors);
		if (errors)
		{
			errors->Release();
		}

		if (FAILED(result))
		{
			throw std::runtime_error(std::format("cannot compile shader \"{}\": {}", name, error_text));
		}

		auto bytecode = blob_string(code);
		code->Release();
		return bytecode;
	}
}
