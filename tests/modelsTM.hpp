#pragma once

#include <sqlite3.h>

#include "model/TModel.hpp"



#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>
#include <limits>
#include <optional>


class LandUnitType : public TModel<LandUnitType> {
    TM_SCHEMA(LandUnitType, "models.geo", TF(ref), TF(name))

    TextField ref;
    TextField name;

};

class LandUnit : public TModel<LandUnit> {
    TM_SCHEMA(LandUnit, "models.geo", TF(name), TF(level), TF(geometry), TF(iso), TF(type), TF(parent), TF(centroid))

    TextField name;
    IntField level;
    ModelField<LandUnit> parent;
    GeoField geometry;
    TextField iso;
    ModelField<LandUnitType> type;

    TextField iso2;

    GeoField centroid;
};

//-----------------------------------------------------------------------------------------------------------

class GeoType : public TModel<GeoType> {
    TM_SCHEMA(GeoType, "models.geo", TF(ref), TF(name))

    TextField ref;    // "point:4326" (clé métier unique)
    TextField name;   // "Point"
    TextField type;   // "POINT", "POLYGON", "LINESTRING"...
    IntField srid;    // 4326

};

class GeoObjectType : public TModel<GeoObjectType> {
    TM_SCHEMA(GeoObjectType, "models.geo", TF(ref), TF(name))

    TextField ref; // pharmacies:302
    TextField name; // Pharmacy
    ModelField<GeoType> geometry_type;

};

// Base Geo data
class GeoObject : public TModel<GeoObject> {
    TM_SCHEMA(GeoObject, "models.geo", TF(name), TF(type), TF(parent_land), TF(geometry), TF(orientation))

    TextField name;
    ModelField<GeoObjectType> type;
    ModelField<LandUnit> parent_land;
    GeoField geometry;

    FloatField orientation;
};

//-----------------------------------------------------------------------------------------------------------
// donnée métié
class Pharmacy : public TModel<Pharmacy> {
    TM_SCHEMA(Pharmacy, "models.geo", TF(opening_hours), TF(has_emergency), TF(geoobject))

    ModelField<GeoObject> geoobject;

    TextField opening_hours;
    BoolField has_emergency;
};

class River : public TModel<River> {
    TM_SCHEMA(River, "models.geo", TF(length), TF(depth), TF(geoobject))

    ModelField<GeoObject> geoobject;

    TextField length;
    TextField depth;
};

class Mall : public TModel<Mall> {
    TM_SCHEMA(Mall, "models.geo", TF(opening_hours), TF(forKid), TF(geoobject))

    ModelField<GeoObject> geoobject;

    TextField opening_hours;
    BoolField forKid;
};

//-----------------------------------------------------------------------------------------------------------

class RoadNode : public TModel<RoadNode> {
    TM_SCHEMA(RoadNode, "models.geo", TF(geometry), TF(name))

    GeoField geometry;
    TextField name;
};

class Road : public TModel<Road> {
    TM_SCHEMA(Road, "models.geo", TF(type), TF(azimuth), TF(geoobject), TF(start_node), TF(end_node), TF(cost), TF(one_way), TF(direction_constraint))

    ModelField<GeoObject> geoobject;

    TextField type;
    FloatField azimuth;

    ModelField<RoadNode> start_node;
    ModelField<RoadNode> end_node;

    FloatField cost;

    BoolField one_way;
    IntField direction_constraint;
};

class RoadLink : public TModel<RoadLink> {
    TM_SCHEMA(RoadLink, "models.geo", TF(from_road), TF(to_road), TF(cost))

    ModelField<Road> from_road;
    ModelField<Road> to_road;

    FloatField cost;
};

//-----------------------------------------------------------------------------------------------------------

class RoadSign : public TModel<RoadSign> {
    TM_SCHEMA(RoadSign, "models.geo", TF(category), TF(direction), TF(is_lit), TF(geoobject))

    ModelField<GeoObject> geoobject;

    TextField category;
    TextField direction;
    BoolField is_lit;
};
//-----------------------------------------------------------------------------------------------------------


inline double bufferMetersToDegrees(double meters);

double radiusMetersToDegrees(double meters);


std::vector<Road> fetchNearbyRoads(sqlite3* db, const Spot& center, double radiusMeters);

std::vector<Road> fetchRoadsAlongPath(sqlite3* db, const std::string& lineWKT, double bufferMeters);

using NodeId = int; // ou autre type
using RoadId = int; // ou autre type

struct PathStep {
    NodeId node_id;
    float total_cost;
    std::vector<RoadId> path;
};

