#!/bin/bash

for file in $(ls | grep .log)
do
	wc -l $file
done
