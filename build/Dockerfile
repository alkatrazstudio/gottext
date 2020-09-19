ARG PHP_VER
FROM php:$PHP_VER

COPY bootstrap.sh ./
RUN ./bootstrap.sh && rm -f bootstrap.sh

RUN mkdir /gottext
VOLUME /gottext
WORKDIR /gottext
