#!/bin/bash

kill `ps -ea | grep go.sh | grep -v grep | awk '{print $1}'`
