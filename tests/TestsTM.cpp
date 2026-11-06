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

#include <gtest/gtest.h>
#include "core/Tob.hpp"
#include "field/TFields.hpp"
#include "ms/Tms.hpp"
#include "model/TModel.hpp"

#include <iostream>
#include <functional>
#include <future>
#include <thread>

#include "db/Tspatialite.h"
#include "db/Ttable.h"

#include "modeler/TModeler.h"
#include "ms/TFilterBuilder.h"


using namespace ai::tambo::tmodeler;

class Familly : public TModel<Familly> {
    TM_SCHEMA(Familly, "models.group", TF(name))

    TextField name;
};
TM_MANAGER(Familly) // dans un cpp : pour avoir acces à l'outil de gestion tms : Familly::tms

class Person : public TModel<Person> {
    TM_SCHEMA(Person, "models.users",
        TF(name), TF(design), TF(ratio), TF(empty),
        TF(dob), TF(update_at),
        TF(myFamilly), TF(bigFamilly), TF(friends))

    TextField name;
    TextField design;
    FloatField ratio;
    BoolField empty;
    TimeField dob = init<TimeField>().format(TF::DATE);
    TimeField update_at = init<TimeField>().format(TF::DATE_TIME);
    ModelField<Person> myFamilly = init<ModelField<Person>>().onDelete(TF::CASCADE);
    ModelField<Familly> bigFamilly = init<ModelField<Familly>>().onDelete(TF::CASCADE);
    ListField<Person> friends = init<ListField<Person>>();
};
TM_MANAGER(Person)

class Client : public TModel<Client> {
    TM_SCHEMA(Client, "models.shops", TF(name), TF(age), TF(height), TF(dob), TF(friends), TF(location))

    TextField name;
    IntField age;
    FloatField height;
    TimeField dob = init<TimeField>().format(TF::DATE);
    ListField<Client> friends;
    GeoField location;
};
TM_MANAGER(Client)

class Product : public TModel<Product> {
    TM_SCHEMA(Product, "models.shops", TF(name))

    TextField name;
};
TM_MANAGER(Product)

class Command : public TModel<Command> {
    TM_SCHEMA(Command, "models.shops", TF(client), TF(product))

    ModelField<Client> client;
    ModelField<Product> product;
};
TM_MANAGER(Command)

class Payment : public TModel<Payment> {
    TM_SCHEMA(Payment, "models.shops", TF(phone), TF(name), TF(mode), TF(amount), TF(code), TF(ref), TF(command))

    TextField phone;
    TextField name;
    TextField mode;// = TextField().defaults("Mobile Money");
    FloatField amount;
    TextField code;
    TextField ref;// = TextField().defaults("Payment d une commande");
    ModelField<Command> command;
};
TM_MANAGER(Payment)

namespace Expr {
    inline Person p{};
    inline Client cl{};
    inline Product pr{};
    inline Command cm{};
    inline Client c0{};
    inline Product p0{};
    inline Command cd{};
}

using Expr::p;
using Expr::cl;
using Expr::pr;
using Expr::cm;
using Expr::c0;
using Expr::p0;
using Expr::cd;

void comm()
{
    Tms<Client> tms;

    tms.onSave([](auto keys)
        {
            Log::d("onSave...\n" +
                Client::tms.get(keys).data());
        });
    tms.onCreate([](auto keys)
        {
            Log::d("onCreate...\n" +
                Client::tms.get(keys).data());
        });
    tms.onUpdate([](auto keys)
        {
            Log::d("onUpdate...\n" +
                Client::tms.get(keys).data());
        });
    tms.onDelete([](auto keys)
        {
            Log::d("onDelete...\n" + 
                vectorToString(keys));
        });
    tms.onModelChange([](auto keys)
        {
            Log::d("onModelChange...\n" + 
                vectorToString(keys));
        });


    /*
    std::thread delayedThread([persons]() {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        Person::tms.get(0);
        Person::tms.create(persons);

        std::thread delayedThread([persons]() {
            Tms<Person> tms;
            tms.name();//remove generate pbs
            std::this_thread::sleep_for(std::chrono::seconds(2));
            tms.create(persons);
            std::this_thread::sleep_for(std::chrono::seconds(2));
            tms.update(persons);
        });

        std::thread delayedThread2([persons]()
        {
                Tms<Person> tms;
                tms.name();//remove generate pbs
                std::this_thread::sleep_for(std::chrono::seconds(2));
                tms.save(persons);
                std::this_thread::sleep_for(std::chrono::seconds(2));
                tms.del(persons);
        });


        while (true)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(5));
        }
    });*/
}


