/*
 * Copyright (c) 2026 TAMBO ai and others
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 */

#pragma once

#include <any>
#include <string>
#include <chrono>
#include <vector>

#include "field/TField.hpp"
#include "field/TTextField.hpp"

namespace ai::tambo::tmodeler {

class TimeField : public TextFieldBase<TimeField> {
private:
    std::string _format = DEFAULT_TIME_FORMAT;

public:
    TimeField() : TextFieldBase<TimeField>() {}

    std::string toString() const {
        return TextFieldBase::get();
    }

    std::chrono::system_clock::time_point time() const {
        return str2time(TextFieldBase::get());
    }

    void set(std::chrono::system_clock::time_point v) {
        TextFieldBase::set(time2str(v, _format));
    }

    void set(std::string v) {
        set(str2time(v));
    }

    virtual TimeField& autoUpdate(TF v) {
        //set(v);
        return *this;
    }

    virtual TimeField& defaults(TF v) {
        //set(v);
        return *this;
    }

    virtual TimeField& defaults(std::chrono::system_clock::time_point v) {
        set(v);
        return *this;
    }

    virtual TimeField& defaults(std::string v) {
        set(v);
        return *this;
    }

    virtual TimeField& format(TF v) {
        switch (v)
        {
            case TF::DATE: _format = "%Y-%m-%d";
                break;
            case TF::DATE_TIME: _format = "%Y-%m-%dT%H:%M:%SZ";
                break;
            default: ;
        }
        return *this;
    }

    // time operations
    TimeField& addHours(int hours) {
        set( time() + std::chrono::hours(hours));
        return *this;
    }
    TimeField& subHours(int hours) {
        set(time() - std::chrono::hours(hours));
        return *this;
    }

    // Operators
    std::chrono::duration<double> operator-(const TimeField& other) const {
        return time() - other.time();
    }
    Tx operator-() {
        return TField<TimeField, std::string>::operator-();
    }
    TimeField& operator=(const std::string& v) {
        set(v);
        return static_cast<TimeField&>(*this);
    }
    TimeField& operator=(std::string&& v) {
        set(v);
        return static_cast<TimeField&>(*this);
    }
    TimeField& operator=(const std::chrono::system_clock::time_point& v) {
        set(v);
        return static_cast<TimeField&>(*this);
    }
    TimeField& operator=(std::chrono::system_clock::time_point&& v) {
        set(v);
        return static_cast<TimeField&>(*this);
    }

    // getters
    int getYear() const {
        std::time_t time_t_value = std::chrono::system_clock::to_time_t(time());
        std::tm* tm_ptr = std::localtime(&time_t_value);
        return tm_ptr->tm_year + 1900;  // tm_year est l'année depuis 1900
    }
    int getMonth() const {
        std::time_t time_t_value = std::chrono::system_clock::to_time_t(time());
        std::tm* tm_ptr = std::localtime(&time_t_value);
        return tm_ptr->tm_mon + 1;  // tm_mon commence à partir de 0 (janvier = 0)
    }
    int getDay() const {
        std::time_t time_t_value = std::chrono::system_clock::to_time_t(time());
        std::tm* tm_ptr = std::localtime(&time_t_value);
        return tm_ptr->tm_mday;
    }
    int getHour() const {
        std::time_t time_t_value = std::chrono::system_clock::to_time_t(time());
        std::tm* tm_ptr = std::localtime(&time_t_value);
        return tm_ptr->tm_hour;
    }
    int getMinute() const {
        std::time_t time_t_value = std::chrono::system_clock::to_time_t(time());
        std::tm* tm_ptr = std::localtime(&time_t_value);
        return tm_ptr->tm_min;
    }
    int getSecond() const {
        std::time_t time_t_value = std::chrono::system_clock::to_time_t(time());
        std::tm* tm_ptr = std::localtime(&time_t_value);
        return tm_ptr->tm_sec;
    }

    // static
    static double time2sec(std::chrono::duration<double> v) {
        return std::chrono::duration_cast<std::chrono::seconds>(v).count();
    }
    static std::chrono::system_clock::time_point now() {
        return std::chrono::system_clock::now();
    }
};

} // namespace ai::tambo::tmodeler
