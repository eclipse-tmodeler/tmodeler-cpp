#ifndef TOBSERVER_H
#define TOBSERVER_H

#include "Tserial.h"
#include "Tclass.h"

/*
 *Tob reflexion
 */
class Treflexion {
public:
    virtual ~Treflexion() = default;
    Treflexion() {}

    virtual Tserial* serial_() = 0;
    virtual Tserial& serial() = 0;
    virtual Tclass& clazz() = 0;
};

#endif // TOBSERVER_H
