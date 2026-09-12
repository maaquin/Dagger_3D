#include "camera.h"
#include "render.h"
#include <math.h>
#include <string.h> // memcpy, memoria
#include "default_shbin.h" // shader

#define DISPLAY_TRANSFER_FLAGS \
    (GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
    GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | \
    GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
    GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

void Renderer::init() {
    // inicialización
    gfxInitDefault(); // gpu
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);

    top = C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
    C3D_RenderTargetSetOutput(top, GFX_TOP, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);

    bottom = C3D_RenderTargetCreate(240, 320, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
    C3D_RenderTargetSetOutput(bottom, GFX_BOTTOM, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);

    // color de fondo
    clrClear = 0x68B0D8FF;

    // test
    Vertex tri[3] = {
        {   0.0f, 200.0f, 0.5f,   1.0f, 0.2f, 0.2f, 1.0f},
        {-100.0f,  40.0f, 0.5f,   0.2f, 1.0f, 0.2f, 1.0f},
        { 100.0f,  40.0f, 0.5f,   0.2f, 0.2f, 1.0f, 1.0f},
    };

    // vbo: memoria para gpus
    vbo_data = (Vertex *)linearAlloc(sizeof(tri));
    memcpy(vbo_data, tri, sizeof(tri));

    // carga del shader
    vshader_dvlb = DVLB_ParseFile((u32 *)default_shbin, default_shbin_size);
    shaderProgramInit(&program);
    shaderProgramSetVsh(&program, &vshader_dvlb->DVLE[0]);
    C3D_BindProgram(&program);

    uLoc_mvp = shaderInstanceGetUniformLocation(program.vertexShader, "mvp_mat");

    // Proyección ORTOGRÁFICA (2D con pipeline moderno). Rango visible:
    // X de -200 a 200, Y de 0 (arriba) a 240 (abajo). "Tilt" = adaptada
    // a la pantalla rotada de la 3DS. La de abajo es igual pero más angosta.
    Mtx_OrthoTilt(&projectionTop, -200.0f, 200.0f, 0.0f, 240.0f, 0.0f, 1.0f, true);
    Mtx_OrthoTilt(&projectionBot, -160.0f, 160.0f, 0.0f, 240.0f, 0.0f, 1.0f, true);

    // Configurar cómo la GPU lee nuestros vértices del VBO:
    // atributo 0 = posición (3 floats), atributo 1 = color (4 floats)
    C3D_AttrInfo *attrInfo = C3D_GetAttrInfo();
    AttrInfo_Init(attrInfo);
    AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 3); // x, y, z
    AttrInfo_AddLoader(attrInfo, 1, GPU_FLOAT, 4); // r, g, b, a

    // Vincular el VBO: stride = sizeof(Vertex), 2 atributos,
    // permutation 0x10 = los atributos van a los registros 0 y 1 del shader
    C3D_BufInfo *bufInfo = C3D_GetBufInfo();
    BufInfo_Init(bufInfo);
    BufInfo_Add(bufInfo, vbo_data, sizeof(Vertex), 2, 0x10);

    // TexEnv: cómo se genera el color final de cada píxel.
    C3D_TexEnv *env = C3D_GetTexEnv(0);
    C3D_TexEnvInit(env);
    C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
    C3D_TexEnvFunc(env, C3D_Both, GPU_REPLACE);
}

void Renderer::renderFrame(const Camera& cam) {
    C3D_Mtx view, model;
    cameraViewMatrix(cam, &view);
    Mtx_Identity(&model); // renderizado del test

    C3D_Mtx mvpTop, mvpBot;
    Mtx_Multiply(&mvpTop, &projectionTop, &view);
    Mtx_Multiply(&mvpTop, &mvpTop, &model);
    Mtx_Multiply(&mvpBot, &projectionBot, &view);
    Mtx_Multiply(&mvpBot, &mvpBot, &model);

    // render
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

    // Pantalla superior
    C3D_RenderTargetClear(top, C3D_CLEAR_ALL, clrClear, 0);
    C3D_FrameDrawOn(top);
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_mvp, &mvpTop);
    C3D_DrawArrays(GPU_TRIANGLES, 0, 3);

    // Pantalla inferior (lo mismo, otra proyección)
    C3D_RenderTargetClear(bottom, C3D_CLEAR_ALL, clrClear, 0);
    C3D_FrameDrawOn(bottom);
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_mvp, &mvpBot);
    C3D_DrawArrays(GPU_TRIANGLES, 0, 3);

    C3D_FrameEnd(0);
}

void Renderer::cleanup() {
    // limpieza en orden inverso
    linearFree(vbo_data);
    shaderProgramFree(&program);
    DVLB_Free(vshader_dvlb);
    C3D_Fini();
    gfxExit();
}