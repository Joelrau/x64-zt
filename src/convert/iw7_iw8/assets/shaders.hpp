#pragma once

namespace zonetool::iw7::converter::iw8::shaders
{
	inline constexpr std::string_view source = R"(
cbuffer code_constants : register(b2)
{
	float4 view_projection[4];
	float4 game_time;
	int4 camera_offset;
	float4 render_target_size;
	float4 resolution_scale;
};

cbuffer frame_constants : register(b7)
{
	float4 frame_rows_0[42];
	float4 exposure;
	float4 lighting_scale;
	float4 frame_rows_44[10];
	float4 screen_size;
	float4 frame_row_55;
	float4 sun_direction;
	float4 sun_color;
	float4 frame_rows_58[25];
	float4 sun_shadow_tiles;
	float4 frame_rows_84[128];
	float4 lightmap_atlas;
};

cbuffer material_constants : register(b3)
{
	float4 color_tint;
	float4 uv_anim;
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
Texture2D alpha_map : register(t2);
SamplerState material_sampler : register(s3);

StructuredBuffer<uint2> lightmap_records : register(t0);
Texture2DArray lightmap_irradiance : register(t114);
SamplerState lightmap_sampler : register(s8);

StructuredBuffer<uint2> screen_tiles : register(t5);
Texture2D sun_shadow_mask : register(t85);
SamplerState screen_sampler : register(s5);

struct prepass_vertex
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD0;
};

struct shadow_vertex
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

struct lit_vertex
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
	float2 lightmap_uv : TEXCOORD1;
	float3 normal : NORMAL;
};

