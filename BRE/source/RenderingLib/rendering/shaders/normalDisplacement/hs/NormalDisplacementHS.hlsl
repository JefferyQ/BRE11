/*******************  Data  *************************/
#define NUM_PATCH_POINTS 3

struct VS_OUTPUT {
	float4 PosOS : POSITION;
	float3 NormalOS : NORMAL;
	float2 TexCoord : TEXCOORD0;
	float3 TangentOS : TANGENT;
};

struct HS_CONSTANT_OUTPUT {
	float EdgeFactors[3] : SV_TessFactor;
	float InsideFactors : SV_InsideTessFactor;
};

struct HS_OUTPUT {
	float4 PosOS : POSITION;
	float3 NormalOS : NORMAL;
	float2 TexCoord : TEXCOORD0;
	float3 TangentOS : TANGENT;
};

/*******************  Resources  *************************/
cbuffer CBufferImmutable : register (b0) {
	// 3 edge factors + 1 inside factor
	float4 TessellationFactors;
}

/*******************  Shader  *************************/
HS_CONSTANT_OUTPUT constant_hull_shader(const InputPatch<VS_OUTPUT, NUM_PATCH_POINTS> patch, const uint patchID : SV_PrimitiveID) {
	HS_CONSTANT_OUTPUT OUT = (HS_CONSTANT_OUTPUT)0;

	OUT.EdgeFactors[0] = TessellationFactors.x;
	OUT.EdgeFactors[1] = TessellationFactors.y;
	OUT.EdgeFactors[2] = TessellationFactors.z;
	OUT.InsideFactors = TessellationFactors.w;

	return OUT;
}

[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(NUM_PATCH_POINTS)]
[patchconstantfunc("constant_hull_shader")]
HS_OUTPUT main(const InputPatch <VS_OUTPUT, NUM_PATCH_POINTS> patch, const uint controlPointID : SV_OutputControlPointID) {
	HS_OUTPUT OUT = (HS_OUTPUT)0;

	// Simply pass through
	OUT.PosOS = patch[controlPointID].PosOS;
	OUT.NormalOS = patch[controlPointID].NormalOS;
	OUT.TexCoord = patch[controlPointID].TexCoord;
	OUT.TangentOS = patch[controlPointID].TangentOS;

	return OUT;
}