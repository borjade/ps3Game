#include "Shader.h"

/*
Constructor. Receives the correct 'type' from the derived
subclasses VertexShader and FragmentShader. New this year
is the ability to load in both text shaders, and pre-compiled
binary shaders. We're going to assume a shader that ends in
".cg" is a text file, while "vpo" or "fpo" (x program object)
types are binary. 

*/
Shader::Shader(std::string name, ShaderType type)	{
	std::string ext = name.substr(name.size()-3,3);

	if(ext == ".cg") {
		LoadTextShader(name,type);
	}
	else if(ext == "vpo" || ext == "fpo") {
		LoadBinaryShader(name);
	}
	else{
		std::cout << "Unknown shader type: " << ext << std::endl;
	}
}

Shader::~Shader(void)	{
}


/*
Binary shader loading. This way should be preferred over text-based
shader loading, as the runtime compiler messes things up ALL THE TIME.

Although vertex shaders can be in system memory, we're going to put
all shader types straight into graphics memory for simplicity. 
*/
void	Shader::LoadBinaryShader(std::string name) {
	name = SYS_APP_HOME + name;

	unsigned int dataSize = 0;
	char* data = NULL;

	//Open the file
	std::ifstream	file(name.c_str(),std::ios::in|std::ios::binary);
	//Oh no!
	if(!file) {
		std::cout << "LoadBinaryShader: Can't find file: " << name << std::endl;
		return;
	}
	//This is the slightly messy default way to determine the size
	//of a file (seek to the end of it, grab the read position, seek
	//back to the start)
	file.seekg(0, std::ios::end);
	dataSize = file.tellg();
	file.seekg(0, std::ios::beg);

	//Allocate some graphics memory for the shader.
	//data = (char*)GCMRenderer::localMemoryAlign(64,dataSize);

	data = (char*)malloc(dataSize);
	//Load the shader straight into graphics memory. What type of data transfer
	//is this? How fast will it be?
	file.read(data,dataSize);
	file.close();	//Done with the data, close the file.

	//Now we have the binary data, we can cast it to a CGprogram
	//data structure. 
	program		= (CGprogram)(void*)data;

	//And initialise it. This will make sure the binary data is in the correct
	//format etc. 
	cellGcmCgInitProgram(program);

	//Lastly, we need to figure out where the ucode of the shader is located
	//and save the offset of it. It's really only fragment shaders that need
	//to know this, but just in case there's some obscure GCM function that
	//uses it, we'll save it even for vertex shaders.
	unsigned int ucodeSize;
	void *ucodePtr;

	cellGcmCgGetUCode(program, &ucodePtr, &ucodeSize);
	ucode = GCMRenderer::localMemoryAlign(64,ucodeSize);
	memcpy(ucode, ucodePtr, ucodeSize);
	cellGcmAddressToOffset(ucode, &offset);
}

/*
The 'old' way of compiling shaders is to read in the shader text file,
and run it through the shader compiler. See the GCM tutorial text for
an overview of how this function works, but for now just know that it's
not very good, and the shaders it compiles will probably break.
*/
void	Shader::LoadTextShader(std::string name, ShaderType type) {
	CGCcontext* cgCompiler	= sceCgcNewContext();
	CGCinclude include		= {0,0};

	CGCbin* bin		= sceCgcNewBin();
	CGCbin* output	= sceCgcNewBin();

	std::cout << "Compiling shader: " << name << std::endl;

	if(sceCgcCompileString(cgCompiler, LoadShaderFile(name).c_str(),
		type == VERTEX_SHADER ? "sce_vp_rsx" : "sce_fp_rsx",
		NULL,NULL,bin,0,output,&include) != SCECGC_OK){
			std::cout << "Compiling failed!" << std::endl;
			return;
	}
	else{
		std::cout << "Compiling success!" << std::endl;
	}

	program		= (CGprogram)(void*)sceCgcGetBinData(bin);


	char* outputchars = (char*)sceCgcGetBinData(output);

	std::string outstring = std::string(outputchars);

	std::cout << "Compiling output:" << outstring << std::endl;

	cellGcmCgInitProgram(program);

	unsigned int ucodeSize;
	void *ucodePtr;

	cellGcmCgGetUCode(program, &ucodePtr, &ucodeSize);
	ucode = GCMRenderer::localMemoryAlign(64,ucodeSize);

	memcpy(ucode, ucodePtr, ucodeSize);

	cellGcmAddressToOffset(ucode, &offset);


	sceCgcDeleteContext(cgCompiler);
	sceCgcDeleteBin(bin);
	sceCgcDeleteBin(output);
}

std::string Shader::LoadShaderFile(std::string filename)	{
	filename = SYS_APP_HOME + filename;

	std::ifstream	file;
	std::string		output;
	std::string		temp;

	std::cout << "Loading shader string from(" << filename << ")" << std::endl;

	file.open(filename.c_str());
	if(!file.is_open()){
		std::cout << "load_shader file error!" << std::endl;
		return output;
	}

	while(!file.eof()){
		std::getline(file,temp);
		output += temp;
		output += "\n";
	}

	file.close();

	std::cout << output;

	std::cout << "Finished loading shader string!" << std::endl;

	return output;
}