struct sky_vertex
{
	float4 position : SV_POSITION;
	float3 direction : TEXCOORD0;
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

float2 world_lightmap_uv(world_surface surface, uint vertex_id)
{
	return asfloat(world_vertex_data.Load2(surface.lmap_coord_offset + 8 * vertex_id));
}

float2 animate_uv(float2 uv)
{
	const float time = game_time.w;
	float s, c;
	sincos(time * uv_anim.z, s, c);
	const float2 offset = uv - 0.5;
	return float2(c * offset.x - s * offset.y, c * offset.y + s * offset.x) + 0.5 + uv_anim.xy * time;
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

float sun_visibility(float4 position)
{
	const float2 pixel = position.xy * resolution_scale.x;
	if (sun_shadow_tiles.w > 0.0)
	{
		const uint2 tile = uint2(pixel) >> 3;
		if ((screen_tiles[tile.y * ((uint(screen_size.x) + 7) >> 3) + tile.x].y >> 28) == 11)
		{
			return 0.0;
		}
	}

	return abs(sun_shadow_mask.SampleLevel(screen_sampler, pixel * render_target_size.zw, 0).x);
}

float3 sun_light(float3 normal, float4 position)
{
	return sun_direction.w != 0.0 ? sun_color.rgb * saturate(dot(normalize(normal), sun_direction.xyz)) * sun_visibility(position) : 0.0;
}

float3 light_surface(float3 albedo, float3 normal, float4 position)
{
	return albedo * (sun_color.rgb * 0.35 + sun_light(normal, position)) * exposure.x; // TODO: light grid
}

float3 light_lightmapped_surface(float3 albedo, float3 normal, float4 position, float2 lightmap_uv)
{
	if (world_draw.y >= 510)
	{
		return light_surface(albedo, normal, position);
	}

	const uint2 record = lightmap_records[world_draw.y];
	const float2 size = float2(record.y & 0xFFFF, record.y >> 16);
	const float2 origin = float2(record.x & 8191, (record.x >> 13) & 8191);
	const float3 coord = float3((lightmap_uv * size + origin) * lightmap_atlas.y, record.x >> 26);

	const float3 irradiance = lightmap_irradiance.SampleLevel(lightmap_sampler, coord, 0).rgb * lighting_scale.z;
	return albedo * (irradiance + sun_light(normal, position)) * exposure.x;
}

prepass_vertex world_prepass_vs(uint vertex_id : SV_VertexID)
{
	const world_surface surface = world_surfaces[world_draw.x];
	precise float4 position = clip_position(world_position(surface, vertex_id));

	prepass_vertex output;
	output.position = position;
	output.normal = world_normal(surface, vertex_id);
	output.uv = world_uv(surface, vertex_id);
	return output;
}

float4 world_shadow_vs(uint vertex_id : SV_VertexID) : SV_POSITION
{
	precise float4 position = clip_position(world_position(world_surfaces[world_draw.x], vertex_id));
	return position;
}

shadow_vertex world_shadow_alpha_vs(uint vertex_id : SV_VertexID)
{
	const world_surface surface = world_surfaces[world_draw.x];
	precise float4 position = clip_position(world_position(surface, vertex_id));

	shadow_vertex output;
	output.position = position;
	output.uv = world_uv(surface, vertex_id);
	return output;
}

lit_vertex world_lit_vs(uint vertex_id : SV_VertexID)
{
	const world_surface surface = world_surfaces[world_draw.x];
	precise float4 position = clip_position(world_position(surface, vertex_id));

	lit_vertex output;
	output.position = position;
	output.uv = world_uv(surface, vertex_id);
	output.lightmap_uv = world_lightmap_uv(surface, vertex_id);
	output.normal = world_normal(surface, vertex_id);
	return output;
}

lit_vertex world_lit_uv_anim_vs(uint vertex_id : SV_VertexID)
{
	lit_vertex output = world_lit_vs(vertex_id);
	output.uv = animate_uv(output.uv);
	return output;
}

prepass_vertex smodel_prepass_vs(uint vertex_id : SV_VertexID)
{
	const smodel_vertex vertex = get_smodel_vertex(vertex_id);
	precise float4 position = clip_position(smodel_world_position(vertex));

	prepass_vertex output;
	output.position = position;
	output.normal = smodel_normal(vertex);
	output.uv = smodel_uv(vertex);
	return output;
}

float4 smodel_shadow_vs(uint vertex_id : SV_VertexID) : SV_POSITION
{
	precise float4 position = clip_position(smodel_world_position(get_smodel_vertex(vertex_id)));
	return position;
}

shadow_vertex smodel_shadow_alpha_vs(uint vertex_id : SV_VertexID)
{
	const smodel_vertex vertex = get_smodel_vertex(vertex_id);
	precise float4 position = clip_position(smodel_world_position(vertex));

	shadow_vertex output;
	output.position = position;
	output.uv = smodel_uv(vertex);
	return output;
}

lit_vertex smodel_lit_vs(uint vertex_id : SV_VertexID)
{
	const smodel_vertex vertex = get_smodel_vertex(vertex_id);
	precise float4 position = clip_position(smodel_world_position(vertex));

	lit_vertex output;
	output.position = position;
	output.uv = smodel_uv(vertex);
	output.lightmap_uv = 0.0;
	output.normal = smodel_normal(vertex);
	return output;
}

lit_vertex smodel_lit_uv_anim_vs(uint vertex_id : SV_VertexID)
{
	lit_vertex output = smodel_lit_vs(vertex_id);
	output.uv = animate_uv(output.uv);
	return output;
}

sky_vertex world_sky_vs(uint vertex_id : SV_VertexID)
{
	const float3 position = world_position(world_surfaces[world_draw.x], vertex_id);
	precise float4 clip = clip_position(position);

	sky_vertex output;
	output.position = clip;
	output.direction = position;
	return output;
}

float2 octahedron_uv(float3 direction)
{
	float2 uv = direction.xy / dot(abs(direction), 1.0);
	if (direction.z < 0.0)
	{
		uv = (1.0 - abs(uv.yx)) * (uv >= 0.0 ? 1.0 : -1.0);
	}

	return uv * 0.5 + 0.5;
}

float material_alpha(float2 uv)
{
	return alpha_map.Sample(material_sampler, uv).a;
}

uint pack_normal(float3 normal)
{
	const uint3 packed = uint3(saturate(normalize(normal) * 0.5 + 0.5) * 1023.0);
	return packed.x | (packed.y << 10) | (packed.z << 20);
}

uint prepass_ps(prepass_vertex input) : SV_TARGET1
{
	return pack_normal(input.normal);
}

uint prepass_alpha_ps(prepass_vertex input) : SV_TARGET1
{
	clip(material_alpha(input.uv) - 0.5);
	return pack_normal(input.normal);
}

void shadow_alpha_ps(shadow_vertex input)
{
	clip(material_alpha(input.uv) - 0.5);
}

float3 world_albedo(lit_vertex input)
{
	return world_color_map.Sample(material_sampler, input.uv).rgb;
}

float3 smodel_albedo(lit_vertex input)
{
	return model_color_map.Sample(material_sampler, input.uv).rgb;
}

float3 world_lit_color(lit_vertex input)
{
	return light_lightmapped_surface(world_albedo(input), input.normal, input.position, input.lightmap_uv);
}

float3 smodel_lit_color(lit_vertex input)
{
	return light_surface(smodel_albedo(input), input.normal, input.position);
}

float4 world_lit_ps(lit_vertex input) : SV_TARGET0
{
	return float4(world_lit_color(input), 1.0);
}

float4 smodel_lit_ps(lit_vertex input) : SV_TARGET0
{
	return float4(smodel_lit_color(input), 1.0);
}

float4 world_blend_ps(lit_vertex input) : SV_TARGET0
{
	const float alpha = material_alpha(input.uv);
	return float4(world_lit_color(input) * alpha, alpha);
}

float4 smodel_blend_ps(lit_vertex input) : SV_TARGET0
{
	const float alpha = material_alpha(input.uv);
	return float4(smodel_lit_color(input) * alpha, alpha);
}

float4 world_add_ps(lit_vertex input) : SV_TARGET0
{
	return float4(world_lit_color(input) * material_alpha(input.uv), 0.0);
}

float4 smodel_add_ps(lit_vertex input) : SV_TARGET0
{
	return float4(smodel_lit_color(input) * material_alpha(input.uv), 0.0);
}

float3 unlit_surface(float3 albedo)
{
	return albedo * color_tint.rgb * exposure.x;
}

float4 world_unlit_ps(lit_vertex input) : SV_TARGET0
{
	return float4(unlit_surface(world_albedo(input)), 1.0);
}

float4 smodel_unlit_ps(lit_vertex input) : SV_TARGET0
{
	return float4(unlit_surface(smodel_albedo(input)), 1.0);
}

float4 world_unlit_blend_ps(lit_vertex input) : SV_TARGET0
{
	const float alpha = material_alpha(input.uv);
	return float4(unlit_surface(world_albedo(input)) * alpha, alpha);
}

float4 smodel_unlit_blend_ps(lit_vertex input) : SV_TARGET0
{
	const float alpha = material_alpha(input.uv);
	return float4(unlit_surface(smodel_albedo(input)) * alpha, alpha);
}

float4 world_unlit_add_ps(lit_vertex input) : SV_TARGET0
{
	return float4(unlit_surface(world_albedo(input)) * material_alpha(input.uv), 0.0);
}

float4 smodel_unlit_add_ps(lit_vertex input) : SV_TARGET0
{
	return float4(unlit_surface(smodel_albedo(input)) * material_alpha(input.uv), 0.0);
}

float4 world_multiply_ps(lit_vertex input) : SV_TARGET0
{
	return float4(lerp(1.0, world_albedo(input), material_alpha(input.uv)), 1.0);
}

float4 smodel_multiply_ps(lit_vertex input) : SV_TARGET0
{
	return float4(lerp(1.0, smodel_albedo(input), material_alpha(input.uv)), 1.0);
}

float4 sky_ps(sky_vertex input) : SV_TARGET0
{
	return float4(world_color_map.SampleLevel(material_sampler, octahedron_uv(input.direction), 0).rgb * exposure.x, 1.0);
}
)";
}
