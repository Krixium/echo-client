#!/bin/bash

# 1: number of instances
# 2: host
# 3: port
# 4: msg
# 5: packet size
# 6: packet count
# 7: delay

for i in $(eval echo {1..$1})
do
    echo "spawning $i"
    ~/echo-client/echoc.out $2 $3 $4 $5 $6 $7 &
done
