#version 460
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_ray_tracing : require
//#extension GL_EXT_debug_printf : enable
#include "Material.glsl"

#define ITER_COUNT 15

layout(binding = 4) readonly buffer VertexArray { float Vertices[]; };
layout(binding = 5) readonly buffer IndexArray { uint Indices[]; };
layout(binding = 6) readonly buffer MaterialArray { Material[] Materials; };
layout(binding = 7) readonly buffer OffsetArray { uvec2[] Offsets; };
layout(binding = 8) uniform sampler2D[] TextureSamplers;
layout(binding = 12) readonly buffer MandelbulbArray { vec4[] Mandelbulbs; };

#include "Scatter.glsl"
#include "Vertex.glsl"

hitAttributeEXT vec4 Sphere;
rayPayloadInEXT RayPayload Ray;

vec2 GetSphereTexCoord(const vec3 point)
{
	const float phi = atan(point.x, point.z);
	const float theta = asin(point.y);
	const float pi = 3.1415926535897932384626433832795;

	return vec2
	(
		(phi + pi) / (2* pi),
		1 - (theta + pi /2) / pi
	);
}

float _fact(float x)
{
	float r = 1.;

	for (float k = 1.; k <= x; k = k + 1.)
	{
		r = r * k;
	}

	return r;
}

float _cos(float x)
{
	float r = 0.;

	for (int i = 0; i < ITER_COUNT; i++)
	{
		float n = i;
		r = r + pow(x, (2.*n)) * pow(-1., n)/_fact((2.*n));
	}

	return r;
}

float _sin(float x)
{
	float r = 0.;

	for (int i = 0; i < ITER_COUNT; i++)
	{
		float n = i;
		r = r + pow(x, (2.*n+1.)) * pow(-1., n)/_fact((2.*n+1.));
	}

	return r;
}

float _atan(float x)
{
	float r = 0.;

	for (int i = 0; i < ITER_COUNT; i++)
	{
		float n = i;
		r = r + pow(x, (2.*n+1.)) * pow(-1., n)/(2.*n+1.);
	}

	return r;
}

float _atan(float x, float y) { return _atan(x/y); }

float _log(float x)
{
	float r = 0.;

	for (int i = 0; i < ITER_COUNT; i++) 
	{
		float n = i;
		float l = 2.*n - 1.;
		r = r + 1./l * pow(((x - 1.) / (x + 1.)), l);
	}

	return 2. * r;
}

vec3 rotate( vec3 pos, float x, float y, float z )
{
	mat3 rotX = mat3( 1.0, 0.0, 0.0, 0.0, _cos( x ), -_sin( x ), 0.0, _sin( x ), _cos( x ) );
	mat3 rotY = mat3( _cos( y ), 0.0, _sin( y ), 0.0, 1.0, 0.0, -_sin(y), 0.0, _cos(y) );
	mat3 rotZ = mat3( _cos( z ), -_sin( z ), 0.0, _sin( z ), _cos( z ), 0.0, 0.0, 0.0, 1.0 );

	return rotX * rotY * rotZ * pos;
}

vec2 sphere_inter(in vec3 sc, in float sr, in vec3 ro, in vec3 rd)
{
	vec3 oc = ro - sc;
	float a = dot(rd, rd);
	float b = 2. * dot(oc, rd);
	float c = dot(oc, oc) - sr*sr;
	float d = b*b - 4.*a*c;
	float s = sqrt(d);
	if (d < 0.) return vec2(-1., -1.);
	return (-b + vec2(-s, s))/(2.*a);
}

float dist(in vec3 p)
{
	const float pi = 3.1415926535897932384626433832795;
	p = rotate(p, _sin(.25*pi), _cos(.25*pi), 0.);
	vec3 zn = vec3(p.xyz);
	float m = dot(zn, zn);
	float dz = 1.;

	vec4 tp = vec4(abs(zn), m);
	float h = 0.;

	float n = 8.;

	float th, phi, rad;
	float _th, _phi, _rad;

	for (int i = 0; i < 8; i++)
	{
		rad = length(zn);
		if (rad > 2.) 
		{
		    h = 0.25*_log(m)*sqrt(m)/dz;
		} 
		else 
		{
		    dz = 8.*pow(m,3.5)*dz + 1.;
		    
		    th = _atan(length(zn.xy), zn.z);
		    phi = _atan(zn.y, zn.x);

		    _rad = pow(rad, n);
		    _th = n * th;
		    _phi = n * phi;

		    zn.x = _rad * _sin(_th) * _cos(_phi);
		    zn.y = _rad * _sin(_th) * _sin(_phi);
		    zn.z = _rad * _cos(_th);
		    zn += p;
		}

		m = dot(zn, zn);
	}

	return h;
}

void main()
{
	//debugPrintfEXT("Cube chit\n");
	// Get the material.
	const uvec2 offsets = Offsets[gl_InstanceCustomIndexEXT];
	const uint indexOffset = offsets.x;
	const uint vertexOffset = offsets.y;
	const Vertex v0 = UnpackVertex(vertexOffset + Indices[indexOffset]);
	const Material material = Materials[v0.MaterialIndex];

	// Compute the ray hit point properties.
	const vec4 sphere = Mandelbulbs[gl_InstanceCustomIndexEXT];
	const vec3 center = sphere.xyz;
	const float radius = sphere.w;
	const vec3 point = gl_WorldRayOriginEXT + gl_HitTEXT * gl_WorldRayDirectionEXT;

	// use my own normal calc
	const vec3 eps = vec3(.01, 0., 0.);
	const vec3 n = normalize(vec3( dist( point + eps - center ) - dist( point - eps - center ),
	  			dist( point + eps.yxz - center ) - dist( point - eps.yxz - center ),
	  			dist( point + eps.zyx - center ) - dist( point - eps.zyx - center ) ));

	Ray = Scatter(material, gl_WorldRayDirectionEXT, n, v0.TexCoord, gl_HitTEXT, Ray.RandomSeed);


	//const vec3 normal = (point - center) / radius;
	//const vec2 texCoord = GetSphereTexCoord(normal);

	//Ray = Scatter(material, gl_WorldRayDirectionEXT, normal, texCoord, gl_HitTEXT, Ray.RandomSeed);
}
