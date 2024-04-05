#!/bin/bash

RESPONSE_PATH="./tests"
PATH_TO_BIN="./build/bin/server"
PATH_TO_CONFIG="./src/server_config"

pwd

# Set-up to get the PID of the server and make sure we have a connection to it
$PATH_TO_BIN $PATH_TO_CONFIG &
pid_server=$!
echo $pid_server
echo -n -e "Starting server...\n"


sleep 5

# Start by sending request to sleep handler
echo -n -e "Sending sleep request\n"
curl -i http://localhost:8080/sleep &
pid_sleep_request=$!
echo -n -e "Sent sleep request which will last for 5 seconds... \n"

# Time echo request
TIMEFORMAT=%R
(time curl -i http://localhost:8080/echo) 2> time_out
echo -n -e "Sent echo request... \n"
ECHO_TIME=`cat time_out`
rm time_out

echo "Echo request finished in ${ECHO_TIME}"

wait $pid_sleep_request

# If echo request took less time than sleep
TEST=`echo "${ECHO_TIME} < 5" | bc`
echo "Echo finished quicker than sleep: ${TEST}"

if [ $TEST -eq 1 ]
then
    echo -e "Success. \n"; 
else 
    echo -e "Failed. \n"; 
    kill -9 $pid_server
    exit 1;
fi

kill -9 $pid_server

exit 0