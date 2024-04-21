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
	client.put(key, val);
    client.finalize();
    return;
}

void get(string key){
    GTStoreClient client;
	client.init(0);
	client.get(key);
    client.finalize();
    return;
}

int main(int argc, char **argv) {
    //put
    string key, value;
    cout<<argc<<"\n";
    if(argc==5){
        key = string(argv[2]);
        value = string(argv[4]);
        put(key,value);
        return 0;
    }
    else if(argc==4){
        key = string(argv[2]);
        get(key);
        return 0;
    }
    else{
        cout<<"Usage: ./client --put (key) --val (value)\n./client --get (key)\n";
        return 0;
    }
}
