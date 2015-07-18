#ifndef LIGHTING_HEADER
#define LIGHTING_HEADER

#define PI 3.14159f

struct DirectionalLight {
	float3 Color;
	float3 Direction;
};

struct PointLight {
	float3 Color;
	float3 Pos;
	float Range;
};

struct MaterialData {
	float3 BaseColor;
	float Smoothness;
	float3 Reflectance;
	float MetalMask;
};

float G_SmithGGXCorrelated(float NdotL, float NdotV, float alpha) {
	// Original formulation of G_SmithGGX Correlated
	// lambda_v = (-1 + sqrt ( alphaG2 * (1 - NdotL2 ) / NdotL2 + 1)) * 0.5 f;
	// lambda_l = (-1 + sqrt ( alphaG2 * (1 - NdotV2 ) / NdotV2 + 1)) * 0.5 f;
	// G_SmithGGXCorrelated = 1 / (1 + lambda_v + lambda_l );
	// V_SmithGGXCorrelated = G_SmithGGXCorrelated / (4.0 f * NdotL * NdotV );

	// This is the optimize version
	float alphaG2 = alpha * alpha;
	// Caution : the " NdotL *" and " NdotV *" are explicitely inversed , this is not a mistake .
	float Lambda_GGXV = NdotL * sqrt((-NdotV * alphaG2 + NdotV) * NdotV + alphaG2);
	float Lambda_GGXL = NdotV * sqrt((-NdotL * alphaG2 + NdotL) * NdotL + alphaG2);

	return 0.5f / (Lambda_GGXV + Lambda_GGXL);
}

float3 F_Schlick(const float3 f0, const float f90, in float dotLH) {
	return f0 + (f90 - f0) * pow(1.0f - dotLH, 5.0f);
}

float Fd_Disney(const float dotVN, const float dotLN, const float dotLH, float linearRoughness) {
	float energyBias = lerp(0, 0.5, linearRoughness);
	float energyFactor = lerp(1.0, 1.0 / 1.51, linearRoughness);
	float fd90 = energyBias + 2.0 * dotLH * dotLH * linearRoughness;
	float f0 = 1.0f;
	float lightScatter = F_Schlick(f0, fd90, dotLN).x;
	float viewScatter = F_Schlick(f0, fd90, dotVN).x;
	return lightScatter * viewScatter * energyFactor;
}

float D_GGX_TR(const float dotNH, const float alpha) {
	// Divide by PI is applied later
	const float alphaSqr = alpha * alpha;
	const float f = (dotNH * alphaSqr - dotNH) * dotNH + 1.0f;
	return alphaSqr / (f * f);
}

float3 brdf(const float3 N, const float3 V, const float3 L, const MaterialData data) {
	const float roughness = 1.0f - data.Smoothness;
	const float alpha = roughness * roughness;

	const float dotNV = saturate(dot(N, V));
	const float3 H = normalize(V + L);
	const float dotLH = saturate(dot(L, H));
	const float dotNH = saturate(dot(N, H));
	const float dotNL = saturate(dot(N, L));

	// Specular BRDF
	const float3 f0 = (1.0f - data.MetalMask) * data.Reflectance + data.BaseColor * data.MetalMask;
	const float3 F = F_Schlick(f0, 1.0f, dotLH); 
	const float G = G_SmithGGXCorrelated(dotNV, dotNL, alpha);
	const float D = D_GGX_TR(dotNH, alpha);
	const float3 Fr = F * G * D;

	// Diffuse BRDF
	const float Fd = Fd_Disney(dotNV, dotNL, dotLH, roughness);

	// Put all the parts together to generate the final color
	const float3 diffuseColor = (1.0f - data.MetalMask) * data.BaseColor;
	return dotNL * (Fr + diffuseColor * Fd) / PI;
}

#endif