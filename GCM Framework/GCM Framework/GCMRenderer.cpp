#include "GCMRenderer.h"

//Static member variables need initialising!
uint32_t		GCMRenderer::localHeapStart	= 0;

GCMRenderer::GCMRenderer(void)	{
	if(cellGcmInit(COMMAND_SIZE, BUFFER_SIZE, memalign(1024*1024, BUFFER_SIZE))!= CELL_OK) {
		std::cout << "cellGcmInit failed!" << std::endl;	
	}

	camera	= NULL;
	root	= NULL;

	InitDisplay();
	InitSurfaces();
}

/*
This function is as it was in the GCM tutorial text. It sets the
display up, and also grabs the pointer to the start of graphics
memory. It'll also reserve enough memory for the front, back, and depth
buffers. 
*/
void GCMRenderer::InitDisplay() {
	std::cout << "Initialising display!" << std::endl;

	if(SetResolution(GCM_RESOLUTION_1080)) {
		std::cout << "Using 1080p" << std::endl;
	}
	else if(SetResolution(GCM_RESOLUTION_720)) {
		std::cout << "Using 720p" << std::endl;
	}
	else if(SetResolution(GCM_RESOLUTION_576)) {
		std::cout << "Using NTSC" << std::endl;
	}
	else if(SetResolution(GCM_RESOLUTION_480)) {
		std::cout << "Using PAL" << std::endl;
	}
	else {
		std::cout << "No valid resolution available!" << std::endl;
		return;
	}

	CellVideoOutState			vid_state;
	CellVideoOutResolution		resolution;

	cellVideoOutGetState(CELL_VIDEO_OUT_PRIMARY, 0, &vid_state);				//Query the current state of the video output
	cellVideoOutGetResolution(vid_state.displayMode.resolutionId, &resolution);	//Query the current state of the video resolution

	screenWidth			= resolution.width;
	screenHeight		= resolution.height;
	screenRatio			= (float)screenWidth / (float)screenHeight;



	//Configure the video output...
	CellVideoOutConfiguration	video_cfg;
	memset(&video_cfg, 0, sizeof(CellVideoOutConfiguration));
	video_cfg.resolutionId	= vid_state.displayMode.resolutionId;
	video_cfg.format		= CELL_VIDEO_OUT_BUFFER_COLOR_FORMAT_X8R8G8B8;
	video_cfg.pitch			= screenWidth*4;

	cellVideoOutConfigure(CELL_VIDEO_OUT_PRIMARY,	&video_cfg, NULL, 0);
	cellGcmSetFlipMode(CELL_GCM_DISPLAY_VSYNC);	//V-sync, aka swapinterval

	//Get the pointer to the start of graphics memory, as we need it to 
	//allocate the front, back, and depth buffers. 
	CellGcmConfig config;
	cellGcmGetConfiguration(&config);
	setLocalMem((uint32_t)config.localAddress);
}

