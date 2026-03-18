#include "core/Tob.hpp"

#include <iostream>

#include <nlohmann/json.hpp>

Json Tob::toJson(Tserial& serializer)
{
    return toJson(serializer, this->clazz().getFields());
}

Json Tob::toJson(Tserial& serializer, AnyList fields)
{
    Json jsonObj = Json::object();

    try {
        const std::type_info& type = typeid(serializer);
        std::string nam = type.name();

        for (auto& [key, field] : fields) {
            try {
                serializer.serialize(jsonObj, key.c_str(), field);
            }
            catch (...) {}
        }
    }catch (...){}

    return jsonObj;
}

void Tob::fromJson(Tserial& serializer, Json jsonObj)
{
    fromJson(serializer, jsonObj, this->clazz().getFields());
}

void Tob::fromJson(Tserial& serializer, Json jsonObj, AnyList fields)
{
    try {
        int k = -1;
        for (auto& [key, field] : fields) {
            try {
                k++;
                if (jsonObj.contains(key)) {
                    serializer.deserialize(jsonObj, key.c_str(), field);
                }
            }
            catch (...) {}
        }
    }catch (...){}
}


std::string Tob::data() {
    try {
        Json jsonObj;

        Tserial& serializer = this->serial();
        auto fields = this->clazz().getFields();

        std::string jsonStr = toJson(serializer, fields).dump(4);

        return jsonStr;
    }
    catch (const std::exception& e) {
        return "{}";
    }
}

void Tob::parse(const std::string& jsonStr) {
    try {
        Json jsonObj = Json::parse(jsonStr);

        Tserial& serializer = serial();
        auto fields = this->clazz().getFields();

        fromJson(serializer, jsonObj, fields);
    }
    catch (...) {}
}

bool Tob::equals()
{
    return false;
}
std::string Tob::toString()
{
    return clazz().name();
}



int Log::level = 1;  // Valeur par défaut

void Log::r(const std::string& i) {
#ifdef RELEASE
    std::cout << "___RELEASE__> " << i << std::endl;
#endif
}

void Log::d(const std::string& i) {
    //#ifdef DEBUG
    std::cout << "> " << i << std::endl;
    //#endif
}

void Log::i(const int i, const std::string& s) {
    if (i >= level) {
        std::cout << "i> " << s << std::endl;
    }
}




// Convertit une chaîne en `std::chrono::system_clock::time_point`
std::chrono::system_clock::time_point str2time(const std::string& dateStr) {
    std::tm tm = {};
    std::istringstream ss;

    for (const auto& fmt : time_formats) {
        ss.clear();
        ss.str(dateStr);
        ss >> std::get_time(&tm, fmt.c_str());
        if (!ss.fail()) {
            return std::chrono::system_clock::from_time_t(std::mktime(&tm));
        }
    }

    throw std::invalid_argument("Format de date non reconnu: " + dateStr);
}

// Convertit un `std::chrono::system_clock::time_point` en chaîne de caractères
std::string time2str(const std::chrono::system_clock::time_point& tp, const std::string& format) {
    std::time_t time = std::chrono::system_clock::to_time_t(tp);
    std::tm tm = *std::localtime(&time);

    std::ostringstream ss;
    ss << std::put_time(&tm, format.c_str());
    return ss.str();
}
