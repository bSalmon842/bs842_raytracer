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
#define ARRAY_COUNT(array) (sizeof(array) / sizeof((array)[0]))
#define MIN_VAL(a,b) (((a) < (b)) ? (a) : (b))
#define WIN32_MAX_FILEPATH_LENGTH 0x00000104

#define WIDTH 800
#define HEIGHT 600

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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

// Function for Concatenating C Strings using Standard Library Functions
char* concat(const char *string1, const char *string2)
{
    const size_t stringLength1 = strlen(string1);
    const size_t stringLength2 = strlen(string2);
    char *result = (char *)malloc(stringLength1 + stringLength2 + 1);
    memcpy(result, string1, stringLength1);
    memcpy(result + stringLength1, string2, stringLength2 + 1);
    return result;
}

// Returns resultant vector of the subtraction of 2 vectors
inline Vector3 Vector3Sub(Vector3 *v1, Vector3 *v2)
{
	Vector3 result;
	result.x = v1->x - v2->x;
	result.y = v1->y - v2->y;
	result.z = v1->z - v2->z;
	return result;
}

// Return the scalar value of the dot product of 2 vectors
inline f32 Vector3Dot(Vector3 *v1, Vector3 *v2)
{
	f32 result = 0.0f;
	result += v1->x * v2->x;
	result += v1->y * v2->y;
	result += v1->z * v2->z;
	return result;
}

// Returns multiplication of a Vector by a Scalar Value
inline Vector3 Vector3Scale(Vector3 *vec, float scale)
{
	Vector3 result = {};
	result.x = vec->x * scale;
	result.y = vec->y * scale;
	result.z = vec->z * scale;
	return result;
}

