/*
Project: bs842 RayTracer
File: raytracer.h
Author: Brock Salmon
Notice: (C) Copyright 2018 by Brock Salmon. All Rights Reserved.
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

// Structs
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

// Inlines
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
inline Vector3 Vector3Add(Vector3 *v1, Vector3 *v2)
{
	Vector3 result = {};
	result.x = v1->x + v2->x;
	result.y = v1->y + v2->y;
	result.z = v1->z + v2->z;
	return result;
}
