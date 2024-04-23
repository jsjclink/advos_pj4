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

testcase_1(){
    echo "Testcase 1: simple put and get, single server"
    service 1 1

    ./client --put key1 --val value1
    ./client --get key1
    ./client --put key1 --val value2
    ./client --put key2 --val value3
    ./client --put key3 --val value4
    ./client --get key1
    ./client --get key2
    ./client --get key3
    kill_service "$manager_pid"
    echo ""
}

testcase_2(){
    echo "Testcase 2: simple put and get, multi server"
    service 5 3

    ./client --put key1 --val value1
    ./client --get key1
    ./client --put key1 --val value2
    ./client --put key2 --val value3
    ./client --put key3 --val value4
    ./client --get key1
    ./client --get key2
    ./client --get key3
    kill_service "$manager_pid"
    echo ""
}

testcase_3(){
    echo "Testcase 3: storage node failure"
    service 3 2

    ./client --put key1 --val value1
    ./client --put key1 --val newvalue1
    ./client --put key2 --val value2
    ./client --put key3 --val value3
    ./client --put key4 --val value4
    ./client --put key5 --val value5
    ./client --put key6 --val value6
    ./client --put key7 --val value7
    ./client --put key8 --val value8
    ./client --put key9 --val value9
    ./client --put key10 --val value10
    
    kill "${storage_pids[0]}"
    echo "killed ${storage_pids[0]}"
    ./client --get key1
    ./client --get key2
    ./client --get key3
    ./client --get key4
    ./client --get key5
    ./client --get key6
    ./client --get key7
    ./client --get key8
    ./client --get key9
    ./client --get key10
    kill_service "$manager_pid"
    echo ""
}

testcase_4(){
    echo "Testcase 4: multiple storage node failure"
    service 7 3

    ./client --put key1 --val value1
    ./client --put key1 --val newvalue1
    ./client --put key2 --val value2
    ./client --put key3 --val value3
    ./client --put key4 --val value4
    ./client --put key5 --val value5
    ./client --put key6 --val value6
    ./client --put key7 --val value7
    ./client --put key8 --val value8
    ./client --put key9 --val value9
    ./client --put key10 --val value10
    ./client --put key2 --val newvalue2
    ./client --put key3 --val newvalue3
    ./client --put key11 --val value11
    ./client --put key12 --val value12
    ./client --put key13 --val value13
    ./client --put key14 --val value14
    ./client --put key15 --val value15
    ./client --put key16 --val value16
    ./client --put key17 --val value17
    ./client --put key18 --val value18
    ./client --put key19 --val value19
    ./client --put key20 --val value20

    
    kill "${storage_pids[0]}"
    echo "killed ${storage_pids[0]}"
    kill "${storage_pids[1]}"
    echo "killed ${storage_pids[1]}"

    ./client --get key1
    ./client --get key2
    ./client --get key3
    ./client --get key4
    ./client --get key5
    ./client --get key6
    ./client --get key7
    ./client --get key8
    ./client --get key9
    ./client --get key10
    ./client --get key11
    ./client --get key12
    ./client --get key13
    ./client --get key14
    ./client --get key15
    ./client --get key16
    ./client --get key17
    ./client --get key18
    ./client --get key19
    ./client --get key20
    kill_service "$manager_pid"
    echo ""
}

main(){
    testcase_1
    sleep 3
    testcase_2
    sleep 3
    testcase_3
    sleep 5
    testcase_4
    sleep 5
}

cd cmake/build
main $1 $2
cd ../..