#!/bin/bash
declare -a storage_pids

service(){
    local nodes=$1
    local reps=$2
    ./manager --nodes $nodes --reps $reps &
    manager_pid=$!

    sleep 2
    for i in $(seq $nodes)
    do
        port=$((50051 + i))
        ./storage $port &
        storage_pids+=($!)
        sleep 2
    done
}

kill_service(){
    local manager_pid=$1
    kill "$manager_pid"
    for pid in "${storage_pids[@]}"
    do
        kill "$pid"
    done
    unset storage_pids 
}

throughput(){
    echo "Performance Test#1: Throughput"
    service 7 1
    ./test_app perf_throughput
    kill_service "$manager_pid"
    echo ""
    service 7 3
    ./test_app perf_throughput
    kill_service "$manager_pid"
    echo ""
    service 7 5
    ./test_app perf_throughput
    kill_service "$manager_pid"
    echo ""
}

loadbalance(){
    echo "Performance Test#2: Loadbalance"
    service 7 1
    ./test_app perf_loadbalance
    kill_service "$manager_pid"
    echo ""
}

main(){
    throughput
    sleep 3
    loadbalance
}

cd cmake/build
main
cd ../..