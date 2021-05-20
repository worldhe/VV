#!/bin/bash
#cd ./core/onvifdiscovery
#GSOAP_DIR=../../../gsoap-2.8.111/gsoap
cd ./demo
GSOAP_DIR=../../gsoap-2.8.111/gsoap
/usr/local/bin/soapcpp2 -2 -j -S -T -p onvifdiscovery -x -I ${GSOAP_DIR}:${GSOAP_DIR}/import ${GSOAP_DIR}/import/wsdd10.h
#/usr/local/bin/soapcpp2 -2 -a -j -S -T -x -I ${GSOAP_DIR}:${GSOAP_DIR}/import ${GSOAP_DIR}/import/wsdd10.h
