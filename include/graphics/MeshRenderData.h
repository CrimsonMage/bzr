#ifndef BZR_GRAPHICS_MESHRENDERDATA_H
#define BZR_GRAPHICS_MESHRENDERDATA_H

#include "Destructable.h"
#include "Noncopyable.h"
#include "Resource.h"
#include <vector>

class Model;
class Structure;
struct Vertex;
struct TriangleStrip;

class MeshRenderData : public Destructable, Noncopyable
{
public:
    MeshRenderData(const Model& model);
    MeshRenderData(const Structure& structure);
    ~MeshRenderData();

    void bind();

    GLsizei indexCount() const;

private:
    void initTexture(const vector<ResourcePtr>& textures);
    void initGeometry(const vector<Vertex>& vertices, const vector<TriangleStrip>& triangleStrips);
    
    GLuint _vertexArray;
    GLuint _vertexBuffer;
    GLuint _indexBuffer;
    GLsizei _indexCount;
    GLuint _textures;
    GLuint _textureSizes;
};

#endif