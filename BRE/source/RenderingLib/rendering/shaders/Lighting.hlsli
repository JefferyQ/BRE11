#ifndef LIGHTING_HEADER
#define LIGHTING_HEADER

/******************** Directional Light  ***************************/

struct DirectionalLight {
	float3 Color;
	float3 DirectionWS;
};

struct DirLightContributionData {
	DirectionalLight Light;

	// Surface material information
	float4 SpecularColor; // 3 color + 1 power
	float3 DiffuseColor;

	// Normalized surface normal
	float3 NormalWS;

	// View direction vector (from surface to camera/eye)
	float3 ViewDirWS;
};

// Computes contribution from material and directional light to diffuse & specular colors
void computeDirLightContribution(const in DirLightContributionData data, out float3 outColor) {
	// Compute normalized light direction from surface to light source.
	// Because there is not light source, then we simply negate it.
	const float3 nLightDirWS = -normalize(data.Light.DirectionWS);

	// Compute normalized view direction
	const float3 nViewDirWS = normalize(data.ViewDirWS);

	// Compute light coefficient vector (1, diffuse, specular, 1)
	const float n_dot_l = dot(data.NormalWS, nLightDirWS);
	const float3 halfVector = normalize(nLightDirWS + nViewDirWS);
	const float n_dot_h = dot(data.NormalWS, halfVector);
	const float4 lightCoefficients = lit(n_dot_l, n_dot_h, data.SpecularColor.w);

	// Material Diffuse Color * Diffuse Light Coefficient
	outColor = data.DiffuseColor * lightCoefficients.y;

	// + Material Specular Color * Specular Light Coefficient
	outColor += data.SpecularColor.rgb * lightCoefficients.z;

	// * Light Color
	outColor *= data.Light.Color;
}

/******************** Point Light  ***************************/

struct PointLight {
	float3 Color;
	float3 Pos;
	float Range;
};

struct PointLightContributionData {
	PointLight Light;

	// Surface material information
	float4 SpecularColor; // 3 color + 1 power
	float3 DiffuseColor;
	float3 Pos;

	// Normalized surface normal
	float3 Normal;

	// View direction vector (from surface to camera/eye)
	float3 ViewDir;
};

// Computes contribution from material and point light to diffuse & specular colors
void computePointLightContribution(const in PointLightContributionData data, out float3 outColor) {
	// Compute attenuation
	const float3 lightDir = data.Light.Pos - data.Pos;
	const float dist = length(lightDir);
	const float attenuation = max(0.0f, 1.0f - (dist / data.Light.Range));

	// Compute normalized light direction from surface to light source.
	const float3 nLightDir = normalize(lightDir);

	// Compute normalized view direction
	const float3 nViewDir = normalize(data.ViewDir);

	// Compute light coefficient vector (1, diffuse, specular, 1)
	const float n_dot_l = dot(data.Normal, nLightDir);
	const float3 halfVector = normalize(nLightDir + nViewDir);
	const float n_dot_h = dot(data.Normal, halfVector);
	const float4 lightCoefficients = lit(n_dot_l, n_dot_h, data.SpecularColor.w);

	// Material Diffuse Color * Light Color * Diffuse Light Coefficient * Attenuation
	outColor = data.DiffuseColor * lightCoefficients.y;

	// + Material Specular Color * Light Color * Specular Light Coefficient * Attenuation
	outColor += data.SpecularColor.rgb * lightCoefficients.z;

	// * Light Color * Attenuation
	outColor *= data.Light.Color * attenuation;
}

/******************** Spot Light  ***************************/
struct SpotLight {
	float3 Color;
	float InnerAngle;

	float3 PosWS;
	float OuterAngle;

	float3 DirectionWS;
	float Range;
};

struct SpotLightContributionData {
	SpotLight Light;

	// Surface material information
	float4 SpecularColor; // 3 color + 1 power
	float3 DiffuseColor;
	float3 PosWS;

	// Normalized surface normal
	float3 NormalWS;

	// View direction vector (from surface to camera/eye)
	float3 ViewDirWS;
};

// Computes contribution from material and spot light to diffuse & specular colors
void computeSpotLightContribution(const in SpotLightContributionData data, out float3 outColor) {
	// Compute attenuation (adding also the spotlight attenuation factor)
	const float3 lightDirWS = data.Light.PosWS - data.PosWS;
	const float dist = length(lightDirWS);
	float attenuation = max(0.0f, 1.0f - (dist / data.Light.Range));

	// Compute normalized light direction from surface to light source.
	const float3 nLightDirWS = normalize(lightDirWS);

	// Compute normalized view direction
	const float3 nViewDirWS = normalize(data.ViewDirWS);

	// Adding the spotlight attenuation factor
	const float rho = dot(-nLightDirWS, normalize(data.Light.DirectionWS));
	attenuation *= smoothstep(data.Light.OuterAngle, data.Light.InnerAngle, rho);

	// Compute light coefficient vector (1, diffuse, specular, 1)
	const float n_dot_l = dot(data.NormalWS, nLightDirWS);
	const float3 halfVector = normalize(nLightDirWS + nViewDirWS);
	const float n_dot_h = dot(data.NormalWS, halfVector);
	const float4 lightCoefficients = lit(n_dot_l, n_dot_h, data.SpecularColor.w);

	// Material Diffuse Color * Diffuse Light Coefficient
	outColor = data.DiffuseColor * lightCoefficients.y;

	// + Material Specular Color * Specular Light Coefficient
	outColor += data.SpecularColor.rgb * lightCoefficients.z;

	// * Light Color * Attenuation
	outColor *= data.Light.Color * attenuation;
}

#endif