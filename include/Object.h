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
#ifndef BZR_OBJECT_H
#define BZR_OBJECT_H

#include "Noncopyable.h"
#include "Location.h"
#include "ObjectId.h"
#include "Resource.h"

class Object : Noncopyable
{
public:
    explicit Object(ObjectId id);

    void setModel(ResourcePtr model);
    void setLocation(const Location& location);

    ObjectId id() const;
    const ResourcePtr& model() const;
    const Location& location() const;

private:
    ObjectId _id;
    ResourcePtr _model;
    Location _location;
};

#endif
