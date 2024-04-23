#include "gtstore.hpp"
#include <chrono>

void single_set_get(int client_id) {
		cout << "Testing single set-get for GTStore by client " << client_id << ".\n";

		GTStoreClient client;
		client.init(client_id);

		string key = to_string(client_id);
		vector<string> value;
		value.push_back("phone");
		value.push_back("phone_case");

		client.put(key, value);
		val_t res = client.get(key);
		string print_value ="";
		for(uint i=0; i<res.size();i++){
			print_value += res[i] + " ";
		}
		cout << "client - key: " << key << " value: " << print_value << "\n";
		
		client.finalize();
}

void perf_throughput() {
	cout << "Testing performance for GTStore by client\n";
	GTStoreClient client;
	string key, prevkey;
	client.init(0);
	auto start = chrono::high_resolution_clock::now();
	for(int i = 0; i<100000; i++){
		vector<string> value;
		key = "key" + to_string(i);
		prevkey = "key" + to_string(i-1);
		value.push_back("value"+to_string(i));
		client.put(key, value);
		if(i%2==0) client.get(key);
		else client.get(prevkey);
	}
	auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();
    double throughput = 200000 / (duration / 1000.0); // Operations per second
    std::cout << "Total time: " << duration << " milliseconds" << std::endl;
    std::cout << "Throughput: " << throughput << " operations per second" << std::endl;
	client.finalize();
	return;
}

void perf_loadbalance(){
	cout << "Testing performance for GTStore by client\n";
	GTStoreClient client;
	client.init(0);
	for(int i = 0; i<100000; i++){
		vector<string> value;
		string key = "key" + to_string(i);
		value.push_back("value"+to_string(i));
		client.put(key, value);
	}
	cout<<"Done Storing Keys\n";
	client.printHistogram();
	cout<<"";
	client.finalize();
	return;
}


int main(int argc, char **argv) {
		string test = string(argv[1]);

		string test1 = "single_set_get";
		string test2 = "perf_throughput";
		string test3 = "perf_loadbalance";
		if (string(argv[1]) ==  test1) {
				single_set_get(0);
		}
		else if(string(argv[1]) == test2){
				perf_throughput();
		}
		else if (string(argv[1]) == test3){
				perf_loadbalance();
		}
		return 0;
}
