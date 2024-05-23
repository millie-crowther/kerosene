FROM ubuntu:24.04

RUN apt update && apt install -y python3 python3-pip python3.12-venv
RUN python3.12 -m venv env && . env/bin/activate
COPY src /src

WORKDIR /src

RUN pip install -r requirements.txt