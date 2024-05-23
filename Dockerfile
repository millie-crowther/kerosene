FROM ubuntu:24.04

RUN apt install python3.12
RUN python --version

COPY src /src

WORKDIR /src

RUN pip install -r requirements.txt