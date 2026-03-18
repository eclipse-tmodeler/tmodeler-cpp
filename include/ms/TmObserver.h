#pragma once

#include "core/Tclass.h"
#include "field/TFields.hpp"
#include "field/TBaseField.hpp"
#include "db/Tdb.h"
#include "db/Ttable.h"
#include <memory>
#include <string>
#include <vector>

#include "modeler/TModeler.h"
#include "ms/TFilterBuilder.h"

using Callback = std::function<void(KeysType& keys)>;

class TModelCallback : public TModelObserver
{
private:
    std::vector<Callback> _onChangeCallbacks;
    std::vector<Callback> _onSaveCallbacks;
    std::vector<Callback> _onCreateCallbacks;
    std::vector<Callback> _onUpdateCallbacks;
    std::vector<Callback> _onDeleteCallbacks;

public:
    TModelCallback() = default;


    void addOnChangeCallback(Callback cb) { _onChangeCallbacks.push_back(std::move(cb)); }
    void addOnSaveCallback(Callback cb) { _onSaveCallbacks.push_back(std::move(cb)); }
    void addOnCreateCallback(Callback cb) { _onCreateCallbacks.push_back(std::move(cb)); }
    void addOnUpdateCallback(Callback cb) { _onUpdateCallbacks.push_back(std::move(cb)); }
    void addOnDeleteCallback(Callback cb) { _onDeleteCallbacks.push_back(std::move(cb)); }


    void onModelChange(KeysType& keys) override {
        for (const auto& cb : _onChangeCallbacks) cb(keys);
    }

    void onSave(KeysType& keys) override {
        for (const auto& cb : _onSaveCallbacks) cb(keys);
    }

    void onCreate(KeysType& keys) override {
        for (const auto& cb : _onCreateCallbacks) cb(keys);
    }

    void onUpdate(KeysType& keys) override {
        for (const auto& cb : _onUpdateCallbacks) cb(keys);
    }

    void onDelete(KeysType& keys) override {
        for (const auto& cb : _onDeleteCallbacks) cb(keys);
    }
};
