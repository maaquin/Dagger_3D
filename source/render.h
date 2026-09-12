#pragma once
#include <3ds.h>
#include <citro3d.h>  // gpu
#include "camera.h"

// más adelante uv para texturas, normales para iluminación.
struct Vertex
{
    float x, y, z;
    float r, g, b, a;
};

class Renderer {
public:
    void init();
    void renderFrame(const Camera& cam);
    void cleanup();

private:
    C3D_RenderTarget* top;
    C3D_RenderTarget* bottom;
    DVLB_s* vshader_dvlb;
    shaderProgram_s program;
    int uLoc_mvp;
    Vertex* vbo_data;
    C3D_Mtx projectionTop;
    C3D_Mtx projectionBot;
    u32 clrClear;
};