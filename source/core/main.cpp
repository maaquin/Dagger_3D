#include <cstdio>
#include <3ds.h>
#include "render.h"
#include "camera.h"
#include "mesh.h"
#include "bsaReader.h"
#include "arch3dParser.h"
#include "palette.h"
#include "textureParser.h"
#include "textureUtils.h"

int main(int argc, char **argv)
{
    // inicialización
    Renderer renderer;
    renderer.init();

    Camera cam;
    cameraInit(cam);

    // inicialización de mesh
    Mesh mesh;
    C3D_Mtx locationMesh;
    Mtx_Identity(&locationMesh);

    consoleInit(GFX_BOTTOM, NULL);

    BsaReader bsa;
    if (bsa.open("sdmc:/3ds/DAGGER/ARENA2/ARCH3D.BSA"))
    {
        printf("BSA abierto. Total: %d\n", bsa.getRecordCount());

        // Extraer el registro 0 de prueba
        std::vector<uint8_t> buffer;
        if (bsa.extractRecord(105, buffer))
        {
            std::vector<DaggerPoint> points;
            std::vector<DaggerPlane> planes;

            if (Arch3dParser::parse(buffer, points, planes))
            {
                // Pasamos los datos leídos a la GPU
                mesh.initFromDaggerData(points, planes, 256.0f);
                printf("Mesh inicializada con vertices: %d\n", mesh.vertexCount);
            }
        }
    }

    // test texturas

    Palette palette;
    if (!palette.load("sdmc:/3ds/DAGGER/ARENA2/PAL.PAL"))
    {
        printf("Error al cargar PAL.PAL\n");
    }
    else
    {
        printf("PAL.PAL cargada con exito\n");
    }

    C3D_Tex texture;
    DecodedTexture tex;

    if (TextureParser::loadRecord("sdmc:/3ds/dagger/arena2/TEXTURE.119", 2, palette, tex))
    {
        // 1. Swizzling (Tiling 8x8)
        std::vector<uint32_t> tiledPixels(tex.potWidth * tex.potHeight);
        TextureUtils::tileImage8x8(tex.linearPixels.data(), tiledPixels.data(),
                                   tex.width, tex.height, tex.potWidth, tex.potHeight);

        // 2. Subir a la GPU
        C3D_TexInit(&texture, (u16)tex.potWidth, (u16)tex.potHeight, GPU_RGBA8);
        C3D_TexSetFilter(&texture, GPU_NEAREST, GPU_NEAREST);
        C3D_TexUpload(&texture, tiledPixels.data());
        C3D_TexSetWrap(&texture, GPU_REPEAT, GPU_REPEAT);
    }

    // juego
    while (aptMainLoop())
    {
        // inputs
        hidScanInput();
        u32 kHeld = hidKeysHeld();
        u32 kDown = hidKeysDown();

        if (kDown & KEY_START)
            break; // start = salir

        // --- UPDATE ---
        // La única "lógica de juego" por ahora: mover la cámara.
        cameraUpdate(cam, kHeld);

        // printf("\x1b[1;1H"); // Mueve el cursor a la esquina superior izquierda
        // printf("PosX: %f  PosY: %f  PosZ: %f   \n", cam.pos.x, cam.pos.y, cam.pos.z);
        // printf("Pitch: %f Yaw: %f              \n", cam.pitch, cam.yaw);

        // texturizado
        C3D_TexBind(0, &texture);

        C3D_TexEnv *env = C3D_GetTexEnv(0);
        C3D_TexEnvInit(env);
        C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
        C3D_TexEnvOpRgb(env, GPU_TEVOP_RGB_SRC_COLOR, GPU_TEVOP_RGB_SRC_COLOR, GPU_TEVOP_RGB_SRC_COLOR);
        C3D_TexEnvOpAlpha(env, GPU_TEVOP_A_SRC_ALPHA, GPU_TEVOP_A_SRC_ALPHA, GPU_TEVOP_A_SRC_ALPHA);
        
        C3D_TexEnvFunc(env, C3D_Both, GPU_REPLACE);

        // renderizado
        renderer.renderFrame(cam, mesh, locationMesh);
    }

    // limpieza
    renderer.cleanup();
    return 0;
}