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

#define min(a,b) (((a) < (b)) ? (a) : (b))

#define WIDTH 800
#define HEIGHT 600

#include <stdio.h>
#include <math.h>

struct Vector3
{
	f32 x;
	f32 y;
	f32 z;
};

struct Ray
{
	Vector3 origin;
	Vector3 direction;
};

struct Colour
{
	f32 red;
	f32 green;
	f32 blue;
};

struct Light
{
	Vector3 position;
	Colour intensity;
};

struct Material
{
	Colour diffuse;
	f32 reflection;
};

struct Sphere
{
	Vector3 position;
	f32 radius;
	s32 material;
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

Vector3 Vector3Scale(Vector3 *vec, float scale)
{
	Vector3 result = {};
	result.x = vec->x * scale;
	result.y = vec->y * scale;
	result.z = vec->z * scale;
	return result;
}

Vector3 Vector3Add(Vector3 *v1, Vector3 *v2)
{
	Vector3 result = {};
	result.x = v1->x + v2->x;
	result.y = v1->y + v2->y;
	result.z = v1->z + v2->z;
	return result;
}

b32 RaySphereIntersection(Ray *ray, Sphere *sphere, f32 *closestIntersect)
{
	b32 result = false;
	
	f32 a = Vector3Dot(&ray->direction, &ray->direction);
	Vector3 distanceToSphere = Vector3Sub(&ray->origin, &sphere->position);
	f32 b = 2.0f * Vector3Dot(&ray->direction, &distanceToSphere);
	f32 c = Vector3Dot(&distanceToSphere, &distanceToSphere) - (sphere->radius * sphere->radius);
	
	f32 discriminant = (b * b) - (4 * (a * c));
	
	if (discriminant < 0)
	{
		result = false;
	}
	else
	{
		f32 sqrtDiscriminant = sqrtf(discriminant);
		f32 intersect0 = (-b + sqrtDiscriminant) / 2;
		f32 intersect1 = (-b - sqrtDiscriminant) / 2;
		
		if (intersect0 > intersect1)
		{
			intersect0 = intersect1;
		}
		
		if ((intersect0 > 0.001f) && (intersect0 < *closestIntersect))
		{
			*closestIntersect = intersect0;
			result = true;
		}
		else
		{
			result = false;
		}
	}
	
	return result;
}

void SavePPM(char *filename, u8 *imgData, s32 width, s32 height)
{
	FILE *f;
	fopen_s(&f, filename, "wb");
	fprintf(f, "P6 %d %d %d\n", width, height, 255);
	fwrite(imgData, 3, width * height, f);
	fclose(f);
}

s32 main(s32 argc, char* argv[])
{
	for (s32 i = 0; i < argc; ++i)
	{
		printf("%d - %s\n", i, argv[i]);
	}
	
	Ray baseRay = {};
	
	Material mats[3];
	
	mats[0].diffuse.red = 1.0f;
	mats[0].diffuse.green = 0.0f;
	mats[0].diffuse.blue = 0.0f;
	mats[0].reflection = 0.2f;
	
	mats[1].diffuse.red = 0.0f;
	mats[1].diffuse.green = 1.0f;
	mats[1].diffuse.blue = 0.0f;
	mats[1].reflection = 0.5f;
	
	mats[2].diffuse.red = 0.0f;
	mats[2].diffuse.green = 0.0f;
	mats[2].diffuse.blue = 1.0f;
	mats[2].reflection = 0.9f;
	
	Sphere spheres[3];
	
	spheres[0].position.x = 200.0f;
	spheres[0].position.y = 300.0f;
	spheres[0].position.z = 0.0f;
	spheres[0].radius = 100.0f;
	spheres[0].material = 0;
	
	spheres[1].position.x = 400.0f;
	spheres[1].position.y = 400.0f;
	spheres[1].position.z = 0.0f;
	spheres[1].radius = 100.0f;
	spheres[1].material = 1;
	
	spheres[2].position.x = 500.0f;
	spheres[2].position.y = 140.0f;
	spheres[2].position.z = 0.0f;
	spheres[2].radius = 100.0f;
	spheres[2].material = 2;
	
	Light lights[3];
	
	lights[0].position.x = 0.0f;
	lights[0].position.y = 240.0f;
	lights[0].position.z = -100.0f;
	lights[0].intensity.red = 1.0f;
	lights[0].intensity.green = 1.0f;
	lights[0].intensity.blue = 1.0f;
	
	lights[1].position.x = 3200.0f;
	lights[1].position.y = 3000.0f;
	lights[1].position.z = -1000.0f;
	lights[1].intensity.red = 0.6f;
	lights[1].intensity.green = 0.7f;
	lights[1].intensity.blue = 1.0f;
	
	lights[2].position.x = 600.0f;
	lights[2].position.y = 0.0f;
	lights[2].position.z = -100.0f;
	lights[2].intensity.red = 0.3f;
	lights[2].intensity.green = 0.5f;
	lights[2].intensity.blue = 1.0f;
	
	u8 imgData[3 * WIDTH * HEIGHT];
	
	for (s32 y = 0; y < HEIGHT; ++y)
	{
		for (s32 x = 0; x < WIDTH; ++x)
		{
			f32 red = 0.0f;
			f32 green = 0.0f;
			f32 blue = 0.0f;
			
			s32 level = 0;
			f32 coef = 1.0f;
			
			baseRay.origin.x = x;
			baseRay.origin.y = y;
			baseRay.origin.z = -2000.0f;
			
			baseRay.direction.x = 0.0f;
			baseRay.direction.y = 0.0f;
			baseRay.direction.z = 1.0f;
			
			do
			{
				// Find closest intersection
				f32 closestIntersect = 20000.0f;
				s32 currSphere = -1;
				
				for (u32 i = 0; i < 3; ++i)
				{
					if (RaySphereIntersection(&baseRay, &spheres[i], &closestIntersect))
					{
						currSphere = i;
					}
				}
				
				if (currSphere == -1)
				{
					break;
				}
				
				Vector3 scaled = Vector3Scale(&baseRay.direction, closestIntersect);
				Vector3 newStart = Vector3Add(&baseRay.origin, &scaled);
				
				// Find normal for the new Vector at point of intersection
				Vector3 normal = Vector3Sub(&newStart, &spheres[currSphere].position);
				f32 temp = Vector3Dot(&normal, &normal);
				if (temp == 0)
				{
					break;
				}
				
				temp = 1.0f / sqrtf(temp);
				normal = Vector3Scale(&normal, temp);
				
				// Find material for current sphere
				Material currMat = mats[spheres[currSphere].material];
				
				// Find value of the light at this point
				for (u32 j = 0; j < 3; ++j)
				{
					Light currLight = lights[j];
					Vector3 distance = Vector3Sub(&currLight.position, &newStart);
					
					if (Vector3Dot(&normal, &distance) <= 0.0f)
					{
						continue;
					}
					
					closestIntersect = sqrtf(Vector3Dot(&distance, &distance));
					
					if (closestIntersect <= 0.0f)
					{
						continue;
					}
					
					Ray lightRay;
					lightRay.origin = newStart;
					lightRay.direction = Vector3Scale(&distance, (1.0f/closestIntersect));
					
					// Lambert Diffusion
					f32 lambert = Vector3Dot(&lightRay.direction, &normal) * coef;
					
					red += lambert * currLight.intensity.red * currMat.diffuse.red;
					green += lambert * currLight.intensity.green * currMat.diffuse.green;
					blue += lambert * currLight.intensity.blue * currMat.diffuse.blue;
				}
				
				// Iterate over the reflection
				coef *= currMat.reflection;
				
				// Reflected ray start and direction
				baseRay.origin = newStart;
				f32 reflect = 2.0f * Vector3Dot(&baseRay.direction, &normal);
				Vector3 tmp = Vector3Scale(&normal, reflect);
				baseRay.direction = Vector3Sub(&baseRay.direction, &tmp);
				
				level++;
			}
			while ((coef > 0.0f) && (level < 15));
			
			imgData[(x + y * WIDTH) * 3 + 0] = (u8)min(red * 255.0f, 255.0f);
			imgData[(x + y * WIDTH) * 3 + 1] = (u8)min(green * 255.0f, 255.0f);
			imgData[(x + y * WIDTH) * 3 + 2] = (u8)min(blue * 255.0f, 255.0f);
		}
	}
	
	SavePPM(argv[1], imgData, WIDTH, HEIGHT);
	
	return 0;
}