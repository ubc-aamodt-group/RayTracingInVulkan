
struct RayPayload
{
	float valid;
	vec4 ColorAndDistance; // rgb + t
	vec4 ScatterDirection; // xyz + w (is scatter needed)
	uint RandomSeed;
};
