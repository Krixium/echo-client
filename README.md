# Echo Client

Basic TCP echo client

## Build

    make clean
    make

## Run

    Usage: ./echoc.out -a [address] -p [port] -m [message] -l [length] -c [count] -d [delay] -n [instances]
        -a - The address of the server.
        -p - The port of the server.
        -m - The message.
        -l - The length of the message - optional, defaults to length of message string.
        -c - The number of times each instance should send the message - optional, defaults to 1.
        -d - The amount of delay between each message in milliseconds - optional, defaults to 0.
        -n - The number of instances - optional, defaults to 1.
