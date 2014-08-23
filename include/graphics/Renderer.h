#ifndef BZR_GRAPHICS_RENDERER_H
#define BZR_GRAPHICS_RENDERER_H

#include "Noncopyable.h"
#ifdef OCULUSVR
#include <OVR_CAPI.h>
#include <OVR_CAPI_GL.h>
#endif

class LandblockRenderer;
class ModelRenderer;
class SkyRenderer;
class StructureRenderer;
class TextureAtlas;

class Renderer : Noncopyable
{
public:
    Renderer();
    ~Renderer();

    void init();
    void render(double interp);

    TextureAtlas& textureAtlas();
    GLenum textureMinFilter() const;
    GLfloat textureMaxAnisotropy() const;

private:
    void createWindow();
#ifdef OCULUSVR
    void initOVR();
    void cleanupOVR();
    void renderOVR(double interp);
#endif

    double _fieldOfView;
    GLenum _textureMinFilter;
    GLfloat _textureMaxAnisotropy;

    bool _videoInit;
    SDL_Window* _window;
    SDL_GLContext _context;

#ifdef OCULUSVR
    ovrHmd _hmd;
    ovrSizei _renderTexSize;
    ovrRecti _eyeViewport[ovrEye_Count];
    ovrGLTexture _eyeTexture[ovrEye_Count];
    ovrEyeRenderDesc _eyeRenderDesc[ovrEye_Count];
    GLuint _renderTex;
    GLuint _depthTex;
    GLuint _framebuffer;
#endif

    unique_ptr<TextureAtlas> _textureAtlas;
    unique_ptr<SkyRenderer> _skyRenderer;
    unique_ptr<LandblockRenderer> _landblockRenderer;
    unique_ptr<StructureRenderer> _structureRenderer;
    unique_ptr<ModelRenderer> _modelRenderer;
};

#endif
