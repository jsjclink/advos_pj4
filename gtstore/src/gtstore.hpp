#ifndef GTSTORE
#define GTSTORE

#include <string>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <memory>
#include <unordered_map>
#include "keyvaluestore.grpc.pb.h"

#define MAX_KEY_BYTE_PER_REQUEST 20
#define MAX_VALUE_BYTE_PER_REQUEST 1000

using namespace std;
using keyvaluestore::KeyValueService;

typedef vector<string> val_t;

class GTStoreClient {
		private:
				int client_id;
				val_t value;
				unique_ptr<KeyValueService::Stub> stub_;
		public:
				void init(int id);
				void finalize();
				val_t get(string key);
				bool put(string key, val_t value);
};

class GTStoreManager {
		public:
				void init();
};

class GTStoreStorage {
		public:
				void init();
};

#endif
