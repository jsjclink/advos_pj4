#include "gtstore.hpp"
#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;
using keyvaluestore::Key;
using keyvaluestore::KeyValue;
using keyvaluestore::Value;
using keyvaluestore::Port;
using keyvaluestore::Void;


unordered_map<string, val_t> ClientMap;


Key MakeKey(string key) {
  Key k;
  k.set_key(key);
  return k;
}


KeyValue MakeKeyValue(string key, val_t val){
	KeyValue kv;
	kv.set_key(key);
	for(string v_temp: val){
		kv.add_values(v_temp);
	}
	return kv;
}


void getHashMap(const keyvaluestore::Map& message) {
    for (const auto& entry : message.entries()) {
        ClientMap[entry.key()] = entry.values();
    }
    return;
}

string ParsePort(string msg){
	std::string substring;
    for (char ch : msg) {
        if (ch != ',') substring += ch;
        else break;
    }
    return substring;
}

val_t ConnStrGetValue(string port, string key){
	val_t value;
	Key gkey = MakeKey(key);
	grpc::ClientContext context;
	Value response;
	auto channel = grpc::CreateChannel("localhost:"+port, grpc::InsecureChannelCredentials());
	unique_ptr<KeyValueService::Stub> stub_ = KeyValueService::NewStub(channel);
	Status stat = stub_->get(&context,gkey,&response);
	if(stat.ok()){
		value[0] = response.values();
		return value;
	}
	else{
		return {};
	}
}
bool ConnStrPutValue(string port,string key,val_t value){
	KeyValue gkey = MakeKeyValue(key,value);
	grpc::ClientContext context;
	Void vd;
	auto channel = grpc::CreateChannel("localhost:"+port, grpc::InsecureChannelCredentials());
	unique_ptr<KeyValueService::Stub> stub_ = KeyValueService::NewStub(channel);
	Status stat = stub_->put(&context,gkey,&vd);
	if(stat.ok()){
		return true;
	}
	else{
		return false;
	}
}
void GTStoreClient::init(int id) {

		cout << "Inside GTStoreClient::init() for client " << id << "\n";
		client_id = id;
		auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
		this->stub_ = KeyValueService::NewStub(channel);
		grpc::ClientContext context;
		keyvaluestore::Map response;
		keyvaluestore::Void vd;
		Status status = this->stub_->get_map(&context, vd, &response);
		if (status.ok()) {
			std::cout << "Response received" << std::endl;
			getHashMap(response);
		} else {
			std::cerr << "RPC failed: " << status.error_message() << std::endl;
		}
		return;
}

val_t GTStoreClient::get(string key) {

		cout << "Inside GTStoreClient::get() for client: " << client_id << " key: " << key << "\n";
		val_t value;
		string port;
		grpc::ClientContext context;
		// Get the value!
		if (ClientMap.find(key) != ClientMap.end()) {
			//key inside map! Connect to Storage Node
			port = ParsePort(ClientMap[key]);
			value = ConnStrGetValue(port,key);
    	}
		else{
			//key not inside map, contact Manager.
			Key gkey = MakeKey(key);
			Port response;
			Status stat = this->stub_->get_snn(&context,gkey,&response);
			if(stat.ok()){
				std::cout << "Response received" << std::endl;
				ClientMap[key] = response.port();
				port = to_string(response.port());
				value = ConnStrGetValue(port,key);
			}
			else{
				std::cerr << "RPC failed: " << stat.error_message() << std::endl;
				return {};
			}
		}
		return value;
}

bool GTStoreClient::put(string key, val_t value) {

		string print_value = "";
		for (uint i = 0; i < value.size(); i++) {
				print_value += value[i] + " ";
		}
		cout << "Inside GTStoreClient::put() for client: " << client_id << " key: " << key << " value: " << print_value << "\n";
		// Put the value!

		Port response;
		string port;
		Key gkey = MakeKey(key);
		grpc::ClientContext context;
		Status stat = this->stub_->get_snn(&context,gkey,&response);
		if(stat.ok()){
			std::cout << "Response received" << std::endl;
			ClientMap[key] = response.port();
			port = to_string(response.port());
			return ConnStrPutValue(port,key,value);
		}
		else{
			std::cerr << "RPC failed: " << stat.error_message() << std::endl;
			return false;
		}
}

void GTStoreClient::finalize() {
		cout << "Inside GTStoreClient::finalize() for client " << client_id << "\n";
}
