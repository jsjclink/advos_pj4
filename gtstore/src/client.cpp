#include "gtstore.hpp"
#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include "keyvaluestore.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;
using keyvaluestore::Key;
using keyvaluestore::KeyValue;

void GTStoreClient::init(int id) {

		cout << "Inside GTStoreClient::init() for client " << id << "\n";
		client_id = id;
		auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
		auto stub_ = KeyValueService::NewStub(channel);
}

val_t GTStoreClient::get(string key) {

		cout << "Inside GTStoreClient::get() for client: " << client_id << " key: " << key << "\n";
		val_t value;
		// Get the value!
		Key gkey;
		gkey.key = key;
		auto response = this->stub_.get(gkey);
		return value;
}

bool GTStoreClient::put(string key, val_t value) {

		string print_value = "";
		for (uint i = 0; i < value.size(); i++) {
				print_value += value[i] + " ";
		}
		cout << "Inside GTStoreClient::put() for client: " << client_id << " key: " << key << " value: " << print_value << "\n";
		// Put the value!

		KeyValue pkv;
		pkv.key = key;
		pkv.value = value;
		this->stub_.put(pkv);
		return true;
}

void GTStoreClient::finalize() {

		cout << "Inside GTStoreClient::finalize() for client " << client_id << "\n";
}
