FROM ubuntu:24.04

RUN apt update && apt install -y python3
COPY src /src

WORKDIR /src

RUN pip3 install -r requirements.txt