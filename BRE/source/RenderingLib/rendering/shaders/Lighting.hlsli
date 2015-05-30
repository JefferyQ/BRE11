#ifndef LIGHTING_HEADER
#define LIGHTING_HEADER

#define PI 3.14159f

/******************** Directional Light  ***************************/

struct DirectionalLight {
	float3 Color;
	float3 Direction;
};

/******************** Point Light  ***************************/

struct PointLight {
	float3 Color;
	float3 Pos;
	float Range;
};

/******************** Spot Light  ***************************/
struct SpotLight {
	float3 Color;
	float InnerAngle;

	float3 PosWS;
	float OuterAngle;

	float3 DirectionWS;
	float Range;
};

struct MaterialData {
	float3 SpecularColor;
	float3 DiffuseColor;
	float Roughness;
	float ReflectanceAtNormalIncidence;
};

float G1V(float dotNV, float k)
{
	return 1.0f / (dotNV*(1.0f - k) + k);
}


float3 cook_torrance(const float3 N, const float3 V, const float3 L, const MaterialData data)
{
	const float3 cSpecular = data.SpecularColor;
	const float3 cDiffuse = data.DiffuseColor;

	const float alpha = data.Roughness * data.Roughness;

	const float3 H = normalize(V + L);

	const float dotNL = saturate(dot(N, L));
	const float dotNV = saturate(dot(N, V));
	const float dotNH = saturate(dot(N, H));
	const float dotLH = saturate(dot(L, H));
	
	// Specular D (Normal Distribution Function NDF)
	const float alphaSqr = alpha * alpha;
	float denom = dotNH * dotNH * (alphaSqr - 1.0) + 1.0f;
	const float D = alphaSqr / (PI * denom * denom);

	// Specular F (Fresnel)
	const float dotLH5 = pow(1.0f - dotLH, 5);
	const float F = data.ReflectanceAtNormalIncidence + (1.0 - data.ReflectanceAtNormalIncidence) * (dotLH5);

	// Specular G (Geometryc Attenuation)
	const float k = alpha / 2.0f;
	const float G = (1.0f / (dotNL * (1.0f - k) + k)) * (1.0f / (dotNV * (1.0f - k) + k));

	const float microfacetSpecular = dotNL * D * F * G;
	
	// Put all the parts together to generate
	// the final colour
	return max(0.0f, dotNL) * (cSpecular * microfacetSpecular + cDiffuse);
}

#endif