#!/bin/bash
#gunicorn --workers 1 -b 127.0.0.1:5001 docker-server:falconApi

nohup /root/anaconda3/bin/python /root/srv/docker-server.py &

echo STARTED
