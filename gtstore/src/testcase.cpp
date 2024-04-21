#include "gtstore.hpp"

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
		cout << "key: " << key << " value: " << print_value << "\n";
		
		client.finalize();
}

void put(string key, string value){
    GTStoreClient client;
	client.init(0);
    val_t val;
	val.push_back(value);
	bool resp = client.put(key, val);
    if(resp) cout << "client - put success\n";
    else cout << "client - put failure\n";
    client.finalize();
    return;
}

void get(string key){
    GTStoreClient client;
	client.init(0);
    val_t res = client.get(key);
    string print_value ="";
    for(uint i=0; i<res.size();i++){
        print_value += res[i] + " ";
    }
    cout << "client - key: " << key << " value: " << print_value << "\n";
    client.finalize();
    return;
}

int main(int argc, char **argv) {
    string key, value;
    if(argc==5){
        key = string(argv[2]);
        value = string(argv[4]);
        put(key,value);
    }
    else if(argc==3){
        key = string(argv[2]);
        get(key);
    }
    else{
        cout<<"Usage: ./client --put (key) --val (value) or ./client --get (key)\n";
    }
    return 0;
}
