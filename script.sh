#!/bin/bash

bison -d parser.y

gcc *.c -o exec
