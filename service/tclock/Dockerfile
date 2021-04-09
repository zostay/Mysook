FROM rakudo-star:2019.03
MAINTAINER Sterling Hanenkamp <hanenkamp@cpan.org>

RUN apt-get update

COPY /Cofra /Cofra
WORKDIR /Cofra
RUN ls -l && zef install . --force-test

RUN mkdir /app && mkdir /config
COPY . /app
WORKDIR /app

RUN zef install HTTP::Headers HTTP::Supply::Request && zef install --force-test --deps-only .

ENV DEPLOYMENT_HOST="0.0.0.0" \
    DEPLOYMENT_PORT="5000" \
    DEPLOYMENT_STAGE="production" \
    TCLOCK_CONFIG="/config"

EXPOSE 5000
VOLUME /config

CMD perl6 -I/app/lib /app/bin/tclock.p6
