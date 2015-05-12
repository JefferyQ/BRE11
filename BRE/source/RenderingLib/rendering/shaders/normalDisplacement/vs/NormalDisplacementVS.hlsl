struct VS_INPUT {
	float4 PosL : POSITION;
	float2 TexCoord : TEXCOORD;
	float3 NormalL : NORMAL;
	float3 TangentL : TANGENT;
};

struct VS_OUTPUT {
	float4 PosL : POSITION;
	float3 NormalL : NORMAL;
	float2 TexCoord : TEXCOORD0;
	float3 TangentL : TANGENT;
};

VS_OUTPUT main(const VS_INPUT IN) {
	VS_OUTPUT OUT = (VS_OUTPUT)0;

	// Pass through local position
	OUT.PosL = IN.PosL;

	// Pass through local normal
	OUT.NormalL = IN.NormalL;

	// Pass through texture coordinates
	OUT.TexCoord = IN.TexCoord;

	// Pass throught tangent
	OUT.TangentL = IN.TangentL;

	return OUT;
}