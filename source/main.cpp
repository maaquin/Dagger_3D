#include <3ds.h>
#include "render.h"
#include "camera.h"

int main(int argc, char **argv)
{
    // inicialización
    Renderer renderer;
    renderer.init();

    Camera cam;
    cameraInit(cam);

    // juego
    while (aptMainLoop())
    {
        // inputs
        hidScanInput();
        u32 kHeld  = hidKeysHeld();
        u32 kDown  = hidKeysDown();

        if (kDown & KEY_START) break;   // start = salir

        // --- UPDATE ---
        // La única "lógica de juego" por ahora: mover la cámara.
        cameraUpdate(cam, kHeld);

        // --- RENDER ---
        // El renderer recibe la cámara y se encarga de TODO lo visual:
        // construir las matrices, limpiar, dibujar, presentar.
        renderer.renderFrame(cam);
    }

    // limpieza
    renderer.cleanup();
    return 0;
}