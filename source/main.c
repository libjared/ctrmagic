#include <3ds.h>
#include <citro3d.h>
#include <stdio.h>
#include <string.h>
#include "vshader_shbin.h"
#include "myfs.h"
#include "music.h"

#define CLEAR_COLOR 0x68B0D8FF

#define DISPLAY_TRANSFER_FLAGS \
	(GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
	GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
	GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

typedef struct { float position[3]; float texcoord[2]; float normal[3]; } vertex;

static const vertex cube_vertex_list[] =
{
	// First face (PZ)
	// First triangle
	{ {-0.5f, -0.5f, +0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, +1.0f} },
	{ {+0.5f, -0.5f, +0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, +1.0f} },
	{ {+0.5f, +0.5f, +0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, +1.0f} },
	// Second triangle
	{ {+0.5f, +0.5f, +0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, +1.0f} },
	{ {-0.5f, +0.5f, +0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, +1.0f} },
	{ {-0.5f, -0.5f, +0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, +1.0f} },

	// Second face (MZ)
	// First triangle
	{ {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f} },
	{ {-0.5f, +0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f} },
	{ {+0.5f, +0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f} },
	// Second triangle
	{ {+0.5f, +0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f} },
	{ {+0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f} },
	{ {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f} },

	// Third face (PX)
	// First triangle
	{ {+0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {+1.0f, 0.0f, 0.0f} },
	{ {+0.5f, +0.5f, -0.5f}, {1.0f, 0.0f}, {+1.0f, 0.0f, 0.0f} },
	{ {+0.5f, +0.5f, +0.5f}, {1.0f, 1.0f}, {+1.0f, 0.0f, 0.0f} },
	// Second triangle
	{ {+0.5f, +0.5f, +0.5f}, {1.0f, 1.0f}, {+1.0f, 0.0f, 0.0f} },
	{ {+0.5f, -0.5f, +0.5f}, {0.0f, 1.0f}, {+1.0f, 0.0f, 0.0f} },
	{ {+0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {+1.0f, 0.0f, 0.0f} },

	// Fourth face (MX)
	// First triangle
	{ {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f} },
	{ {-0.5f, -0.5f, +0.5f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f} },
	{ {-0.5f, +0.5f, +0.5f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f} },
	// Second triangle
	{ {-0.5f, +0.5f, +0.5f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f} },
	{ {-0.5f, +0.5f, -0.5f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f} },
	{ {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f} },

	// Fifth face (PY)
	// First triangle
	{ {-0.5f, +0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, +1.0f, 0.0f} },
	{ {-0.5f, +0.5f, +0.5f}, {1.0f, 0.0f}, {0.0f, +1.0f, 0.0f} },
	{ {+0.5f, +0.5f, +0.5f}, {1.0f, 1.0f}, {0.0f, +1.0f, 0.0f} },
	// Second triangle
	{ {+0.5f, +0.5f, +0.5f}, {1.0f, 1.0f}, {0.0f, +1.0f, 0.0f} },
	{ {+0.5f, +0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, +1.0f, 0.0f} },
	{ {-0.5f, +0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, +1.0f, 0.0f} },

	// Sixth face (MY)
	// First triangle
	{ {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f} },
	{ {+0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f} },
	{ {+0.5f, -0.5f, +0.5f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f} },
	// Second triangle
	{ {+0.5f, -0.5f, +0.5f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f} },
	{ {-0.5f, -0.5f, +0.5f}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f} },
	{ {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f} },
};

#define cube_vertex_list_count (sizeof(cube_vertex_list)/sizeof(cube_vertex_list[0]))

static DVLB_s* vshader_dvlb;
static shaderProgram_s program;
static int uLoc_projection, uLoc_modelView;
static int uLoc_lightVec, uLoc_lightHalfVec, uLoc_lightClr, uLoc_material;
static C3D_Mtx projection;
static C3D_Mtx material =
{
	{
	{ { 0.0f, 0.2f, 0.2f, 0.2f } }, // Ambient
	{ { 0.0f, 0.4f, 0.4f, 0.4f } }, // Diffuse
	{ { 0.0f, 0.8f, 0.8f, 0.8f } }, // Specular
	{ { 1.0f, 0.0f, 0.0f, 0.0f } }, // Emission
	}
};

static void* vbo_data;
static C3D_Tex kitten_tex;
static float angleX = 0.0, angleY = 0.0, angleZ = 0.0;

static void sceneInit(void)
{
	// Load the vertex shader, create a shader program and bind it
	vshader_dvlb = DVLB_ParseFile((u32*)vshader_shbin, vshader_shbin_size);
	shaderProgramInit(&program);
	shaderProgramSetVsh(&program, &vshader_dvlb->DVLE[0]);
	C3D_BindProgram(&program);

	// Get the location of the uniforms
	uLoc_projection   = shaderInstanceGetUniformLocation(program.vertexShader, "projection");
	uLoc_modelView    = shaderInstanceGetUniformLocation(program.vertexShader, "modelView");
	uLoc_lightVec     = shaderInstanceGetUniformLocation(program.vertexShader, "lightVec");
	uLoc_lightHalfVec = shaderInstanceGetUniformLocation(program.vertexShader, "lightHalfVec");
	uLoc_lightClr     = shaderInstanceGetUniformLocation(program.vertexShader, "lightClr");
	uLoc_material     = shaderInstanceGetUniformLocation(program.vertexShader, "material");

	// Configure attributes for use with the vertex shader
	C3D_AttrInfo* attrInfo = C3D_GetAttrInfo();
	AttrInfo_Init(attrInfo);
	AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 3); // v0=position
	AttrInfo_AddLoader(attrInfo, 1, GPU_FLOAT, 2); // v1=texcoord
	AttrInfo_AddLoader(attrInfo, 2, GPU_FLOAT, 3); // v2=normal

	// Create the VBO (vertex buffer object)
	vbo_data = linearAlloc(sizeof(cube_vertex_list));
	memcpy(vbo_data, cube_vertex_list, sizeof(cube_vertex_list));

	// Configure buffers
	C3D_BufInfo* bufInfo = C3D_GetBufInfo();
	BufInfo_Init(bufInfo);
	BufInfo_Add(bufInfo, vbo_data, sizeof(vertex), 3, 0x210);

	// Load the texture from file
	Handle fsHandle;
	u32 fsSize;
	fsopen(&fsHandle, &fsSize, "/3ds/ctrmagic/lava512.bin");
	char* buf = linearAlloc(sizeof(char) * fsSize);
	fsread(fsHandle, fsSize, buf);

	// and bind it to the first texture unit
	C3D_TexInit(&kitten_tex, 512, 512, GPU_RGBA8);
	C3D_TexUpload(&kitten_tex, buf);
	C3D_TexSetFilter(&kitten_tex, GPU_LINEAR , GPU_LINEAR);
	C3D_TexBind(0, &kitten_tex);

	// free the texture file
	linearFree(buf);

	// Configure the first fragment shading substage to blend the texture color with
	// the vertex color (calculated by the vertex shader using a lighting algorithm)
	// See https://www.opengl.org/sdk/docs/man2/xhtml/glTexEnv.xml for more insight
	C3D_TexEnv* env = C3D_GetTexEnv(0);
	C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_PRIMARY_COLOR, 0);
	C3D_TexEnvOp(env, C3D_Both, 0, 0, 0);
	C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);
}

