FROM gcc:latest

WORKDIR /app

COPY udp_chat.c .

RUN gcc udp_chat.c -o udp_chat

ENTRYPOINT ["./udp_chat"]