#ifndef BZR_LANDBLOCK_H
#define BZR_LANDBLOCK_H

#include "math/Vec2.h"
#include "Noncopyable.h"

class Landblock : Noncopyable
{
public:
    static const int GRID_SIZE = 9;

    Landblock(const void* data, size_t length);

    // get the height of the original mesh
    double getOriginalHeight(Vec2 point) const;
    
    // get the height of the subdivided mesh
    double getSubdividedHeight(Vec2 point) const;

private:
    PACK(struct RawData
    {
        uint32_t fileid;
        uint32_t flags;
        uint16_t styles[GRID_SIZE][GRID_SIZE];
        uint8_t heights[GRID_SIZE][GRID_SIZE];
        uint8_t pad;
    });

    void subdivide(int n);
    void subdivideOnce();

    RawData _data;
    unique_ptr<double[]> _original;
    unique_ptr<double[]> _subdivided;
    int _nsubdivisions;
};

#endif
