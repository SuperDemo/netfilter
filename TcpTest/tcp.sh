#!/usr/bin/env bash
gcc -o tcpclient tcpclient.c
gcc -o tcpserver tcpserver.c
gnome-terminal -x bash -c "./tcpserver"
gnome-terminal -x bash -c "./tcpclient"