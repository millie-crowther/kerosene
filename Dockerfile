FROM ubuntu:24.04

RUN apt update && apt install python3 -y
RUN python --version

COPY src /src

WORKDIR /src

RUN pip install -r requirements.txt