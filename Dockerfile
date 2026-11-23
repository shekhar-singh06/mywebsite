FROM gcc:latest
WORKDIR /app
COPY . .
RUN g++ -std=c++17 -O3 main.cpp -o server -pthread
EXPOSE 8080
CMD ["./server"]
