#include "field/Geo.hpp"

#include <sstream>
#include <string>


Rect TGeo::bound()
{
    return Rect();
}

Rect Line::bound()
{
    if (size() == 0)
        throw std::logic_error("Cannot compute bound of empty location");

    double minX = get(0).x;
    double maxX = minX;
    double minY = get(0).y;
    double maxY = minY;

    for (int i = 1; i < size(); ++i) {
        Spot p = get(i);
        double x = p.x;
        double y = p.y;

        minX = std::min(minX, x);
        maxX = std::max(maxX, x);
        minY = std::min(minY, y);
        maxY = std::max(maxY, y);
    }

    Spot a(minX, minY);
    Spot b(maxX, minY);
    Spot c(maxX, maxY);
    Spot d(minX, maxY);

    return Rect(a, b, c, d);
}





// Vérifie si l'objet est de type Spot
bool TGeo::isSpot() {
    return static_cast<Type>(type) == Type::Spot;
}

// Vérifie si l'objet est de type Line
bool TGeo::isLine() {
    return static_cast<Type>(type) == Type::Line;
}

// Vérifie si l'objet est de type Polygon
bool TGeo::isPolygon() {
    return static_cast<Type>(type) == Type::Polygon;
}

// Vérifie si l'objet peut être représenté comme un rectangle
bool TGeo::isRect() {
    if (spots.size() != 5 || spots[0]!=spots[4])
    {
        return false;
    }

    std::vector<Spot> points = { spots[0], spots[1], spots[2], spots[3], spots[4] };

    for (size_t i = 0; i < 4; ++i) {
        const Spot& p1 = points[i];
        const Spot& p2 = points[i + 1];

        bool isAligned = (p1.x == p2.x) || (p1.y == p2.y);
        if (!isAligned) {
            return false;
        }
    }

    return true;
}

bool TGeo::isMultiPolygon() {
    return static_cast<Type>(type) == Type::MultiPolygon;
}


// Convertit vers un Spot (si type compatible)
Spot TGeo::asSpot() {
    if (!isSpot() || spots.empty())
        throw std::logic_error("TGeo: not a Spot");
    return spots[0];
}

// Convertit vers une Line (si type compatible)
Line TGeo::asLine() {
    if (!isLine())
        throw std::logic_error("TGeo: not a Line");

    Line line;
    for (const Spot& s : spots)
        line.add(s);
    return line;
}

// Convertit vers un TPolygon (si type compatible)
TPolygon TGeo::asPolygon() {
    if (!isPolygon())
        throw std::logic_error("TGeo: not a Polygon");

    TPolygon poly;
    for (const Spot& s : spots)
        poly.add(s);
    return poly;
}

// Convertit vers un Rect (si possible)
Rect TGeo::asRect() {
    if (!isRect())
        throw std::logic_error("TGeo: not a Rect");
    return Rect(spots[0], spots[1], spots[2], spots[3]);
}

MultiPolygon TGeo::asMultiPolygon() {
    if (!isMultiPolygon())
        throw std::logic_error("TGeo: not a MultiPolygon");

    MultiPolygon multiPoly;

    for (const TPolygon& p : polygons)
        multiPoly.add(p);

    return multiPoly;
}

TGeo TGeo::clone() const {
    return fromString(toString());
}

TGeo TGeo::buffer(double p) const
{
    return clone().buffer(p);
}

TGeo& TGeo::buffer(double p)
{
    _buffer = p;
    return *this;
}

bool TGeo::hasFunction() const
{
    return _buffer > 0;
}


std::string TGeo::toString() const
{
    std::string str;
    const TGeo& v = *this;

    switch (static_cast<TGeo::Type>(v.type))
    {
    case TGeo::Type::Spot:
    {
        const Spot& spot = static_cast<const Spot&>(v);
        str = "POINT(" + std::to_string(spot.spots[0].x) + " " + std::to_string(spot.spots[0].y) + ")";
        break;
    }

    case TGeo::Type::Line:
    {
        const Line& line = static_cast<const Line&>(v);
        str = "LINESTRING(";
        for (int i = 0; i < line.size(); ++i)
        {
            Spot s = line.get(i);
            str += std::to_string(s.x) + " " + std::to_string(s.y);
            if (i != line.size() - 1) str += ", ";
        }
        str += ")";
        break;
    }

    case TGeo::Type::Polygon:
    {
        const TPolygon& poly = static_cast<const TPolygon&>(v);
        poly.validate();
        str = "POLYGON((";
        for (int i = 0; i < poly.size(); ++i)
        {
            Spot s = poly.get(i);
            str += std::to_string(s.x) + " " + std::to_string(s.y);
            if (i != poly.size() - 1) str += ", ";
        }
        str += "))";
        break;
    }

    case TGeo::Type::MultiPolygon:
    {
        str = "MULTIPOLYGON(";
        for (size_t i = 0; i < polygons.size(); ++i)
        {
            const TPolygon& poly = polygons[i];
            poly.validate();
            str += "((";
            for (int j = 0; j < poly.size(); ++j)
            {
                Spot s = poly.get(j);
                str += std::to_string(s.x) + " " + std::to_string(s.y);
                if (j != poly.size() - 1) str += ", ";
            }
            str += "))";
            if (i != polygons.size() - 1) str += ", ";
        }
        str += ")";
        break;
    }

    default:
        throw std::logic_error("Unsupported geometry type");
    }

    // 🔁 Ajout du buffer si défini
    if (_buffer != 0.0)
    {
        str = "ST_Buffer("+filterFunction+"('" + str + "', "+std::to_string(srid)+"), " + std::to_string(_buffer) + ")";
    }

    return str;
}

