#include "Server.hpp"

int main()
{
 	Nectere::Server server(4242);
    server.LoadApplications();
 	server.Start();
	return 0;
}
