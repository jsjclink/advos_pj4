CXX = g++
CFLAGS  += `pkg-config --cflags protobuf grpc`
LDFLAGS  = -L/usr/local/lib `pkg-config --libs protobuf grpc++` \
					-Wl,--no-as-needed -lgrpc++_reflection \
					-Wl,--as-needed

PROTOC = protoc
GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`

RM      = /bin/rm -rf

TESTS = test_app testcase manager storage
SRC = src/test_app.cpp src/client.cpp src/testcase.cpp


all: $(TESTS)

manager: keyvaluestore.pb.o keyvaluestore.grpc.pb.o src/gtstore.hpp src/manager.o
	$(CXX) $^ $(LDFLAGS) -o $@

storage: keyvaluestore.pb.o keyvaluestore.grpc.pb.o src/gtstore.hpp src/storage.o
	$(CXX) $^ $(LDFLAGS) -o $@

test_app : keyvaluestore.pb.o keyvaluestore.grpc.pb.o src/gtstore.hpp src/client.o src/test_app.o
	$(CXX) $^ $(LDFLAGS) -o $@

testcase : keyvaluestore.pb.o keyvaluestore.grpc.pb.o src/gtstore.hpp src/client.o src/testcase.o
	$(CXX) $^ $(LDFLAGS) -o $@

clean:
	$(RM) *.o *.pb.cc *.pb.h $(TESTS)

%.grpc.pb.cc: %.proto
	$(PROTOC) -I . --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

%.pb.cc: %.proto
	$(PROTOC) -I . --cpp_out=. $<

%.o: %.cc
	$(CXX) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@