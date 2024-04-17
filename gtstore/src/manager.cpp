#include "gtstore.hpp"
#include <grpc/grpc.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

using grpc::Server;
using grpc::ServerBuilder;

void GTStoreManager::init() {
	std::string server_address("0.0.0.0:50051");
	cout << "Inside GTStoreManager::init()\n";
	ManagerImpl service();
	ServerBuilder builder;
  	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  	builder.RegisterService(&service);
  	std::unique_ptr<Server> server(builder.BuildAndStart());
  	std::cout << "Server listening on " << server_address << std::endl;
  	server->Wait();
}


int main(int argc, char **argv) {

	GTStoreManager manager;
	manager.init();
	
}
