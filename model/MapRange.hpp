#ifndef _MAP_RANGE_HPP_
#define _MAP_RANGE_HPP_

#include "PlayerView.hpp"
#include "Vec2Int.hpp"

#include <cstdio>
#include <functional>

class MapRange
{
public:
    MapRange(int beginX, int beginY, int endX, int endY);
    MapRange(const PlayerView& playerView, const Vec2Int point, const size_t distance);
    
    bool contains(const int x, const int y) const;
    bool contains(const Vec2Int& point) const;
    bool contains(const MapRange& other) const;
    
    void forEachCell(const std::function<void(int, int)> function) const;
    bool findIf(const std::function<bool(int, int)> function) const;
    
private:
    int _beginX;
    int _beginY;
    int _endX;
    int _endY;
};

#endif /* _MAP_RANGE_HPP_ */
