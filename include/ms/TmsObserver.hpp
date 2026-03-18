#pragma once

#include "Tms.h"

#include "ms/TmsDeep.hpp"
#include "field/TField.h"



template <typename... Ts>
void Tms<Ts...>::addObserver(const std::shared_ptr<TModelObserver>& observer) {
    prepare();
    TmsDeep::addObserver(_modelClass, observer);
}
template <typename... Ts>
void Tms<Ts...>::removeObserver(const std::shared_ptr<TModelObserver>& observer) {
    prepare();
    TmsDeep::removeObserver(_modelClass, observer);
}
template <typename... Ts>
void Tms<Ts...>::onModelChange(Callback c)
{
    prepare();
    _model_callback->addOnChangeCallback(c);
}
template <typename... Ts>
void Tms<Ts...>::onSave(Callback c)
{
    prepare();
    _model_callback->addOnSaveCallback(c);
}
template <typename... Ts>
void Tms<Ts...>::onCreate(Callback c)
{
    prepare();
    _model_callback->addOnCreateCallback(c);
}
template <typename... Ts>
void Tms<Ts...>::onUpdate(Callback c)
{
    prepare();
    _model_callback->addOnUpdateCallback(c);
}
template <typename... Ts>
void Tms<Ts...>::onDelete(Callback c)
{
    prepare();
    _model_callback->addOnDeleteCallback(c);
}



template <typename... Ts>
void Tms<Ts...>::initCallbacks()
{
    _model_callback = std::make_shared<TModelCallback>();
    addObserver(_model_callback);
}