/*
Surfaces are GCMs implementation of Frame Buffers. In fact, in GCM, even the
front and back buffers are 'FBOs'. We could probably have a single surface,
and swap the colour target for the back and front buffer, but it's easier to
just have a surface for the two buffers, and simply swap between them. 

Surfaces can have 4 colour attachments; attachments are turned off by giving
them a memory offset of 0.

Note: We set the memory offset for the first colour attachment on line 63, in
the init_display function.
*/
void GCMRenderer::InitSurfaces()	{
	std::cout << "Initialising surfaces!" << std::endl;

	//Memory for these buffers has to be 64-byte aligned!
	depthPitch = screenWidth*4;
	uint32_t depthSize	= depthPitch*screenHeight;
	void* depthBuffer	= localMemoryAlign(64, depthSize);
	cellGcmAddressToOffset(depthBuffer, &depthOffset);

	uint32_t colourPitch	=	screenWidth*4;	//Pitch is defined as (width * bytes per pixel)
	uint32_t colourSize	=	colourPitch*screenHeight;

	for(int i = 0; i < 2; ++i) {
		surfaces[i].colorPitch[0] = colourPitch;
		void*buffer = localMemoryAlign(64, colourSize);	//Allocate enough memory for the buffer
		cellGcmAddressToOffset(buffer, &surfaces[i].colorOffset[0]);	//and calculate the offset
		//Attach the memory to a surface
		cellGcmSetDisplayBuffer(i, surfaces[i].colorOffset[0], colourPitch, screenWidth, screenHeight);

		//Disable the other colour attachments for this surface
		for(int j = 1; j < 4; ++j) {
			surfaces[i].colorLocation[j]	=	CELL_GCM_LOCATION_LOCAL;
			surfaces[i].colorOffset[j]		=	0;
			surfaces[i].colorPitch[j]		=	64;
		}

		surfaces[i].colorFormat 		= CELL_GCM_SURFACE_A8R8G8B8;
		surfaces[i].colorTarget			= CELL_GCM_SURFACE_TARGET_0;
		surfaces[i].colorLocation[0]	= CELL_GCM_LOCATION_LOCAL;

		surfaces[i].depthFormat 		= CELL_GCM_SURFACE_Z24S8;
		surfaces[i].depthLocation		= CELL_GCM_LOCATION_LOCAL;

		//Both surfaces are sharing the same depth buffer. 
		surfaces[i].depthOffset			= depthOffset;
		surfaces[i].depthPitch 			= depthPitch;

		surfaces[i].type				= CELL_GCM_SURFACE_PITCH;
		surfaces[i].antialias			= CELL_GCM_SURFACE_CENTER_1;

		surfaces[i].width 				= screenWidth;
		surfaces[i].height 				= screenHeight;
		surfaces[i].x 					= 0;
		surfaces[i].y 					= 0;
	}

	//This is how we tell GCM to render into a surface, like we do in OpenGL
	//using the function glBindFramebuffer.
	cellGcmSetSurface(&surfaces[0]);
}

/*
Render out the final frame, and quit GCM.
*/
GCMRenderer::~GCMRenderer(void)	{
	cellGcmSetWaitFlip();
	cellGcmFinish(1);	// Let PPU wait for all commands done (include waitFlip)
}

/*
In GCM, all you are provided with is a pointer to graphics memory - all we can do
is advance that pointer along as we allocate new memory. We could write a fancy
memory manager, but that's left up to you, if you need it! (One such system could
be implemented with a doubly linked list of a simple data structure to keep track
of start pointer and size....enough help!)
*/

/*
Allocate some amount of graphics memory. Returns a pointer to the start of the 
allocated area.
*/
void* GCMRenderer::localMemoryAlloc(const uint32_t size)	{
	uint32_t currentHeap = localHeapStart;		//store current memory pointer
	localHeapStart += (size + 1023) & (~1023);	//advance the memory pointer
	return (void*)currentHeap;					//return the temp 'old' memory pointer
}

/*
Allocate an ALIGNED area of graphics memory. Certain GCM data structures (fragment shaders, textures)
must be at a certain alignment, or they'll not work correctly. This function uses localMemoryAlloc,
but 'twiddles' the graphics pointer to advance it to the next valid alignment position first. 
*/
void* GCMRenderer::localMemoryAlign(const uint32_t alignment, const uint32_t size)	{
	localHeapStart = (localHeapStart + alignment-1) & (~(alignment-1));
	return (void*)localMemoryAlloc(size);
}

/*
Explicitly set the location in graphics memory the pointer is pointing at. I can't remember
why I added this function, or made it public...it's pretty dangerous.
*/
void GCMRenderer::setLocalMem(uint32_t to) {
	localHeapStart = to;
}

