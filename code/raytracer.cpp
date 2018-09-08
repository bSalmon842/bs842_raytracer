/*
Project: bs842 RayTracer
File: raytracer.cpp
Author: Brock Salmon
Notice: (C) Copyright 2018 by Brock Salmon. All Rights Reserved.
*/

/*
NOTE(bSalmon):

RAYCASTER_INTERNAL:
0 - Public Build
1 - Dev Build

RAYCASTER_SLOW:
 0 - Debugging Code Disabled
 1 - Non-performant debugging code enabled
*/

// Static Definitions
#define internal_func static
#define local_persist static
#define global_var static

// Typedefs
#include <stdint.h>
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef int32_t b32;

typedef float f32;
typedef double f64;

// NOTE(bSalmon): Utilities
#if URBAN_SLOW
#define ASSERT(expr) if(!(expr)) {*(int *)0 = 0;}
#else
#define ASSERT(expr)
#endif

#include <stdio.h>

struct Vector3
{
	f32 x;
	f32 y;
	f32 z;
};

struct Sphere
{
	Vector3 position;
	f32 radius;
};

struct Ray
{
	Vector3 origin;
	Vector3 direction;
};

// Return resultant vector of the subtraction of 2 vectors
Vector3 Vector3Sub(Vector3 *v1, Vector3 *v2)
{
	Vector3 result;
	result.x = v1->x - v2->x;
	result.y = v1->y - v2->y;
	result.z = v1->z - v2->z;
	return result;
}

// Return the scalar value of the dot product of 2 vectors
f32 Vector3Dot(Vector3 *v1, Vector3 *v2)
{
	f32 result = 0.0f;
	result += v1->x * v2->x;
	result += v1->y * v2->y;
	result += v1->z * v2->z;
	return result;
}

b32 RaySphereIntersection(Ray *ray, Sphere *sphere)
{
	f32 a = Vector3Dot(&ray->direction, &ray->direction);
	Vector3 distanceToSphere = Vector3Sub(&ray->origin, &sphere->position);
	f32 b = 2.0f * Vector3Dot(&ray->direction, &distanceToSphere);
	f32 c = Vector3Dot(&distanceToSphere, &distanceToSphere) - (sphere->radius * sphere->radius);
	
	f32 discriminant = (b * b) - (4 * (a * c));
	
	if (discriminant < 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

s32 main(s32 argc, u8** argv)
{
	Sphere baseSphere = {};
	Ray baseRay = {};
	
	// Sphere Info
	baseSphere.position.x = 20.0f;
	baseSphere.position.y = 20.0f;
	baseSphere.position.z = 20.0f;
	baseSphere.radius = 10.0f;
	
	// Ray Info
	baseRay.direction.z = 1.0f;
	
	for (s32 y = 0; y < 40; ++y)
	{
		baseRay.origin.y = y;
		
		for(s32 x = 0; x < 40; ++x)
		{
			baseRay.origin.x = x;
			
			if (RaySphereIntersection(&baseRay, &baseSphere))
			{
				printf("x");
			}
			else
			{
				printf(".");
			}
		}
		
		printf("\n");
	}
	
	return 0;
}