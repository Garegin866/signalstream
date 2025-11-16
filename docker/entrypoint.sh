version: "3.9"

services:
  db:
    image: postgres:16
    container_name: signalstream_db
    environment:
      POSTGRES_USER: signaluser
      POSTGRES_PASSWORD: signalpass
      POSTGRES_DB: signaldb
    ports:
      - "5432:5432"
    volumes:
      - db_data:/var/lib/postgresql/data

  app:
    build:
      context: .
      dockerfile: docker/Dockerfile
    container_name: signalstream_app
    depends_on:
      - db
    ports:
      - "8080:8080"
    environment:
      # We'll use these later in Drogon config
      DB_HOST: db
      DB_PORT: 5432
      DB_NAME: signaldb
      DB_USER: signaluser
      DB_PASS: signalpass

volumes:
  db_data:
