#include "camera.h"
#include <math.h>

void cameraInit(Camera& cam)
{
    cam.pos = FVec4_New(0.0f, 0.0f, 0.0f, 1.0f); // nace en el origen
    cam.yaw   = 0.0f;   // mirando hacia -z
    cam.pitch = 0.0f;   // mirando al horizonte
}

void cameraUpdate(Camera& cam, u32 kHeld)
{
    // --- Rotación (D-pad): el "mouse look" de la 3DS ---
    if (kHeld & KEY_DLEFT)  cam.yaw   -= 0.04f;
    if (kHeld & KEY_DRIGHT) cam.yaw   += 0.04f;
    if (kHeld & KEY_DUP)    cam.pitch += 0.04f;
    if (kHeld & KEY_DDOWN)  cam.pitch -= 0.04f;

    // Limitar pitch para no dar vueltas completas hacia atrás
    if (cam.pitch >  1.4f) cam.pitch =  1.4f;
    if (cam.pitch < -1.4f) cam.pitch = -1.4f;

    // hacia adelante
    float cp = cosf(cam.pitch);
    C3D_FVec fwd = {
        cp * sinf(cam.yaw),    // x
        sinf(cam.pitch),       // y
        -cp * cosf(cam.yaw),   // z (negativo: con yaw=0 miramos hacia -Z)
        1.0f
    };

    // "Derecha" = adelante girado 90° en el plano horizontal (vector perpendicular)
    C3D_FVec right = { cosf(cam.yaw), 0.0f, sinf(cam.yaw), 1.0f };

    // --- Movimiento (circle pad) ---
    // hidCircleRead da -155..155 en cada eje; lo normalizamos a aprox -1..1
    circlePosition stick;
    hidCircleRead(&stick);
    float fwdAmt  = -stick.dy / 155.0f;  // stick arriba = avanzar
    float rightAmt = stick.dx / 155.0f;  // stick derecha = estraviar

    const float speed = 0.08f;
    cam.pos.x += (fwd.x * fwdAmt + right.x * rightAmt) * speed;
    cam.pos.y += (fwd.y * fwdAmt + right.y * rightAmt) * speed;
    cam.pos.z += (fwd.z * fwdAmt + right.z * rightAmt) * speed;

    // Subir/bajar con los gatillos (volar, por ahora — no hay gravedad)
    if (kHeld & KEY_R) cam.pos.y += speed;
    if (kHeld & KEY_L) cam.pos.y -= speed;
}

void cameraViewMatrix(const Camera& cam, C3D_Mtx* out)
{
    // Punto al que miramos = posición + un paso en la dirección "adelante"
    float cp = cosf(cam.pitch);
    C3D_FVec fwd = FVec4_New(cp * sinf(cam.yaw), sinf(cam.pitch), -cp * cosf(cam.yaw), 1.0f);
    C3D_FVec target = FVec4_New(cam.pos.x + fwd.x, cam.pos.y + fwd.y, cam.pos.z + fwd.z, 1.0f);

    C3D_FVec up = FVec4_New(0.0f, 1.0f, 0.0f, 1.0f); // "arriba" del mundo siempre es +Y

    Mtx_LookAt(out, cam.pos, target, up, true);
}