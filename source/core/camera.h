#pragma once
#include <3ds.h>
#include <citro3d.h>

struct Camera
{
    C3D_FVec pos;     // posición
    float pitch, yaw; // hacia dónde mira
};

void cameraInit(Camera& cam);
void cameraUpdate(Camera& cam, u32 kHeld);   // mover la cámara
void cameraViewMatrix(const Camera& cam, C3D_Mtx* out); // construye la matriz de vista