#pragma once

namespace zonetool::iw7::converter::iw8::shaders
{
	inline constexpr std::string_view source = R"(
cbuffer code_constants : register(b2)
{
	float4 view_projection[4];
	float4 code_row4;
	int4 camera_offset;
};

cbuffer frame_constants : register(b7)
{
	float4 frame_rows[56];
	float4 sun_direction;
	float4 sun_color;
};

cbuffer world_draw_constants : register(b9)
{
	uint4 world_draw;
};

struct world_surface
{
	uint transient_zone;
	uint layer_count;
	uint xyz_offset;
	uint tangent_frame_offset;
	uint lmap_coord_offset;
	uint color_offset;
	uint tex_coord_offset;
	uint normal_transform_offset[7];
	uint displacement_offset[8];
};

struct smodel_surface
{
	float3 position_offset;
	float position_scale;
	float4 unknown;
};

struct smodel_instance
{
	int3 origin;
	uint2 rotation;
	float scale;
};

struct smodel_vertex
{
	uint instance;
	uint surface;
	uint index;
};

StructuredBuffer<world_surface> world_surfaces : register(t13);
ByteAddressBuffer world_positions : register(t14);
ByteAddressBuffer world_vertex_data : register(t15);

ByteAddressBuffer smodel_draws : register(t63);
ByteAddressBuffer smodel_vertices : register(t64);
ByteAddressBuffer smodel_clusters : register(t65);
StructuredBuffer<smodel_surface> smodel_surfaces : register(t75);
StructuredBuffer<smodel_instance> smodel_instances : register(t76);

Texture2D model_color_map : register(t0);
Texture2D world_color_map : register(t1);
SamplerState material_sampler : register(s3);

struct prepass_vertex
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
};

struct lit_vertex
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
	float3 normal : NORMAL;
};

float4 decode_tangent_frame(uint packed)
{
	const float3 values = float3(packed & 1023, (packed >> 10) & 1023, (packed >> 20) & 511) * float3(2.0 / 1023.0, 2.0 / 1023.0, 2.0 / 511.0) - 1.0;
	const float3 frame = values * 0.707107;
	const float w = sqrt(1.0 - min(dot(frame, frame), 1.0));

	switch (packed >> 30)
	{
	case 0:
		return float4(w, frame);
	case 1:
		return float4(frame.x, w, frame.yz);
	case 2:
		return float4(frame.xy, w, frame.z);
	default:
		return float4(frame, w);
	}
}

float3 frame_normal(float4 q)
{
	return float3(2.0 * (q.x * q.z + q.y * q.w), 2.0 * (q.y * q.z - q.x * q.w), 1.0 - 2.0 * (q.x * q.x + q.y * q.y));
}

float3 rotate(float4 q, float3 v)
{
	const float3x3 rotation = float3x3(
		1.0 - 2.0 * (q.y * q.y + q.z * q.z), 2.0 * (q.x * q.y - q.z * q.w), 2.0 * (q.x * q.z + q.y * q.w),
		2.0 * (q.x * q.y + q.z * q.w), 1.0 - 2.0 * (q.x * q.x + q.z * q.z), 2.0 * (q.y * q.z - q.x * q.w),
		2.0 * (q.x * q.z - q.y * q.w), 2.0 * (q.y * q.z + q.x * q.w), 1.0 - 2.0 * (q.x * q.x + q.y * q.y));
	return mul(rotation, v);
}

float4 clip_position(float3 world)
{
	const float4 position = float4(world, 1.0);
	return float4(dot(position, view_projection[0]), dot(position, view_projection[1]), dot(position, view_projection[2]), dot(position, view_projection[3]));
}

float3 world_position(world_surface surface, uint vertex_id)
{
	return asfloat(world_positions.Load3(surface.xyz_offset + 12 * vertex_id)) + float3(-camera_offset.xyz) / 4096.0;
}

float3 world_normal(world_surface surface, uint vertex_id)
{
	return frame_normal(decode_tangent_frame(world_vertex_data.Load(surface.tangent_frame_offset + 4 * vertex_id)));
}

float2 world_uv(world_surface surface, uint vertex_id)
{
	return asfloat(world_vertex_data.Load2(surface.tex_coord_offset + surface.layer_count * 8 * vertex_id));
}

smodel_vertex get_smodel_vertex(uint vertex_id)
{
	const uint3 draw = smodel_draws.Load3((vertex_id >> 16) * 16);
	const uint cluster = smodel_clusters.Load((((vertex_id >> 6) & 1023) + draw.z) * 4);

	smodel_vertex vertex;
	vertex.instance = draw.x;
	vertex.surface = draw.y & 0xFFFF;
	vertex.index = (cluster << 6) | (vertex_id & 63);
	return vertex;
}

