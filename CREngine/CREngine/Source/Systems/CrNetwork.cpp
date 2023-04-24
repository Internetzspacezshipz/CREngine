#include "CrNetwork.h"

#include <CrLogging.h>

CrNetwork::CrNetwork()
{
	if (SDLNet_Init() == -1)
	{
		CrLOG("failed to init networking, reason: {}", SDLNet_GetError());
	}
}

CrNetwork::~CrNetwork()
{
	SDLNet_Quit();
}

bool CrNetwork::Connect(String IPAndPort)
{
	IPaddress IP;

	auto Delim = IPAndPort.find_last_of(':');
	String IPStr = IPAndPort.substr(0, Delim);
	String PortStr = IPAndPort.substr(Delim + 1);
	auto PortInt = atoi(PortStr.c_str());

	CrLOG("Attempting connection to {} with port {} ({})", IPStr, PortStr, PortInt);

	if (SDLNet_ResolveHost(&IP, IPStr.c_str(), PortInt) == -1)
	{
		CrLOG("failed to connect, reason: {}", SDLNet_GetError());
		return false;
	}

	return true;
}

bool CrNetwork::Host(uint16_t Port)
{
	return false;
}

void CrNetwork::Flush()
{
}
