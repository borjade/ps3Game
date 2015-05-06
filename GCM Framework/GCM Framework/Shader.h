/******************************************************************************
Class:Shader
Implements:
Author:Rich Davison
Description: A shader. Note: Unlike the Graphics for Games framework, where
the Shader class represented an entire shader program (vertex, optional geom, 
fragment, all together), this time, the Shader class is a shader /object/, so
is either a vertex shader, or a fragment shader. 

GCM syntax for getting and setting uniforms is slightly less horrid than 
OpenGL, but I've still wrapped them up in a few simple function calls. These
functions use a map to store the cg handle for a given uniform name, so we 
don't have to keep asking cg.

In Cg, matrices are transposed to what you expect (ie, flipped across its 
diagonal). This is a complete pain to remember, so the overloaded SetParameter
function that takes in a matrix will always transpose it before it is set on 
the shader. 

Since last year's tutorials, I've realised just how bad the PS3 runtime
shader compiler is, and figured out how to compile the shaders at exe compile
time, which does a much better job of producing shaders that actually work. 

Check the post build events for the provided fragment and vertex program - 
they contain calls to the Cg compiler to compile the text, saved out into a
binary file. We can then send that binary data straight to the RSX processor
and run it.

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////
#pragma once
#include <cell/gcm.h>
#include <Cg/cg.h>
#include <Cg/cgBinary.h>
#include <cell/cgb.h>
#include <Cg/cgc.h>

#include <cell/sysmodule.h>

#include <sys/paths.h>							//And know about SYS_APP_HOME
#include <vectormath/cpp/vectormath_aos.h>
#include <cell/cell_fs.h>

#include <string>
#include <fstream>
#include <iostream>
#include <map>

using namespace Vectormath::Aos;

using namespace cell::Gcm;

#include "GCMRenderer.h"
#include "Mesh.h"

class Shader	{
public:
	friend class GCMRenderer;

	virtual ~Shader(void);

	static std::string	LoadShaderFile(std::string filename);

	void				UpdateShaderMatrices(Matrix4 &model,Matrix4 &view, Matrix4 &proj);
	void				UpdateShaderMatrices(Matrix4& mvp);
	
	virtual void		SetParameter(std::string name, float*data);
	void				SetParameter(std::string name, Matrix4 &totranpose);
	CGparameter			GetParameter(std::string name);
	
protected:
	/*
	A quick enum to mark which shader type is which...
	*/
	enum ShaderType {
		VERTEX_SHADER,
		FRAGMENT_SHADER
	};

	//We don't want to ever make an instance of the Shader class, only
	//Vertex and Fragment shaders...
	Shader(std::string name, ShaderType type);
	
	void	LoadTextShader(std::string filename, ShaderType type);		//Load shader from text file (NOT RECOMMENDED)
	void	LoadBinaryShader(std::string filename);	//Load shader from a binary (PREFERRED!!)


	CGprogram		program;
	void *			ucode;					//Pointer to the ucode for this shader
	unsigned int	offset;					//Offset into graphics memory for this shader

	std::map<std::string,CGparameter>		uniforms;
};

/*
Vertex shaders need to link their attributes to 
mesh vertex data, so they get their own subclass
that allows this. See the Mesh class for the
VertexAttributes struct.
*/
class VertexShader : public Shader	{
public:
	VertexShader(std::string name);
	~VertexShader(void);

	unsigned int GetAttributeIndex(VertexAttributes attribute) {
		return attributes[attribute];
	}

protected:
	void SetDefaultAttributes();
	unsigned int attributes[VERTEX_SIZE];
};

/*
A subclass, to call a single extra API function. Thanks GCM!
*/
class FragmentShader : public Shader	{
	friend class GCMRenderer;
public:
	FragmentShader(std::string name);
	~FragmentShader(void);

	virtual void		SetParameter(std::string name, float*data);
	void	UpdateShaderVariables();

protected:
};