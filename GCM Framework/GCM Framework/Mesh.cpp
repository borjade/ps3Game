#include "Mesh.h"

Mesh::Mesh(void)	{
	vertexData		= 0;
	type			= CELL_GCM_PRIMITIVE_TRIANGLES;	//named constant for triangles, oddly enough
	numVertices		= 0;
	defaultTexture  = NULL;

	for(int i = 0; i < VERTEX_SIZE; ++i) {
		vertexOffsets[i] = 0;
	}
}

Mesh::~Mesh(void)	{
	//we'd delete vertex data here if the renderer allowed it...
}

/*
Returns a Mesh with vertex data to draw a triangle.

Yep, you've seen this function before! Slightly different this time, 
as we're writing directly to graphics memory, and we're using interleaved
vertices, rather than VBOs.
*/
Mesh* Mesh::GenerateTriangle() {
	Mesh*m = new Mesh();
	m->numVertices = 3;	//Triangles have 3 vertices...

	m->vertexData = (Vertex*)GCMRenderer::localMemoryAlign(128, sizeof(Vertex) * 3); //allocate some memory

	m->vertexData[0].x		= 0.0f;			//Top of our triangle
	m->vertexData[0].y		= 1.0f; 
	m->vertexData[0].z		= 0; 
	m->vertexData[0].rgba	= 0xff0000ff;	//Red

	m->vertexData[1].x		= -1.0f;		//Bottom Left
	m->vertexData[1].y		= -1.0f; 
	m->vertexData[1].z		= 0 ;
	m->vertexData[1].rgba	= 0x00ff00ff;	//Green

	m->vertexData[2].x		= 1;			//Bottom Right
	m->vertexData[2].y		= -1.0f; 
	m->vertexData[2].z		= 0; 
	m->vertexData[2].rgba	= 0x0000ffff;	//Blue

	//Think of this as setting the VBO for the positions and colours
	//(we're actually grabbing the position in memory for the first position and 
	//colour, so GCM knows how to stride over the data)

	cellGcmAddressToOffset(&m->vertexData->x, &m->vertexOffsets[VERTEX_POSITION]);
	cellGcmAddressToOffset(&m->vertexData->rgba, &m->vertexOffsets[VERTEX_COLOUR]);

	return m;
}

/*
Returns a Mesh with vertex data to draw an indexed quad.

Yep, you've seen this function before! Slightly different this time, 
as we're writing directly to graphics memory, and we're using interleaved
vertices, rather than VBOs.

*/
Mesh* Mesh::GenerateQuad(float s) {
	Mesh*m = new Mesh();

	m->numIndices	 = 6;
	m->numVertices   = 4;

	short *indices = (short*)GCMRenderer::localMemoryAlign(128, sizeof(short) * 6);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	indices[3] = 2;
	indices[4] = 3;
	indices[5] = 0;

	m->vertexData =  (Vertex*)GCMRenderer::localMemoryAlign(128, sizeof(Vertex) * 4);

	float size = s;

	m->vertexData[0].x = -size;			//Top left of our quad
	m->vertexData[0].y = size; 
	m->vertexData[0].z = 0.0f; 
	m->vertexData[0].u = 0.0f;
	m->vertexData[0].v = 1.0f;

	m->vertexData[1].x = -size;		//Bottom left
	m->vertexData[1].y = -size; 
	m->vertexData[1].z = 0.0f; 
	m->vertexData[1].u = 0.0f;
	m->vertexData[1].v = 0.0f;

	m->vertexData[2].x = size;			//Bottom Right
	m->vertexData[2].y = -size; 
	m->vertexData[2].z = 0.0f; 
	m->vertexData[2].u = 1.0f;
	m->vertexData[2].v = 0.0f;

	m->vertexData[3].x = size;			//Top Right
	m->vertexData[3].y = size; 
	m->vertexData[3].z = 0.0f; 
	m->vertexData[3].u = 1.0f;
	m->vertexData[3].v = 1.0f;

	m->vertexData[0].rgba=0xffffffff;
	m->vertexData[1].rgba=0xffffffff;
	m->vertexData[2].rgba=0xffffffff;
	m->vertexData[3].rgba=0xffffffff;

	cellGcmAddressToOffset(&m->vertexData->x, &m->vertexOffsets[VERTEX_POSITION]);
	cellGcmAddressToOffset(&m->vertexData->rgba, &m->vertexOffsets[VERTEX_COLOUR]);
	cellGcmAddressToOffset(&m->vertexData->u, &m->vertexOffsets[VERTEX_TEXCOORD]);
	cellGcmAddressToOffset(indices, &m->vertexOffsets[VERTEX_INDEX]);
	//Just like in the OpenGL framework, we're treating indices like a vertex attribute...

	return m;
}

