#!/bin/bash

RESPONSE_PATH="./tests"
PATH_TO_BIN="./build/bin/server"
PATH_TO_CONFIG="./src/server_config"

pwd

# Set-up to get the PID of the server and make sure we have a connection to it
$PATH_TO_BIN $PATH_TO_CONFIG &
pid_server=$!
echo $pid_server


sleep 5

# Test 1 should succeed by outputting an HTTP OK header and echoing the request
curl -i http://localhost:8080/echo -H "User-Agent: curl" > ${RESPONSE_PATH}/test_response1
echo -n -e "Test 1 for testing echo ... \n"
sleep 1

diff ${RESPONSE_PATH}/expected_response1 ${RESPONSE_PATH}/test_response1

if [[ $? -eq 0 ]]; then
    echo -e "Success. \n"; 
else 
    echo -e "Failed. \n"; 
    kill -9 $pid_server
    exit 1;
fi

# Test 2 should succeed by outputting an HTTP OK header and echoing the request and index.html
curl -i http://localhost:8080/static/index.html > ${RESPONSE_PATH}/test_response2 
echo -n -e "Test 2 for testing static file, index.html ... \n"
sleep 1

diff ${RESPONSE_PATH}/expected_response2 ${RESPONSE_PATH}/test_response2

if [[ $? -eq 0 ]]; then
    echo -e "Success. \n"; 
else 
    echo -e "Failed. \n"; 
    kill -9 $pid_server
    exit 1;
fi

# Test 3 should succeed by outputting an HTTP OK header and echoing the request and staticv2.txt
curl -i http://localhost:8080/static2/staticv2.txt > ${RESPONSE_PATH}/test_response3 
echo -n -e "Test 3 for testing static file, staticv2.txt ... \n"
sleep 1

diff ${RESPONSE_PATH}/expected_response3 ${RESPONSE_PATH}/test_response3

if [[ $? -eq 0 ]]; then
    echo -e "Success. \n"; 
else 
    echo -e "Failed. \n"; 
    kill -9 $pid_server
    exit 1;
fi

# Test 4 - CRUD
# 1. Create an entity
# 2. Retrieve the entity and verify its correctness
# 3. Delete the entity and verify that it is no longer retrievable
curl -X POST -i http://localhost:8080/api/Shoes -H 'Content-Type: application/json' -d '{"Brand":"Nike"}' > ${RESPONSE_PATH}/test_response4 
printf "\n\n" >> ${RESPONSE_PATH}/test_response4
curl -X GET -i http://localhost:8080/api/Shoes/0 >> ${RESPONSE_PATH}/test_response4
printf "\n\n" >> ${RESPONSE_PATH}/test_response4
curl -X DELETE -i http://localhost:8080/api/Shoes/0
curl -X GET -i http://localhost:8080/api/Shoes/0 >> ${RESPONSE_PATH}/test_response4

echo -n -e "Test 4 for CRUD actions \n"
sleep 1

diff --strip-trailing-cr ${RESPONSE_PATH}/expected_response4 ${RESPONSE_PATH}/test_response4

if [[ $? -eq 0 ]]; then
    echo -e "Success. \n"; 
else 
    echo -e "Failed. \n"; 
    kill -9 $pid_server
    exit 1;
fi

kill -9 $pid_server

exit 0