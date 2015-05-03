#define NUM_LIGHTS 512

struct VSInput {
	uint vertexId : SV_VertexID;
};

cbuffer CBufferPerFrame : register (b0) {
	float4 gLightPosWAndRadius[NUM_LIGHTS];
	float4 gLightColor[NUM_LIGHTS];
};

struct VSOutput {
	nointerpolation float3 mLightCenterPosW : POSITION;
	nointerpolation float4 mLightColorAndRadius : LIGHT_COLOR_AND_RADIUS;
};

VSOutput
main(in const VSInput input) {
	VSOutput output;
	output.mLightCenterPosW = gLightPosWAndRadius[input.vertexId].xyz;
	output.mLightColorAndRadius.xyz = gLightColor[input.vertexId].xyz;
	output.mLightColorAndRadius.w = gLightPosWAndRadius[input.vertexId].w;
	return output;
}