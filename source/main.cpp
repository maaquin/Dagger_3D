#include <cstdio>
#include <3ds.h>
#include "render.h"
#include "camera.h"
#include "mesh.h"

int main(int argc, char **argv)
{
    // inicialización
    Renderer renderer;
    renderer.init();
    
    Camera cam;
    cameraInit(cam);

    // cuarto test
    Mesh mesh;
    mesh.initCube();
    C3D_Mtx locationMesh;
    Mtx_Identity(&locationMesh);

    consoleInit(GFX_BOTTOM, NULL);

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

        printf("\x1b[1;1H"); // Mueve el cursor a la esquina superior izquierda
        printf("PosX: %f  PosY: %f  PosZ: %f   \n", cam.pos.x, cam.pos.y, cam.pos.z);
        printf("Pitch: %f Yaw: %f              \n", cam.pitch, cam.yaw);

        // renderizado
        renderer.renderFrame(cam, mesh, locationMesh);
    }

    // limpieza
    renderer.cleanup();
    return 0;
}