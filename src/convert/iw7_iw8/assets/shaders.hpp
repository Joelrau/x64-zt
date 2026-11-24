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
	float4 camera_position;
};

cbuffer frame_constants : register(b7)
{
	float4 frame_rows_0[42];
	float4 exposure;
	float4 lighting_scale;
	float4 frame_rows_44[4];
	float4 probe_cluster_depths;
	float4 probe_cluster_depth_scales;
	float4 probe_cluster_tiles;
	float4 probe_cluster_tile_offset;
	int4 probe_cluster_words;
	float4 frame_row_53;
	float4 screen_size;
	float4 frame_row_55;
	float4 sun_direction;
	float4 sun_color;
	float4 frame_rows_58[25];
	float4 sun_shadow_tiles;
	float4 frame_rows_84[102];
	float4 reflection_probe_lerp;
	float4 frame_rows_187[25];
	float4 lightmap_atlas;
};

cbuffer material_constants : register(b3)
{
	float4 color_tint;
	float4 uv_anim;
	float4 ocean_scroll_normal0;
	float4 ocean_scroll_normal1;
	float4 falloff_parms;
	float4 falloff_begin_color;
	float4 falloff_end_color;
	float4 feather_parms;
	float4 eye_offset_parms;
};

#ifdef BRUSH_MODEL
cbuffer brush_placement_constants : register(b0)
{
	float4 brush_placement[3];
};
#endif

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

struct reflection_probe
{
	float3 origin;
	uint slice;
	float3 fade;
	float4 rotation;
	float2 box_x;
	float2 box_y;
	float2 box_z;
};

struct reflection_probe_volume
{
	float3 center;
	float3 axis_x;
	float3 axis_y;
	float3 axis_z;
	float3 half_size;
};

StructuredBuffer<reflection_probe> reflection_probes : register(t44);
StructuredBuffer<reflection_probe_volume> reflection_probe_volumes : register(t45);
StructuredBuffer<uint> reflection_probe_masks : register(t46);
StructuredBuffer<uint> reflection_probe_zbins : register(t47);
Texture3D specular_occlusion : register(t121);
Texture2D env_brdf : register(t123);
Texture2DArray reflection_probe_array : register(t124);
SamplerState reflection_probe_sampler : register(s11);

Texture2D model_color_map : register(t0);
Texture2D world_color_map : register(t1);
Texture2D alpha_map : register(t2);
Texture2D normal_gloss_map : register(t3);
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

struct water_vertex
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
	float2 lightmap_uv : TEXCOORD1;
	float3 normal : NORMAL;
	float4 tangent : TANGENT;
	float3 view : TEXCOORD2;
	float4 color : COLOR;
};

struct effect_vertex
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
	float4 color : COLOR;
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

float3 frame_tangent(float4 q)
{
	return float3(1.0 - 2.0 * (q.y * q.y + q.z * q.z), 2.0 * (q.x * q.y + q.z * q.w), 2.0 * (q.x * q.z - q.y * q.w));
}

