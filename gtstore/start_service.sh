#!/bin/bash

main(){
    local nodes=$1
    local reps=$2
    ./manager --nodes $nodes --reps $reps &
    sleep 2;
    for i in $(seq $nodes)
    do
        port=$((50051 + i))
        ./storage $port &
        sleep 5;
    done
}

main $1 $2
