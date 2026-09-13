#include "camera.h"
#include "mesh.h"
#include "render.h"
#include <math.h>
#include <string.h>        // memcpy, memoria
#include "default_shbin.h" // shader

#define DISPLAY_TRANSFER_FLAGS                                                       \
    (GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
     GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) |                                  \
     GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) |                                  \
     GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

void Renderer::init()
{
    // inicialización
    gfxInitDefault(); // gpu
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);

    top = C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
    C3D_RenderTargetSetOutput(top, GFX_TOP, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);

    bottom = C3D_RenderTargetCreate(240, 320, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
    C3D_RenderTargetSetOutput(bottom, GFX_BOTTOM, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);

    // color de fondo
    clrClear = 0x68B0D8FF;

    // carga del shader
    vshader_dvlb = DVLB_ParseFile((u32 *)default_shbin, default_shbin_size);
    shaderProgramInit(&program);
    shaderProgramSetVsh(&program, &vshader_dvlb->DVLE[0]);
    C3D_BindProgram(&program);

    uLoc_mvp = shaderInstanceGetUniformLocation(program.vertexShader, "mvp_mat");

    // perspectiva o lente
    Mtx_PerspTilt(&projectionTop, 80.0f * 3.14159265f / 180.0f, 400.0f / 240.0f, 0.01f, 1000.0f, true);
    Mtx_OrthoTilt(&projectionBot, -160.0f, 160.0f, 0.0f, 240.0f, 0.0f, 1.0f, true);

    // Configurar cómo la GPU lee nuestros vértices del VBO:
    // atributo 0 = posición (3 floats), atributo 1 = color (4 floats)
    C3D_AttrInfo *attrInfo = C3D_GetAttrInfo();
    AttrInfo_Init(attrInfo);
    AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 3); // x, y, z
    AttrInfo_AddLoader(attrInfo, 1, GPU_FLOAT, 4); // r, g, b, a

    // TexEnv: cómo se genera el color final de cada píxel.
    C3D_TexEnv *env = C3D_GetTexEnv(0);
    C3D_TexEnvInit(env);
    C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
    C3D_TexEnvFunc(env, C3D_Both, GPU_REPLACE);

    // profundidad: que pixeles están mas cerca que otros
    C3D_DepthTest(true, GPU_GEQUAL, GPU_WRITE_ALL);
}

void Renderer::renderFrame(const Camera &cam, const Mesh &mesh, const C3D_Mtx &model)
{
    C3D_Mtx view;
    cameraViewMatrix(cam, &view);

    C3D_Mtx mvpTop;
    Mtx_Multiply(&mvpTop, &projectionTop, &view);
    Mtx_Multiply(&mvpTop, &mvpTop, &model);

    // render
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

    // Pantalla superior
    C3D_RenderTargetClear(top, C3D_CLEAR_ALL, clrClear, 0);
    C3D_FrameDrawOn(top);

    // conexión de memoria del mesh con gpu
    C3D_BufInfo *bufInfo = C3D_GetBufInfo();
    BufInfo_Init(bufInfo);
    BufInfo_Add(bufInfo, mesh.vbo_data, sizeof(Vertex), 2, 0x10);

    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_mvp, &mvpTop);
    C3D_DrawArrays(GPU_TRIANGLES, 0, mesh.vertexCount);

    C3D_FrameEnd(0);
}

void Renderer::cleanup()
{
    // limpieza en orden inverso
    shaderProgramFree(&program);
    DVLB_Free(vshader_dvlb);
    C3D_Fini();
    gfxExit();
}