/*
Sets up the current viewport
*/
void GCMRenderer::SetViewport() {
	uint16_t x,y,w,h;
	float min, max;
	float scale[4],offset[4];

	/*
	I kinda gloss over what this function does in the GCM tutorial text,
	but if you can't be bothered to look up the cellGcmSetViewport SDK
	documentation, here's basically what it does. 

	Remember how in the Introduction to Graphics tutorial, it was explained
	that at the 'end' of the pipeline, your graphics end up in 'Native Device'
	space, which has an axis ranging from -1 to 1? The viewport function must
	then map those -1 to 1 axis coordinates to your screen space. Internally 
	to GCM, this will be done with another matrix.

	To map an x axis range of -1 to 1 to screen coordinates, we can multiply
	the x axis by half the width (assuming a default width of 720, that gives
	us a new range of -360 to 360), and then translating / offsetting it also
	by half of the width (moving us to 0 to 720). The same is done with the Y
	axis, while the Z axis goes from 0.0 to 1.0 (remember the depth buffer
	values?)

	Note also the y axis scaling is inverted - NDC has <MAX AXIS VALUE> at the 
	'top', whereas our screenspace coordinates have <MIN AXIS VALUE> at the 
	'top', so we can 'flip' the image the right way up with a negative scale.
	*/

	x = 0;	//starting position of the viewport (left of screen)
	y = 0;  //starting position of the viewport (top of screen)
	w = screenWidth;
	h = screenHeight;
	min = 0.0f;
	max = 1.0f;
	//Scale our NDC coordinates to the size of the screen
	scale[0] = w * 0.5f;
	scale[1] = h * -0.5f;
	scale[2] = (max - min) * 0.5f;
	scale[3] = 0.0f;

	//Translate from a range starting from -1 to a range starting at 0
	offset[0] = x + scale[0];
	offset[1] = y + h * 0.5f;
	offset[2] = (max + min) * 0.5f;
	offset[3] = 0.0f;

	//analogous to the glViewport function...but with extra values!
	cellGcmSetViewport(x, y, w, h, min, max, scale, offset);
}

void GCMRenderer::SwapBuffers() {
	// wait until FlipStatus = 0 so that PPU does not run too ahead of RSX
	// FlipStatus turns to 0 when the previous flip is finished
	while (cellGcmGetFlipStatus()!=0){
		sys_timer_usleep(300);
	}

	// reset FlipStatus = 1
	cellGcmResetFlipStatus();

	// queue Flip command
	cellGcmSetFlip((uint8_t)swapValue);
	//Force all commands up to and including the flip command to be processed
	cellGcmFlush();

	// wait the flip is done before start rendering to the other buffer
	cellGcmSetWaitFlip();

	//Invert the boolean 
	swapValue = !swapValue;
	//Use the bool as an index into our array of surfaces (a bit naughty!)
	cellGcmSetSurface(&surfaces[swapValue]);
}

/*
Clears the screen. Technically 'clears the current buffer', but that'll always be
the 'back buffer' when everything is used properly. 
*/
void GCMRenderer::ClearBuffer() {

	cellGcmSetColorMask(
		CELL_GCM_COLOR_MASK_R|
		CELL_GCM_COLOR_MASK_G|
		CELL_GCM_COLOR_MASK_B|
		CELL_GCM_COLOR_MASK_A);

	//glClearColor, but taking a single 32 bit value, in ARGB byte format
	//Note how we're shifting bytes to byte aligned positions in the 32
	//bit value we're sending.
	cellGcmSetClearColor((64<<0)|(64<<8)|(64<<16)|(255<<24));

	//Tell GCM that we want to clear all of the colours of the current
	//surface
	cellGcmSetClearSurface(CELL_GCM_CLEAR_Z|CELL_GCM_CLEAR_S|
		CELL_GCM_CLEAR_R|CELL_GCM_CLEAR_G|CELL_GCM_CLEAR_B|CELL_GCM_CLEAR_A);
}

/*
Sets the current vertex and shader programs. It was never really discussed
during the graphics module, but you can have shader programs that are part
of multiple shader executables. GCM is /always/ like this, with vertex
and fragment shaders entirely separate. To keep consistency with the method
you were all introduced to in Graphics for Games, this function sets both
fragment and vertex shader.

Note how the vertex shader takes a pointer to the ucode, while fragment
shaders take an offset. This looks messy, and might initially seem
ridiculous, but it is because vertex shaders can be ran from system memory,
while fragment shaders MUST be ran in graphics memory, and so its address
must be 'offsettable'. 
*/
void	GCMRenderer::SetCurrentShader(VertexShader & vert, FragmentShader &frag) {
	currentFrag = &frag;
	currentVert = &vert;

	cellGcmSetFragmentProgram(currentFrag->program, currentFrag->offset);
	cellGcmSetVertexProgram(currentVert->program, currentVert->ucode);
}

//Sets the camera. Can be NULL
void	GCMRenderer::SetCamera(Camera* c) {
	camera = c;
}