//<----- Added by Chris Fraser ----->
Mesh* Mesh::GenerateCube(float size)
{
	Mesh* m = new Mesh();

	m->numVertices = 8;
	m->numIndices = 36;

	short *indices = (short*)GCMRenderer::localMemoryAlign(128, sizeof(short) * 6);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	indices[3] = 2;
	indices[4] = 3;
	indices[5] = 0;

	indices[6] = 1;
	indices[7] = 6;
	indices[8] = 2;

	indices[9] = 6;
	indices[10] = 1;
	indices[11] = 5;

	indices[12] = 4;
	indices[13] = 6;
	indices[14] = 5;

	indices[15] = 6;
	indices[16] = 4;
	indices[17] = 7;

	indices[18] = 2;
	indices[19] = 6;
	indices[20] = 3;

	indices[21] = 6;
	indices[22] = 7;
	indices[23] = 3;

	indices[24] = 0;
	indices[25] = 3;
	indices[26] = 7;

	indices[27] = 0;
	indices[28] = 7;
	indices[29] = 4;

	indices[30] = 0;
	indices[31] = 5;
	indices[32] = 1;

	indices[33] = 0;
	indices[34] = 4;
	indices[35] = 5;

	m->vertexData = (Vertex*)GCMRenderer::localMemoryAlign(128, sizeof(Vertex) * 4);

	//float size = 1.0f;

	m->vertexData[0].x = -size; //Top Left Back
	m->vertexData[0].y = size;
	m->vertexData[0].z = size;
	m->vertexData[0].u = 0.0f;
	m->vertexData[0].v = 1.0f;

	m->vertexData[1].x = -size; //Bottom Left Back
	m->vertexData[1].y = -size;
	m->vertexData[1].z = size;
	m->vertexData[1].u = 0.0f;
	m->vertexData[1].v = 0.0f;

	m->vertexData[2].x = size; //Bottom Right Back
	m->vertexData[2].y = -size;
	m->vertexData[2].z = size;
	m->vertexData[2].u = 1.0f;
	m->vertexData[2].v = 0.0f;

	m->vertexData[3].x = size; //Top Right Back
	m->vertexData[3].y = size;
	m->vertexData[3].z = size;
	m->vertexData[3].u = 1.0f;
	m->vertexData[3].v = 1.0f;

	m->vertexData[4].x = -size; //Top Left Front
	m->vertexData[4].y = size;
	m->vertexData[4].z = -size;
	m->vertexData[4].u = 1.0f;
	m->vertexData[4].v = 0.0f;

	m->vertexData[5].x = -size; //Bottom Left Front
	m->vertexData[5].y = -size;
	m->vertexData[5].z = -size;
	m->vertexData[5].u = 1.0f;
	m->vertexData[5].v = 1.0f;

	m->vertexData[6].x = size; //Bottom Right Front
	m->vertexData[6].y = -size;
	m->vertexData[6].z = -size;
	m->vertexData[6].u = 0.0f;
	m->vertexData[6].v = 1.0f;

	m->vertexData[7].x = size; //Top Right Front
	m->vertexData[7].y = size;
	m->vertexData[7].z = -size;
	m->vertexData[7].u = 0.0f;
	m->vertexData[7].v = 0.0f;

	m->vertexData[0].rgba = 0xffffffff;
	m->vertexData[1].rgba = 0xffffffff;
	m->vertexData[2].rgba = 0xffffffff;
	m->vertexData[3].rgba = 0xffffffff;
	m->vertexData[4].rgba = 0xffffffff;
	m->vertexData[5].rgba = 0xffffffff;
	m->vertexData[6].rgba = 0xffffffff;
	m->vertexData[7].rgba = 0xffffffff; /*0x308014ff*/

	cellGcmAddressToOffset(&m->vertexData->x, &m->vertexOffsets[VERTEX_POSITION]);
	cellGcmAddressToOffset(&m->vertexData->rgba, &m->vertexOffsets[VERTEX_COLOUR]);
	cellGcmAddressToOffset(&m->vertexData->u, &m->vertexOffsets[VERTEX_TEXCOORD]);
	cellGcmAddressToOffset(indices, &m->vertexOffsets[VERTEX_INDEX]);
	//Just like in the OpenGL framework, we're treating indices like a vertex attribute...

	return m;
}

/*
Draws a mesh, via linking its vertex attributes to the vertex shader.

This function is mostly as described in the GCM tutorial, except we have
a branch statement to draw an indexed mesh. It's very similar to how to
draw an indexed mesh with OpenGL!
*/
void	Mesh::Draw(VertexShader &vertex, FragmentShader &fragment) {
	cellGcmSetVertexDataArray(vertex.GetAttributeIndex(VERTEX_POSITION),
		0, 
		sizeof(Vertex), 
		3, 
		CELL_GCM_VERTEX_F, 
		CELL_GCM_LOCATION_LOCAL, 
		(uint32_t)vertexOffsets[VERTEX_POSITION]
	);

	if(vertexOffsets[VERTEX_COLOUR])	{
		cellGcmSetVertexDataArray(vertex.GetAttributeIndex(VERTEX_COLOUR),
			0, 
			sizeof(Vertex), 
			4, 
			CELL_GCM_VERTEX_UB, 
			CELL_GCM_LOCATION_LOCAL, 
			(uint32_t)vertexOffsets[VERTEX_COLOUR]
		);
	}
	//else{
	//	cellGcmSetVertexDataArray(vertex.GetAttributeIndex(VERTEX_COLOUR),0,0,0,CELL_GCM_VERTEX_UB,CELL_GCM_LOCATION_LOCAL,0);
	//}

	if(vertexOffsets[VERTEX_TEXCOORD])	{
		cellGcmSetVertexDataArray(vertex.GetAttributeIndex(VERTEX_TEXCOORD),
			0, 
			sizeof(Vertex), 
			2, 
			CELL_GCM_VERTEX_F, 
			CELL_GCM_LOCATION_LOCAL, 
			(uint32_t)vertexOffsets[VERTEX_TEXCOORD]
		);
	}
	//else{
	//	cellGcmSetVertexDataArray(vertex.GetAttributeIndex(VERTEX_TEXCOORD),0,0,0,CELL_GCM_VERTEX_F,CELL_GCM_LOCATION_LOCAL,0);
	//}

	if(vertexOffsets[VERTEX_INDEX]) {	//If we have indices

		cellGcmSetDrawIndexArray(type, numIndices, CELL_GCM_DRAW_INDEX_ARRAY_TYPE_16,
			CELL_GCM_LOCATION_LOCAL, vertexOffsets[VERTEX_INDEX]);
	}
	else{
		//else just draw an ordered list of vertices
		cellGcmSetDrawArrays(type, 0, numVertices);
	}	
}