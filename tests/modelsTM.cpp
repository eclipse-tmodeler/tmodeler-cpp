/*
 * Copyright (c) 2026 TAMBO ai and others
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 */

#include "modelsTM.hpp"



using namespace ai::tambo::tmodeler;

void startDataEngine() {

    std::thread delayedThread([]() {

        testInitTM();
        linker();

        while (true)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(5));
        }
    });

}


void linker() {
    //linkerVilleTest();

    Road::tms.clear();
    RoadNode::tms.clear();
    GeoObject::tms.clear();


    Spot s1(0, 0);
    Spot s2(0, 5);
    Spot s3(5, 5);
    Spot s4(5, 10);
    Line l1(s1, s2, s3, s4);

    GeoObject go1;
    go1.geometry = l1;
    go1.save();

    Road r0;
    r0.geoobject = go1;
    r0.cost = l1.length();
    //...
    r0.save();

    RoadNode rn1, rn2;
    rn1.name = "I" + std::to_string(r0.key());
    rn1.geometry = l1.first();
    rn1.save();
    rn2.name = "O" + std::to_string(r0.key());
    rn2.geometry = l1.last();
    rn2.save();

    r0.start_node = rn1;
    r0.end_node = rn2;
    r0.save();


    // Créer des RoadNode
    RoadNode A, B, C, D;
    A.name = "A"; B.name = "B"; C.name = "C"; D.name = "D";
    A.save(); B.save(); C.save(); D.save();

    // Créer et sauvegarder les routes
    Road r1, r2, r3, r4;

    // A → B
    r1.start_node = A;
    r1.end_node = B;
    r1.cost = 1.0f;
    r1.one_way = true;
    r1.type = "A->B";
    r1.save();

    // B → C
    r2.start_node = B;
    r2.end_node = C;
    r2.cost = 1.0f;
    r2.one_way = true;
    r2.type = "B->C";
    r2.save();

    // A → D
    r3.start_node = A;
    r3.end_node = D;
    r3.cost = 3.0f;
    r3.one_way = true;
    r3.type = "A->D";
    r3.save();

    // D ↔ C (bidirectionnelle)
    r4.start_node = D;
    r4.end_node = C;
    r4.cost = 2.0f;
    r4.one_way = false;
    r4.type = "D<->C";
    r4.save();

    Log::d(Road::tms.all().data());

    // Appel de la fonction de plus court chemin
    std::vector<Road> chemin = shortestPath(A, C);

    // Affichage du résultat
    std::cout << "Itineraire de A à C :\n";
    for (Road& r : chemin) {
        std::cout << "- " << r.type.get() << " [" << r.start_node.ref() << " -> " << r.end_node.ref() << "]\n";
    }
}



void linkerVilleTest() {
    Road::tms.clear();
    RoadNode::tms.clear();
    GeoObject::tms.clear();

    const int ROWS = 5;
    const int COLS = 5;
    const float STEP = 10.0f; // distance entre les intersections

    RoadNode grid[ROWS][COLS];

    // Étape 1 : Créer les intersections (RoadNode)
    for (int y = 0; y < ROWS; ++y) {
        for (int x = 0; x < COLS; ++x) {
            RoadNode node;
            node.name = "N" + std::to_string(y) + "_" + std::to_string(x);
            node.geometry = Spot(x * STEP, y * STEP);
            node.save();
            grid[y][x] = node;
        }
    }

    int roadId = 0;

    // Étape 2 : Relier horizontalement (gauche-droite)
    for (int y = 0; y < ROWS; ++y) {
        for (int x = 0; x < COLS - 1; ++x) {
            Spot a = grid[y][x].geometry;
            Spot b = grid[y][x + 1].geometry;
            Line l(a, b);

            GeoObject go;
            go.geometry = l;
            go.save();

            Road r;
            r.geoobject = go;
            r.cost = l.length() + (rand() % 5); // ajoute variation
            r.one_way = (rand() % 2 == 0); // parfois à sens unique
            r.type = "H" + std::to_string(roadId++);
            r.start_node = grid[y][x];
            r.end_node = grid[y][x + 1];
            r.save();
        }
    }

    // Étape 3 : Relier verticalement (haut-bas)
    for (int y = 0; y < ROWS - 1; ++y) {
        for (int x = 0; x < COLS; ++x) {
            Spot a = grid[y][x].geometry;
            Spot b = grid[y + 1][x].geometry;
            Line l(a, b);

            GeoObject go;
            go.geometry = l;
            go.save();

            Road r;
            r.geoobject = go;
            r.cost = l.length() + (rand() % 5);
            r.one_way = (rand() % 2 == 0);
            r.type = "V" + std::to_string(roadId++);
            r.start_node = grid[y][x];
            r.end_node = grid[y + 1][x];
            r.save();
        }
    }

    // Test de calcul : du coin haut gauche (A1) au coin bas droit (E5)
    RoadNode from = grid[0][0]; // A1
    RoadNode to = grid[4][4];   // E5

    std::vector<Road> chemin = shortestPath(from, to);

    std::cout << "Itinéraire de " << from.name.get() << " à " << to.name.get() << " :\n";
    for (Road& r : chemin) {
        std::cout << "- " << r.type.get()
            << " [" << r.start_node.get().name.get()
            << " -> " << r.end_node.get().name.get()
            << "] cost=" << r.cost.get() << "\n";
    }
}

