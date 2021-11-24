#include "MapRange.hpp"

MapRange::MapRange(int beginX, int beginY, int lastX, int lastY)
    : _beginX(beginX)
    , _beginY(beginY)
    , _lastX(lastX)
    , _lastY(lastY)
{}

MapRange::MapRange(const PlayerView& playerView, const Vec2Int point, const size_t distance)
    : _beginX(std::max(static_cast<int>(point.x - distance), 0))
    , _beginY(std::max(static_cast<int>(point.y - distance), 0))
    , _lastX(std::min(static_cast<int>(point.x + distance), playerView.mapSize))
    , _lastY(std::min(static_cast<int>(point.y + distance), playerView.mapSize))
{}

bool MapRange::contains(const int x, const int y) const
{
    return x >= _beginX && x <= _lastX && y >= _beginY && y <= _lastY;
}

bool MapRange::contains(const Vec2Int& point) const
{
    return contains(point.x, point.y);
}

bool MapRange::contains(const MapRange& other) const
{
    return contains(other.getBeginX(), other.getBeginY()) && contains(other.getLastX(), other.getLastY());
}

bool MapRange::intersects(const MapRange& other) const
{
    return !(getBeginX() > other.getLastX() || getLastX() < other.getBeginX() ||
             getBeginY() > other.getLastY() || getLastY() < other.getBeginY());
}

void MapRange::forEachCell(const std::function<void(int, int)> function) const
{
    for (auto x = _beginX; x <= _lastX; ++x)
    {
        for (auto y = _beginY; y < _lastY; ++y)
        {
            function(x, y);
        }
    }
}

bool MapRange::findIf(const std::function<bool(int, int)> function) const
{
    for (auto x = _beginX; x <= _lastX; ++x)
    {
        for (auto y = _beginY; y <= _lastY; ++y)
        {
            if (function(x, y))
            {
                return true;
            }
        }
    }
    
    return false;
}

int MapRange::getBeginX() const
{
    return _beginX;
}

int MapRange::getBeginY() const
{
    return _beginY;
}

int MapRange::getLastX() const
{
    return _lastX;
}

int MapRange::getLastY() const
{
    return _lastY;
}

