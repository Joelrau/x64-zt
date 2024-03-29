#pragma once
#include "../zonetool.hpp"

namespace zonetool::iw6
{
	class techset : public asset_interface
	{
	private:
		std::string name_;
		MaterialTechniqueSet* asset_ = nullptr;

		std::uintptr_t get_vertexdecl_pointer(std::string vertexdecl);
		void add_vertexdecl_pointer(std::string name, std::uintptr_t pointer);

	public:
		static std::unordered_map<std::string, std::uintptr_t> vertexdecl_pointers;

		MaterialTechniqueSet* parse(const std::string& name, zone_memory* mem);
		static void parse_stateinfo(const std::string& techset, const std::string& material, Material* mat, zone_memory* mem);
		static void parse_statebits(const std::string& techset, const std::string& material, unsigned char* statebits, zone_memory* mem);
		static void parse_statebitsmap(const std::string& techset, const std::string& material, GfxStateBits** map, unsigned char* count,
			std::vector<std::array<std::uint64_t, 11>>*,
			std::vector<std::array<std::uint32_t, 1>>*,
			zone_memory* mem);
		static void parse_constant_buffer_indexes(const std::string& techset, const std::string& material, unsigned char* indexes, zone_memory* mem);
		static void parse_constant_buffer_def_array(const std::string& techset, const std::string& material, 
			MaterialConstantBufferDef** def_ptr, unsigned char* count, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump_stateinfo(const std::string& techset, const std::string& material, Material* mat);
		static void dump_statebits(const std::string& techset, const std::string& material, unsigned char* statebits);
		static void dump_statebits_map(const std::string& techset, const std::string& material, GfxStateBits* map, unsigned char count);
		static void dump_constant_buffer_indexes(const std::string& techset, const std::string& material, unsigned char* cbi);
		static void dump_constant_buffer_def_array(const std::string& techset, const std::string& material, unsigned char count, MaterialConstantBufferDef* def);
		static void dump_technique(MaterialTechnique* asset);
		static void dump(MaterialTechniqueSet* asset);
	};
}