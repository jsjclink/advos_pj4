#/bin/bash
# Launch the GTStore Manager
cd cmake/build
./manager --nodes 1 --reps 1 &
manager_pid=$!
sleep 3

# Launch couple GTStore Storage Nodes
./storage 50052 &
storage_pid=$!
sleep 3
#./storage &
#sleep 5

# Launch the client testing app
# Usage: ./test_app <test> <client_id>
./test_app single_set_get 1 &
sleep 3
./test_app single_set_get 2 &
sleep 3
./test_app single_set_get 3 &
sleep 3

kill $manager_pid
kill $storage_pid

cd ../..