// itineraire

inline double bufferMetersToDegrees(double meters) {
    return meters / 111320.0;  // approx en latitude (valide pour de petits buffers)
}

double radiusMetersToDegrees(double meters) {
    // 1° de latitude ≈ 111 000 mètres
    return meters / 111000.0;
}

std::vector<Road> tmpRoads;
std::vector<Road> shortestPath(RoadNode from, RoadNode to) {
    Line geo(from.geometry.geo().asSpot(), to.geometry.geo().asSpot());
    double baseLength = geo.length();
    double bMin = baseLength / 2.0;
    double bMax = baseLength * 2.0;
    double step = (bMax - bMin) / 5.0;

    return shortestPath(from, to, geo, bMin, bMax, step);
}

std::vector<Road> shortestPath(RoadNode from, RoadNode to, const Line geo, double bMin, double bMax, double step) {
    double b = bMin;
    while (b <= bMax) {
        Road r0;
        GeoObject g0;

        auto result = Road::tms.with(r0, g0)
            .lazy()
            .filter(r0.geoobject == g0)
            .filter(g0.geometry.index(geo.buffer(b)))
            .order(+g0.geometry.distance(geo))
            .build()
            .select<Road>();

        if (!result.empty())
        {
            std::vector<Road> roads;

            if (tmpRoads.empty()) {
                roads = shortestPath(from, to, result);
            }
            else {
                tmpRoads.insert(tmpRoads.end(), result.begin(), result.end());
                roads = shortestPath(from, to, tmpRoads);
            }

            if (!roads.empty()) {
                return roads;
            }
        }

        b += step;
    }

    throw std::runtime_error("Aucune route proche trouvée dans la plage de buffer");
}

Road reverseRoad(Road& road) {
    Road rev;
    rev.parse(road.data()); //clone
    rev.reset(); //dissocie
    rev.start_node = road.end_node;
    rev.end_node = road.start_node;
    rev.key() = -road.key().get();
    if (road.geoobject.useCache()) {
        rev.geoobject = road.geoobject;
    }
    return rev;
}

std::vector<Road> shortestPath(RoadNode from, RoadNode to, std::vector<Road> allRoads) {
    using namespace std;

    unordered_map<NodeId, vector<Road>> graph;
    unordered_map<RoadId, Road> roadCache;

    for (Road& road : allRoads) {
        NodeId src = road.start_node.ref();
        NodeId dst = road.end_node.ref();

        graph[src].push_back(road);
        roadCache[road.key()] = road;

        // Ajouter l'inverse si bidirectionnelle
        if (!road.one_way) {
            Road rev = reverseRoad(road);
            graph[dst].push_back(rev);
            roadCache[rev.key()] = rev; // facultatif si les id restent uniques
        }
    }

    if (!graph.count(from.key()))
        return {}; // Aucun chemin possible

    // Initialisation de la file de priorité
    priority_queue<PathStep, vector<PathStep>, CompareCost> queue;
    queue.push({ from.key(), 0.0f, {} });

    unordered_set<NodeId> visited;
    unordered_map<NodeId, float> minCost;

    while (!queue.empty()) {
        PathStep current = queue.top();
        queue.pop();

        if (visited.count(current.node_id)) continue;
        visited.insert(current.node_id);

        if (current.node_id == to.key()) {
            vector<Road> result;
            for (RoadId rid : current.path) {
                if (roadCache.count(rid))
                    result.push_back(roadCache.at(rid));
            }
            return result;
        }

        // Parcourir les routes à partir du noeud courant
        for (Road& edge : graph[current.node_id]) {
            NodeId neighbor = edge.end_node.ref();
            float newCost = current.total_cost + edge.cost;

            if (!minCost.count(neighbor) || newCost < minCost[neighbor]) {
                minCost[neighbor] = newCost;

                vector<RoadId> newPath = current.path;
                newPath.push_back(edge.key());

                queue.push({ neighbor, newCost, newPath });
            }
        }
    }

    // Aucun chemin trouvé
    return {};
}

