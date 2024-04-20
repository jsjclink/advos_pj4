#!/bin/bash

testcase_1(){
    ./start_service.sh 1 1 &
    service_pid=$!
    ./client --put key1 --val value1
    ./client --get key1
    ./client --put key1 --val value2
    ./client --put key2 --val value3
    ./client --put key3 --val value4
    ./client --get key1
    ./client --get key2
    ./client --get key3
    sleep 2;
    kill "$service_pid"
}

testcase_2(){
    ./start_service.sh 5 3 &
    service_pid=$!

    ./client --put key1 --val value1
    ./client --get key1
    ./client --put key1 --val value2
    ./client --put key2 --val value3
    ./client --put key3 --val value4
    ./client --get key1
    ./client --get key2
    ./client --get key3
    sleep 2;
    kill "$service_pid"
}

testcase_3(){
    ./start_service 3 2 &
    service_pid=$!

    ./client --put key1 --val value1
    ./client --put key1 --val newvalue1
    ./client --put key1 --val value2
    ./client --put key2 --val value3
    ./client --put key3 --val value4

    #Insert more key,value pairs until 3 of them are mapped to serverX

    ## kill serverX

    ./client --get key1
}

main(){
    testcase_1
    testcase_2
    testcase_3
    testcase_4
}

cd cmake/build
main $1 $2
cd ../..