struct CompareCost {
    bool operator()(const PathStep& a, const PathStep& b) const {
        return a.total_cost > b.total_cost; // pour que le coût le plus bas soit prioritaire
    }
};

extern std::vector<Road> tmpRoads;
std::vector<Road> shortestPath(RoadNode from, RoadNode to);
std::vector<Road> shortestPath(RoadNode from, RoadNode to, std::vector<Road> allRoads);
std::vector<Road> shortestPath(RoadNode from, RoadNode to, const Line geo, double bMin, double bMax, double step);

//-----------------------------------------------------------------------------------------------------------


void testInitTM(std::function<void(void)> listener=nullptr);

//-----------------------------------------------------------------------------------------------------------

void linkerVilleTest();

void linker();

//-----------------------------------------------------------------------------------------------------------

std::vector<TGeo> doualaCity();

std::vector<TGeo> doualaCityFromModel();

void startDataEngine();

//-----------------------------------------------------------------------------------------------------------




//-----------------------------------------------------------------------------------------------------------
// itineraire

class Direction1 {

public:
    static Road nearestRoad(const Spot& p, double bMin = 0.0003, double bMax = 0.01, double step = 0.0005) {

        double b = bMin;
        while (b <= bMax) {
            Road r0;
            GeoObject g0;


            auto result = Road::tms.with<GeoObject>(r0, g0)
                .lazy()
                .filter(r0.geoobject == g0)
                .filter(g0.geometry.index(p.buffer(b)) && 
                    g0.geometry.intersects(p.buffer(b)))
                .order(+g0.geometry.distance(p))
                .build()
                .select<Road>();


            if (!result.empty()) return result.first();

            b += step;
        }


        throw std::runtime_error("Aucune route proche trouvée dans la plage de buffer");
    }


    static std::vector<TGeo> direction(const Spot& start, const Spot& end) {
        std::vector<TGeo> data;

        try
        {
            // à exploiter pour obtenir la route reel : je propose de creer un nouveau line, qu'on ajoute dans geometries
            Road r1 = nearestRoad(start);
            Road r2 = nearestRoad(end);

            Line line1 = r1.geoobject.get().geometry.geo().asLine();
            Line line2 = r2.geoobject.get().geometry.geo().asLine();

            Spot p1 = line1.projection(start);
            Spot p2 = line2.projection(end);


            RoadNode ns, ne;
            ns.name = " "; ns.geometry = p1;
            ne.name = " ";   ne.geometry = p2;
            RoadNode::tms.simulate(&ns, &ne);

            GeoObject gs1, gs2, ge1, ge2;
            std::vector<GeoObject> allG;
            gs1.geometry = line1.sub(line1.first(), p1);
            gs2.geometry = line1.sub(p1, line1.last());
            ge1.geometry = line2.sub(line2.first(), p2);
            ge2.geometry = line2.sub(p2, line2.last());
            GeoObject::tms.simulate(&gs1, &gs2, &ge1, &ge2);

            auto createTempRoad = [&](const std::string& name, RoadNode& from, RoadNode& to, GeoObject& go, bool one_way) -> Road {
                Road temp;
                temp.geoobject.cache();
                temp.start_node.cache();
                temp.end_node.cache();
                temp.type = name;
                temp.geoobject = go;
                temp.start_node = from;
                temp.end_node = to;
                temp.cost = go.geometry.geo().asLine().length();
                temp.one_way = one_way;
                return temp;
            };

            Road nr1 = createTempRoad(" ", r1.start_node.get(), ns, gs1, r1.one_way);
            Road nr2 = createTempRoad(" ", ns, r1.end_node.get(), gs2, r1.one_way);
            Road nr3 = createTempRoad(" ", r2.start_node.get(), ne, ge1, r2.one_way);
            Road nr4 = createTempRoad(" ", ne, r2.end_node.get(), ge2, r2.one_way);
            Road::tms.simulate(&nr1, &nr2, &nr3, &nr4);

          
            tmpRoads.clear();
            tmpRoads.push_back(nr1);
            tmpRoads.push_back(nr2);
            tmpRoads.push_back(nr3);
            tmpRoads.push_back(nr4);

            auto filter = shortestPath(ns, ne);

            if (!filter.empty()) {
                for (Road& r : filter) {
                    data.push_back(parse(r));
                }
                //
                TGeo geo = ns.geometry.geo();
                geo.label = ns.name;
                geo.style.borderColor = 0x22aa22;
                geo.style.borderWidth = 2;
                geo.style.selected = true;
                data.push_back(geo);
                //
                geo = ne.geometry.geo();
                geo.label = ne.name;
                geo.style.borderColor = 0x22aa22;
                geo.style.borderWidth = 5;
                geo.style.selected = true;
                data.push_back(geo);
            }
        }
        catch (...) {};

        return data;
    }

