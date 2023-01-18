#pragma once
#include "../zonetool.hpp"

#define TECHNIQUES_COUNT 240u

namespace zonetool::h1
{
	class ITechset : public IAsset
	{
	private:
		std::string name_;
		MaterialTechniqueSet* asset_ = nullptr;

		std::uintptr_t get_vertexdecl_pointer(std::string vertexdecl);
		void add_vertexdecl_pointer(std::string name, std::uintptr_t pointer);

	public:
		static std::unordered_map<std::string, std::uintptr_t> vertexdecl_pointers;

		MaterialTechniqueSet* parse(const std::string& name, ZoneMemory* mem);
		static void parse_stateinfo(const std::string& techset, Material* mat, ZoneMemory* mem);
		static void parse_statebits(const std::string& techset, unsigned char* statebits, ZoneMemory* mem);
		static void parse_statebitsmap(const std::string& techset, GfxStateBits** map, unsigned char* count,
			std::vector<std::array<std::uint64_t, 10>>*,
			std::vector<std::array<std::uint32_t, 3>>*,
			ZoneMemory* mem);
		static void parse_constant_buffer_indexes(const std::string& techset, unsigned char* indexes, ZoneMemory* mem);
		static void parse_constant_buffer_def_array(const std::string& techset, MaterialConstantBufferDef** def_ptr, unsigned char* count, ZoneMemory* mem);

		void init(const std::string& name, ZoneMemory* mem) override;
		void prepare(ZoneBuffer* buf, ZoneMemory* mem) override;
		void load_depending(IZone* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		void write(IZone* zone, ZoneBuffer* buffer) override;

		static void dump_stateinfo(const std::string& techset, Material* mat);
		static void dump_statebits(const std::string& techset, unsigned char* statebits);
		static void dump_statebits_map(const std::string& techset, GfxStateBits* map, unsigned char count);
		static void dump_constant_buffer_indexes(const std::string& techset, unsigned char* cbi);
		static void dump_constant_buffer_def_array(const std::string& techset, unsigned char count, MaterialConstantBufferDef* def);
		static void dump_technique(MaterialTechnique* asset);
		static void dump(MaterialTechniqueSet* asset);
	};
}