//Sets the root node. Can be NULL
void	GCMRenderer::SetRootNode(SceneNode * r) {
	root = r;
}
/*
Draws a scene node. Based on the second scene graph tutorial code.

For those who are interested, the reason node drawing has become part of the
renderer, and not a function of the SceneNode class was to keep SceneNode
'agnostic' - it could be used for an OpenGL system, or GCM, or anything else,
without having to be changed. This makes it slightly less flexible in that
it means the renderer has to know what to do with every type of scene node
(think about things like a shader change node, or a uniform change node). It
also helps keep track of where API header files have to be included, libraries
added, etc. 

We can't really get around Mesh having to have some sort of API-specific code
in it, though (think about separate VBOs vs interleaves ones etc), so API code
still ends up leaking out of the Renderer class, so maybe you'll want to 
refactor SceneNode so it handles it's own drawing after all (via platform 
specific #ifdefs to encapsulate API code). You'll have to think up some way
of safely setting shaders and textures on a renderer...
*/
void	GCMRenderer::DrawNode(SceneNode*n)	{
	if(n->GetMesh()) {
		//GCC complains about function returns being used as parameters passed
		//around, or we'd just use GetWorldTransform as the function param
		Matrix4 transform = n->GetWorldTransform() * Matrix4::scale(n->GetModelScale());
		//Matrix4 tester = Matrix4::identity();

		//tester.setTranslation(transform.getTranslation());
		currentVert->SetParameter("color",(float*)n->GetColour());
		currentVert->SetParameter("modelMat", transform);
		//currentVert->SetParameter("modelMat", tester);

		/*
		If your SceneNodes can have their own texture, handle the setting of it
		here! In Graphics for Games, the Mesh::Draw function handled the setting
		of textures to units, but the slight changes in how GCM handles textures
		make it more intuitive to place it here, instead.
		*/
	
		SetTextureSampler(currentFrag->GetParameter("texture"),n->GetMesh()->GetDefaultTexture());

		/*
		The GCM Mesh class needs the current vertex shader, fragment
		shader is just sent for convenience, in case it's needed in future...
		*/
		n->GetMesh()->Draw(*currentVert,*currentFrag);
	}

	//Remember to draw our children!
	for(std::vector<SceneNode*>::const_iterator i = n->GetChildIteratorStart(); i != n->GetChildIteratorEnd(); ++i) {
		DrawNode((*i));
	}
}

