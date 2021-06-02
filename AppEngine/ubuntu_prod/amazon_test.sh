#!/bin/bash

TOKEN=$(aws ecr get-authorization-token --output text --query 'authorizationData[].authorizationToken')

curl -i -H "Authorization: Basic $TOKEN" https://976640767083.dkr.ecr.us-west-2.amazonaws.com/v2/

curl -i -H "Authorization: Basic $TOKEN" https://976640767083.dkr.ecr.us-west-2.amazonaws.com/v2/_catalog

curl -i -H "Authorization: Basic $TOKEN" https://976640767083.dkr.ecr.us-west-2.amazonaws.com/liveness_check

curl -i -H "Authorization: Basic $TOKEN" https://976640767083.dkr.ecr.us-west-2.amazonaws.com:8080/liveness_check

echo ""

aws cloudformation describe-stacks --stack-name ecr-continuous-scan
