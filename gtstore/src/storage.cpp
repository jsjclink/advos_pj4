#include "gtstore.hpp"
#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include "keyvaluestore.grpc.pb.h"

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


int store_num = 0;

Port MakePort(int64_t port) {
	Port p;
	p.set_port(port);
	return p;
}

class KeyValueServiceStorageImpl final : public KeyValueService::Service {
	public:
	KeyValueServiceStorageImpl() {

	}

	Status get(ServerContext* context, const Key* key, Value* value) override {
		return Status::OK;
	}

	Status put(ServerContext* context, const KeyValue* keyvalue, Void* response) override {
		return Status::OK;
	}

	Status check_alive(ServerContext* context, const Void* request, Void* response) {
		return Status::OK;
	}
};

class KeyValueServiceManagerClient {
	public: 
	KeyValueServiceManagerClient(std::shared_ptr<Channel> channel) : stub_(KeyValueService::NewStub(channel)) {

	}
	void str_cnt(int64_t port_int) {
		ClientContext context;
		Port port = MakePort(port_int);
		Void response;

		Status status = stub_->str_cnt(&context, port, &response);

		if(status.ok()) {
			cout << "OK\n";
		} else {
			cout << "Not OK\n";
		}
	}

	private:
	std::unique_ptr<KeyValueService::Stub> stub_;
};


void GTStoreStorage::init() {
	cout << "Inside GTStoreStorage::init()\n";
	//TODO: change server port to go to a unique port everytime it is called
	std::string server_address("0.0.0.0:50052");
	KeyValueServiceStorageImpl service;

	ServerBuilder builder;
  	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  	builder.RegisterService(&service);
  	std::unique_ptr<Server> server(builder.BuildAndStart());
  	std::cout << "Server listening on " << server_address << std::endl;
  	server->Wait();
}

int main(int argc, char **argv) {
	KeyValueServiceManagerClient client(grpc::CreateChannel("0.0.0.0:50051", grpc::InsecureChannelCredentials()));
	client.str_cnt(50052);

	GTStoreStorage storage;
	storage.init();
}
