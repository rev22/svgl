#!/bin/sh

set -e

( cd glft && build/automake/bootstrap && ./configure && make )

build/automake/bootstrap

./configure

make
