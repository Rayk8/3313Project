FROM gcc:latest
WORKDIR /app
COPY . .
RUN apt-get update && apt-get install -y make
RUN g++ -std=c++17 -Wall -pthread -Iinclude \
    src/main.cpp src/server.cpp src/library.cpp src/auth.cpp \
    -o library_server
EXPOSE 8080
CMD ["./library_server"]