class Mo : public TModel<Mo> {
    TM_SCHEMA(Mo, "", TF(title), TF(desc))

    TextField title;
    TextField desc;
};
TM_MANAGER(Mo)

class Pixel : public TModel<Pixel> {
    TM_SCHEMA(Pixel, "", TF(r), TF(g), TF(b))

    IntField r;
    IntField g;
    IntField b;
};
TM_MANAGER(Pixel)

class Geo : public TModel<Geo> {
    TM_SCHEMA(Geo, "models.space3", TF(title), TF(description), TF(loc), TF(geometry))

    TextField title;
    TextField description;
    GeoField loc = GeoField().spatialIndex();
    GeoField geometry;
};



TM_MANAGER(Geo)

void geoTest()
{
    /*
    Geo::tms.clear();
    Geo::tms.all();*/

    Spot s1(0, 0);
    Spot s2(0, 5);
    Spot s3(5, 5);
    Spot s4(5, 0);
    Line l1(s1, s2, s3, s4);
    TPolygon p1(s1, s2, s3, s4);
    Rect r1(s1, s2, s3, s4);
    TPolygon p2(s1, s2, s4);
    TPolygon p3(s2, s3, s4);
    MultiPolygon mp(p2, p3);

    if (Geo::tms.all().empty())
    {
        Geo geo;
        geo.title = "First";
        geo.loc = s3;
        Log::d("Geo: " + geo.data());
        geo.loc = l1;
        Log::d("Geo: " + geo.data());
        geo.loc = p1;
        Log::d("Geo: " + geo.data());
        geo.loc = r1;
        Log::d("Geo: " + geo.data());

        TPolygon p = geo.loc;
        p.get(2).x = 5;
        p.get(2).y = 5;
        geo.loc = p;
        geo.save();
    }

    /*
    auto count = Geo::tms.all().size();
    auto geo = Geo::tms.all().first();
    if (!geo.loc.geo().isPolygon())
    {
        geo.loc = p1;
        geo.save();
    }
    TPolygon p = geo.loc;
    p.get(2).x = 5;
    p.get(2).y = 5;
    geo.loc = p;
    geo.save();
    Log::d("Geo: " + Geo::tms.all().data());
    geo.loc = r1;
    geo.save();
    Log::d("Geo: " + Geo::tms.all().data());
    geo = Geo::tms.all().first();
    Rect r = geo.loc;
    Log::d("area: " + std::to_string(r.area()));
    geo.loc = l1;
    geo.save();
    Log::d("Geo: " + Geo::tms.all().data());
    geo = Geo::tms.all().first();
    Line l = geo.loc;
    Log::d("length: " + std::to_string(l.length()));
    geo.loc = s4;
    geo.save();
    Log::d("Geo: " + Geo::tms.all().data());
    geo = Geo::tms.all().first();
    Spot s = geo.loc;
    Log::d("x, y: " + std::to_string(s.x) + "," + std::to_string(s.y));
    geo.loc = mp;
    geo.save();
    Log::d("Geo: " + Geo::tms.all().data());
    geo = Geo::tms.all().first();
    MultiPolygon m = geo.loc;
    Log::d("TPolygon count : " + std::to_string(m.size()));
    geo.loc = p1;
    geo.save();
    Log::d("Geo: " + Geo::tms.all().data());
    */

    Tlist<Geo> filter;
    Geo g0;
    filter = Geo::tms.with(g0).filter(g0.loc == s1);
    filter = Geo::tms.with(g0).filter(g0.loc == nullptr);
    filter = Geo::tms.with(g0).filter(g0.loc != nullptr);
    filter = Geo::tms.with(g0).filter(g0.loc == l1);
    filter = Geo::tms.with(g0).filter(g0.loc == p1);
    filter = Geo::tms.with(g0).filter(g0.loc != r1);
    filter = Geo::tms.with(g0).filter(g0.loc != mp);

    filter = Geo::tms.with(g0).filter(g0.loc.index(s1) && g0.loc.contains(s1));
    Log::d(filter.data());
    filter = Geo::tms.with(g0).filter(g0.loc.index(mp) && g0.loc.inside(mp));
    Log::d(filter.data());
    filter = Geo::tms.with(g0).filter(g0.loc.index(p1) && g0.loc.intersects(p1));
    Log::d(filter.data());
    filter = Geo::tms.with(g0).filter(g0.loc.crosses(l1));
    Log::d(filter.data());
    filter = Geo::tms.with(g0).filter(g0.loc.touches(l1));
    Log::d(filter.data());

    filter = Geo::tms.with(g0)
        .lazy()
        .filter(g0.loc.intersects(l1.buffer(0.001)))
        .group(g0.title)
        .filter(g0._id.count() >= 2)
        .build();
    Log::d(filter.data());
    filter = Geo::tms.with(g0)
        .lazy()
        .filter((g0.loc.distance(s1) <= 10) && (g0.loc.azimuth(s1) < 30))
        .group(g0.title)
        .filter(g0._id.count() >= 2)
        .build();
    Log::d(filter.data());
}

