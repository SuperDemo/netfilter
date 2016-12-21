#!/usr/bin/env bash

sudo gcc -o client client.c

sudo gnome-terminal -x bash -c "./client;exec bash;"