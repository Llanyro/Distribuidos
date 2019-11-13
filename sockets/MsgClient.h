#pragma once
#include "Cliente.h"
#include "Singleton.h"

#define ECHO_CLIENT MsgClient::getInstancia()

class MsgClient : public Cliente, public Singleton<MsgClient>
{
	private:
		#pragma region Constructores
		friend class Singleton<MsgClient>;
		MsgClient();
		~MsgClient();
		#pragma endregion
};
