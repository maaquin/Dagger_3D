#include <cstdio>
#include <3ds.h>
#include "render.h"
#include "camera.h"
#include "mesh.h"
#include "bsaReader.h"
#include "arch3dParser.h"
#include "textureParser.h"
#include "textureUtils.h"
#include "resourceManager.h"
#include "meshManager.h"

int main(int argc, char **argv)
{
    // inicialización
    Renderer renderer;
    renderer.init();

    Camera cam;
    cameraInit(cam);

    // inicialización de mesh
    C3D_Mtx locationMesh;
    Mtx_Identity(&locationMesh);

    consoleInit(GFX_BOTTOM, NULL);

    BsaReader bsa;
    bsa.open("sdmc:/3ds/DAGGER/ARENA2/ARCH3D.BSA");
    MeshManager meshManager;
    meshManager.setBsaReader(&bsa);

    // test mesh
    MeshResource *testMesh = meshManager.getMesh({105}, 64.0);

    // texturas
    Palette palette;
    palette.load("sdmc:/3ds/DAGGER/ARENA2/PAL.PAL");
    ResourceManager resourceManager;
    resourceManager.setPalette(&palette);

    // configuración de gpu
    C3D_TexEnv *env = C3D_GetTexEnv(0);
    C3D_TexEnvInit(env);
    C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
    C3D_TexEnvOpRgb(env, GPU_TEVOP_RGB_SRC_COLOR, GPU_TEVOP_RGB_SRC_COLOR, GPU_TEVOP_RGB_SRC_COLOR);
    C3D_TexEnvOpAlpha(env, GPU_TEVOP_A_SRC_ALPHA, GPU_TEVOP_A_SRC_ALPHA, GPU_TEVOP_A_SRC_ALPHA);
    C3D_TexEnvFunc(env, C3D_Both, GPU_REPLACE);

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

        // renderizado
        if (testMesh)
        {
            renderer.renderFrame(cam, *testMesh, locationMesh, resourceManager);
        }
    }

    // limpieza
    renderer.cleanup();
    return 0;
}