float4 smodel_rotation(smodel_instance instance)
{
	return float4(instance.rotation.x & 0xFFFF, instance.rotation.x >> 16, instance.rotation.y & 0xFFFF, instance.rotation.y >> 16) / 65535.0 * 2.0 - 1.0;
}

float3 smodel_local_position(smodel_vertex vertex)
{
	const uint2 packed = smodel_vertices.Load2(vertex.index * 8);
	const uint3 quantized = uint3(packed.x & 0x1FFFFF, (packed.x >> 21) | ((packed.y & 1023) << 11), (packed.y >> 10) & 0x1FFFFF);
	const smodel_surface surface = smodel_surfaces[vertex.surface];
	return (float3(quantized) / 2097151.0 * 2.0 - 1.0) * surface.position_scale + surface.position_offset;
}

float3 smodel_world_position(smodel_vertex vertex)
{
	const smodel_instance instance = smodel_instances[vertex.instance];
	const float3 origin = float3(instance.origin - camera_offset.xyz) / 4096.0;
	return rotate(smodel_rotation(instance), smodel_local_position(vertex)) * instance.scale + origin;
}

float3 smodel_normal(smodel_vertex vertex)
{
	const float4 frame = decode_tangent_frame(smodel_vertices.Load(0x2BC0004 + vertex.index * 12));
	return rotate(smodel_rotation(smodel_instances[vertex.instance]), frame_normal(frame));
}

float2 smodel_uv(smodel_vertex vertex)
{
	const uint packed = smodel_vertices.Load(0x2BC0008 + vertex.index * 12);
	return f16tof32(uint2(packed & 0xFFFF, packed >> 16));
}

float3 light_surface(float3 albedo, float3 normal)
{
	const float3 direction = sun_direction.w != 0.0 ? sun_direction.xyz : float3(0.0, 0.0, 1.0);
	return albedo * (0.4 + 0.6 * saturate(dot(normalize(normal), direction))); // TODO: first pass lighting
}

prepass_vertex world_prepass_vs(uint vertex_id : SV_VertexID)
{
	const world_surface surface = world_surfaces[world_draw.x];
	precise float4 position = clip_position(world_position(surface, vertex_id));

	prepass_vertex output;
	output.position = position;
	output.normal = world_normal(surface, vertex_id);
	return output;
}

float4 world_shadow_vs(uint vertex_id : SV_VertexID) : SV_POSITION
{
	precise float4 position = clip_position(world_position(world_surfaces[world_draw.x], vertex_id));
	return position;
}

lit_vertex world_lit_vs(uint vertex_id : SV_VertexID)
{
	const world_surface surface = world_surfaces[world_draw.x];
	precise float4 position = clip_position(world_position(surface, vertex_id));

	lit_vertex output;
	output.position = position;
	output.uv = world_uv(surface, vertex_id);
	output.normal = world_normal(surface, vertex_id);
	return output;
}

prepass_vertex smodel_prepass_vs(uint vertex_id : SV_VertexID)
{
	const smodel_vertex vertex = get_smodel_vertex(vertex_id);
	precise float4 position = clip_position(smodel_world_position(vertex));

	prepass_vertex output;
	output.position = position;
	output.normal = smodel_normal(vertex);
	return output;
}

float4 smodel_shadow_vs(uint vertex_id : SV_VertexID) : SV_POSITION
{
	precise float4 position = clip_position(smodel_world_position(get_smodel_vertex(vertex_id)));
	return position;
}

lit_vertex smodel_lit_vs(uint vertex_id : SV_VertexID)
{
	const smodel_vertex vertex = get_smodel_vertex(vertex_id);
	precise float4 position = clip_position(smodel_world_position(vertex));

	lit_vertex output;
	output.position = position;
	output.uv = smodel_uv(vertex);
	output.normal = smodel_normal(vertex);
	return output;
}

uint prepass_ps(prepass_vertex input) : SV_TARGET1
{
	const uint3 normal = uint3(saturate(normalize(input.normal) * 0.5 + 0.5) * 1023.0);
	return normal.x | (normal.y << 10) | (normal.z << 20);
}

float4 world_lit_ps(lit_vertex input) : SV_TARGET0
{
	return float4(light_surface(world_color_map.Sample(material_sampler, input.uv).rgb, input.normal), 1.0);
}

float4 smodel_lit_ps(lit_vertex input) : SV_TARGET0
{
	return float4(light_surface(model_color_map.Sample(material_sampler, input.uv).rgb, input.normal), 1.0);
}
)";
}
