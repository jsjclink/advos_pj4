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
using keyvaluestore::Put_Ports;


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


// string ParsePort(string msg){
// 	std::string substring;
//     for (char ch : msg) {
//         if (ch != ',') substring += ch;
//         else break;
//     }
//     return substring;
// }

val_t ConnStrGetValue(string port, string key){
	val_t value;
	Key gkey = MakeKey(key);
	grpc::ClientContext context;
	Value response;
	auto channel = grpc::CreateChannel("localhost:"+port, grpc::InsecureChannelCredentials());
	unique_ptr<KeyValueService::Stub> stub_ = KeyValueService::NewStub(channel);
	Status stat = stub_->get(&context,gkey,&response);
	if(stat.ok()){
		for(auto msg : response.values()){
			value.push_back(msg);
		}
		return value;
	}
	else{
		return {};
	}
}
bool ConnStrPutValue(val_t ports,string key,val_t value){
	KeyValue gkey = MakeKeyValue(key,value);
	grpc::ClientContext context;
	Void vd;
	for(string port : ports){
		auto channel = grpc::CreateChannel("localhost:"+port, grpc::InsecureChannelCredentials());
		unique_ptr<KeyValueService::Stub> stub_ = KeyValueService::NewStub(channel);
		Status stat = stub_->put(&context,gkey,&vd);
		if(!stat.ok()){
			return false;
		}
	}
	return true;
}
void GTStoreClient::init(int id) {

		cout << "Inside GTStoreClient::init() for client " << id << "\n";
		client_id = id;
		auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
		this->stub_ = KeyValueService::NewStub(channel);
		// grpc::ClientContext context;
		// keyvaluestore::Map response;
		// keyvaluestore::Void vd;
		// Status status = this->stub_->get_map(&context, vd, &response);
		// if (status.ok()) {
		// 	std::cout << "Response received" << std::endl;
		// 	getHashMap(response);
		// } else {
		// 	std::cerr << "RPC failed: " << status.error_message() << std::endl;
		// }
		return;
}

val_t GTStoreClient::get(string key) {

		cout << "Inside GTStoreClient::get() for client: " << client_id << " key: " << key << "\n";
		val_t value,ports;
		string port;
		grpc::ClientContext context;
		// Get the value!
		if (ClientMap.find(key) != ClientMap.end()) {
			//key inside map! Connect to Storage Node
			port = ClientMap[key].front();
			value = ConnStrGetValue(port,key);
    	}
		else{
			//key not inside map, contact Manager.
			Key gkey = MakeKey(key);
			Port response;
			Status stat = this->stub_->get_snn(&context,gkey,&response);
			if(stat.ok()){
				std::cout << "Response received" << std::endl;
				ports.push_back(response.port());
				ClientMap[key] = ports;
				port = response.port();
				value = ConnStrGetValue(port,key);
				return value;
			}
			else{
				std::cerr << "RPC failed: " << stat.error_message() << std::endl;
				return {};
			}
		}
}

bool GTStoreClient::put(string key, val_t value) {

		string print_value = "";
		for (uint i = 0; i < value.size(); i++) {
				print_value += value[i] + " ";
		}
		cout << "Inside GTStoreClient::put() for client: " << client_id << " key: " << key << " value: " << print_value << "\n";
		// Put the value!

		Put_Ports response;
		Key gkey = MakeKey(key);
		grpc::ClientContext context;
		val_t ports;
		Status stat = this->stub_->put_snn(&context,gkey,&response);
		if(stat.ok()){
			std::cout << "Response received" << std::endl;
			for (auto msg: response.ports()){
				ports.push_back(msg);
			}
			ClientMap[key] = ports;
			return ConnStrPutValue(ports,key,value);
		}
		else{
			std::cerr << "RPC failed: " << stat.error_message() << std::endl;
			return false;
		}
}

void GTStoreClient::finalize() {
		cout << "Inside GTStoreClient::finalize() for client " << client_id << "\n";
}
