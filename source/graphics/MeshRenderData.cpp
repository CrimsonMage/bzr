/*
 * Bael'Zharon's Respite
 * Copyright (C) 2014 Daniel Skorupski
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#include "graphics/MeshRenderData.h"
#include "graphics/Renderer.h"
#include "graphics/TextureRenderData.h"
#include "resource/Environment.h"
#include "resource/ImgColor.h"
#include "resource/ImgTex.h"
#include "resource/Model.h"
#include "resource/Surface.h"
#include "Config.h"
#include "Core.h"
#include "ResourceCache.h"
#include "Structure.h"
#include <algorithm>

// FIXME Not the neatest thing in the world
// Ought to find an existing 0x08 file with a nice transparent surface
static weak_ptr<const Resource> g_hitSurface;

struct SortByTexSurface
{
    bool operator()(const TriangleFan* a, const TriangleFan* b)
    {
        return a->surfaceIndex < b->surfaceIndex;
    }
};

MeshRenderData::MeshRenderData(const Model& model)
{
    init(model.surfaces,
        model.vertices,
        model.triangleFans,
        model.hitTriangleFans);
}

MeshRenderData::MeshRenderData(const Structure& structure)
{
    assert(structure.partNum() < structure.environment().parts.size());
    const CellStruct& part = structure.environment().parts[structure.partNum()];

    init(structure.surfaces(),
        part.vertices,
        part.triangleFans,
        part.hitTriangleFans);
}

MeshRenderData::~MeshRenderData()
{
    glDeleteVertexArrays(1, &vertexArray_);
    glDeleteBuffers(1, &vertexBuffer_);
    glDeleteBuffers(1, &indexBuffer_);
}

void MeshRenderData::render()
{
    glBindVertexArray(vertexArray_);

    int indexBase = 0;

    for(Batch& batch : batches_)
    {
        const ImgColor& imgColor = batch
            .surface->cast<Surface>()
            .imgTex->cast<ImgTex>()
            .imgColor->cast<ImgColor>();

        if(!imgColor.renderData)
        {
            imgColor.renderData.reset(new TextureRenderData{imgColor});
        }

        TextureRenderData& renderData = static_cast<TextureRenderData&>(*imgColor.renderData);

        glActiveTexture(GL_TEXTURE0);
        renderData.bind();

        glDrawElements(GL_TRIANGLE_FAN, batch.indexCount, GL_UNSIGNED_SHORT, reinterpret_cast<GLvoid*>(indexBase * sizeof(uint16_t)));

        indexBase += batch.indexCount;
    }
}

void MeshRenderData::init(
    const vector<ResourcePtr>& surfaces,
    const vector<Vertex>& vertices,
    const vector<TriangleFan>& triangleFans,
    const vector<TriangleFan>& hitTriangleFans)
{
    // vx, vy, vz, nx, ny, nz, s, t
    static const int kComponentsPerVertex = 8;

    // Sort triangle fans by texture
    vector<const TriangleFan*> sortedTriangleFans;

    for(const TriangleFan& triangleFan : triangleFans)
    {
        sortedTriangleFans.push_back(&triangleFan);
    }

    sort(sortedTriangleFans.begin(), sortedTriangleFans.end(), SortByTexSurface());

    // Build batches
    vector<float> vertexData;
    vector<uint16_t> indexData;

    for(const TriangleFan* triangleFan : sortedTriangleFans)
    {
        // Skip portal/lighting polygons
        if(triangleFan->stipplingType == 0x04)
        {
            continue;
        }

        if(batches_.empty() || surfaces[triangleFan->surfaceIndex].get() != batches_.back().surface.get())
        {
            // Start a new batch
            batches_.push_back({surfaces[triangleFan->surfaceIndex], 0});
        }
        else if(batches_.back().indexCount != 0)
        {
            // Starting a new triangle fan in existing batch
            indexData.push_back(0xFFFF);
            batches_.back().indexCount++;
        }

        for(const TriangleFan::Index& index : triangleFan->indices)
        {
            indexData.push_back(static_cast<uint16_t>(vertexData.size() / kComponentsPerVertex));
            batches_.back().indexCount++;

            const Vertex& vertex = vertices[index.vertexIndex];

            vertexData.push_back(static_cast<float>(vertex.position.x));
            vertexData.push_back(static_cast<float>(vertex.position.y));
            vertexData.push_back(static_cast<float>(vertex.position.z));

            vertexData.push_back(static_cast<float>(vertex.normal.x));
            vertexData.push_back(static_cast<float>(vertex.normal.y));
            vertexData.push_back(static_cast<float>(vertex.normal.z));

            if(vertex.texCoords.empty())
            {
                vertexData.push_back(0.0f);
                vertexData.push_back(0.0f);
            }
            else
            {
                vertexData.push_back(static_cast<float>(vertex.texCoords[index.texCoordIndex].x));
                vertexData.push_back(static_cast<float>(vertex.texCoords[index.texCoordIndex].y));
            }
        }
    }

    if(Core::get().renderer().renderHitGeometry())
    {
        ResourcePtr surface = g_hitSurface.lock();

        if(!surface)
        {
            ResourcePtr imgColor{new ImgColor{0x800000FF}};
            ResourcePtr imgTex{new ImgTex{imgColor}};
            surface.reset(new Surface{imgTex});
            g_hitSurface = surface;
        }

        batches_.push_back({surface, 0});

        for(const TriangleFan& triangleFan : hitTriangleFans)
        {
            if(batches_.back().indexCount != 0)
            {
                indexData.push_back(0xFFFF);
                batches_.back().indexCount++;
            }

            for(const TriangleFan::Index& index : triangleFan.indices)
            {
                indexData.push_back(static_cast<uint16_t>(vertexData.size() / kComponentsPerVertex));
                batches_.back().indexCount++;

                const Vertex& vertex = vertices[index.vertexIndex];

                vertexData.push_back(static_cast<float>(vertex.position.x));
                vertexData.push_back(static_cast<float>(vertex.position.y));
                vertexData.push_back(static_cast<float>(vertex.position.z));

                vertexData.push_back(static_cast<float>(vertex.normal.x));
                vertexData.push_back(static_cast<float>(vertex.normal.y));
                vertexData.push_back(static_cast<float>(vertex.normal.z));

                vertexData.push_back(0.0f);
                vertexData.push_back(0.0f);
            }
        }
    }

    glGenVertexArrays(1, &vertexArray_);
    glBindVertexArray(vertexArray_);

    glGenBuffers(1, &vertexBuffer_);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &indexBuffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexData.size() * sizeof(uint16_t), indexData.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * kComponentsPerVertex, nullptr);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * kComponentsPerVertex, reinterpret_cast<GLvoid*>(sizeof(float) * 3));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * kComponentsPerVertex, reinterpret_cast<GLvoid*>(sizeof(float) * 6));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
}