    static TGeo parse(Road& r) {
        TGeo geo = r.geoobject.get().geometry.geo();
        geo.label = r.type;
        geo.style.borderColor = 0x333333;
        geo.style.borderWidth = 2;
        geo.style.selected = true;
        return geo;
    }
};

//-----------------------------------------------------------------------------------------------------------

class Direction2 {

public:
    static Road nearestRoad(const Spot& p, double bMin = 0.0003, double bMax = 0.01, double step = 0.0005) {

        double b = bMin;
        while (b <= bMax) {
            Road r0;
            GeoObject g0;

            auto result = Road::tms.with(r0, g0)
                .lazy()
                .filter(r0.geoobject == g0)
                .filter(g0.geometry.index(p.buffer(b)))
                .order(+g0.geometry.distance(p))
                .build()
                .select<Road>();

            if (!result.empty()) return result.first();

            b += step;
        }

        throw std::runtime_error("Aucune route proche trouvée dans la plage de buffer");
    }


    static std::vector<TGeo> direction(const Spot& start, const Spot& end) {
        std::vector<TGeo> data;

        try
        {
            // à exploiter pour obtenir la route reel : je propose de creer un nouveau line, qu'on ajoute dans geometries
            Road r1 = nearestRoad(start);
            Road r2 = nearestRoad(end);

            Line line1 = r1.geoobject.get().geometry.geo().asLine();
            Line line2 = r2.geoobject.get().geometry.geo().asLine();

            Spot p1 = line1.projection(start);
            Spot p2 = line2.projection(end);


            RoadNode ns, ne;
            ns.name = " "; ns.geometry = p1;
            ne.name = " ";   ne.geometry = p2;
            RoadNode::tms.create(&ns, &ne);

            GeoObject gs1, gs2, ge1, ge2;
            std::vector<GeoObject> allG;
            gs1.geometry = line1.sub(line1.first(), p1);
            gs2.geometry = line1.sub(p1, line1.last());
            ge1.geometry = line2.sub(line2.first(), p2);
            ge2.geometry = line2.sub(p2, line2.last());
            GeoObject::tms.create(&gs1, &gs2, &ge1, &ge2);

            auto createTempRoad = [&](const std::string& name, RoadNode& from, RoadNode& to, GeoObject& go, bool one_way) -> Road {
                Road temp;
                temp.type = name;
                temp.geoobject = go;
                temp.start_node = from;
                temp.end_node = to;
                temp.cost = go.geometry.geo().asLine().length();
                temp.one_way = one_way;
                return temp;
            };

            Road nr1 = createTempRoad(" ", r1.start_node.get(), ns, gs1, r1.one_way);
            Road nr2 = createTempRoad(" ", ns, r1.end_node.get(), gs2, r1.one_way);
            Road nr3 = createTempRoad(" ", r2.start_node.get(), ne, ge1, r2.one_way);
            Road nr4 = createTempRoad(" ", ne, r2.end_node.get(), ge2, r2.one_way);
            Road::tms.create(&nr1, &nr2, &nr3, &nr4);


            auto filter = shortestPath(ns, ne);

            if (!filter.empty()) {
                for (Road& r : filter) {
                    TGeo geo = r.geoobject.get().geometry.geo();
                    geo.label = r.type;
                    geo.style.borderColor = 0x333333;
                    geo.style.borderWidth = 2;
                    geo.style.selected = true;
                    data.push_back(geo);
                }
                //
                TGeo geo = ns.geometry.geo();
                geo.label = ns.name;
                geo.style.borderColor = 0x22aa22;
                geo.style.borderWidth = 2;
                geo.style.selected = true;
                data.push_back(geo);
                //
                geo = ne.geometry.geo();
                geo.label = ne.name;
                geo.style.borderColor = 0x22aa22;
                geo.style.borderWidth = 5;
                geo.style.selected = true;
                data.push_back(geo);
            }

            // release 
            RoadNode::tms.del(ns, ne);
            GeoObject::tms.del(gs1, gs2, ge1, ge2);
            Road::tms.del(nr1, nr2, nr3, nr4);
        }
        catch (...) {};

        return data;
    }
};

//-----------------------------------------------------------------------------------------------------------