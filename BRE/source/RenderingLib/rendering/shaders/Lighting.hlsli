#ifndef LIGHTING_HEADER
#define LIGHTING_HEADER

/******************** Directional Light  ***************************/

struct DirectionalLight {
	float3 mColor;
	float3 mDirection;
};

struct DirLightContributionData {
	// Surface material information
	float4 mSpecularColor; // 3 color + 1 power
	float3 mDiffuseColor;
	// Normalized surface normal
	float3 mNormal;
	DirectionalLight mLight;

	// View direction vector (from surface to camera/eye)
	float3 mViewDir;
};

// Computes contribution from material and directional light to diffuse & specular colors
void computeDirLightContribution(const in DirLightContributionData data, out float3 outColor) {
	// Compute normalized light direction from surface to light source.
	// Because there is not light source, then we simply negate it.
	const float3 nLightDir = -normalize(data.mLight.mDirection);

	// Compute normalized view direction
	const float3 nViewDir = normalize(data.mViewDir);

	// Compute light coefficient vector (1, diffuse, specular, 1)
	const float n_dot_l = dot(data.mNormal, nLightDir);
	const float3 halfVector = normalize(nLightDir + nViewDir);
	const float n_dot_h = dot(data.mNormal, halfVector);
	const float4 lightCoefficients = lit(n_dot_l, n_dot_h, data.mSpecularColor.w);

	// Material Diffuse Color * Diffuse Light Coefficient
	outColor = data.mDiffuseColor * lightCoefficients.y;

	// + Material Specular Color * Specular Light Coefficient
	outColor += data.mSpecularColor.rgb * lightCoefficients.z;

	// * Light Color
	outColor *= data.mLight.mColor;
}

/******************** Point Light  ***************************/

struct PointLight {
	float3 mColor;
	float3 mPosition;
	float mRange;
};

struct PointLightContributionData {
	// Surface material information
	float4 mSpecularColor; // 3 color + 1 power
	float3 mDiffuseColor;
	float3 mPosition;

	// Normalized surface normal
	float3 mNormal;

	PointLight mLight;

	// View direction vector (from surface to camera/eye)
	float3 mViewDir;
};

// Computes contribution from material and point light to diffuse & specular colors
void computePointLightContribution(const in PointLightContributionData data, out float3 outColor) {
	// Compute attenuation
	const float3 lightDir = data.mLight.mPosition - data.mPosition;
	const float dist = length(lightDir);
	const float attenuation = max(0.0f, 1.0f - (dist / data.mLight.mRange));

	// Compute normalized light direction from surface to light source.
	const float3 nLightDir = normalize(lightDir);

	// Compute normalized view direction
	const float3 nViewDir = normalize(data.mViewDir);

	// Compute light coefficient vector (1, diffuse, specular, 1)
	const float n_dot_l = dot(data.mNormal, nLightDir);
	const float3 halfVector = normalize(nLightDir + nViewDir);
	const float n_dot_h = dot(data.mNormal, halfVector);
	const float4 lightCoefficients = lit(n_dot_l, n_dot_h, data.mSpecularColor.w);

	// Material Diffuse Color * Light Color * Diffuse Light Coefficient * Attenuation
	outColor = data.mDiffuseColor * lightCoefficients.y;

	// + Material Specular Color * Light Color * Specular Light Coefficient * Attenuation
	outColor += data.mSpecularColor.rgb * lightCoefficients.z;

	// * Light Color * Attenuation
	outColor *= data.mLight.mColor * attenuation;
}


/******************** Spot Light  ***************************/
struct SpotLight {
	float3 mColor;
	float mInnerAngle;

	float3 mPosition;
	float mOuterAngle;

	float3 mDirection;
	float mRange;
};

struct SpotLightContributionData {
	// Surface material information
	float4 mSpecularColor; // 3 color + 1 power
	float3 mDiffuseColor;
	float3 mPosition;

	// Normalized surface normal
	float3 mNormal;

	SpotLight mLight;

	// View direction vector (from surface to camera/eye)
	float3 mViewDir;
};

// Computes contribution from material and spot light to diffuse & specular colors
void computeSpotLightContribution(const in SpotLightContributionData data, out float3 outColor) {
	// Compute attenuation (adding also the spotlight attenuation factor)
	const float3 lightDir = data.mLight.mPosition - data.mPosition;
	const float dist = length(lightDir);
	float attenuation = max(0.0f, 1.0f - (dist / data.mLight.mRange));

	// Compute normalized light direction from surface to light source.
	const float3 nLightDir = normalize(lightDir);

	// Compute normalized view direction
	const float3 nViewDir = normalize(data.mViewDir);

	// Adding the spotlight attenuation factor
	const float rho = dot(-nLightDir, normalize(data.mLight.mDirection));
	attenuation *= smoothstep(data.mLight.mOuterAngle, data.mLight.mInnerAngle, rho);

	// Compute light coefficient vector (1, diffuse, specular, 1)
	const float n_dot_l = dot(data.mNormal, nLightDir);
	const float3 halfVector = normalize(nLightDir + nViewDir);
	const float n_dot_h = dot(data.mNormal, halfVector);
	const float4 lightCoefficients = lit(n_dot_l, n_dot_h, data.mSpecularColor.w);

	// Material Diffuse Color * Diffuse Light Coefficient
	outColor = data.mDiffuseColor * lightCoefficients.y;

	// + Material Specular Color * Specular Light Coefficient
	outColor += data.mSpecularColor.rgb * lightCoefficients.z;

	// * Light Color * Attenuation
	outColor *= data.mLight.mColor * attenuation;
}

#endif