#define NUM_LIGHTS 512

struct VSInput {
	uint VertexId : SV_VertexID;
};

cbuffer CBufferPerFrame : register (b0) {
	float4 LightPosWAndRadius[NUM_LIGHTS];
	float4 LightColor[NUM_LIGHTS];
};

struct VSOutput {
	nointerpolation float3 LightCenterPosWS : POSITION;
	nointerpolation float4 LightColorAndRadius : LIGHT_COLOR_AND_RADIUS;
};

VSOutput
main(in const VSInput input) {
	VSOutput output;
	output.LightCenterPosWS = LightPosWAndRadius[input.VertexId].xyz;
	output.LightColorAndRadius.xyz = LightColor[input.VertexId].xyz;
	output.LightColorAndRadius.w = LightPosWAndRadius[input.VertexId].w;
	return output;
}