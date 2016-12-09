#!/usr/bin/env bash

make
insmod netfilter.ko
rmmod netfilter