/*
Loads a TGA file in. Unlike in the Graphics for Games module, in which we relied
on an external library (SOIL) to load in our textures, this time we're going to
have a go at doing it ourselves! Unlike OpenGL, which represents textures just 
with a 'name', in GCM they have a structure type, CellGcmTexture.

Pretty much the easiest type of texture to load in is the TARGA file. Sadly, 
I've lost the tutorial that explained how it's so easy, so I'll have to do
it here, instead. Basically, a targa file has an 18 byte header that defines
the size and bit depth of the texture, and then there's a big chunk of data->
If it's uncompressed, that data is simply the texture, ready for using, but
if it's compressed you'll have to do more with it (I've not yet written this
bit, but it should be fairly straightforward, TGA uses simple 'run length
encoding' for its compression technique. 

*/
CellGcmTexture* GCMRenderer::LoadTGA(std::string filename)	{
	CellGcmTexture* texture = new CellGcmTexture();	//A new GCM texture!

	std::ifstream file;

	std::string folder = SYS_APP_HOME + filename;
	//Attempt to open the texture file
	std::cout << "Loading TGA from(" << folder << ")" << std::endl;
	file.open(folder.c_str(), std::ios::binary);
	if(!file.is_open()){
		//File hasn't loaded! 
		delete texture;
		std::cout << "LoadTGATexture file error" << std::endl;
		return NULL;
	}

	//Read in the first 18 bytes of the file, containing the header
	unsigned char TGAheader[18];
	file.read((char*)TGAheader,sizeof(TGAheader));

	//Width and height take up 2 bytes each, so we need to do a bit of
	//bit shifting to make up the final value
	texture->width	= (TGAheader[12] + (TGAheader[13] << 8));
	texture->height = (TGAheader[14] + (TGAheader[15] << 8));
	texture->pitch  = texture->width*(TGAheader[16] / 8);
	texture->depth  = 1;

	std::cout << filename << " has width " << texture->width << std::endl;
	std::cout << filename << " has height " << texture->height << std::endl;
	std::cout << filename << " has bitdepth " << (unsigned int)TGAheader[16] << std::endl;

	//We're going to assume it's uncompressed data, and just read in a load of data->
	int size = texture->width*texture->height*(TGAheader[16] / 8);

	std::cout << filename << " has size "	 << (unsigned int)size		<< std::endl;

	char*rsxdata = (char*)localMemoryAlign(128, size);	//A texture in graphics memory needs aligning...

	//Read in the texture straight into graphics memory. Challenge! How fast will this data
	//transfer be? Why? How could it be sped up?
	file.read(rsxdata,size);	
	file.close();				//Close the file


	unsigned int offset;

	//As we're dealing with graphics memory, we need an offset
	if(cellGcmAddressToOffset( rsxdata, &offset ) != CELL_OK) {
		std::cout << "offset failure (texture)!" << std::endl;		
	};

	texture->location = CELL_GCM_LOCATION_LOCAL;	//We want the texture in graphics memory
	texture->offset   = offset;						//at he following offset

	std::cout << filename << " has RSX memory location " << (unsigned int)rsxdata	<< std::endl;
	std::cout << filename << " has RSX memory offset "   << (unsigned int)offset	<< std::endl;

	texture->format  = CELL_GCM_TEXTURE_A8R8G8B8;	//ARGB format - there doesn't seem to be a RGB format!
	texture->format |= CELL_GCM_TEXTURE_LN;		//Data is a 'linear' array of values

	/*
	Remap allows us to swap around values on the texture when it is sampled.
	For example we could swap around red and blue (normally, we think of textures
	as being in 'red green blue alpha' format, whereas TGA saves its data in
	'blue green red alpha' format, so you might think we need a remapping to handle
	that, but no! GCM uses 'b g r a' format natively, so the data can stay as-is
	*/

	texture->remap = 
		CELL_GCM_TEXTURE_REMAP_REMAP << 14 |
		CELL_GCM_TEXTURE_REMAP_REMAP << 12 |
		CELL_GCM_TEXTURE_REMAP_REMAP << 10 |
		CELL_GCM_TEXTURE_REMAP_REMAP << 8 |
		CELL_GCM_TEXTURE_REMAP_FROM_A << 6 |
		CELL_GCM_TEXTURE_REMAP_FROM_R << 4 |
		CELL_GCM_TEXTURE_REMAP_FROM_G << 2 |
		CELL_GCM_TEXTURE_REMAP_FROM_B;

	texture->mipmap		= 1;		//How many mipmap levels are there (1 == 'largest level mipmap')
	texture->cubemap	= CELL_GCM_FALSE;	//No...it's not a cubemap
	texture->dimension	= CELL_GCM_TEXTURE_DIMENSION_2;	//It's a 2D Texture...

	std::cout << "Finished loading texture!" << std::endl;

	return texture;
}

/*
Drop a DDS file onto the DDS2GTF exe inside the 
C:\usr\local\cell\host-win32\bin folder to make a GTF 
file suitable for loading with this function.
*/
CellGcmTexture* GCMRenderer::LoadGTF(std::string filename) {
	CellGcmTexture* texture = new CellGcmTexture();	//A new GCM texture!

	std::ifstream file;

	std::string folder = SYS_APP_HOME + filename;
	//Attempt to open the texture file
	std::cout << "Loading GTF from(" << folder << ")" << std::endl;
	file.open(folder.c_str(), std::ios::binary);
	if(!file.is_open()){
		//File hasn't loaded! 
		delete texture;
		std::cout << "LoadGTF file error" << std::endl;
		return NULL;
	}

	CellGtfFileHeader header;
	file.read((char*)&header,sizeof(CellGtfFileHeader));

	CellGtfTextureAttribute*attributes = new CellGtfTextureAttribute[header.NumTexture];

	file.read((char*)attributes,header.NumTexture * sizeof(CellGtfTextureAttribute));


	/*
	GTF textures can technically have multiple textures in it, for now
	lets just load 1
	*/
	for(int i = 0; i < min(1,header.NumTexture); ++i) {
		memcpy((void*)texture,(void*)&(attributes[i].tex), sizeof(CellGcmTexture));

		file.seekg(attributes[i].OffsetToTex);

		char*rsxdata = (char*)localMemoryAlign(128, attributes[i].TextureSize);
		file.read(rsxdata,attributes[i].TextureSize);

		cellGcmAddressToOffset( rsxdata, &texture->offset );
	}

	std::cout << "LoadGTF success!" << std::endl;

	delete[] attributes;

	return texture;
}