void joinTest()
{
    Client cl1, cl2, cl3, cl4, cl5;
    Product pr1, pr2, pr3;
    Command cm1, cm2, cm3, cm4, cm5, cm6;

    Client::tms.clear();
    Product::tms.clear();
    Command::tms.clear();
    Payment::tms.clear();
    Client::tms.migrate();
    Product::tms.migrate();
    Command::tms.migrate();
    Client::tms.name();
    Product::tms.name();
    Command::tms.name();

    comm();

    Client cli;
    cli.name = "My";
    cli.save();
    Command c1;
    c1.client = cli;
    Log::d(c1.data());
    c1.save();
    Log::d(Command::tms.all().data());

    Payment p;
    //p.parse("{\"phone\":\"53645654\",\"name\":\"sdfsdfsd\",\"mode\":\"Mobile_Money\",\"amount\":800,\"code\":\"dsfsdf\",\"ref\":\"sdfsdfsdf\"}");
    p.parse("{\"phone\":\"+23753645654\",\"name\":\"sdfsdfsd\",\"mode\":\"Mobile_Money\",\"amount\":800,\"code\":\"dsfsdf\",\"ref\":\"sdfsdfsdf\",\"command\":1}");
    p.save();
    Log::d(p.data());
    Log::d(Payment::tms.all().data());

    if (Client::tms.all().empty() || true)
    {
        cl1.name = "Olivier TAMBO";
        cl1.age = 15;
        cl1.height = 1.70;
        cl1.dob = "2010-01-01";
        cl2.name = "Michel TAMBO";
        cl2.age = 16;
        cl2.height = 1.65;
        cl2.dob = "2010-02-02";
        cl3.name = "Patie TAMBO";
        cl3.age = 17;
        cl3.height = 1.60;
        cl3.dob = "2010-02-01";
        cl4.name = "Olivier TB";
        cl4.age = 18;
        cl4.height = 1.55;
        cl4.dob = "2011-01-05";
        cl5.name = "Annie TAMBO";
        cl5.age = 19;
        cl5.height = 1.50;
        cl5.dob = "2015-05-10";
        //
        if (!cl1.create())
        {
            Log::d("create: \ncan't");
        }
        if (!cl1.create())
        {
            Log::d("create: \ncan't");
        }
        Log::d("create: \n" + cl1.data());
        cl2.save();
        cl3.save();
        cl4.save();
        cl5.save();
        cl4.name = "Olivier TAMBO";
        cl4.save();
        cl5.del();
        //
        cl1.friends = { &cl2, &cl3 }; //vector, c'est toujours ok
        cl1.friends += cl4;
        //
        cl1.save();
    }
    if (Product::tms.all().empty() || true)
    {
        pr1.name = "Poulet";
        pr2.name = "Poisson";
        pr3.name = "Porc";
        //
        pr1.save();
        pr2.save();
        pr3.save();
    }
    if (Command::tms.all().empty() || true)
    {
        cm1.client = cl1;
        cm1.product = pr1;
        cm1.save();
        cm2.client = cl1;
        cm2.product = pr2;
        cm2.save();
        cm3.client = cl2;
        cm3.product = pr3;
        cm3.save();
        cm4.client = cl2;
        cm4.product = pr1;
        cm4.save();
        cm5.client = cl1;
        cm5.product = pr2;
        cm5.save();
        cm6.client = cl3;
        cm6.product = pr3;
        cm6.save();
    }

    Log::d("Client: \n" + Client::tms.all().data());
    Log::d("Product: \n" + Product::tms.all().data());
    Log::d("Command: \n" + Command::tms.all().data());
    Client ct = Command::tms.all().first().client;
    Log::d("_client: \n" + ct.data());

    auto filter1 = Client::tms.with(cl).filter(cl._id > 0)
                                .order(+cl.name)
                                .group(cl.name).filter(cl._id.count() == 2);
    Log::d("filter1: \n" + filter1.data());

    auto keys = Client::tms.fieldsKeys();


    Tlist<Client, Command> join = Client::tms.with<Command>(cl, cm).join(JoinType::LEFT).filter(cm.client == cl)
        .filter(cm.client == nullptr)
        .order(-cl.dob)
        .group(cl._id).filter(cl._id.count() <= 2);

    auto joinClient = join.select<Client>();

    auto joinCommandClient = join.select<Command, Client>();

    Log::d("join: \n" + join.data());
    Log::d("join: \n" + joinClient.data());
    Log::d("join: \n" + joinCommandClient.data());



    Tms<Client> tms;
    Tlist<Client> l = tms.all();
    l.add(tms.get(0));
    l.remove(0);
    l.set(0, tms.get(0));
    auto index = l.indexOf(tms.get(4));
    auto index2 = l.indexOf(tms.get(5));
    auto index3 = l.indexOf(4);
    auto index4 = l.indexOf(5);



    Spot pharmacy(3.1646, 9.6461);
    cl = Client();
    cm = Command();
    pr = Product();

    // Find all clients who have ordered Aspirin at least 5 times  
    // near the pharmacy, sorted by age and proximity.
    auto filter = Client::tms.with<Command>(cl, cm)
        .lazy()
        .filter(cm.client == cl)
        .filter(cl.location.index(pharmacy.buffer(0.0003)) )
        .order(+cl.location.distance(pharmacy)) //+cl.dob, 
        .group(cl.id).filter(cl.id.count() >= 5)
        .build()
        .select<Client>();



}




