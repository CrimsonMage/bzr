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
#include "TextureLookup8.h"
#include "BinReader.h"
#include "Core.h"
#include "ResourceCache.h"
#include "Texture.h"
#include "TextureLookup5.h"

TextureLookup8::TextureLookup8(uint32_t id, const void* data, size_t size) : ResourceImpl{id}
{
    BinReader reader(data, size);

    uint8_t flags = reader.readByte();
    assert(flags == 0x01 || flags == 0x02 || flags == 0x04 || flags == 0x11 || flags == 0x12 || flags == 0x14);

    reader.readByte();
    reader.readByte();
    reader.readByte();

    if(flags & 0x01)
    {
        uint32_t bgra = reader.readInt();
        ResourcePtr texture(new Texture(bgra));
        textureLookup5.reset(new TextureLookup5(texture));
    }
    else
    {
        uint32_t textureId = reader.readInt();
        textureLookup5 = Core::get().resourceCache().get(textureId);
        assert(textureLookup5->resourceType() == ResourceType::kTextureLookup5);

        uint32_t zero = reader.readInt();
        assert(zero == 0);
    }

    // I suspect these may be texture coordinates within the texture
    float f1 = reader.readFloat();

    if(flags & 0x10)
    {
        assert(f1 != 0.0);
    }
    else
    {
        assert(f1 == 0.0);
    }

    reader.readFloat();
    reader.readFloat();

    reader.assertEnd();
}

TextureLookup8::TextureLookup8(ResourcePtr textureLookup5) : ResourceImpl(ResourceType::kTextureLookup8 | 0xFFFF), textureLookup5(textureLookup5)
{
    assert(textureLookup5->resourceType() == ResourceType::kTextureLookup5);
}