/*
GCM handles texturing in a pretty similar way to OpenGL - texture samplers in 
the shader. Unlike OpenGL, where you could bind a texture to a texture unit,
and not necessarily know which texture sampler was sampling from it (and vice
versa, of course), in GCM you just grab the sampler, and set a texture on it -
the assignment between texture sampler and units is handled automatically.

Instead of spreading texture sampler and unit assignments out throughout the code,
it therefore makes sense to just have a function that takes a handle to a uniform
texture sampler and a texture, and binds one to the other. 

Example Usage:
Set the sampler called 'texture' to the meshes default texture:
	SetTextureSampler(currentFrag->GetParameter("texture"), n->GetMesh()->GetDefaultTexture())

Set the sampler called 'diffuse' to have no texture
	SetTextureSampler(currentFrag->GetParameter("diffuse"), NULL)

*/
void	GCMRenderer::SetTextureSampler(CGparameter sampler, const CellGcmTexture *texture) {
	if(!sampler) {
		return; //cellGcmCgGetParameterResource dies on an invalid parameter!
	}
	//Get a pointer to the actual bound texture unit for this sampler
	CGresource unitResource = (CGresource)(cellGcmCgGetParameterResource(currentFrag->program, sampler) - CG_TEXUNIT0);

	cellGcmSetTexture(unitResource, texture);		//Set texture unit to sample from the given texture

	//If we want to turn off a texture unit, for example
	if(!texture) {
		cellGcmSetTextureControl(unitResource, CELL_GCM_FALSE, 0, 0, 0); //Disable sampling on the TU
		return;
	}

	cellGcmSetTextureControl(unitResource, CELL_GCM_TRUE, 0, 0, 0); //Enable sampling on the TU

	//Set how the texture is accessed. This is analogous to the 
	//glTexParameteri function you should be used to, by now.
	cellGcmSetTextureAddress(unitResource,
		CELL_GCM_TEXTURE_CLAMP,
		CELL_GCM_TEXTURE_CLAMP,
		CELL_GCM_TEXTURE_CLAMP,
		CELL_GCM_TEXTURE_UNSIGNED_REMAP_NORMAL,
		CELL_GCM_TEXTURE_ZFUNC_LESS,
		0);

	//Set how the texture is filtered, just like glTexParameteri, again.
	//We get an extra function on top of OpenGL - getting to choose the
	//actual type of linear filter to be applied. We're going to use
	//quincunx filtering, which takes 5 samples per fragment.
	cellGcmSetTextureFilter(unitResource, 0,
		CELL_GCM_TEXTURE_LINEAR,
		CELL_GCM_TEXTURE_LINEAR,
		CELL_GCM_TEXTURE_CONVOLUTION_QUINCUNX);
}

bool GCMRenderer::SetResolution(GCMResolution resolution) {
	//First, query if resolution is viable
	CellVideoOutResolution resInfo;

	if(!cellVideoOutGetResolutionAvailability(CELL_VIDEO_OUT_PRIMARY,resolution, CELL_VIDEO_OUT_ASPECT_AUTO,0)) {
		return false;
	}

	if(cellVideoOutGetResolution( resolution, &resInfo ) != 0) {
		return false;
	}

	CellVideoOutConfiguration config = {	resolution, 
		CELL_VIDEO_OUT_BUFFER_COLOR_FORMAT_X8R8G8B8,
		CELL_VIDEO_OUT_ASPECT_AUTO,
		{0,0,0,0,0,0,0,0,0},
		cellGcmGetTiledPitchSize( resInfo.width * 4 )
	};


	if(cellVideoOutConfigure( CELL_VIDEO_OUT_PRIMARY, &config, NULL, 0 ) != 0) {
		return false;
	}

	return true;
}