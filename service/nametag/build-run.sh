#!/bin/bash

set -x

docker kill nametag

set -e

docker build . -t nametag

docker run -p 10000:10000 -d --name nametag --network nametag --rm \
    -e DB_DATABASE=nametag \
    -e DB_USER=nametag \
    -e DB_PASSWORD=nametag123 \
    -e DB_HOST=nametag-mysql \
    -e DB_PORT=3306 \
    -e ADMIN_TOKEN=$(cat admin.token) \
    nametag

docker ps
docker logs nametag -f
