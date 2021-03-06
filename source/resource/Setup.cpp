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
#include "resource/Setup.h"
#include "physics/CylSphere.h"
#include "physics/Sphere.h"
#include "BinReader.h"
#include "Core.h"
#include "ResourceCache.h"
#include "util.h"

enum SetupFlag
{
    kHasParentIndex = 0x1,
    kHasDefaultScale = 0x2,
    kAllowFreeHeading = 0x4,
    kHasPhysicsBSP = 0x8
};

Setup::Setup(uint32_t id, const void* data, size_t size) : ResourceImpl{id}
{
    BinReader reader(data, size);

    uint32_t resourceId = reader.readInt();
    assert(resourceId == id);
    UNUSED(resourceId);

    uint32_t flags = reader.readInt();
    assert(flags <= 0xF);

    uint32_t numModels = reader.readInt();

    models.reserve(numModels);

    for(uint32_t i = 0; i < numModels; i++)
    {
        uint32_t modelId = reader.readInt();
        models.emplace_back(Core::get().resourceCache().get(modelId));
    }

    parents.reserve(numModels);

    for(uint32_t i = 0; i < numModels; i++)
    {
        uint32_t parent = 0xFFFFFFFF;

        if(flags & kHasParentIndex)
        {
            parent = reader.readInt();
        }

        parents.push_back(parent);
    }

    scales.reserve(numModels);

    for(uint32_t i = 0; i < numModels; i++)
    {
        glm::vec3 scale{1.0, 1.0, 1.0};

        if(flags & kHasDefaultScale)
        {
            read(reader, scale);
        }

        scales.push_back(scale);
    }

    uint32_t numHoldingLocations = reader.readInt();

    for(uint32_t i = 0; i < numHoldingLocations; i++)
    {
        /*key*/ reader.readInt();
        /*partIndex*/ reader.readInt();

        Location holdingLocation;
        read(reader, holdingLocation);
    }

    uint32_t numConnectionPoints = reader.readInt();
    assert(numConnectionPoints == 0);

    for(uint32_t i = 0; i < numConnectionPoints; i++)
    {
        /*key*/ reader.readInt();
        /*partIndex*/ reader.readInt();

        Location connectionPoint;
        read(reader, connectionPoint);
    }

    uint32_t numPlacementFrames = reader.readInt();
    placementFrames.resize(numPlacementFrames);

    for(AnimationFrame& frame : placementFrames)
    {
        /*key*/ reader.readInt();

        read(reader, frame, numModels);
    }

    uint32_t numCylSpheres = reader.readInt();

    for(uint32_t i = 0; i < numCylSpheres; i++)
    {
        CylSphere cylSphere;
        read(reader, cylSphere);
    }

    uint32_t numSpheres = reader.readInt();

    for(uint32_t i = 0; i < numSpheres; i++)
    {
        Sphere sphere;
        read(reader, sphere);
    }

    /*height*/ reader.readFloat();
    /*radius*/ reader.readFloat();
    /*stepUpHeight*/ reader.readFloat();
    /*stepDownHeight*/ reader.readFloat();

    Sphere sortingSphere;
    read(reader, sortingSphere);

    Sphere selectionSphere;
    read(reader, selectionSphere);

    uint32_t numLights = reader.readInt();

    for(uint32_t i = 0; i < numLights; i++)
    {
        uint32_t lightIndex = reader.readInt();
        assert(lightIndex == i);
        UNUSED(lightIndex);

        Location lightLocation;
        read(reader, lightLocation);

        /*color*/ reader.readInt();
        /*intensity*/ reader.readFloat();
        /*falloff*/ reader.readFloat();
        /*coneAngle*/ reader.readFloat(); // junk 0xcdcdcdcd most of the time
    }

    ResourceCache& cache = Core::get().resourceCache();

    uint32_t defaultAnimId = reader.readInt();

    if(defaultAnimId)
    {
        defaultAnimation = cache.get(defaultAnimId);
    }

    uint32_t defaultPhysScriptId = reader.readInt();

    if(defaultPhysScriptId)
    {
        defaultPhysScript = cache.get(defaultPhysScriptId);
    }

    uint32_t defaultMotionTableId = reader.readInt();

    if(defaultMotionTableId)
    {
        defaultMotionTable = cache.get(defaultMotionTableId);
    }

    uint32_t defaultSoundTableId = reader.readInt();

    if(defaultSoundTableId)
    {
        defaultSoundTable = cache.get(defaultSoundTableId);
    }

    uint32_t defaultPhysScriptTableId = reader.readInt();

    if(defaultPhysScriptTableId)
    {
        defaultPhysScriptTable = cache.get(defaultPhysScriptTableId);
    }

    assert(reader.remaining() == 0);
}
