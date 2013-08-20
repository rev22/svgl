#!/bin/sh

set -e

( cd glft && build/automake/bootstrap && ./configure && make )

build/automake/bootstrap

PKG_CONFIG_PATH="$(cd glft && pwd)" ./configure

make
