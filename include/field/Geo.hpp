#pragma once

#include <iostream>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <algorithm>

class Spot;
class Rect;
class Line;
class TPolygon;
class MultiPolygon;

struct DrawingStyle {
    int fillColor = 0xFFFFFF;      // Blanc
    int borderColor = 0x000000;    // Noir
    int borderWidth = 1;
    int zIndex = 0;
    bool visible = true;

    bool selected = false;

    bool oneWay = false; //add new

    DrawingStyle() = default;
    DrawingStyle(int fill, int border, int width = 1, int z = 0)
        : fillColor(fill), borderColor(border), borderWidth(width), zIndex(z) {}
};

class TGeo {
public:
    enum class Type {
        Spot = 0,
        Line = 1,
        Polygon = 2,
        MultiPolygon = 3
    };

    int type = -1;
    int bufferDistance = 5;
    int distanceThreshold = 10;

    TGeo() {}

    TGeo(Type t) {
        type = static_cast<int>(t);
    }

    virtual Rect bound();

public:
    std::vector<Spot> spots;
    std::vector<TPolygon> polygons;

public:
    void add(const Spot& s) {
        spots.push_back(s);
    }

public:
    bool isSpot(); 
    bool isLine();
    bool isPolygon(); 
    bool isRect();
    bool isMultiPolygon();
    Spot asSpot();
    Line asLine();
    TPolygon asPolygon();
    Rect asRect();
    MultiPolygon asMultiPolygon();

public:
    std::string toString() const;
    static TGeo fromString(const std::string& str);

    std::string filterFunction = "ST_GeomFromText";
    int srid = 4326;
    double _buffer = 0;

public:
    TGeo clone() const;
    TGeo& buffer(double p);
    TGeo buffer(double p) const;
    bool hasFunction() const;

public:
    DrawingStyle style;
    std::string label;
};

class Spot : public TGeo {
public:
    double x;
    double y;

    Spot() : TGeo(Type::Spot), x(0), y(0) {}

    Spot(double _x, double _y) : TGeo(Type::Spot), x(_x), y(_y) {
        spots.push_back(*this);
    }

    bool operator!=(Spot& spot) const
    {
        return x != spot.x || y != spot.y;
    }
    bool operator==(Spot& spot) const
    {
        return x == spot.x && y == spot.y;
    }

    Spot projection(const Spot& a, const Spot& b) const {
        double dx = b.x - a.x;
        double dy = b.y - a.y;
        double len2 = dx * dx + dy * dy;

        if (len2 == 0.0) return a;

        double t = ((x - a.x) * dx + (y - a.y) * dy) / len2;
        t = std::max(0.0, std::min(1.0, t));

        return Spot(a.x + t * dx, a.y + t * dy);
    }

    double distance(const Spot& other) const {
        double dx = x - other.x;
        double dy = y - other.y;
        return std::sqrt(dx * dx + dy * dy);
    }

};

class Line : public TGeo {
protected:
    void add_(const Spot& s) {
        spots.push_back(s);
    }

public:
    Line() : TGeo(Type::Line) {}

    template<typename... Spots>
    Line(const Spot& first, const Spots&... rest) : Line() {
        add_(first);
        (add_(rest), ...);
    }

    Line(const std::vector<Spot>& pts) : Line() {
        spots.insert(spots.end(), pts.begin(), pts.end());
    }

    template<typename... Spots>
    void add(const Spot& first, const Spots&... rest) {
        add_(first);
        (add_(rest), ...);
    }

    void add(int i, const Spot& s) {
        spots.insert(spots.begin() + i, s);
    }

    void set(int i, const Spot& s) {
        spots[i] = s;
    }

    Spot get(int i) const {
        return spots[i];
    }

    Spot& get(int i) {
        return spots[i];
    }

    Spot& first() {
        return spots[0];
    }

    Spot& last() {
        return spots[size()-1];
    }

    void remove(int i) {
        spots.erase(spots.begin() + i);
    }

    int size() const {
        return static_cast<int>(spots.size());
    }

    bool empty() const {
        return spots.empty();
    }

    void clear() {
        spots.clear();
    }

    std::vector<Spot> all() const {
        return spots;
    }

    Rect bound() override;

    double length() const {
        if (spots.size() < 2) return 0.0;

        double total = 0.0;
        for (size_t i = 1; i < spots.size(); ++i) {
            const Spot& a = spots[i - 1];
            const Spot& b = spots[i];
            double dx = b.x - a.x;
            double dy = b.y - a.y;
            total += std::sqrt(dx * dx + dy * dy);
        }
        return total;
    }

    Spot projection(const Spot& p) const {
        double minDist = std::numeric_limits<double>::max();
        Spot closest;

        for (size_t i = 1; i < size(); ++i) {
            Spot a = get(i - 1);
            Spot b = get(i);
            Spot proj = p.projection(a, b);
            double dist = proj.distance(p);
            if (dist < minDist) {
                minDist = dist;
                closest = proj;
            }
        }
        return closest;
    }

