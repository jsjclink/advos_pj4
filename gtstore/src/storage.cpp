#include "gtstore.hpp"
#include <grpc/grpc.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

using grpc::Server;
using grpc::ServerBuilder;
int store_num = 0;

class StorageImpl final : public KeyValueService::Service {
	
}

void GTStoreStorage::init() {
	//TODO: change server port to go to a unique port everytime it is called
	std::string server_address("0.0.0.0:50052");
	cout << "Inside GTStoreStorage::init()\n";
	ServerBuilder builder;
  	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  	builder.RegisterService(this);
  	std::unique_ptr<Server> server(builder.BuildAndStart());
  	std::cout << "Server listening on " << server_address << std::endl;
  	server->Wait();
}

int main(int argc, char **argv) {

	GTStoreStorage storage;
	storage.init();
	
}
