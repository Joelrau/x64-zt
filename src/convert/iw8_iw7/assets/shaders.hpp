#pragma once

namespace zonetool::iw8::converter::iw7::shaders
{
	inline constexpr std::string_view common = R"(
cbuffer per_prim : register(b0)
{
	float4 world_rows[4];
};

cbuffer per_object : register(b1)
{
	float4 view_projection_rows[4];
};

cbuffer stable : register(b2)
{
	float4 world_matrix_eye_offset;
	float4 sun_light_dir;
	float4 sun_light_color;
};

cbuffer instances : register(b5)
{
	float4 instance_rows[512];
};

float4 to_world_rows(float3 position, float4 rows[4])
{
	const float4 p = float4(position, 1.0f);
	return float4(
		dot(p, float4(rows[0].xyz, rows[0].w - world_matrix_eye_offset.x)),
		dot(p, float4(rows[1].xyz, rows[1].w - world_matrix_eye_offset.y)),
		dot(p, float4(rows[2].xyz, rows[2].w - world_matrix_eye_offset.z)),
		dot(p, rows[3]));
}

float3 to_world_direction_rows(float3 direction, float4 rows[4])
{
	return float3(dot(direction, rows[0].xyz), dot(direction, rows[1].xyz), dot(direction, rows[2].xyz));
}

void get_instance_rows(uint instance, out float4 rows[4])
{
	[unroll] for (uint i = 0; i < 4; i++)
	{
		rows[i] = instance_rows[instance * 4 + i];
	}
}

float4 to_world(float3 position)
{
	return to_world_rows(position, world_rows);
}

float4 to_clip(float4 world_position)
{
	return float4(
		dot(world_position, view_projection_rows[0]),
		dot(world_position, view_projection_rows[1]),
		dot(world_position, view_projection_rows[2]),
		dot(world_position, view_projection_rows[3]));
}

float3 to_world_direction(float3 direction)
{
	return to_world_direction_rows(direction, world_rows);
}
)";

	inline constexpr std::string_view surface_material = R"(
cbuffer material : register(b3)
{
	float4 uv_transform;
	float4 uv_rotation;
	float4 color_tint;
	float4 surface_ranges;
	float4 decal_params;
};

float2 transform_uv(float2 uv)
{
	const float2 scaled = uv * uv_transform.xy;
	return float2(dot(uv_rotation.xy, scaled), dot(float2(-uv_rotation.y, uv_rotation.x), scaled)) + uv_transform.zw;
}
)";

	inline constexpr std::string_view position_only_vs = R"(
float4 main(float3 position : SV_POSITION) : SV_POSITION
{
	precise const float4 clip_position = to_clip(to_world(position));
	return clip_position;
}
)";

	inline constexpr std::string_view position_only_instanced_vs = R"(
float4 main(float3 position : SV_POSITION, uint instance : SV_InstanceID) : SV_POSITION
{
	float4 rows[4];
	get_instance_rows(instance, rows);
	precise const float4 clip_position = to_clip(to_world_rows(position, rows));
	return clip_position;
}
)";

	inline constexpr std::string_view unlit_vs_body = R"(
struct pixel_input
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
};

pixel_input shade_vertex(float3 position, float2 texcoord, float4 rows[4])
{
	pixel_input output;
	precise const float4 clip_position = to_clip(to_world_rows(position, rows));
	output.position = clip_position;
	output.texcoord = texcoord;
	return output;
}
)";

	inline constexpr std::string_view unlit_vs = R"(
pixel_input main(float3 position : SV_POSITION, float4 texcoord : TEXCOORD0)
{
	return shade_vertex(position, texcoord.xy, world_rows);
}
)";

	inline constexpr std::string_view unlit_instanced_vs = R"(
pixel_input main(float3 position : SV_POSITION, float4 texcoord : TEXCOORD0, uint instance : SV_InstanceID)
{
	float4 rows[4];
	get_instance_rows(instance, rows);
	return shade_vertex(position, texcoord.xy, rows);
}
)";

	inline constexpr std::string_view unlit_ps = R"(
Texture2D color_map : register(t0);
SamplerState color_sampler : register(s0);

float4 main(float4 position : SV_POSITION, float2 texcoord : TEXCOORD0) : SV_TARGET
{
	const float4 color = color_map.Sample(color_sampler, transform_uv(texcoord));
	return float4(color.rgb * color_tint.rgb, 1.0f);
}
)";

	inline constexpr std::string_view null_ps = R"(
void main(float4 position : SV_POSITION)
{
}
)";

	inline constexpr std::string_view lit_vs_body = R"(
struct vertex_input
{
	float3 position : SV_POSITION;
	float4 texcoord : TEXCOORD0;
	float3 normal : NORMAL;
	float4 tangent : TEXCOORD2;
};

struct pixel_input
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
	float3 normal : TEXCOORD1;
	float4 tangent : TEXCOORD2;
	float3 world_position : TEXCOORD3;
};

pixel_input shade_vertex(vertex_input input, float4 rows[4])
{
	pixel_input output;
	const float4 world_position = to_world_rows(input.position, rows);
	precise const float4 clip_position = to_clip(world_position);
	output.position = clip_position;
	output.world_position = world_position.xyz;
	output.texcoord = input.texcoord.xy;
	output.normal = normalize(to_world_direction_rows(normalize(input.normal * 2.0f - 1.0f), rows));
	output.tangent.xyz = normalize(to_world_direction_rows(normalize(input.tangent.xyz * 2.0f - 1.0f), rows));
	output.tangent.w = input.tangent.w > 0.0f ? -1.0f : 1.0f;
	return output;
}
)";

	inline constexpr std::string_view lit_vs = R"(
