#pragma once

template<typename T>
class TMlistener {
public:
    virtual ~TMlistener() = default;

    virtual void onChanged(const T& value) = 0;

    virtual void onDeleted(const T& value) {}
};
