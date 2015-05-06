/******************************************************************************
Class:Mesh
Implements:
Author:Rich Davison
Description: As with the Graphics for Games Mesh class, this wraps up the GCM
functionality for primitives, geometry, and so on. 

The GenerateTriangle demonstrates basic, linear vertex processing, while
GenerateQuad demonstrates indexed vertex processing.

Unlike Graphics for Games, which used separate VBOs for each vertex attribute,
this class uses interleaved vertices. Thinking memory-wise, before we had
a series of separate VBOs in memory:

Position A, Position B, Position C, Colour A, Colour B, Colour C

whereas now, we have interleaving:

Position A, Colour A, Position B, Colour B, Position C, Colour C

It's not quite as neat as the separate VBOs in terms of avoiding unnecessary
data for simple Meshes, but oh well.

As an offshoot of this, instead of having a number of pointers to the data for
each attribute, this time we have a single pointer, to an array of Vertex
structs. 

The local variable vertexOffsets is roughly analogous to the bufferObject array
variable in the OpenGL framework, and provides GCM with the memory positions of 
the first position, colour, tex coord etc. I even use the handy enum to
explicitly set its size. 

Texture has been renamed defaultTexture, to make it a bit more obvious that you
can have a texture defined in a SceneNode if you want, and use that instead,
you don't need separate cube meshes for crates and companion cubes, just 
separate scene node texture pointers...


-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////

#pragma once
#include <sysutil/sysutil_sysparam.h>	
#include <cell/gcm.h>

/*
MeshBuffer by another name. Used by the Mesh and
Shader classes to determine which vertex attribute
is which
*/
enum VertexAttributes {
	VERTEX_POSITION = 0,
	VERTEX_COLOUR	= 1,
	VERTEX_TEXCOORD = 2,
	VERTEX_INDEX	= 3,
	VERTEX_SIZE		= 4
};

#include "Shader.h"

class VertexShader;		//Compile our shader classes first, please
class FragmentShader;

/*
Instead of separate data pointers for each vertex attribute, this time
we have a single array of this data type. It should be fairly obvious
that x,y, and z are position, while u and v are tex coord. Why aren't
they in Vector3/2 structs? because the Vector3 struct is padded, and
so wastes a byte for every vec3 per vertex (if we had a normal and tangent
in addition to position, we'd be wasting 3 bytes...)
*/
struct	Vertex{
	float x, y, z,u,v;
	uint32_t rgba; 
};

class Mesh	{
public:
	Mesh(void);
	virtual ~Mesh(void);

	static Mesh* GenerateTriangle();
	static Mesh* GenerateQuad(float s);

	virtual void	Draw(VertexShader &vertex, FragmentShader &fragment);

	void					SetDefaultTexture(CellGcmTexture &def)  {defaultTexture = &def;}
	const CellGcmTexture*	GetDefaultTexture()						{return defaultTexture;}

	static Mesh* GenerateCube(float size);

protected:
	unsigned int type;				//Primitive type (triangles, points, etc)
	unsigned int numVertices;		//Number of vertices for this mesh
	unsigned int numIndices;		//Number of indices for this mesh. 

	CellGcmTexture* defaultTexture;	//Default texture for this mesh, if any

	Vertex*		 vertexData;	//Pointer to the vertex data, in graphics memory!
	unsigned int vertexOffsets[VERTEX_SIZE]; //offsets into vertexData for each attribute
};
