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
#include <algorithm>
#include <thread>
#include <chrono>

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
using keyvaluestore::Put_Ports;

using grpc::Channel;
using grpc::ClientContext;

unordered_map<string, int> node_vol_map;
unordered_map<string, vector<string>> key_nodes_map;
std::mutex node_mtx;

class KeyValueServiceStorageClient {
	public:
	string port;
	KeyValueServiceStorageClient(string port, std::shared_ptr<Channel> channel) : port(port), stub_(KeyValueService::NewStub(channel)) {

	}

	Status check_alive() {
		ClientContext context;
		Void request, response;

		return stub_->check_alive(&context, request, &response);
	}

	private:
	std::unique_ptr<KeyValueService::Stub> stub_;
};

vector<KeyValueServiceStorageClient> storages;
std::mutex storages_mtx;

class KeyValueServiceManagerImpl final : public KeyValueService::Service {
	public:
	KeyValueServiceManagerImpl(int nodes, int rep) : nodes(nodes), rep(rep) {

	}

	Status get_snn(ServerContext* context, const Key* key, Port* port) override {
		cout << "manager - get_snn is called by client.\n";
		std::lock_guard<std::mutex> lock(node_mtx);
		if(auto it = key_nodes_map.find(key->key()); it != key_nodes_map.end()) {
			vector<string>& ports = it->second;
			port->set_port(get_target_port(ports));
			cout << get_target_port(ports) << "\n";
		} else {
			port->set_port("");
			cout << "manager - get error\n";
		}
		
		cout << "manager - get_snn end\n";
		return Status::OK;
	}

	Status put_snn(ServerContext* context, const Key* key, Put_Ports* put_ports) override {
		cout << "manager - put_snn is called by client. key: " << key->key() << "\n";
		std::lock_guard<std::mutex> lock(node_mtx);
		if(auto it = key_nodes_map.find(key->key()); it != key_nodes_map.end()) {
			// moidfy operation
			vector<string>& ports = it->second;
			for(const auto& port : ports) {
				put_ports->add_ports(port);
			}
		} else {
			// initial put operation
			vector<string> ports = get_target_ports();
			key_nodes_map[key->key()] = ports;
			for(const auto& port : ports) {
				node_vol_map[port] += 1;
				put_ports->add_ports(port);
			}
		}
		
		cout << "manager - put_snn end\n";
		return Status::OK;
	}


	Status str_cnt(ServerContext* context, const Port* port, Void* response) override {
		cout << "manager - str_cnt is called by storage node. port num: " << port->port() << "\n";
		cout << "manager - Connect to storage node. port num: " << port->port() << "\n";

		{
			// connect to storage node
			std::lock_guard<std::mutex> lock(storages_mtx);
			std::string server_address = "0.0.0.0:" + port->port();
			storages.push_back(KeyValueServiceStorageClient(port->port(), grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials())));
		}

		{
			std::lock_guard<std::mutex> lock(node_mtx);
			node_vol_map[port->port()] = 0;
		}

		return Status::OK;
	}

	private:
	int nodes;
	int rep;

	string get_target_port(vector<string>& ports) {
		return ports[0];
	}
	vector<string> get_target_ports() {
		vector<pair<int, string>> pairs;

		for(const auto& kv: node_vol_map) {
			pairs.push_back({kv.second, kv.first});
		}

		sort(pairs.begin(), pairs.end());

		vector<string> result;

		for(int i = 0; i < rep && i < pairs.size(); i++) {
			result.push_back(pairs[i].second);
		}

		return result;
	}
};

void PeriodicCheckAlive(int interval) {
	while(true) {
		std::unique_lock<mutex> lock(storages_mtx);

		for (auto it = storages.rbegin(); it != storages.rend(); ++it) {
			cout << "manager - check storage : " << it->port << "\n";
			Status status = it->check_alive();
			if (!status.ok()) {
				cout << "manager - check_alive Not OK, removing storage node info\n";
				// remove storage info in storages vector
				storages.erase((it + 1).base());
				// todo : remove storage info in node_vol_map
				// todo : remove stroage info in key_nodes_map
				// todo : choose another storage node
			} else {
				cout << "manager - check_alive OK\n";
			}
		}
		lock.unlock();

		std::this_thread::sleep_for(std::chrono::milliseconds(interval));
	}
}

void GTStoreManager::init(int nodes, int rep) {
	cout << "Inside GTStoreManager::init()\n";
	std::string server_address("0.0.0.0:50051");
	KeyValueServiceManagerImpl service(nodes, rep);

	ServerBuilder builder;
  	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  	builder.RegisterService(&service);
  	std::unique_ptr<Server> server(builder.BuildAndStart());
  	std::cout << "manager - Server listening on " << server_address << std::endl;

	// 주기적으로 살아있는지 확인
	std::thread checkThread(PeriodicCheckAlive, 500);
    checkThread.detach();
	
  	server->Wait();
}


int main(int argc, char **argv) {
	int nodes = std::atoi(argv[2]);
    int rep = std::atoi(argv[4]);

	std::cout << "manager - Nodes: " << nodes << ", Replications: " << rep << std::endl;

	GTStoreManager manager;
	manager.init(nodes, rep);

	return 0;
}
