#include "MapRange.hpp"

MapRange::MapRange(int beginX, int beginY, int endX, int endY)
    : _beginX(beginX)
    , _beginY(beginY)
    , _endX(endX)
    , _endY(endY)
{}

MapRange::MapRange(const PlayerView& playerView, const Vec2Int point, const size_t distance)
    : _beginX(std::max(static_cast<int>(point.x - distance), 0))
    , _beginY(std::max(static_cast<int>(point.y - distance), 0))
    , _endX(std::min(static_cast<int>(point.x + distance + 1), playerView.mapSize))
    , _endY(std::min(static_cast<int>(point.y + distance + 1), playerView.mapSize))
{}

bool MapRange::contains(const int x, const int y) const
{
    return x >= _beginX && x < _endX && y >= _beginY && y < _endY;
}

bool MapRange::contains(const Vec2Int& point) const
{
    return contains(point.x, point.y);
}

bool MapRange::contains(const MapRange& other) const
{
    return contains(other._beginX, other._beginY) && contains(other._endX, other._endY);
}

void MapRange::forEachCell(const std::function<void(int, int)> function) const
{
    for (auto x = _beginX; x < _endX; ++x)
    {
        for (auto y = _beginY; y < _endY; ++y)
        {
            function(x, y);
        }
    }
}

bool MapRange::findIf(const std::function<bool(int, int)> function) const
{
    for (auto x = _beginX; x < _endX; ++x)
    {
        for (auto y = _beginY; y < _endY; ++y)
        {
            if (function(x, y))
            {
                return true;
            }
        }
    }
    
    return false;
}
