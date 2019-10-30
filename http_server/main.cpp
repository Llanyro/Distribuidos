#include "httpserver.h"
#include <iostream>
#include <signal.h>
using namespace std;

httpServer* server = nullptr;

void sigClose(int dummy)
{
    server->closeServer();
    delete server;
    exit(0);
}

int main()
{
    bool temp = true;
    server = new httpServer(8080);
    signal(SIGINT, sigClose);

    if(server->getServer())
    {
        temp = true;
        cout << "Connected to DB" << endl;
    }
    else
        cout << "Not connected to DB" << endl << "El servido se apagará, comprueba los valores de la DB y si esta esta encendida." << endl;


    while(temp)
    {
        int newfd = server->waitForConnections();
        server->resolveRequests(newfd);
    }
}
