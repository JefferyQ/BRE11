struct VS_INPUT {
	float4 PositionH : POSITION;
	float2 TexCoord : TEXCOORD0;
};

struct VS_OUTPUT {
	float4 PositionH : SV_Position;
	float2 TexCoord : TEXCOORD0;
};

VS_OUTPUT main(const VS_INPUT IN) {
	VS_OUTPUT OUT = (VS_OUTPUT)0;
	OUT.PositionH = IN.PositionH;
	OUT.TexCoord = IN.TexCoord;
	return OUT;
}