static void sceneRender(int eye)
{
	float iod = osGet3DSliderState();
	iod *= eye == 0 ? -0.2f : 0.2f;
	// Compute the projection matrix
	Mtx_Identity(&projection);
	Mtx_PerspStereoTilt(&projection, 80.0f*M_PI/180.0f, C3D_AspectRatioTop, 0.01f, 1000.0f, iod, 2.0f, false);
	Mtx_RotateX(&projection, M_PI / 4, true);
	Mtx_Translate(&projection, 0, -5, -5, true);

	// Calculate the modelView matrix
	C3D_Mtx modelView;
	Mtx_Identity(&modelView);
	Mtx_RotateX(&modelView, angleX, true);
	Mtx_RotateY(&modelView, angleY, true);
	Mtx_RotateZ(&modelView, angleZ, true);

	// Update the uniforms
	C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projection,   &projection);
	C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_modelView,    &modelView);
	C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_material,     &material);
	C3D_FVUnifSet(GPU_VERTEX_SHADER, uLoc_lightVec,     0.0f, 0.0f, -1.0f, 0.0f);
	C3D_FVUnifSet(GPU_VERTEX_SHADER, uLoc_lightHalfVec, 0.0f, 0.0f, -1.0f, 0.0f);
	C3D_FVUnifSet(GPU_VERTEX_SHADER, uLoc_lightClr,     1.0f, 1.0f,  1.0f, 1.0f);

	// Draw the VBO
	C3D_DrawArrays(GPU_TRIANGLES, 0, cube_vertex_list_count);
}

