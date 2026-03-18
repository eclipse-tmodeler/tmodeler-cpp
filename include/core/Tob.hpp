#pragma once

#include "Tob.h"

#include <vector>
#include <string>
#include <type_traits>

#include <nlohmann/json.hpp>


#include <tuple>
#include <vector>
#include <string>
#include <type_traits>


template<typename... Ts, std::size_t... Is>
void assignFromJsonImpl(std::tuple<Ts...>& tupleObj, const Json& row, std::index_sequence<Is...>) {
    // Pour chaque index I, on récupère le type Ti = Ts[I]
    // Puis on fait appel à fromJson(instance<Ti>.serial(), row[I])
    ((std::get<Is>(tupleObj).fromJson(Tob::instance<std::tuple_element_t<Is, std::tuple<Ts...>>>().serial(), row[Is])), ...);
}

template<typename... Ts>
void assignFromJsonTuple(std::tuple<Ts...>& tupleObj, const Json& row) {
    assignFromJsonImpl(tupleObj, row, std::index_sequence_for<Ts...>{});
}

template<typename... Ts>
std::vector<std::tuple<Ts...>> Tob::parseTuples(const std::string& jsonStr) {
    try {
        static_assert((std::is_base_of<Tob, Ts>::value && ...),
            "All types must inherit from Tob");

        Json jsonArray = Json::parse(jsonStr);
        std::vector<std::tuple<Ts...>> results;

        for (const auto& row : jsonArray) {
            if (!row.is_array() || row.size() != sizeof...(Ts))
                continue;

            std::tuple<Ts...> tupleObj;

            assignFromJsonTuple(tupleObj, row); // <== assignation des champs

            results.emplace_back(std::move(tupleObj));
        }

        return results;
    }
    catch (...) {
        return {};
    }
}




template<typename T>
std::string Tob::data(std::vector<T>& objects) {
    try {
        static_assert(std::is_base_of<Tob, T>::value, "T must inherit from Tob");

        Json jsonArray = Json::array();
        Tserial& serializer = instance<T>().serial();

        for (auto& obj : objects) {
            jsonArray.push_back(obj.toJson(serializer));
        }

        return jsonArray.dump(4);
    }
    catch (...) {
        return "[]";
    }
}

template<typename T>
std::vector<T> Tob::parse(const std::string& jsonStr) {
    try {
        static_assert(std::is_base_of<Tob, T>::value, "T must inherit from Tob");

        Json jsonArray = Json::parse(jsonStr);
        std::vector<T> objects;
        Tserial& serializer = instance<T>().serial();

        for (const auto& jsonObj : jsonArray) {
            T obj;
            obj.fromJson(serializer, jsonObj);
            objects.push_back(std::move(obj));
        }

        return objects;
    }
    catch (...) {
        return {};
    }
}

template<typename T>
std::string Tob::data(const std::vector<T*>& objects) {
    try {
        static_assert(std::is_base_of<Tob, T>::value, "T must inherit from Tob");

        Tserial& serializer = objects.at(0)->serial();
        Json jsonArray = Json::array();

        for (const auto& obj : objects) {
            jsonArray.push_back(
                obj->toJson(serializer)
            );
        }

        std::string jsonStr = jsonArray.dump(4);

        return jsonStr;
    }
    catch (const std::exception& e) {
        return "[]";
    }
}

template<typename T>
std::vector<T*> Tob::parse_ptr(const std::string& jsonStr) {
    try {
        static_assert(std::is_base_of<Tob, T>::value, "T must inherit from Tob");

        Json jsonArray = Json::parse(jsonStr);
        std::vector<T*> objects;

        Tserial& serializer = instance<T>().serial();

        for (const auto& jsonObj : jsonArray) {
            T* obj = new T();

            obj->fromJson(serializer, jsonObj);

            objects.push_back(obj);
        }

        return objects;
    }
    catch (const std::exception& e) {
        return {};
    }
}

template<typename T>
T* Tob::ptr() {
    static T* instance = new T();
    return instance;
}

template<typename T>
T& Tob::instance() {
    static T instance; 
    return instance;
}

template<typename T>
T Tob::init() {
    T obj;
    return obj;
}



template <typename T>
to<T>::to(const std::any& v) {
    try {
        value = std::any_cast<T>(v);
    }
    catch (const std::bad_any_cast& e) {
        ///std::cerr << "Erreur de cast : " << e.what() << std::endl;
        throw;
    }
}

template <typename T>
to<T>::operator T() const { return value; }


template <typename T>
std::string str(const T& v) {
    if constexpr (std::is_arithmetic_v<T>) {
        return std::to_string(v);
    }
    else if constexpr (std::is_same_v<T, std::string>) {
        return v;
    }
    else {
        return "[toString not supported for this type]";
    }
}



template<typename T, typename... Args>
std::vector<T> list(Args&&... args) {
    return { std::forward<Args>(args)... };
}

template<typename T, typename... Args>
std::vector<T*> list(Args*... args)
{
    return { args... };
}