void testInitTM(std::function<void(void)> listener)
{
    TModeler::start()
        .dbReady(listener)
        .init(Tdb::Builder()
            .type(Tdb::Type::SPATIALITE)
            .dbDir("/tmp/sql")
            .dbName("test.db")
            .accept("models.shops")
            .accept("models.users")
            .get());
}



std::vector<TGeo> doualaCity() {
    std::vector<TGeo> geometries;

    const int gridMin = -30;
    const int gridMax = 30;
    const int gridStep = 10;

    // === RÉSEAU ROUTIER (GRID) ===
    int routeId = 0;
    for (int y = gridMin; y <= gridMax; y += gridStep) {
        for (int x = gridMin; x < gridMax; x += gridStep) {
            Line l(Spot(x, y), Spot(x + gridStep, y));
            l.style.borderColor = 0x444444;
            l.style.borderWidth = 4;
            l.style.zIndex = 0;
            l.label = "Rue H " + std::to_string(y);
            l.style.selected = false;
            geometries.push_back(l);
        }
    }
    for (int x = gridMin; x <= gridMax; x += gridStep) {
        for (int y = gridMin; y < gridMax; y += gridStep) {
            Line l(Spot(x, y), Spot(x, y + gridStep));
            l.style.borderColor = 0x666666;
            l.style.borderWidth = 3;
            l.style.zIndex = 0;
            l.label = "Avenue V " + std::to_string(x);
            l.style.selected = false;
            geometries.push_back(l);
        }
    }

    // === BÂTIMENTS VARIÉS ===
    auto addBuilding = [&](double x, double y, int w, int h, int fill, int border, const std::string& label) {
        Rect b(Spot(x, y), Spot(x, y + h), Spot(x + w, y + h), Spot(x + w, y));
        b.style.fillColor = fill;
        b.style.borderColor = border;
        b.style.borderWidth = 1;
        b.style.zIndex = 1;
        b.label = label;
        geometries.push_back(b);
        };

    addBuilding(-28, 28, 4, 3, 0xFFE4C4, 0xB8860B, "Hôpital Central");
    addBuilding(22, 26, 3, 3, 0xFF69B4, 0x8B008B, "Pharmacie Municipale");
    addBuilding(-12, 12, 5, 4, 0xD3D3D3, 0x999999, "École Publique A");
    addBuilding(6, 6, 4, 3, 0xC0C0C0, 0x666666, "Centre Commercial Akwa");
    addBuilding(-5, -25, 3, 3, 0xFFD700, 0x8B4513, "Station-service");
    addBuilding(20, -10, 4, 4, 0x98FB98, 0x006400, "Mairie de Douala");
    addBuilding(-22, -12, 3, 3, 0xFFB6C1, 0xDC143C, "Commissariat");
    addBuilding(-30, 5, 3, 4, 0xFFFACD, 0xDAA520, "Église St-Pierre");

    // === SUBDIVISIONS ===
    TPolygon bonapriso(Spot(-30, 0), Spot(-30, 30), Spot(0, 30), Spot(0, 0));
    bonapriso.style.fillColor = 0xFFF0F5;
    bonapriso.style.borderColor = 0xCC99FF;
    bonapriso.style.borderWidth = 2;
    bonapriso.style.zIndex = -1;
    bonapriso.label = "Bonapriso";
    geometries.push_back(bonapriso);

    TPolygon akwa(Spot(0, 0), Spot(0, 30), Spot(30, 30), Spot(30, 0));
    akwa.style.fillColor = 0xF0FFFF;
    akwa.style.borderColor = 0x99CCCC;
    akwa.style.borderWidth = 2;
    akwa.style.zIndex = -1;
    akwa.label = "Akwa";
    geometries.push_back(akwa);

    TPolygon bali(Spot(-30, -30), Spot(-30, 0), Spot(0, 0), Spot(0, -30));
    bali.style.fillColor = 0xF5F5F5;
    bali.style.borderColor = 0xAAAAAA;
    bali.style.borderWidth = 2;
    bali.style.zIndex = -1;
    bali.label = "Bali";
    geometries.push_back(bali);

    TPolygon ndogbong(Spot(0, -30), Spot(0, 0), Spot(30, 0), Spot(30, -30));
    ndogbong.style.fillColor = 0xF0F8FF;
    ndogbong.style.borderColor = 0xAAAAFF;
    ndogbong.style.borderWidth = 2;
    ndogbong.style.zIndex = -1;
    ndogbong.label = "Ndongbong";
    geometries.push_back(ndogbong);

    // === PARC CENTRAL ===
    TPolygon parc(Spot(-5, -5), Spot(-5, 0), Spot(0, 0), Spot(0, -5));
    parc.style.fillColor = 0xA8DDA8;
    parc.style.borderColor = 0x558855;
    parc.style.borderWidth = 1;
    parc.style.zIndex = 2;
    parc.label = "Parc Central";
    geometries.push_back(parc);

    // === POINTS D’INTÉRÊT ===
    Spot poi1(-2, 2);
    poi1.style.fillColor = 0xFF0000;
    poi1.style.borderColor = 0x000000;
    poi1.style.zIndex = 5;
    poi1.label = "Marché Central";
    geometries.push_back(poi1);

    Spot poi2(6, 6);
    poi2.style.fillColor = 0x0000FF;
    poi2.style.borderColor = 0x000000;
    poi2.style.zIndex = 5;
    poi2.label = "Mosquée";
    geometries.push_back(poi2);

    return geometries;
}

