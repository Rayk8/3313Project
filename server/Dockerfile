FROM gcc:latest
WORKDIR /app

# Copy all necessary files
COPY src/ ./src/
COPY include/ ./include/
COPY *.json ./
COPY Makefile .

# Install dependencies and build
RUN apt-get update && \
    apt-get install -y make && \
    make clean && \
    make all

# Set environment variables
ENV PORT=8080

# Expose the port
EXPOSE ${PORT}

# Run the server
CMD ["./library_server"]
