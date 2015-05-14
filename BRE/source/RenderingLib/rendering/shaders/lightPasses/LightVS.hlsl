struct VS_INPUT {
	float4 PosCS : POSITION;
};

struct VS_OUTPUT {
	float4 PosCS : SV_Position;
};

VS_OUTPUT main(const VS_INPUT IN) {
	VS_OUTPUT OUT = (VS_OUTPUT)0;

	// Pass through clip space position
	OUT.PosCS = IN.PosCS;

	return OUT;
}