static void sceneExit(void)
{
	// Free the texture
	C3D_TexDelete(&kitten_tex);

	// Free the VBO
	linearFree(vbo_data);

	// Free the shader program
	shaderProgramFree(&program);
	DVLB_Free(vshader_dvlb);
}

int main()
{
	// Initialize graphics
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	gfxSet3D(true);

	// Init the console
	consoleInit(GFX_BOTTOM, NULL);
	printf("What's up?\n");

	//init some other junk
	fsinit();
	musicinit();

	// Initialize the renderbuffer
	static C3D_RenderBuf rbLeft, rbRight;
	C3D_RenderBufInit(&rbLeft, 240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
	C3D_RenderBufInit(&rbRight, 240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
	rbLeft.clearColor = CLEAR_COLOR;
	rbRight.clearColor = CLEAR_COLOR;
	C3D_RenderBufClear(&rbLeft);
	C3D_RenderBufClear(&rbRight);

	// Initialize the scene
	sceneInit();

	// Main loop
	while (aptMainLoop())
	{
		gfxSwapBuffersGpu(); // Swap the framebuffers BEFORE waiting for vblank!!!
		gspWaitForVBlank();  // wait for VBlank
		musicTick(); //update music
		hidScanInput();

		// Respond to user input
		u32 kDown = hidKeysDown();
		u32 kHeld = hidKeysHeld();
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu

		circlePosition analog;
		hidCircleRead(&analog);
		if (analog.dx < 20 && analog.dx > -20) analog.dx = 0; //deadzones
		if (analog.dy < 20 && analog.dy > -20) analog.dy = 0;
		float howFarX = analog.dx / 160.0;
		float howFarY = analog.dy / 160.0; //no idea why its max and min is this
		float radsPerFrame = M_TAU / 90.0;

		if (kHeld & KEY_Y)
		{
			angleX += howFarX * radsPerFrame;
			angleY += howFarY * radsPerFrame;
		}
		else
		{
			angleZ += howFarX * radsPerFrame;
			angleY += howFarY * radsPerFrame;
		}

		// Render the scene twice
		C3D_RenderBufBind(&rbLeft);
		sceneRender(0);
		C3D_Flush();
		u32* leftFrame = (u32*)gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
		C3D_RenderBufTransfer(&rbLeft, leftFrame, DISPLAY_TRANSFER_FLAGS);
		C3D_RenderBufClear(&rbLeft);

		C3D_RenderBufBind(&rbRight);
		sceneRender(1);
		C3D_Flush();
		u32* rightFrame = (u32*)gfxGetFramebuffer(GFX_TOP, GFX_RIGHT, NULL, NULL);
		C3D_RenderBufTransfer(&rbRight, rightFrame, DISPLAY_TRANSFER_FLAGS);
		C3D_RenderBufClear(&rbRight);

		// Flush the framebuffers out of the data cache (not necessary with pure GPU rendering)
		//gfxFlushBuffers();
	}

	// Deinitialize the scene
	sceneExit();

	// Deinitialize graphics
	C3D_Fini();
	gfxExit();
	musicFree();
	return 0;
}