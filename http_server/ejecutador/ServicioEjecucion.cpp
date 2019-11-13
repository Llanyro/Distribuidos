#include "ServicioEjecucion.h"
#include "../../sockets/String.h"
#include "../../sockets/File.h"
#include "../../sockets/List.h"

ServicioEjecucion::ServicioEjecucion(){}

ServicioEjecucion::~ServicioEjecucion() {}

void ServicioEjecucion::resolverSolicitud(const int & newsock_fd) const
{
	String peticion = FILE_SINGLETON->leerSocket(newsock_fd, '\0');
	List<String> list = peticion.stringCat(' ');
	String resultado;
	if (peticion.similar("SuMa")) {
		//resultado = funcion que ejecuta en terminal el codigo bash
	}
	else if (peticion.similar("LeeR")) {
		//resultado = funcion que ejecuta en terminal el codigo bash
	}
	else {
		resultado = "Invalid request";
	}

	send(newsock_fd, &resultado[0], resultado.getCount(), 0);

	#ifdef _WIN32
	closesocket(newsock_fd);
	#elif __unix__
	close(newsock_fd);
	#endif

}



