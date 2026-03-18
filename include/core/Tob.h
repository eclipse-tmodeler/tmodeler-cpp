#pragma once

#include <iostream>
#include <string>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <vector>
#include <string>
#include <unordered_map>

#include "Tclass.h"
#include "Treflexion.h"
#include "Tserial.h"

#define DEFAULT_TIME_FORMAT "%Y-%m-%dT%H:%M:%SZ" 

class Tob : public Treflexion{
protected:
    Tserial _serializer;
private:
    Tclass _class = Tclass(*this, nullptr);

    Json toJson(Tserial& serializer);
    Json toJson(Tserial& serializer, AnyList fields);
    void fromJson(Tserial& serializer, Json jsonObj, AnyList fields);

public:
    void fromJson(Tserial& serializer, Json jsonObj);

    virtual Tserial* serial_() {
        _serializer = instance<Tserial>();
        return &_serializer;
    }

    virtual Tserial& serial() {
        _serializer = instance<Tserial>();
        return _serializer;
    }

    virtual Tclass& clazz() {
        _class.fields({
            });
        return _class;
    };

    virtual Tclass* clazz2() {
        _class.fields({
            });
        return &_class;
    };

    Tclass* _class3;
    virtual Tclass* clazz3() {
        _class3 = new Tclass(*this, nullptr);
        _class3->fields({
            });
        return _class3;
    };

    template <typename T>
    static Tclass& clazz() {
        return instance<T>().clazz();
    };

    void parse(const std::string& jsonStr);
    std::string data();



    template<typename T>
    static std::string data(std::vector<T>& objects);
    template<typename T>
    static std::vector<T> parse(const std::string& jsonStr);
    //
    template<typename... Ts>
    static std::vector<std::tuple<Ts...>> parseTuples(const std::string& jsonStr);

    template<typename T>
    static std::string data(const std::vector<T*>& objects);
    template<typename T>
    static std::vector<T*> parse_ptr(const std::string& jsonStr);

    virtual bool equals();
    virtual std::string toString();

    template<typename T>
    static T* ptr();

    template<typename T>
    static T& instance();

    template<typename T>
    static T init();
};

class Log {
public:
    static int level;

    static void r(const std::string& i);

    static void d(const std::string& i);

    static void i(const int i, const std::string& s);
};

template <typename T>
struct to {
    T value;

    explicit to(const std::any& v);

    operator T() const;
};


template <typename T>
std::string str(const T& v);


template<typename T, typename... Args>
std::vector<T> list(Args&&... args);

template<typename T, typename... Args>
std::vector<T*> list(Args*... args);




// Liste des formats possibles
const std::vector<std::string> time_formats = {
    "%Y-%m-%d %H:%M:%S",   // 2025-04-02 14:30:00
    "%Y-%m-%d",            // 2025-04-02
    "%d/%m/%Y %H:%M:%S",   // 02/04/2025 14:30:00
    "%d-%m-%Y",            // 02-04-2025
    "%m/%d/%Y %I:%M %p",   // 04/02/2025 02:30 PM
    "%H:%M:%S",            // 14:30:00
    "%H:%M",               // 14:30
    "%I:%M %p",            // 02:30 PM
    "%Y-%m-%dT%H:%M:%SZ"   // 2025-04-02T14:30:00Z (ISO 8601 sans fuseau)
};

// Convertit une chaîne en `std::chrono::system_clock::time_point`
std::chrono::system_clock::time_point str2time(const std::string& dateStr);

// Convertit un `std::chrono::system_clock::time_point` en chaîne de caractères
std::string time2str(const std::chrono::system_clock::time_point& tp, const std::string& format = DEFAULT_TIME_FORMAT);
