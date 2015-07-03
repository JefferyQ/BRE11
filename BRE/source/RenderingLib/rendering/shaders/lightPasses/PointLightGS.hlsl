#define QUAD_VERTICES (4)

struct Input {
	nointerpolation float3 LightCenterPosVS : POSITION;
	nointerpolation float4 LightColorAndRadius : LIGHT_COLOR_AND_RADIUS;
};

struct Output {
	float4 VertexPosCS : SV_POSITION;
	nointerpolation float3 LightPosVS : POSITION;
	nointerpolation float4 LightColorAndRadius : LIGHT_COLOR_AND_RADIUS;
	float3 VertexPosVS : POS_VIEW_SPACE;
};

cbuffer cbPerFrame : register (b0) {
	float4x4 Proj;
};

[maxvertexcount(QUAD_VERTICES)]
void main(const in point Input input[1], inout TriangleStream<Output> triangleStream) {
	// Compute quad center position in view space.
	// Then we can easily build a quad (two triangles) that face the camera.
	const float4 lightCenterPosVS = float4(input[0].LightCenterPosVS, 1.0f);

	const float quadHalfSize = input[0].LightColorAndRadius.w;

	// Compute vertices positions and texture coordinates based on
	// a quad whose center position is lightCenterPosV
	Output output = (Output)0;

	output.VertexPosVS = lightCenterPosVS.xyz + float3(-quadHalfSize, quadHalfSize, 0.0f);
	output.VertexPosCS = mul(float4(output.VertexPosVS, 1.0f), Proj);
	output.LightPosVS = input[0].LightCenterPosVS;
	output.LightColorAndRadius = input[0].LightColorAndRadius;
	triangleStream.Append(output);

	output.VertexPosVS = lightCenterPosVS.xyz + float3(quadHalfSize, quadHalfSize, 0.0f);
	output.VertexPosCS = mul(float4(output.VertexPosVS, 1.0f), Proj);
	output.LightPosVS = input[0].LightCenterPosVS;
	output.LightColorAndRadius = input[0].LightColorAndRadius;
	triangleStream.Append(output);

	output.VertexPosVS = lightCenterPosVS.xyz + float3(-quadHalfSize, -quadHalfSize, 0.0f);
	output.VertexPosCS = mul(float4(output.VertexPosVS, 1.0f), Proj);
	output.LightPosVS = input[0].LightCenterPosVS;
	output.LightColorAndRadius = input[0].LightColorAndRadius;
	triangleStream.Append(output);

	output.VertexPosVS = lightCenterPosVS.xyz + float3(quadHalfSize, -quadHalfSize, 0.0f);
	output.VertexPosCS = mul(float4(output.VertexPosVS, 1.0f), Proj);
	output.LightPosVS = input[0].LightCenterPosVS;
	output.LightColorAndRadius = input[0].LightColorAndRadius;
	triangleStream.Append(output);

	triangleStream.RestartStrip();
}