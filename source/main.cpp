#include <cstdio>
#include <3ds.h>
#include "render.h"
#include "camera.h"
#include "mesh.h"
#include "bsaReader.h"
#include "arch3dParser.h"

int main(int argc, char **argv)
{
    // inicialización
    Renderer renderer;
    renderer.init();

    Camera cam;
    cameraInit(cam);

    // cuarto test
    Mesh mesh;
    C3D_Mtx locationMesh;
    Mtx_Identity(&locationMesh);

    consoleInit(GFX_BOTTOM, NULL);

    // test
    BsaReader bsa;
    if (bsa.open("sdmc:/3ds/dagger/arena2/ARCH3D.BSA"))
    {
        printf("BSA abierto con exito!\n");
        printf("Total de modelos: %d\n", bsa.getRecordCount());

        // Extraer el registro 0 de prueba
        std::vector<uint8_t> buffer;
        if (bsa.extractRecord(0, buffer))
        {
            printf("Registro 0 leido: %u bytes\n", (unsigned int)buffer.size());
            std::vector<DaggerPoint> points;
            std::vector<DaggerPlane> planes;

            if (Arch3dParser::parse(buffer, points, planes))
            {
                // Pasamos los datos leídos a la GPU
                mesh.initFromDaggerData(points, planes, 256.0f);
                printf("exito");
            }
        }
    }
    else
    {
        printf("No se pudo abrir ARCH3D.BSA\n");
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

        // renderizado
        renderer.renderFrame(cam, mesh, locationMesh);
    }

    // limpieza
    renderer.cleanup();
    return 0;
}