    double distance(const Spot& spot) const {
        Spot p = projection(spot);
        return spot.distance(p);
    }

    Line sub(const Spot& s1, const Spot& s2) const;

};

class TPolygon : public Line {
public:
    TPolygon() {
        type = static_cast<int>(TGeo::Type::Polygon);
    }

    template<typename... Spots>
    TPolygon(const Spot& first, const Spots&... rest) : TPolygon() {
        add_(first);
        (add_(rest), ...);
        close();
    }

    void close() {
        if (size() >= 3) {
            Spot first = get(0);
            Spot last = get(size() - 1);
            if (first.x != last.x || first.y != last.y) {
                add(first);
            }
        }
    }

    bool isClosed() const {
        if (size() < 3) return false;
        const Spot& first = spots.front();
        const Spot& last = spots.back();
        return (first.x == last.x) && (first.y == last.y);
    }

    void validate() const {
        if (!isClosed())
            throw std::logic_error("TPolygon must be closed");
    }
};

class Rect : public TPolygon {
public:
    Rect() : TPolygon() {}

    Rect(const Spot& a, const Spot& b, const Spot& c, const Spot& d) {
        std::vector<Spot> points = { a, b, c, d, a }; // Fermeture incluse

        for (size_t i = 0; i < 4; ++i) {
            const Spot& p1 = points[i];
            const Spot& p2 = points[i + 1];

            bool isAligned = (p1.x == p2.x) || (p1.y == p2.y);
            if (!isAligned) {
                throw std::logic_error("Two consecutive points are not axis-aligned (horizontal or vertical)");
            }
        }

        add(a, b, c, d, a); // Fermeture
        type = static_cast<int>(Type::Polygon);
    }

    double xmin() const {
        double minX = get(0).x;
        for (int i = 1; i < 4; ++i)
            minX = std::min(minX, get(i).x);
        return minX;
    }

    double xmax() const {
        double maxX = get(0).x;
        for (int i = 1; i < 4; ++i)
            maxX = std::max(maxX, get(i).x);
        return maxX;
    }

    double ymin() const {
        double minY = get(0).y;
        for (int i = 1; i < 4; ++i)
            minY = std::min(minY, get(i).y);
        return minY;
    }

    double ymax() const {
        double maxY = get(0).y;
        for (int i = 1; i < 4; ++i)
            maxY = std::max(maxY, get(i).y);
        return maxY;
    }

    Spot gravity() const {
        double cx = (xmin() + xmax()) / 2.0;
        double cy = (ymin() + ymax()) / 2.0;
        return Spot(cx, cy);
    }

    bool cross(TGeo& l) const {
        Rect r = l.bound();
        bool overlapX = !(r.xmax() < xmin() || r.xmin() > xmax());
        bool overlapY = !(r.ymax() < ymin() || r.ymin() > ymax());
        return overlapX && overlapY;
    }

    bool contains(TGeo& l) const {
        Rect r = l.bound();
        return (r.xmin() >= xmin() &&
            r.xmax() <= xmax() &&
            r.ymin() >= ymin() &&
            r.ymax() <= ymax());
    }

    Rect bound() override {
        return *this;
    }

    double area() const {
        const Spot& a = get(0);
        const Spot& c = get(2);
        double width = std::abs(c.x - a.x);
        double height = std::abs(c.y - a.y);
        return width * height;
    }
};

class MultiPolygon : public TGeo {
public:
    MultiPolygon() {
        type = static_cast<int>(Type::MultiPolygon);
    }

    template<typename... Polygons>
    MultiPolygon(const TPolygon& first, const Polygons&... rest) : MultiPolygon() {
        add(first);
        (add(rest), ...);
    }

    void add(const TPolygon& p) {
        polygons.push_back(p);
    }

    TPolygon& get(int i) {
        return polygons[i];
    }

    const TPolygon& get(int i) const {
        return polygons[i];
    }

    int size() const {
        return static_cast<int>(polygons.size());
    }

    void clear() {
        polygons.clear();
    }

    std::vector<TPolygon> all() const {
        return polygons;
    }

    Rect bound() override {
        if (polygons.empty())
            return Rect();

        Rect r = polygons[0].bound();

        for (size_t i = 1; i < polygons.size(); ++i) {
            Rect b = polygons[i].bound();
            double xmin = std::min(r.xmin(), b.xmin());
            double ymin = std::min(r.ymin(), b.ymin());
            double xmax = std::max(r.xmax(), b.xmax());
            double ymax = std::max(r.ymax(), b.ymax());
            r = Rect(Spot(xmin, ymin), Spot(xmin, ymax), Spot(xmax, ymax), Spot(xmax, ymin));
        }

        return r;
    }
};