pixel_input main(vertex_input input)
{
	return shade_vertex(input, world_rows);
}
)";

	inline constexpr std::string_view lit_instanced_vs = R"(
pixel_input main(vertex_input input, uint instance : SV_InstanceID)
{
	float4 rows[4];
	get_instance_rows(instance, rows);
	return shade_vertex(input, rows);
}
)";

	inline constexpr std::string_view lit_ps_body = R"(
Texture2D color_map : register(t0);
Texture2D normal_map : register(t1);
SamplerState color_sampler : register(s0);
SamplerState normal_sampler : register(s1);

struct pixel_input
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
	float3 normal : TEXCOORD1;
	float4 tangent : TEXCOORD2;
	float3 world_position : TEXCOORD3;
};

float3 decode_normal(float4 texel)
{
	const float a = (texel.y - 0.5f) * surface_ranges.w * 2.0f;
	const float b = (texel.w - 0.5f) * surface_ranges.w * 2.0f;
	const float x = (a + b) * 0.5f;
	const float y = (a - b) * 0.5f;
	return normalize(float3(x, y, 1.0f - abs(x) - abs(y)));
}

float3 shade_surface(pixel_input input, float2 uv)
{
	const float4 color = color_map.Sample(color_sampler, uv);
	const float4 normal_texel = normal_map.Sample(normal_sampler, uv);

	const float3 albedo = color.rgb * color_tint.rgb;
	const float specular = color.a * surface_ranges.x + surface_ranges.y;
	const float gloss = saturate(normal_texel.x * surface_ranges.z + color_tint.w);

	const float3 tangent_normal = decode_normal(normal_texel);
	const float3 normal = normalize(input.normal);
	const float3 tangent = normalize(input.tangent.xyz);
	const float3 bitangent = cross(normal, tangent) * input.tangent.w;
	const float3 world_normal = normalize(tangent * tangent_normal.x + bitangent * tangent_normal.y + normal * tangent_normal.z);

	const float3 to_sun = normalize(sun_light_dir.xyz);
	const float3 to_eye = normalize(-input.world_position);
	const float3 half_vector = normalize(to_sun + to_eye);
	const float sun = saturate(dot(world_normal, to_sun));
	const float power = exp2(gloss * 11.0f + 1.0f);
	const float highlight = pow(saturate(dot(world_normal, half_vector)), power) * (power + 8.0f) / 25.0f;

	const float3 ambient = albedo * 0.25f;
	const float3 lit = (albedo + specular * highlight) * sun * sun_light_color.rgb;
	return ambient + lit;
}
)";

	inline constexpr std::string_view lit_ps = R"(
float4 main(pixel_input input) : SV_TARGET
{
	return float4(shade_surface(input, transform_uv(input.texcoord)), 1.0f);
}
)";

	inline constexpr std::string_view decal_ps = R"(
Texture2D alpha_reveal_map : register(t2);
SamplerState alpha_reveal_sampler : register(s2);

float get_decal_alpha(float2 uv)
{
	const float2 mask = alpha_reveal_map.Sample(alpha_reveal_sampler, uv).xy;
	const float reveal = decal_params.y != 0.0f ? saturate(mask.y * decal_params.z) : 1.0f;
	return reveal * (decal_params.x != 0.0f ? mask.x : 1.0f);
}

float4 main(pixel_input input) : SV_TARGET
{
	const float2 uv = transform_uv(input.texcoord);
	const float alpha = get_decal_alpha(uv);
	return float4(shade_surface(input, uv) * alpha, alpha);
}
)";

	inline constexpr std::string_view sky_vs = R"(
struct pixel_input
{
	float4 position : SV_POSITION;
	float3 direction : TEXCOORD0;
};

pixel_input main(float3 position : SV_POSITION)
{
	pixel_input output;
	const float4 world_position = to_world(position);
	precise const float4 clip_position = to_clip(world_position);
	output.position = clip_position;
	output.direction = world_position.xyz;
	return output;
}
)";

	inline constexpr std::string_view sky_ps = R"(
cbuffer material : register(b3)
{
	float4 sky_params;
};

Texture2D sky_map : register(t0);
SamplerState sky_sampler : register(s0);

float2 get_sky_uv(float3 direction)
{
	const bool hemisphere = (asuint(sky_params.y) & 1) != 0;
	const float z = hemisphere ? abs(direction.z) : direction.z;
	const float3 p = float3(direction.xy, z) / (abs(direction.x) + abs(direction.y) + abs(direction.z));
	const float a = z >= 0.0f ? p.y - p.x - 1.0f : p.x - p.y + 1.0f;
	const float2 uv = float2(hemisphere ? a * 0.5f + 1.0f : a * 0.25f + 0.5f, (p.x + p.y) * 0.5f + 0.5f);
	return hemisphere ? uv : uv * (1.0f - 2.0f * sky_params.zw) + sky_params.zw;
}

float4 main(float4 position : SV_POSITION, float3 direction : TEXCOORD0) : SV_TARGET
{
	return float4(sky_map.Sample(sky_sampler, get_sky_uv(direction)).rgb * sky_params.x, 0.0f);
}
)";
}
