FROM gcc:latest
WORKDIR /app
COPY . .
RUN gcc server.c -o server && gcc client.c -o client