/*
class BE {
public:
    std::string ex = "---";

    explicit BE(std::function<void()> back) {
        runBack(back);
    }

    explicit BE(std::function<void(BE& be)> task) {
        task(*this);  // Passe l'objet BE actuel à la fonction lambda
    }

    static void run(std::function<void()> task) {
        std::thread t([task] {
            task();
        });
        t.detach();
    }

    void runBack(std::function<void()> task) {
        // Crée un futur pour savoir quand le thread en arrière-plan se termine
        std::future<void> fut = std::async(std::launch::async, task);
        fut.wait(); // Attend la fin du travail en arrière-plan avant de continuer
    }
};

TEST(TModelerTest, ThreadManager) {
    BE::run([] {
        BE([](BE& be) {
            BE([] {
                std::cout << "Thread ID: " << std::this_thread::get_id() << " - Code 1 en arriere-plan" << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(2)); // Simule un travail
                std::cout << "Thread ID: " << std::this_thread::get_id() << " - Fin code 1" << std::endl;
            });

            be.ex = "Olivia";

            // Ce code (code 2) doit s'exécuter dans le thread principal après "code 1"
            std::cout << "Thread ID: " << std::this_thread::get_id() << " - Code 2 en avant-plan, qui s'execute apres code 1" << std::endl;
        });
    });

    [] (BE<std::string>& be){ // le type de la donné retour est précisé
        be.back = []() {
            //code 1
        };

        ///code 2
        std::string fromBack = be.data; // on peut recuperer une données de l'arrière plan
    };

    // Ce message (code 4) doit s'exécuter après tout
    std::cout << "Thread ID: " << std::this_thread::get_id() << " - Code 4" << std::endl;

    // Donne le temps à l'arrière-plan de finir
    std::this_thread::sleep_for(std::chrono::seconds(5));
}
*/

/*
class BE {
public:
    std::function<void()> _back;
    std::function<void()> _then;

    explicit BE(std::function<void()> t) {
        _back = t;
    }

    void then(std::function<void()> t) {
        _then = t;
        run();
    }

    void run() {
        std::thread t([this] {
            runBack(_back);
        });
        t.detach();

        // ici c'est le Thread principal, et je veux y executer _then uniquement quand _back est terminé. ceci sans bloquer ce Thread
    }

    void runBack(std::function<void()> task) {
        // Crée un futur pour savoir quand le thread en arrière-plan se termine
        std::future<void> fut = std::async(std::launch::async, task);
        fut.wait(); // Attend la fin du travail en arrière-plan avant de continuer
    }
};

BE& bg(std::function<void()> t) {
    return BE (t);
}

TEST(TModelerTest, ThreadManager)
{
    std::cout << "Thread ID: " << std::this_thread::get_id() << " - Code 1" << std::endl;
    bg([] {
        std::cout << "Thread ID: " << std::this_thread::get_id() << " - Code 2" << std::endl;
    }).then([] {
        std::cout << "Thread ID: " << std::this_thread::get_id() << " - Code 3" << std::endl;
    });
    std::cout << "Thread ID: " << std::this_thread::get_id() << " - Code 4" << std::endl;
}
*/

/*
class BE {
public:
    std::function<void()> _back;
    std::function<void()> _then;

    explicit BE(std::function<void()> t) {
        _back = t;
    }

    void then(std::function<void()> t) {
        _then = t;
        run();
    }

    void run() {
        // Lancer _back dans un thread séparé
        std::thread t([this] {
            runBack(_back);  // Exécute _back dans un thread distinct
            });
        t.detach();  // Laisser le thread s'exécuter en arrière-plan sans bloquer le thread principal
    }

    void runBack(std::function<void()> task) {
        // Crée un futur pour savoir quand le thread en arrière-plan se termine
        std::promise<void> prom;
        std::future<void> fut = prom.get_future();

        std::thread taskThread([task, prom = std::move(prom)]() mutable {
            task();  // Exécuter la tâche dans ce thread
            prom.set_value();  // Notifie la fin de la tâche
        });
        taskThread.detach();  // Détacher le thread, pour qu'il s'exécute indépendamment

        // Attendre la fin de la tâche dans le thread principal sans bloquer le thread principal
        fut.wait();  // Attend que la tâche se termine

        // Une fois que _back est terminé, exécuter _then dans le thread principal
        if (_then) {
            _then();
        }
    }
};

BE bg(std::function<void()> t) {
    return BE(t);
}

TEST(TModelerTest, ThreadManager) {
    std::cout << "Thread ID: " << std::this_thread::get_id() << " - Code 1" << std::endl;
    bg([] {
        std::cout << "Thread ID: " << std::this_thread::get_id() << " - Code 2" << std::endl;
        }).then([] {
            std::cout << "Thread ID: " << std::this_thread::get_id() << " - Code 3" << std::endl;
            });
        std::cout << "Thread ID: " << std::this_thread::get_id() << " - Code 4" << std::endl;
}
*/
