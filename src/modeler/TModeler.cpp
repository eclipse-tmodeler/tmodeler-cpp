#include "modeler/TModeler.h"

#include <iomanip>
#include <mutex>
#include <shared_mutex>
#include <thread>

#include "core/Tob.hpp"
#include "db/Tsqlite.h"
#include "modeler/TM.h"

static std::string deadline = "2025-09-01";
static bool isValidUntil(const std::string& deadline) {
    // Convertir la date limite de format "YYYY-MM-DD" en struct tm
    std::tm deadline_tm = {};
    std::istringstream ss(deadline);
    ss >> std::get_time(&deadline_tm, "%Y-%m-%d");
    if (ss.fail()) {
        std::cerr << "Invalid date format." << std::endl;
        return false;
    }

    // Obtenir la date et l'heure actuelles
    std::time_t current_time = std::time(nullptr);
    std::tm* current_tm = std::localtime(&current_time);

    // Comparer les dates
    if (std::difftime(std::mktime(current_tm), std::mktime(&deadline_tm)) < 0) {
        return true; // Valide si avant la date limite
    }

    // Si la date limite est dépassée, afficher le message d'erreur
    std::cerr << "TModeler must be upgraded. The current version has expired." << std::endl;
    return false; // Expiré après la date limite
}


std::unique_ptr<TModeler> TModeler::_instance = nullptr;

TModeler::TModeler(){}

// Singleton Method
TModeler& TModeler::getInstance() {
    if (!_instance) {
        _instance = std::make_unique<TModeler>();
    }
    return *_instance;
}


TModeler& TModeler::start()
{
    // init with some communs params

    return TModeler::getInstance();
}

// Factory Method
TM::Builder TModeler::initialize(TM::DbType type, const std::string& dbDir, const std::string& dbName) {
    if (!isValidUntil(deadline)) { return TM::Builder(); }

    return TM::Builder()
        .type(type)
        .dbDir(dbDir)
        .dbName(dbName);
}



TModeler& TModeler::dbReady(Callback cb) {
    _callback = cb;
    return *this;
}

TModeler& TModeler::init(std::unique_ptr<Tdb> db) {
    if (_callback) {
        std::thread([this, db=std::move(db)]() mutable {
            try {
                start(std::move(db));
                _callback();
            }
            catch (const std::exception& e) {
                Log::d(std::string("Exception in async init: ") + e.what());
            }
        }).detach();
    }
    else {
        try {
            start(std::move(db));
        }
        catch (const std::exception& e) {
            Log::d(std::string("Exception in sync init: ") + e.what());
        }
    }

    return *this;
}

TModeler& TModeler::start(std::unique_ptr<Tdb> db) {
    if (!isValidUntil(deadline)) { return *this; }

    if (!db->open())
    {
        return *this;
    }

    std::unique_lock lock(_mutex); 
    std::string id = db->_dbName;
    _dbs[id] = std::move(db);
    return *this;
}

Tdb& TModeler::geTdb(const std::string& id, bool rw) {
    if (id.empty())
    {
        if (_dbs.size()==0){
            throw std::runtime_error("Tdb not found: " + id);
        }
        if (rw) {
            std::unique_lock lock(_mutex);
            try
            {
                return *(_dbs.begin()->second);
            }catch (...){
                throw std::runtime_error("Tdb not found: " + id);
            }
        }
        else {
            std::shared_lock lock(_mutex);
            try
            {
                return *(_dbs.begin()->second);
            }
            catch (...) {
                throw std::runtime_error("Tdb not found: " + id);
            }
        }
    }
    if (rw) {
        std::unique_lock lock(_mutex);  
        auto it = _dbs.find(id);
        if (it == _dbs.end()) {
            throw std::runtime_error("Tdb not found: " + id);
        }
        return *(it->second);
    }
    else {
        std::shared_lock lock(_mutex); 
        auto it = _dbs.find(id);
        if (it == _dbs.end()) {
            throw std::runtime_error("Tdb not found: " + id);
        }
        return *(it->second);
    }
}

/*
TModeler& TModeler::init(std::unique_ptr<Tdb> db) {
    std::string id = db->id();
    _dbs[id] = std::move(db);  // transfert de propriété
    return *this;
}*/

/*
Tdb& TModeler::geTdb(std::string id)
{
    auto it = _dbs.find(id);
    if (it == _dbs.end()) {
        throw std::runtime_error("Tdb not found: " + id);
    }
    return it->second;
}*/
