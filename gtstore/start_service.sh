#!/bin/bash

main(){
    local nodes=$1
    local reps=$2
    ./manager --nodes $nodes --reps $reps &
    for i in $(seq $nodes)
    do
        ./storage &
        sleep 5;
    done
}

cd cmake/build
main $1 $2
cd ../..