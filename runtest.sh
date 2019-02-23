#!/bin/bash

# 1: host
# 2: number of instances
# 3: port
# 4: packet size
# 5: number messages
# 6: delay

for i in {1..$2}
do
    ./echoc.out $1 $3 asdf $4 $5 $6
done