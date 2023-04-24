#pragma once

#include "CrTypes.h"

#include "SDL.h"
#include "SDL_net.h"

class CrNetwork
{
public:
	CrNetwork();
	~CrNetwork();

	bool bIsServer = false;

	//Attempts to connect to a server
	bool Connect(String IPAndPort);

	//Attempts to host a server
	bool Host(uint16_t Port);

	//Sends queued packet(s)
	void Flush();

	//If this is a client, it will only have a size of 1.
	Array<UDPsocket> Connections;
};

