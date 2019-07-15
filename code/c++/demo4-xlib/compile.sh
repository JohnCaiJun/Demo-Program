#!/bin/bash

g++ --std=c++11 -o output prog-3.cc -D DEBUG -I/usr/X11R6/include -L/usr/X11R6/lib -lX11
