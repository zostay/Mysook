#!/bin/bash

set -x

docker run -d --name nametag-mysql --network nametag --rm \
    -e MYSQL_ROOT_PASSWORD=mysql \
    -e MYSQL_DATABASE=nametag \
    -e MYSQL_USER=nametag \
    -e MYSQL_PASSWORD=nametag123 \
    mysql:5.7

docker run -P -d --name nametag --network nametag --rm \
    -e DB_DATABASE=nametag \
    -e DB_USER=nametag \
    -e DB_PASSWORD=nametag123 \
    -e DB_HOST=nametag-mysql \
    -e DB_PORT=3306 \
    -e DEVICE_KEY=$(cat public.key) \
    nametag