// Returns sum of 2 Vectors
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
	// Sphere Equation (circle equation in 3 dimensions)
	// (x - a)^2 + (y - b)^2 + (z - c)^2 = r^2
	//
	// Where point of origin is 0,0,0
	// x^2 + y^2 + z^2 = r^2
	//
	// p = vector on sphere
	// x^2 + y^2 + z^2 = r^2 = p.p
	//
	// Sphere at Arbitrary Point Equation
	// c = Sphere Point of Origin
	// (p - c).(p - c) = r^2
	//
	// t = scalar value
	// d = direction
	// o = point of origin
	// Ray Equation: p = td + o
	//
	// To solve ray/sphere intersection, the Ray Equation is
	// substituted into the Sphere Equation
	// (td + o - c).(td + o - c) = r^2
	//
	// Move r^2 to LHS, begin grouping
	// (td + (o - c)).(td + (o - c)) - r^2 = 0
	//
	// Use Distributive Property of Dot Products
	// d.dt^2 + td(o - c) + td(o - c) + (o - c).(o - c) - r^2 = 0
	//
	// d.dt^2 + 2d.(o - c)t + (o - c).(o - c) - r^2 = 0
	// A^2 + Bt + C = 0 <- Quadratic Equation
	// A = d.d
	// B = 2d.(o - c)
	// C = (o - c).(o - c) - r^2
	// therefore At^2 + Bt + C = 0
	
	b32 result = false;
	
	f32 a = Vector3Dot(&ray->direction, &ray->direction);
	Vector3 distanceToSphere = Vector3Sub(&ray->origin, &sphere->position);
	f32 b = 2.0f * Vector3Dot(&ray->direction, &distanceToSphere);
	f32 c = Vector3Dot(&distanceToSphere, &distanceToSphere) - (sphere->radius * sphere->radius);
	
	f32 discriminant = (b * b) - (4.0f * (a * c));
	
	if (discriminant < 0)
	{
		result = false;
	}
	else
	{
		// If there are real results from the discriminant,
		// find the closest to the camera (Z-Ordering)
		f32 sqrtDiscriminant = sqrtf(discriminant);
		f32 intersect0 = (-b + sqrtDiscriminant) / 2.0f;
		f32 intersect1 = (-b - sqrtDiscriminant) / 2.0f;
		
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

bool ProcessSphereRayTracing(Ray *baseRay, Sphere *spheres, u32 sphereCount, Material *mats, Light *lights, u32 lightCount, f32 *coef, f32 *red, f32 *green, f32 *blue, s32 *level)
{
	// Find closest intersection
	f32 closestIntersect = 20000.0f;
	s32 currSphere = -1;
	
	for (u32 i = 0; i < sphereCount; ++i)
	{
		if (RaySphereIntersection(baseRay, &spheres[i], &closestIntersect))
		{
			currSphere = i;
		}
	}
	
	if (currSphere == -1)
	{
		return false;
	}
	
	Vector3 scaled = Vector3Scale(&baseRay->direction, closestIntersect);
	Vector3 reflectStart = Vector3Add(&baseRay->origin, &scaled);
	
	// Find normal for the new Vector at point of intersection
	Vector3 normal = Vector3Sub(&reflectStart, &spheres[currSphere].position);
	f32 normCheck = Vector3Dot(&normal, &normal);
	if (normCheck == 0)
	{
		return false;
	}
	
	normCheck = 1.0f / sqrtf(normCheck);
	normal = Vector3Scale(&normal, normCheck);
	
	// Find material for current sphere
	Material currMat = mats[spheres[currSphere].material];
	
	// Find value of the light at this point
	for (u32 j = 0; j < lightCount; ++j)
	{
		Light currLight = lights[j];
		Vector3 distance = Vector3Sub(&currLight.position, &reflectStart);
		
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
		lightRay.origin = reflectStart;
		lightRay.direction = Vector3Scale(&distance, (1.0f/closestIntersect));
		
		// Lambert Diffusion
		// D = L.NCI
		// N = Normal at Intersection
		// L = Normalised Vector from point of intersection to the light source
		// C = Colour of Surface
		// I = Intensity of incoming Light
		// D = Scalar representing surface brightness at point
		
		// lambert = N.L (* coefficient of how reflective the surface is)
		f32 lambert = Vector3Dot(&lightRay.direction, &normal) * *coef;
		
		// D (for each colour) = lambert * C * I
		*red += lambert * currMat.diffuse.red * currLight.intensity.red;
		*green += lambert * currMat.diffuse.green * currLight.intensity.green;
		*blue += lambert * currMat.diffuse.blue * currLight.intensity.blue;
	}
	
	// Iterate over the reflection
	*coef *= currMat.reflection;
	
	// Reflection Math
	// Θi = Angle of Incident
	// Θr = Angle of Reflection
	// Θi = Θr
	//
	// Let Incident Ray = P
	// Let Reflected Ray = Q
	// Assume P & Q are Normalised
	//
	//      Q1
	// <-----------
	// \	      ^
	//  \         |
	// Q \        |
	//    \       | Q2
	//     \      |
	//      \     |
	//       \	|
	//       /\   |
	//      /  \  |
	//     |	\ |
	// N   | Θr \|
	// <----------|
	//     | Θi /^
	//     |    / |
	//      \  /  |
	//       \/   |
	//       /    |
	//      /     | P2
	//     /      |
	//    /       |
	// P /        |
	//  /         |
	// /   	   |
	// ---------->
	//      P1
	//
	// |Q1| = cos(Θr) = cos(Θi) = |P1|
	// |Q2| = sin(Θr) = sin(Θi) = |P2|
	//
	// Q1 = -P1 * Q1 = Q1
	//
	// Q = Q2 + Q1 = P2 - P1
	// Q = P2 - P1
	//
	// With Orthogonal Projection
	// 
	//         /|
	//        / |
	//       /  |
	//      /   |
	//     /    |
	//  V /     | ProjU(V) - V
	//   /      |
	//  /       |
	// / U      v
	// -->------>
	//  ProjU(V)
	//
	// ProjU(V) = aU
	// a = Arbitrary Scalar
	//
	// Since ProjU(V) - V is orthogonal to U
	// (aU - V).U = 0
	// 
	// Multiply out the dot product
	// aU.U - V.U = 0
	// a = V.U / U.U
	// Which Results in
	// ProjU(V) = (U.V / U.U)U
	//
	// Knowing that by definition of the Dot Product
	// U.U = |U|^2
	// ProjU(V) = (U.V / |U|^2)U
	//
	// Apply this to the reflection situation
	// P1 = (P.N / |N|^2)N
	//
	// |N| = 1, therefore
	// P1 = (P.N)N
	//
	// With P2 being P - P1
	// Q = (P - (P.N)N) - ((P.N)N)
	//
	// Q = P - 2(P.N)N
	
	// Reflected ray start and direction
	// Q = New baseRay.direction
	// P = Current baseRay.direction
	// N = normal
	// reflect = 2(P.N)
	// temp = reflect * N
	baseRay->origin = reflectStart;
	f32 reflect = 2.0f * Vector3Dot(&baseRay->direction, &normal);
	Vector3 temp = Vector3Scale(&normal, reflect);
	baseRay->direction = Vector3Sub(&baseRay->direction, &temp);
	
	*level++;
	
	return true;
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
	Ray baseRay = {};
	
	Material mats[3];
	
	mats[0].diffuse.red = 1.0f;
	mats[0].diffuse.green = 1.0f;
	mats[0].diffuse.blue = 1.0f;
	mats[0].reflection = 0.33f;
	
	mats[1].diffuse.red = 1.0f;
	mats[1].diffuse.green = 1.0f;
	mats[1].diffuse.blue = 0.0f;
	mats[1].reflection = 0.5f;
	
	mats[2].diffuse.red = 0.3f;
	mats[2].diffuse.green = 0.0f;
	mats[2].diffuse.blue = 1.0f;
	mats[2].reflection = 0.1f;
	
	Sphere spheres[3];
	
	spheres[0].position.x = 100.0f;
	spheres[0].position.y = 100.0f;
	spheres[0].position.z = 0.0f;
	spheres[0].radius = 75.0f;
	spheres[0].material = 0;
	
	spheres[1].position.x = 300.0f;
	spheres[1].position.y = 400.0f;
	spheres[1].position.z = 0.0f;
	spheres[1].radius = 100.0f;
	spheres[1].material = 1;
	
	spheres[2].position.x = 600.0f;
	spheres[2].position.y = 250.0f;
	spheres[2].position.z = 0.0f;
	spheres[2].radius = 120.0f;
	spheres[2].material = 2;
	
	Light lights[3];
	
	lights[0].position.x = 0.0f;
	lights[0].position.y = 0.0f;
	lights[0].position.z = -200.0f;
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
	
	u8* imgData = new u8[3 * WIDTH * HEIGHT];
	
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
				if (!ProcessSphereRayTracing(&baseRay, spheres, ARRAY_COUNT(spheres), mats, lights, ARRAY_COUNT(lights), &coef, &red, &green, &blue, &level))
				{
					break;
				}
			}
			while ((coef > 0.0f) && (level < 15));
			
			imgData[(x + y * WIDTH) * 3 + 0] = (u8)MIN_VAL(red * 255.0f, 255.0f);
			imgData[(x + y * WIDTH) * 3 + 1] = (u8)MIN_VAL(green * 255.0f, 255.0f);
			imgData[(x + y * WIDTH) * 3 + 2] = (u8)MIN_VAL(blue * 255.0f, 255.0f);
		}
	}
	
	char *extension = strrchr(argv[1], '.');
	if (extension && (strcmp(extension, ".ppm") == 0))
	{
		SavePPM(argv[1], imgData, WIDTH, HEIGHT);
	}
	else
	{
		char *filename = concat(argv[1], ".ppm");
		SavePPM(filename, imgData, WIDTH, HEIGHT);
		free(filename);
	}
	
	delete[] imgData;
	
	return 0;
}