float frame_sign(uint packed)
{
	return packed & (1u << 29) ? -1.0 : 1.0;
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

#ifdef BRUSH_MODEL
float3 brush_rotate(float3 v)
{
	return float3(dot(brush_placement[0].xyz, v), dot(brush_placement[1].xyz, v), dot(brush_placement[2].xyz, v));
}

float3 world_position(world_surface surface, uint vertex_id)
{
	const int3 origin = int3(asint(brush_placement[0].w), asint(brush_placement[1].w), asint(brush_placement[2].w));
	return brush_rotate(asfloat(world_positions.Load3(surface.xyz_offset + 12 * vertex_id))) + float3(origin - camera_offset.xyz) / 4096.0;
}
#else
float3 world_position(world_surface surface, uint vertex_id)
{
	return asfloat(world_positions.Load3(surface.xyz_offset + 12 * vertex_id)) + float3(-camera_offset.xyz) / 4096.0;
}
#endif

float3 world_normal(world_surface surface, uint vertex_id)
{
	const float3 normal = frame_normal(decode_tangent_frame(world_vertex_data.Load(surface.tangent_frame_offset + 4 * vertex_id)));
#ifdef BRUSH_MODEL
	return brush_rotate(normal);
#else
	return normal;
#endif
}

float2 world_uv(world_surface surface, uint vertex_id)
{
	return asfloat(world_vertex_data.Load2(surface.tex_coord_offset + surface.layer_count * 8 * vertex_id));
}

float2 world_lightmap_uv(world_surface surface, uint vertex_id)
{
	return asfloat(world_vertex_data.Load2(surface.lmap_coord_offset + 8 * vertex_id));
}

float4 world_color(world_surface surface, uint vertex_id)
{
	const uint packed = world_vertex_data.Load(surface.color_offset + 4 * vertex_id);
	return float4(packed & 255, (packed >> 8) & 255, (packed >> 16) & 255, packed >> 24) / 255.0;
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

uint smodel_tangent_frame(smodel_vertex vertex)
{
	return smodel_vertices.Load(0x2BC0004 + vertex.index * 12);
}

float3 smodel_normal(smodel_vertex vertex)
{
	const float4 frame = decode_tangent_frame(smodel_tangent_frame(vertex));
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

float3 ambient_light()
{
	return sun_color.rgb * 0.35; // TODO: light grid
}

float3 world_ambient_light(float2 lightmap_uv)
{
	if (world_draw.y >= 510)
	{
		return ambient_light();
	}

	const uint2 record = lightmap_records[world_draw.y];
	const float2 size = float2(record.y & 0xFFFF, record.y >> 16);
	const float2 origin = float2(record.x & 8191, (record.x >> 13) & 8191);
	const float3 coord = float3((lightmap_uv * size + origin) * lightmap_atlas.y, record.x >> 26);
	return lightmap_irradiance.SampleLevel(lightmap_sampler, coord, 0).rgb * lighting_scale.z;
}

float3 light_surface(float3 albedo, float3 normal, float4 position)
{
	return albedo * (ambient_light() + sun_light(normal, position)) * exposure.x;
}

float3 light_lightmapped_surface(float3 albedo, float3 normal, float4 position, float2 lightmap_uv)
{
	return albedo * (world_ambient_light(lightmap_uv) + sun_light(normal, position)) * exposure.x;
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

water_vertex world_water_vs(uint vertex_id : SV_VertexID)
{
	const world_surface surface = world_surfaces[world_draw.x];
	const float3 position = world_position(surface, vertex_id);
	precise float4 clip = clip_position(position);
	const uint frame = world_vertex_data.Load(surface.tangent_frame_offset + 4 * vertex_id);
	const float4 rotation = decode_tangent_frame(frame);

	water_vertex output;
	output.position = clip;
	output.uv = animate_uv(world_uv(surface, vertex_id));
	output.lightmap_uv = world_lightmap_uv(surface, vertex_id);
	output.normal = frame_normal(rotation);
	output.tangent = float4(frame_tangent(rotation), frame_sign(frame));
	output.view = position;
	output.color = world_color(surface, vertex_id);
	return output;
}

water_vertex smodel_water_vs(uint vertex_id : SV_VertexID)
{
	const smodel_vertex vertex = get_smodel_vertex(vertex_id);
	const float3 position = smodel_world_position(vertex);
	precise float4 clip = clip_position(position);
	const uint frame = smodel_tangent_frame(vertex);
	const float4 rotation = decode_tangent_frame(frame);
	const float4 instance_rotation = smodel_rotation(smodel_instances[vertex.instance]);

	water_vertex output;
	output.position = clip;
	output.uv = animate_uv(smodel_uv(vertex));
	output.lightmap_uv = 0.0;
	output.normal = rotate(instance_rotation, frame_normal(rotation));
	output.tangent = float4(rotate(instance_rotation, frame_tangent(rotation)), frame_sign(frame));
	output.view = position;
	output.color = 1.0; // TODO: smodel vertex color
	return output;
}

sky_vertex world_sky_vs(uint vertex_id : SV_VertexID)
{
	const float3 position = world_position(world_surfaces[world_draw.x], vertex_id);
	precise float4 clip = clip_position(position);

	sky_vertex output;
	output.position = float4(clip.xy, 0.0, clip.w);
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
)"
	R"(
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

float3 unlit_evc_surface(float3 albedo)
{
	return albedo * color_tint.rgb; // TODO: IW7 TONEMAP_PARMS.x is not proven to be the inverse exposure
}

float4 world_unlit_ps(lit_vertex input) : SV_TARGET0
{
	return float4(unlit_surface(world_albedo(input)), 1.0);
}

float4 smodel_unlit_ps(lit_vertex input) : SV_TARGET0
{
	return float4(unlit_surface(smodel_albedo(input)), 1.0);
}

float4 world_unlit_evc_ps(lit_vertex input) : SV_TARGET0
{
	return float4(unlit_evc_surface(world_albedo(input)), 1.0);
}

float4 smodel_unlit_evc_ps(lit_vertex input) : SV_TARGET0
{
	return float4(unlit_evc_surface(smodel_albedo(input)), 1.0);
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

float4 world_unlit_blend_evc_ps(lit_vertex input) : SV_TARGET0
{
	const float alpha = material_alpha(input.uv);
	return float4(unlit_evc_surface(world_albedo(input)) * alpha, alpha);
}

float4 smodel_unlit_blend_evc_ps(lit_vertex input) : SV_TARGET0
{
	const float alpha = material_alpha(input.uv);
	return float4(unlit_evc_surface(smodel_albedo(input)) * alpha, alpha);
}

float4 world_unlit_add_ps(lit_vertex input) : SV_TARGET0
{
	return float4(unlit_surface(world_albedo(input)) * material_alpha(input.uv), 0.0);
}

float4 smodel_unlit_add_ps(lit_vertex input) : SV_TARGET0
{
	return float4(unlit_surface(smodel_albedo(input)) * material_alpha(input.uv), 0.0);
}

float4 world_unlit_add_evc_ps(lit_vertex input) : SV_TARGET0
{
	return float4(unlit_evc_surface(world_albedo(input)) * material_alpha(input.uv), 0.0);
}

float4 smodel_unlit_add_evc_ps(lit_vertex input) : SV_TARGET0
{
	return float4(unlit_evc_surface(smodel_albedo(input)) * material_alpha(input.uv), 0.0);
}

float4 world_multiply_ps(lit_vertex input) : SV_TARGET0
{
	return float4(lerp(1.0, world_albedo(input), material_alpha(input.uv)), 1.0);
}

float4 smodel_multiply_ps(lit_vertex input) : SV_TARGET0
{
	return float4(lerp(1.0, smodel_albedo(input), material_alpha(input.uv)), 1.0);
}

float max_component(float3 value)
{
	return max(max(value.x, value.y), value.z);
}

float2 water_normal_layer(float2 uv, float4 scroll, float mip_count)
{
	const float2 layer_uv = uv * scroll.y + game_time.w * scroll.zw;
	const float2 normal = normal_gloss_map.Sample(material_sampler, layer_uv).yw * 2.0 - 1.0;
	const float weight = saturate((mip_count - 1.0 - normal_gloss_map.CalculateLevelOfDetail(material_sampler, layer_uv)) * 0.5);
	return normal * weight * scroll.x;
}

float3 water_normal(water_vertex input, float facing)
{
	uint width, height, mip_count;
	normal_gloss_map.GetDimensions(0, width, height, mip_count);

	const float2 offset = water_normal_layer(input.uv, ocean_scroll_normal0, mip_count) + water_normal_layer(input.uv, ocean_scroll_normal1, mip_count);
	const float2 xy = float2(offset.x + offset.y, offset.x - offset.y) * 0.5;
	const float z = max(1.0 - abs(xy.x) - abs(xy.y), 0.0);
	const float3 bitangent = cross(input.normal, input.tangent.xyz) * input.tangent.w;
	return normalize((xy.x * input.tangent.xyz + xy.y * bitangent + z * input.normal) * facing);
}

float probe_box_distance(float3 reflection, float3 local, float3 axis, float2 box)
{
	const float direction = dot(reflection, -axis);
	const float offset = dot(local, -axis);
	return max((direction > 0.0 ? -(box.x + offset) : box.y + offset) / abs(direction), 0.0);
}

float3 sample_probe_level(float2 uv, uint slice, float mip)
{
	const float inset = float(4 << min(int(mip), 3));
	return reflection_probe_array.SampleLevel(reflection_probe_sampler, float3(uv * (1.0 - inset * 0.0078125) + inset * 0.00390625, slice), mip).rgb;
}

float3 sample_reflection_probes(float4 position, float3 view, float3 reflection, float gloss, float3 fallback)
{
	const float amount = saturate(gloss * reflection_probe_lerp.x + reflection_probe_lerp.y);
	if (amount >= 1.0)
	{
		return fallback;
	}

	const float luminance = dot(fallback, float3(0.2126, 0.7152, 0.0722));
	const float mip = floor(5.0 - 5.0 * gloss);
	const float mip_blend = frac(-5.0 * gloss);

	float4 depths;
	depths.x = position.w - 4.0;
	depths.y = depths.x - probe_cluster_depths.x;
	depths.z = depths.y - probe_cluster_depths.y;
	depths.w = depths.z - probe_cluster_depths.z;
	const uint slice = min(uint(dot(saturate(depths * probe_cluster_depth_scales), 0.25) * 32.0), 31);
	const uint2 tile = uint2(position.xy * resolution_scale.x * probe_cluster_tiles.zw);
	const uint2 counts = uint2((probe_cluster_tiles.xy + probe_cluster_tile_offset.xy - 1.0) * probe_cluster_tiles.zw);
	const uint cluster = (slice * counts.y + tile.y) * counts.x + tile.x;

	const uint zbin = reflection_probe_zbins[min(uint(max(position.w, 0.0)) >> 5, 8191)];
	const uint first_probe = zbin >> 16;
	const int probe_count = clamp(int(zbin & 0xFFFF) - int(first_probe) + 1, 0, 32);
	const uint last_word = min(uint(max(probe_cluster_words.z - 1, 0)), (zbin >> 5) & 0x7FF);

	float3 accumulated = 0.0;
	float remaining = 1.0;
	for (uint word = first_probe >> 5; word <= last_word; word++)
	{
		const uint base = word << 5;
		uint mask = reflection_probe_masks[cluster * uint(probe_cluster_words.z) + word];
		mask &= probe_count == 32 ? 0xFFFFFFFF : ~(0xFFFFFFFF << probe_count) << clamp(int(first_probe) - int(base), 0, 31);

		while (mask != 0)
		{
			const uint bit = firstbitlow(mask);
			mask ^= 1u << bit;

			const reflection_probe_volume volume = reflection_probe_volumes[base + bit];
			const reflection_probe probe = reflection_probes[base + bit];
			const float3 local = view - (volume.center - camera_position.xyz);
			const float3 projected = float3(dot(local, volume.axis_x), dot(local, volume.axis_y), dot(local, volume.axis_z));
			const float3 fade = (volume.half_size - abs(projected)) * probe.fade;
			const float weight = min(saturate(min(min(fade.z, fade.y), fade.x)), remaining);
			if (weight <= 0.0)
			{
				continue;
			}

			float3 direction = reflection;
			if (asuint(probe.origin.x) != 0xFFFFFFFF)
			{
				const float distance = min(min(probe_box_distance(reflection, local, volume.axis_z, probe.box_z),
					probe_box_distance(reflection, local, volume.axis_y, probe.box_y)), probe_box_distance(reflection, local, volume.axis_x, probe.box_x));
				direction = reflection * distance + view - probe.origin;
			}

			if (probe.rotation.w < 1.0)
			{
				const float3 twice_cross = 2.0 * cross(probe.rotation.xyz, direction);
				direction += probe.rotation.w * twice_cross + cross(probe.rotation.xyz, twice_cross);
			}

			const float2 uv = octahedron_uv(direction);
			const uint layer = probe.slice & 0xFFFF;
			accumulated += lerp(sample_probe_level(uv, layer, mip), sample_probe_level(uv, layer, mip + 1.0), mip_blend) * luminance * weight;
			remaining -= weight;
		}
	}

	return lerp(accumulated, fallback, amount);
}

float3 water_sun_light(float3 normal, float3 view, float n_dot_v, float roughness, float3 specular_color, float3 diffuse_color, float4 position)
{
	if (sun_direction.w == 0.0)
	{
		return 0.0;
	}

	const float3 half_vector = normalize(sun_direction.xyz - view);
	const float n_dot_l = dot(sun_direction.xyz, normal);
	const float v_dot_h = dot(-view, half_vector);
	const float n_dot_h = dot(normal, half_vector);
	const float3 light = sun_color.rgb * sun_visibility(position) * saturate(n_dot_l);

	const float distribution = n_dot_h * n_dot_h * (roughness - 1.0) + 1.0;
	const float visibility_v = n_dot_v + sqrt(n_dot_v * n_dot_v * (1.0 - roughness) + roughness);
	const float visibility_l = abs(n_dot_l) + sqrt(n_dot_l * n_dot_l * (1.0 - roughness) + roughness);
	const float specular = roughness / (distribution * distribution * visibility_v * visibility_l) * 1.039062; // TODO: IW7 t12 LUT
	const float3 fresnel = specular_color + (1.0 - specular_color) * pow(1.0 - abs(v_dot_h), 5.0);
	return light * (diffuse_color * (1.0 - max_component(fresnel)) + specular * fresnel);
}

float4 shade_water(water_vertex input, float4 color, float3 ambient, bool front)
{
	const float alpha = input.color.a;
	const float metalness = saturate(color.a - 0.1) * 1.111111;
	const float3 specular_color = metalness * color.rgb + min(color.a, 0.1);
	const float3 diffuse_root = sqrt(color.rgb * max(1.0 - metalness, 0.0)) * input.color.rgb;
	const float3 diffuse_color = diffuse_root * diffuse_root * color_tint.rgb;

	const float gloss = 1.0 - sqrt(1.0 - normal_gloss_map.Sample(material_sampler, input.uv).x);
	const float roughness = 2.0 / (exp2(gloss * 20.0) + 2.0);
	const float facing = front ? 1.0 : -1.0;
	const float3 normal = water_normal(input, facing);
	const float3 view = normalize(input.view);
	const float n_dot_v = saturate(dot(normal, -view));
	const float3 reflected = reflect(view, normal);

	const float2 brdf = env_brdf.SampleLevel(lightmap_sampler, float2(n_dot_v, gloss) * 0.984375 + 0.0078125, 0).xy;
	const float3 env_fresnel = specular_color * brdf.x + brdf.y;
	const float diffuse_fresnel = 1.0 + ((n_dot_v + 1.0) * (1.0 - gloss) - 0.5) * pow(1.0 - n_dot_v, 5.0);
	const float3 probe = sample_reflection_probes(input.position, input.view, reflected, gloss, ambient); // TODO: SSR
	const float occlusion = specular_occlusion.SampleLevel(lightmap_sampler, float3(0.0, roughness, saturate(dot(input.normal * facing, reflected))), 0).x;
	const float3 reflection = lerp(lerp(probe, ambient, 0.85) * 0.18, probe, occlusion);

	const float3 indirect = diffuse_color * ambient * diffuse_fresnel * (1.0 - max_component(env_fresnel)) + reflection * env_fresnel * lighting_scale.w;
	const float3 result = indirect + water_sun_light(normal, view, n_dot_v, roughness, specular_color, diffuse_color, input.position);
	if (alpha <= 6.1e-5)
	{
		discard;
	}

	return float4(min(result * alpha * exposure.x, 32255.0), alpha); // TODO: fog
}

float4 world_water_ps(water_vertex input, bool front : SV_IsFrontFace) : SV_TARGET0
{
	return shade_water(input, world_color_map.Sample(material_sampler, input.uv), world_ambient_light(input.lightmap_uv), front);
}

float4 smodel_water_ps(water_vertex input, bool front : SV_IsFrontFace) : SV_TARGET0
{
	return shade_water(input, model_color_map.Sample(material_sampler, input.uv), ambient_light(), front);
}

float4 sky_ps(sky_vertex input) : SV_TARGET0
{
	return float4(world_color_map.SampleLevel(material_sampler, octahedron_uv(input.direction), 0).rgb * exposure.x, 1.0);
}
)"
	R"(
effect_vertex make_effect_vertex(float3 position, float3 normal, float2 uv, float4 vertex_color, bool near_fade)
{
	position -= normalize(position) * eye_offset_parms.x;
	precise float4 clip = clip_position(position);
	const float n_dot_v = dot(normal, normalize(position));
	const float falloff = saturate(n_dot_v * n_dot_v * falloff_parms.z + falloff_parms.w);

	effect_vertex output;
	output.position = clip;
	output.uv = uv;
	output.color.rgb = falloff * vertex_color.rgb * lerp(falloff_end_color.rgb, falloff_begin_color.rgb, falloff);
	output.color.a = falloff * vertex_color.a * (near_fade ? saturate(clip.w * feather_parms.x) : 1.0);
	return output;
}

effect_vertex world_effect(uint vertex_id, bool near_fade)
{
	const world_surface surface = world_surfaces[world_draw.x];
	return make_effect_vertex(world_position(surface, vertex_id), world_normal(surface, vertex_id), world_uv(surface, vertex_id), world_color(surface, vertex_id), near_fade);
}

effect_vertex smodel_effect(uint vertex_id, bool near_fade)
{
	const smodel_vertex vertex = get_smodel_vertex(vertex_id);
	return make_effect_vertex(smodel_world_position(vertex), smodel_normal(vertex), smodel_uv(vertex), 1.0, near_fade); // TODO: smodel vertex color
}

effect_vertex world_effect_vs(uint vertex_id : SV_VertexID)
{
	return world_effect(vertex_id, false);
}

effect_vertex world_effect_fade_vs(uint vertex_id : SV_VertexID)
{
	return world_effect(vertex_id, true);
}

effect_vertex smodel_effect_vs(uint vertex_id : SV_VertexID)
{
	return smodel_effect(vertex_id, false);
}

effect_vertex smodel_effect_fade_vs(uint vertex_id : SV_VertexID)
{
	return smodel_effect(vertex_id, true);
}

float4 effect_add(float3 albedo, effect_vertex input)
{
	const float3 color = albedo * input.color.rgb * input.color.rgb * color_tint.rgb * exposure.x;
	const float alpha = material_alpha(input.uv) * input.color.a;
	if (max_component(color) <= 0.000061)
	{
		discard;
	}

	return float4(color * alpha, 0.0);
}

float4 world_effect_add_ps(effect_vertex input) : SV_TARGET0
{
	return effect_add(world_color_map.Sample(material_sampler, input.uv).rgb, input);
}

float4 smodel_effect_add_ps(effect_vertex input) : SV_TARGET0
{
	return effect_add(model_color_map.Sample(material_sampler, input.uv).rgb, input);
}

effect_vertex world_unlit_color_vs(uint vertex_id : SV_VertexID)
{
	const world_surface surface = world_surfaces[world_draw.x];
	precise float4 position = clip_position(world_position(surface, vertex_id));

	effect_vertex output;
	output.position = position;
	output.uv = world_uv(surface, vertex_id);
	output.color = world_color(surface, vertex_id);
	return output;
}

effect_vertex world_unlit_color_uv_anim_vs(uint vertex_id : SV_VertexID)
{
	effect_vertex output = world_unlit_color_vs(vertex_id);
	output.uv = animate_uv(output.uv);
	return output;
}

float3 world_vertex_albedo(effect_vertex input)
{
	return world_color_map.Sample(material_sampler, input.uv).rgb * input.color.rgb * input.color.rgb;
}

float4 world_unlit_add_color_ps(effect_vertex input) : SV_TARGET0
{
	return float4(unlit_surface(world_vertex_albedo(input)) * material_alpha(input.uv) * input.color.a, 0.0);
}

float4 world_unlit_add_color_evc_ps(effect_vertex input) : SV_TARGET0
{
	return float4(unlit_evc_surface(world_vertex_albedo(input)) * material_alpha(input.uv) * input.color.a, 0.0);
}

float4 world_multiply_color_ps(effect_vertex input) : SV_TARGET0
{
	return float4(lerp(1.0, world_vertex_albedo(input), material_alpha(input.uv) * input.color.a), 1.0);
}
)";
}
