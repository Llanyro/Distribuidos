#pragma once
#include "Servicio.h"
#include "Singleton.h"

#define ECHO_SERVICE MsgService::getInstancia()

class MsgService : public Servicio, public Singleton<MsgService>
{
	private:
		#pragma region Constructores
		friend class Singleton<MsgService>;
		MsgService();
		~MsgService();
		#pragma endregion
};
