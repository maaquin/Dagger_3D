#pragma once
#include <3ds.h>
#include <citro3d.h>  // gpu
#include "camera.h"
#include "mesh.h"

class Renderer {
public:
    void init();
    void renderFrame(const Camera& cam, const Mesh& mesh, const C3D_Mtx& model, ResourceManager& resourceManager);
    void cleanup();

private:
    C3D_RenderTarget* top;
    C3D_RenderTarget* bottom;
    DVLB_s* vshader_dvlb;
    shaderProgram_s program;
    int uLoc_mvp;
    C3D_Mtx projectionTop;
    C3D_Mtx projectionBot;
    u32 clrClear;
};