TM_MANAGER(LandUnit)
TM_MANAGER(GeoObject)
TM_MANAGER(RoadNode)
TM_MANAGER(Road)
TM_MANAGER(RoadLink)

std::vector<TGeo> doualaCityFromModel() {
    std::vector<TGeo> geometries;

    // Vérifie si la base est déjà remplie
    auto all = Road::tms.all();
    if (!all.empty()) {
        for (Road& r : all) {
            TGeo geo = r.geoobject.get().geometry.geo();
            geo.label = r.type;
            geo.style.borderColor = r.one_way ? 0xCC0000 : 0x009900;
            geo.style.borderWidth = r.one_way ? 3 : 2;
            geo.style.oneWay = r.one_way;
            geometries.push_back(geo);
        }
        return geometries; // ✅ on a déjà chargé les routes de la base
    }

    Road::tms.clear();
    RoadNode::tms.clear();
    GeoObject::tms.clear();

    const int ROWS = 7;
    const int COLS = 7;
    double step = 0.0009; // ≈ 100 mètres
    double x0 = 9.701000, y0 = 4.048000;

    std::map<std::string, RoadNode> nodes;

    auto nodeName = [](int row, int col) {
        return "N" + std::to_string(row) + "_" + std::to_string(col);
        };

    auto addNode = [&](const std::string& name, double x, double y) {
        RoadNode n;
        n.name = name;
        n.geometry = Spot(x, y);
        n.save();
        nodes[name] = n;
        };

    auto addRoad = [&](const std::string& name, const std::string& from, const std::string& to, bool oneWay) {
        Spot a = nodes[from].geometry;
        Spot b = nodes[to].geometry;
        Line l(a, b);

        GeoObject go;
        go.geometry = l;
        go.save();

        Road r;
        r.type = name;
        r.geoobject = go;
        r.start_node = nodes[from];
        r.end_node = nodes[to];
        r.cost = l.length();
        r.one_way = oneWay;
        r.save();

        // Pour affichage
        l.label = name;
        l.style.borderColor = oneWay ? 0xCC0000 : 0x009900;
        l.style.borderWidth = oneWay ? 3 : 2;
        l.style.oneWay = oneWay;
        geometries.push_back(l);
        };

    // === Étape 1 : Créer les intersections ===
    for (int j = 0; j < ROWS; ++j) {
        for (int i = 0; i < COLS; ++i) {
            std::string name = nodeName(j, i);
            double x = x0 + i * step;
            double y = y0 - j * step;
            addNode(name, x, y);
        }
    }

    int roadId = 1;

    // === Étape 2 : Créer les routes horizontales (Est-Ouest)
    for (int j = 0; j < ROWS; ++j) {
        for (int i = 0; i < COLS - 1; ++i) {
            std::string from = nodeName(j, i);
            std::string to = nodeName(j, i + 1);
            bool oneWay = ((i + j) % 2 == 0); // alternance
            addRoad("R" + std::to_string(roadId++), from, to, oneWay);
            if (!oneWay)
                addRoad("R" + std::to_string(roadId++), to, from, oneWay);
        }
    }

    // === Étape 3 : Créer les routes verticales (Nord-Sud)
    for (int j = 0; j < ROWS - 1; ++j) {
        for (int i = 0; i < COLS; ++i) {
            std::string from = nodeName(j, i);
            std::string to = nodeName(j + 1, i);
            bool oneWay = ((i + j) % 2 == 1); // alternance
            addRoad("R" + std::to_string(roadId++), from, to, oneWay);
            if (!oneWay)
                addRoad("R" + std::to_string(roadId++), to, from, oneWay);
        }
    }

    return geometries;
}
