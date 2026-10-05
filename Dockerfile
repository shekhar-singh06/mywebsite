FROM gcc:latest

WORKDIR /app

COPY . .

RUN g++ -std=c++11 main.cpp -pthread -o server

EXPOSE 8080

CMD ["./server"]
