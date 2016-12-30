#!/usr/bin/env bash
sudo gcc -o udpclient udpclient.c
sudo gcc -o udpserver udpserver.c

sudo gnome-terminal -x bash -c "./udpserver"
sudo gnome-terminal -x bash -c "./udpclient"