TGeo TGeo::fromString(const std::string& str)
{
    auto trim = [](std::string& s) {
        size_t first = s.find_first_not_of(" \t\n\r");
        size_t last = s.find_last_not_of(" \t\n\r");
        if (first == std::string::npos || last == std::string::npos)
            s.clear();
        else
            s = s.substr(first, last - first + 1);
    };

    auto parseSpot = [](const std::string& token) -> Spot {
        std::istringstream iss(token);
        double x, y;
        iss >> x >> y;
        return Spot(x, y);
    };

    if (str.find("POINT(") == 0)
    {
        // POINT(x y)
        size_t start = str.find('(');
        size_t end = str.find(')', start);
        std::string coords = str.substr(start + 1, end - start - 1);
        Spot s = parseSpot(coords);

        TGeo geo(TGeo::Type::Spot);
        geo.spots.push_back(s);
        return geo;
    }
    else if (str.find("LINESTRING(") == 0)
    {
        // LINESTRING(x1 y1, x2 y2, ...)
        size_t start = str.find('(');
        size_t end = str.find(')', start);
        std::string coords = str.substr(start + 1, end - start - 1);

        Line line;
        std::istringstream iss(coords);
        std::string token;
        while (std::getline(iss, token, ','))
        {
            trim(token);
            line.add(parseSpot(token));
        }

        TGeo geo(TGeo::Type::Line);
        geo.spots = line.spots;
        return geo;
    }
    else if (str.find("POLYGON((") == 0)
    {
        // POLYGON((x1 y1, x2 y2, ...))
        size_t start = str.find("((");
        size_t end = str.find("))", start);
        std::string coords = str.substr(start + 2, end - start - 2);

        TPolygon poly;
        std::istringstream iss(coords);
        std::string token;
        while (std::getline(iss, token, ','))
        {
            trim(token);
            poly.add(parseSpot(token));
        }

        TGeo geo(TGeo::Type::Polygon);
        geo.spots = poly.spots;
        return geo;
    }
    else if (str.find("MULTIPOLYGON(") == 0)
    {
        // MULTIPOLYGON(((x1 y1, ...)), ((x2 y2, ...)), ...)
        size_t start = str.find("(");
        size_t end = str.rfind(")");
        std::string all = str.substr(start + 1, end - start - 1);

        std::vector<TPolygon> polys;

        size_t pos = 0;
        while ((pos = all.find("((")) != std::string::npos)
        {
            size_t endPoly = all.find("))", pos);
            if (endPoly == std::string::npos)
                throw std::runtime_error("Malformed MULTIPOLYGON");

            std::string coords = all.substr(pos + 2, endPoly - pos - 2);
            all = all.substr(endPoly + 2);

            TPolygon poly;
            std::istringstream iss(coords);
            std::string token;
            while (std::getline(iss, token, ','))
            {
                trim(token);
                poly.add(parseSpot(token));
            }
            polys.push_back(poly);
        }

        TGeo geo(TGeo::Type::MultiPolygon);
        geo.polygons = polys;
        return geo;
    }

    throw std::invalid_argument("Unsupported or malformed geometry string: " + str);
}


Line Line::sub(const Spot& s1, const Spot& s2) const {
    if (spots.empty()) return Line();

    // 1. Trouver projection de s1 et s2 sur la ligne, ainsi que l'indice de segment correspondant
    auto projectPoint = [&](const Spot& p) -> std::pair<Spot, size_t> {
        double minDist = std::numeric_limits<double>::max();
        Spot closest;
        size_t closestSegment = 0;

        for (size_t i = 0; i + 1 < spots.size(); ++i) {
            Spot a = spots[i];
            Spot b = spots[i + 1];
            Spot proj = p.projection(a, b);
            double dist = proj.distance(p);
            if (dist < minDist) {
                minDist = dist;
                closest = proj;
                closestSegment = i;
            }
        }
        return { closest, closestSegment };
        };

    auto [proj1, seg1] = projectPoint(s1);
    auto [proj2, seg2] = projectPoint(s2);

    // 2. S'assurer que la portion est dans le bon sens (seg1 < seg2), sinon on inverse les points
    bool reversed = false;
    if (seg2 < seg1 || (seg1 == seg2 && proj2.distance(spots[seg1]) < proj1.distance(spots[seg1]))) {
        std::swap(proj1, proj2);
        std::swap(seg1, seg2);
        reversed = true;
    }

    // 3. Construire la sous-ligne
    std::vector<Spot> subPoints;
    subPoints.push_back(proj1);

    // Ajouter les points intermédiaires
    for (size_t i = seg1 + 1; i <= seg2; ++i) {
        subPoints.push_back(spots[i]);
    }

    if (subPoints.size() > 1) {
        // Remplacer le dernier point par la projection exacte s2
        subPoints.back() = proj2;
    }
    else {
        subPoints.push_back(proj2);
    }

    // 4. Si ordre inversé, on inverse la ligne
    if (reversed) {
        std::reverse(subPoints.begin(), subPoints.end());
    }

    return Line(subPoints);
}
