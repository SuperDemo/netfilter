#!/usr/bin/env bash
sudo gcc -o tcpclient tcpclient.c
sudo gcc -o tcpserver tcpserver.c

sudo gnome-terminal -x bash -c "./tcpserver"
sudo gnome-terminal -x bash -c "./tcpclient"