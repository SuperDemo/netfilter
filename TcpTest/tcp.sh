#!/usr/bin/env bash
gcc -o tcpclient tcpclient.c
gcc -o tcpserver tcpserver.c

gnome-terminal -t "TcpServer" -x bash -c "./tcpserver;exec bash;"
gnome-terminal -t "TcpClient" -x bash -c "./tcpclient;exec bash;"