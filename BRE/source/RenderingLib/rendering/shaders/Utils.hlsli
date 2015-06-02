#define UTILS_HEADER

// Helper functions to compress/decompress a normal vector
// using octahedron-normal vectors
float2 OctWrap(float2 v) {
	return (1.0f - abs(v.yx)) * (v.xy >= 0.0f ? 1.0f : -1.0f);
}

float2 OctEncode(float3 n) {
	n /= (abs(n.x) + abs(n.y) + abs(n.z));
	n.xy = n.z >= 0.0f ? n.xy : OctWrap(n.xy);
	n.xy = n.xy * 0.5f + 0.5f;
	return n.xy;
}

float3 OctDecode(float2 encN) {
	encN = encN * 2.0f - 1.0f;

	float3 n;
	n.z = 1.0f - abs(encN.x) - abs(encN.y);
	n.xy = n.z >= 0.0f ? encN.xy : OctWrap(encN.xy);
	n = normalize(n);
	return n;
}

// Map a normal from [-1.0f, 1.0f] to [0.0f, 1.0f]
float3 MapNormal(const float3 n) {
	return n * 0.5f + float3(0.5f, 0.5f, 0.5f);
}

// UnMap a normal from [0.0f, 1.0f] to [-1.0f, 1.0f]
float3 UnmapNormal(const float3 n) {
	return n * 2.0f - float3(1.0f, 1.0f, 1.0f);
}

