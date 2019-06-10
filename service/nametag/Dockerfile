FROM croservices/cro-http:0.7.5

RUN apt-get update \
    && apt-get install -y mysql-client default-libmysqlclient-dev

RUN ln -s /usr/lib/x86_64-linux-gnu/libmariadbclient.so.18 /usr/lib/x86_64-linux-gnu/libmysqlclient.so.18

RUN mkdir /app
COPY . /app
WORKDIR /app

RUN zef install --deps-only . && perl6 -c -Ilib service.p6

ENV NAMETAG_PORT="10000" NAMETAG_HOST="0.0.0.0"
EXPOSE 10000

CMD perl6 -Ilib service.p6
