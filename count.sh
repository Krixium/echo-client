#!/bin/bash

( find ./ -name '*.log' -print0 | xargs -0 cat ) | wc -l