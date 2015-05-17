#define QUAD_VERTICES (4)

cbuffer cbPerFrame : register (b0) {
	float4x4 Proj;
	float FarClipPlaneDistance;
};

struct GSInput {
	nointerpolation float3 LightCenterPosVS : POSITION;
	nointerpolation float4 LightColorAndRadius : LIGHT_COLOR_AND_RADIUS;
};

struct GSOutput {
	float4 VertexPosCS : SV_POSITION;
	nointerpolation float3 LightPosVS : POSITION;
	nointerpolation float4 LightColorAndRadius : LIGHT_COLOR_AND_RADIUS;
	float3 ViewRayVS : VIEW_RAY;
};

[maxvertexcount(QUAD_VERTICES)]
void
main(const in point GSInput input[1], inout TriangleStream<GSOutput> triangleStream) {
	// Compute quad center position in view space.
	// Then we can easily build a quad (two triangles) that face the camera.
	const float4 lightCenterPosVS = float4(input[0].LightCenterPosVS, 1.0f);

	const float quadHalfSize = input[0].LightColorAndRadius.w;

	// Compute vertices positions and texture coordinates based on
	// a quad whose center position is lightCenterPosV
	GSOutput output;

	output.VertexPosCS = mul(lightCenterPosVS + float4(-quadHalfSize, quadHalfSize, 0.0f, 0.0f), Proj);
	output.LightPosVS = input[0].LightCenterPosVS;
	output.LightColorAndRadius = input[0].LightColorAndRadius;
	// Extrapolate the view space position to the far clip plane
	float3 vertexPosVS = lightCenterPosVS.xyz + float3(-quadHalfSize, quadHalfSize, 0.0f);
	output.ViewRayVS = float3(vertexPosVS.xy * (FarClipPlaneDistance / vertexPosVS.z), FarClipPlaneDistance);
	triangleStream.Append(output);

	output.VertexPosCS = mul(lightCenterPosVS + float4(quadHalfSize, quadHalfSize, 0.0f, 0.0f), Proj);
	output.LightPosVS = input[0].LightCenterPosVS;
	output.LightColorAndRadius = input[0].LightColorAndRadius;
	// Extrapolate the view space position to the far clip plane
	vertexPosVS = lightCenterPosVS.xyz + float3(quadHalfSize, quadHalfSize, 0.0f);
	output.ViewRayVS = float3(vertexPosVS.xy * (FarClipPlaneDistance / vertexPosVS.z), FarClipPlaneDistance);
	triangleStream.Append(output);

	output.VertexPosCS = mul(lightCenterPosVS + float4(-quadHalfSize, -quadHalfSize, 0.0f, 0.0f), Proj);
	output.LightPosVS = input[0].LightCenterPosVS;
	output.LightColorAndRadius = input[0].LightColorAndRadius;
	// Extrapolate the view space position to the far clip plane
	vertexPosVS = lightCenterPosVS.xyz + float3(-quadHalfSize, -quadHalfSize, 0.0f);
	output.ViewRayVS = float3(vertexPosVS.xy * (FarClipPlaneDistance / vertexPosVS.z), FarClipPlaneDistance);
	triangleStream.Append(output);

	output.VertexPosCS = mul(lightCenterPosVS + float4(quadHalfSize, -quadHalfSize, 0.0f, 0.0f), Proj);
	output.LightPosVS = input[0].LightCenterPosVS;
	output.LightColorAndRadius = input[0].LightColorAndRadius;
	// Extrapolate the view space position to the far clip plane
	vertexPosVS = lightCenterPosVS.xyz + float3(quadHalfSize, -quadHalfSize, 0.0f);
	output.ViewRayVS = float3(vertexPosVS.xy * (FarClipPlaneDistance / vertexPosVS.z), FarClipPlaneDistance);
	triangleStream.Append(output);

	triangleStream.RestartStrip();
}