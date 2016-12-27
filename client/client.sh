#!/usr/bin/env bash

sudo gcc -o client netlink_client.c

sudo gnome-terminal -x bash -c "./client;exec bash;"