double stepToMeters(double step, double degreesPerStep = 0.0009) {
    const double metersPerDegree = 111320.0;  // Approx à l'équateur
    return step * degreesPerStep * metersPerDegree;
}


TEST(TModelerTest, TFieldTest) {
    testInitTM();


    Road r0;
    GeoObject g0;
    const Spot p(0, 0);
    double b = 0.0003;

    auto result = Road::tms.with<GeoObject>(r0, g0)
        .lazy()
        .filter(r0.geoobject == g0)
        .filter(g0.geometry.index(p.buffer(b)) &&
            g0.geometry.intersects(p.buffer(b)))
        .order(+g0.geometry.distance(p))
        .build()
        .select<Road>();

    Log::d(result.data());

    //geoTest();

    //joinTest();

    //doualaCityFromModel();

    // resoudre les filktre à indexation spatial

    /*
    const Spot start(9.7066040818181811, 4.0430152363636367);
    const Spot end(9.7065677181818177, 4.0475788727272732);
    const Spot spot(0, 0);

    Log::d("RoadNode count : " + std::to_string(RoadNode::tms.all().size()));
    Log::d("GeoObject count : " + std::to_string(GeoObject::tms.all().size()));
    Log::d("Road count : " + std::to_string(Road::tms.all().size()));

    std::vector<TGeo> roads = Direction1::direction(start, end);
    for (TGeo& g : roads) {
        Log::d(g.label+" : "+g.toString());
    }

    GeoObject g0;

    auto all = GeoObject::tms.with(g0).filter(g0.name == "-").group(g0.id).filter(g0.id.count()>2).order(+g0.geometry);
    all = GeoObject::tms.with(g0).lazy().filter(g0.name == "-").group(g0.id).filter(g0.id.count()>2).order(+g0.geometry).build();

    auto r = nearestRoad(spot);

    RoadNode rn0;
    Road r0;
    auto filter = Road::tms.with<GeoObject>(r0, g0).filter(r0.geoobject == g0).filter(g0.geometry.index(spot.buffer(10))).select<Road>();

    Log::d(RoadNode::tms.all().data());

    auto n1 = RoadNode::tms.with(rn0).filter(rn0.geometry.index(spot.buffer(0.0001)));
    */

    while (true){std::this_thread::sleep_for(std::chrono::microseconds(5));}
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv); 
    return RUN_ALL_TESTS();
}



