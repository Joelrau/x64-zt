#pragma once
#include "../h1.hpp"

namespace zonetool::h1
{
	class phys_collmap : public asset_interface
	{
	private:
		std::string name_;
		PhysCollmap* asset_ = nullptr;

	public:
		PhysCollmap* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read_polytope(zone_reader& reader, dmPolytopeData* polytope);
		static void read(zone_reader& reader, PhysCollmap* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(PhysCollmap* asset);
	};

	namespace polytope
	{
		struct vec3
		{
			double x;
			double y;
			double z;
		};

		struct plane_t
		{
			vec3 normal;
			double offset;
		};

		vec3 operator+(const vec3& a, const vec3& b);
		vec3 operator-(const vec3& a, const vec3& b);
		vec3 operator*(const vec3& a, double scale);
		double dot(const vec3& a, const vec3& b);
		vec3 cross(const vec3& a, const vec3& b);
		vec3 normalize(const vec3& a);
		vec3 to_vec3(const float* v);

		dmPolytopeData* make_polytope(const vec3& mins, const vec3& maxs, const std::vector<plane_t>& planes, bool material_arrays, utils::memory::allocator& allocator);
		void set_collmap_mass(PhysCollmap* asset);
	}
}
