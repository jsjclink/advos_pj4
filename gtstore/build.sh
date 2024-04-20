./clean.sh
export MY_INSTALL_DIR=$HOME/.local
mkdir -p cmake/build
cd cmake/build
$HOME/.local/bin/cmake -DCMAKE_PREFIX_PATH=$MY_INSTALL_DIR ../..
make keyvaluestore.grpc.pb.o
make