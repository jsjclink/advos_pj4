#include "gtstore.hpp"
#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>

#include <mutex>
#include <vector>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::Status;
using grpc::ServerContext;

using keyvaluestore::KeyValueService;
using keyvaluestore::Key;
using keyvaluestore::Value;
using keyvaluestore::KeyValue;
using keyvaluestore::Void;
using keyvaluestore::Port;

using grpc::Channel;
using grpc::ClientContext;

std::vector<int64_t> node_vector;
std::mutex node_vector_mtx;

class KeyValueServiceStorageClient {
	public:
	KeyValueServiceStorageClient(std::shared_ptr<Channel> channel) : stub_(KeyValueService::NewStub(channel)) {

	}

	void check_alive() {
		ClientContext context;
		Void request, response;

		Status status = stub_->check_alive(&context, request, &response);

		if(status.ok()) {
			cout << "check_alive OK\n";
		} else {
			cout << "check_alive Not OK\n";
		}
	}

	private:
	std::unique_ptr<KeyValueService::Stub> stub_;
};

class KeyValueServiceManagerImpl final : public KeyValueService::Service {
	public:
	KeyValueServiceManagerImpl() {

	}

	Status get(ServerContext* context, const Key* key, Value* value) override {
		return Status::OK;
	}

	Status put(ServerContext* context, const KeyValue* keyvalue, Void* response) override {
		return Status::OK;
	}

	Status str_cnt(ServerContext* context, const Port* port, Void* response) override {
		cout << "str_cnt is called by storage node. port num: " << port->port() << "\n";
		cout << "connect to storage node. port num: " << port->port() << "\n";

		// connect to storage node
		std::string server_address = "0.0.0.0:" + std::to_string(port->port());
		cout << server_address << "\n";
		KeyValueServiceStorageClient client(grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials()));

		// call check alive for test
		client.check_alive();

		std::lock_guard<std::mutex> lock(node_vector_mtx);
    	node_vector.push_back(port->port());

		return Status::OK;
	}
};

void GTStoreManager::init() {
	cout << "Inside GTStoreManager::init()\n";
	std::string server_address("0.0.0.0:50051");
	KeyValueServiceManagerImpl service;

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
