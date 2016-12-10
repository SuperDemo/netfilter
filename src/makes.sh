#!/usr/bin/env bash

make
insmod netfilter.ko
sleep 10
rmmod netfilter