// on passe aux requettes :

/*
TLocation::tms.clear();

Spot spot;
Line line;
TPolygon TPolygon;
Multigon multigon;

spot.x = 0;
spot.y = 0;

spot.save();

line.add(spot, spot, spot, spot);
line.save();
Spot ab = line.get(0);
ab.x = 1;
ab.y = 1;
ab.save();
line.remove(0);
line.save();
line.add(spot);
spot = Spot();
spot.x = 3;
spot.y = 3;
line.add(0,spot);

spot.save();
TPolygon.add(spot, spot, spot, spot);
spot = Spot();
spot.x = 4;
spot.y = 4;
TPolygon.add(0, spot);
spot = Spot();
spot.x = 5;
spot.y = 6;
TPolygon.add(0, spot);
TPolygon.close();
TPolygon.save();

multigon.add(TPolygon, TPolygon);

line.save();
TPolygon.save();
multigon.save();

Log::d("Geo : \n" + TLocation::tms.all().data());



Bitmap::tms.clear();
Geomap::tms.clear();

Pixel p1;
p1.r = 255;
p1.g = 125;
p1.b = 93;

Bitmap bitmap;
bitmap.save();

Log::d("Map : \n" + Bitmap::tms.name());
Log::d("Map : \n" + Bitmap::tms.all().data());



Mo mo;
mo.title = "First Title";
mo.desc = "First Desc";
mo.save();



Spot a{ 0, 0 };
Spot b{ 5, 0 };
Spot c{ 5, 5 };
Spot d{ 0, 5 };

Rect surface(a, b, c, d);

Geomap map;
map.save();
map.init(surface, 5.0);
map.save();

Log::d("Map : \n" + map.data());

Spot s1(1, 2);
Spot s2(1, 4);
Spot s3(4.5, 1);
Spot s4(3, 3);
Mo dt;
dt.title = "title ";
dt.desc = "desc ";
map.set(s1, dt);
map.set(s2, dt);
map.set(s3, dt);
map.set(s4, dt);

Log::d("Map : \n" + Geomap::tms.name());
Log::d("Map : \n" + Geomap::tms.all().data());
*/
