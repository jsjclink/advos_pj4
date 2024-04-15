export MY_INSTALL_DIR=$HOME/.local
mkdir -p cmake/build
cd cmake/build
$HOME/.local/bin/cmake -DCMAKE_PREFIX_PATH=$MY_INSTALL_DIR ../..
make keyvaluestore.grpc.pb.o
make

./manager &
sleep 5

# Launch couple GTStore Storage Nodes
./storage &
sleep 5
./storage &
sleep 5

# Launch the client testing app
# Usage: ./test_app <test> <client_id>
./test_app single_set_get 1 &
./test_app single_set_get 2 &
./test_app single_set_get 3 