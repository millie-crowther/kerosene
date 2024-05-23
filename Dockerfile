FROM ubuntu:24.04

COPY src /src

WORKDIR /src

RUN pip install -r requirements.txt