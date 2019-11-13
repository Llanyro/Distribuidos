#pragma once
#include "../../sockets/Servicio.h"
#include "../../sockets/Singleton.h"

#define SERVICIOEJECUCION ServicioEjecucion::getInstancia()

class ServicioEjecucion : public Servicio, public Singleton <ServicioEjecucion>
{
protected:
	ServicioEjecucion();
	~ServicioEjecucion();
	friend class Singleton <ServicioEjecucion>;
public:
	virtual void resolverSolicitud(const int& newsock_fd) const override;
	

};
