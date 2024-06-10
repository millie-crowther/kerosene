FROM ubuntu:24.04

RUN apt update && apt install -y cmake libpcre2-32-0 python3 python3-pip python3.12-venv
ENV VIRTUAL_ENV=/opt/venv
RUN python3.12 -m venv $VIRTUAL_ENV
ENV PATH="$VIRTUAL_ENV/bin:$PATH"

COPY src /src

WORKDIR /src
RUN cmake .
RUN make

RUN pip install -r requirements.txt
RUN python main.py

CMD ./simorgh