/*
New to the Shader class is the ability to query a shader for a uniform 
variable by name. Storing the results of these queries in a map should
be slightly quicker (worst case) than relying on GCM API functions.

This is essentially analogous to glGetUniformLocation, and will either
return zero (this shader doesn't have a uniform of this name), or
the non-zero parameter 'name' handle of the uniform. 
*/
CGparameter Shader::GetParameter(std::string name) {
	//standard template library map find function
	std::map<std::string,CGparameter>::iterator i = uniforms.find(name);

	if(i != uniforms.end()) {	//if its in the map, return it!
		return i->second;
	}
	//if we get to here, it's not in the map (yet)

	//Get the parameter name for this uniform
	CGparameter p = cellGcmCgGetNamedParameter(program, name.c_str());

	if(!p) {
		//Can't find a parameter of this name in the shader!
		std::cout << "Can't find named parameter:" << name << std::endl;
	}
	//Either way, add it to the map (so we don't repeatedly
	//query GCM for the shader info)

	uniforms.insert(std::make_pair(name,p));

	return p;
}

/*
Sets a uniform value. In OpenGL we had loads of glUniformx type
functions, for the different types of data-> In GCM we have a single
function, which takes a CGparameter and a pointer to some float data,
and the function works out the rest. Simpler, but more likely to go
wrong when incorrect data is sent to it. 
*/
void	Shader::SetParameter(std::string name, float*data) {
	CGparameter p = GetParameter(name);

	//DON'T try to set a non-existent parameter. GCM will instantly
	//fall over.
	if(p) {	
		cellGcmSetVertexProgramParameter(p, data);
	}
}

/*
Sets a matrix. While there's only one uniform setting function, we still
need a special case. In GCM, matrices are transposed to how they are in
Sony's own matrix code. So, we wrap matrix uniform setting around a 
transpose function, so it's less likely you'll accidentally set your 
matrix wrong
*/
void	Shader::SetParameter(std::string name, Matrix4 &totranpose) {
	Matrix4 tempMatrix = transpose(totranpose);
	SetParameter(name, (float*)&tempMatrix);
}

/*
Sets the shaders matrices to the passed in values. Handy to call at the start of
a frame, or when using a shader for the first time. It's more efficient to use
SetParameter directly for setting just a single matrix, though. 
*/
void	Shader::UpdateShaderMatrices(Matrix4 &model,Matrix4 &view, Matrix4 &proj) {
	SetParameter("modelMat", model);
	SetParameter("viewMat", view);
	SetParameter("projMat", proj);
}

void	Shader::UpdateShaderMatrices(Matrix4& mvp) {
	SetParameter("mvpMat", mvp);
}

/*


Vertex Shader Functions!


*/
VertexShader::VertexShader(std::string name) : Shader(name, VERTEX_SHADER)	{
	for(int i = 0; i < VERTEX_SIZE; ++i) {
		attributes[i] = 0;
	}
	SetDefaultAttributes();
}

VertexShader::~VertexShader(void)	{
}
/*
Just as with OpenGL, we must bind our vertex shader inputs to some vertex
attributes. This function essentially does the same as the OpenGL framework
version, but is a little bit more involved
*/
void VertexShader::SetDefaultAttributes() {
	/*
	Get the handles to our vertex attributes
	*/
	CGparameter position_param	= cellGcmCgGetNamedParameter(program, "position");
	CGparameter colour_param	= cellGcmCgGetNamedParameter(program, "color");
	CGparameter tex_param		= cellGcmCgGetNamedParameter(program, "texCoord");

	//And now save out the actual resources (the name of the input registers they'll use)
	attributes[VERTEX_POSITION]	= cellGcmCgGetParameterResource(program, position_param)	- CG_ATTR0;

	if(colour_param)	{
		attributes[VERTEX_COLOUR]	= cellGcmCgGetParameterResource(program, colour_param)  - CG_ATTR0;
	}

	if(tex_param) {
		attributes[VERTEX_TEXCOORD]	= cellGcmCgGetParameterResource(program, tex_param)		- CG_ATTR0;
	}
}


/*


Fragment Shader Functions!


*/
FragmentShader::FragmentShader(std::string name) : Shader(name, FRAGMENT_SHADER)	{
	UpdateShaderVariables();
}

FragmentShader::~FragmentShader(void)	{

}

void	FragmentShader::SetParameter(std::string name, float*data) {
	CGparameter p = GetParameter(name);

	//DON'T try to set a non-existent parameter. GCM will instantly
	//fall over.
	if(p) {	
		cellGcmSetFragmentProgramParameter(program, p, data, offset);
	}
}

/*
When uniforms are changed in a fragment shader, we must tell GCM to reload
the instruction cache of the shader, so call this after you've set anything
on the fragment shader. You might want to overload SetParameter to always
call the gcm function, but it's more efficient to call it only once per 'set'
of uniform changes required.
*/
void	FragmentShader::UpdateShaderVariables() {
	cellGcmSetUpdateFragmentProgramParameter(offset);
}