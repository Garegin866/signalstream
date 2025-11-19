# 🔥 SignalStream
A modern, clean-architecture backend built with **C++20**, **Drogon**, **PostgreSQL**, and **Docker**.

SignalStream is a modular, scalable backend designed as a real-world production system — featuring authentication, tagging, content feeds, async DB access, and strict architectural boundaries.

The project uses best practices from modern backend engineering:
- Clean Architecture
- DTO-based data flow
- Repository–Service–Controller pattern
- Strong typing
- Session-based authentication
- Secure password hashing (Argon2id)
- GitHub Projects as backlog

---

# 🚀 Features

### ✅ Authentication Module
- Register
- Login
- Logout (coming soon)
- Session token generation (UUID, pluggable for JWT)
- Secure hashing with Argon2id
- Strict error handling

### ✅ Tags Module
- Create tags
- List tags
- Attach tags to users (coming)
- Feed filtering based on user tags (coming)

### 🔧 Architecture Highlights
- **No SQL in controllers**
- **No JSON in repositories**
- **Async Postgres queries with Drogon ORM**
- **Separation into controllers → services → repositories → dto → core**
- **Fully typed error system** (ErrorType, AppError)
- **Consistent JSON response layer**

---

# 📂 Project Structure

```
server/
  controllers/        # HTTP endpoints only
  services/           # Business logic & validation
  repositories/       # Raw SQL data access
  dto/                # Typed C++ data models
  core/               # Utilities: errors, hashing, tokens, responses
  CMakeLists.txt
docker-compose.yml    # PostgreSQL container
```

### Layered Architecture

```
           ┌────────────────┐
           │  Controllers   │   → HTTP only, no logic
           └───────▲────────┘
                   │
           ┌───────┴────────┐
           │    Services    │   → validation, orchestration, rules
           └───────▲────────┘
                   │
           ┌───────┴────────┐
           │  Repositories  │   → SQL only, async, no JSON
           └───────▲────────┘
                   │
           ┌───────┴────────┐
           │       DTO      │   → typed data flow
           └────────────────┘
```

---

# 🛠 Getting Started

### 1. Clone the repo

```bash
git clone https://github.com/<yourname>/SignalStream.git
cd SignalStream
```

### 2. Install dependencies (Conan 2)

```bash
conan install . --output-folder=build --build=missing
```

### 3. Build the backend

```bash
cd build
cmake ..
cmake --build . -j$(nproc)
```

### 4. Run PostgreSQL with Docker

```bash
docker compose up -d
```

### 5. Run the server

```bash
./server
```

API now runs on:

```
http://localhost:8080
```

---

# 📦 Endpoints

## 🔐 Auth

### `POST /auth/register`
```json
{
  "email": "user@example.com",
  "password": "mypassword"
}
```

### `POST /auth/login`
```json
{
  "email": "user@example.com",
  "password": "mypassword"
}
```

Returns:
```json
{
  "token": "uuid-here"
}
```

---

## 🏷 Tags

### `GET /tags`
Lists all tags:
```json
{
  "tags": [
    { "id": 1, "name": "cpp" },
    { "id": 2, "name": "backend" }
  ]
}
```

### `POST /tags`
```json
{
  "name": "drogon"
}
```

---

# 🔐 Security

- Passwords hashed via **Argon2id**
- Tokens generated via UUID (secure RNG)
- Database exceptions sanitized (no internal details leaked)
- Strict `ErrorType` mapping
- No plaintext passwords stored
- All user input validated in services

---

# 🗂 Project Management

SignalStream uses **GitHub Projects** for backlog & contributors:

- **Issues** → tasks
- **Boards** → workflow
- **Labels** → categorization
- **Milestones** → modules (Auth, Tags, Items)

PRs must contain:

```
Closes #<issue-number>
```

to link into the workflow.

---

# 🤝 Contributing

We welcome contributions!  
Please read:

👉 [`CONTRIBUTING.md`](./CONTRIBUTING.md)

Includes:
- setup instructions
- branch naming conventions
- coding guidelines
- commit format
- PR rules
- architectural rules

All contributions must follow the existing service/repository architecture.

---

# 📌 Roadmap

### 🔐 Auth Module
- [x] Register
- [x] Login
- [ ] Logout
- [ ] /auth/me
- [ ] Token middleware
- [ ] Password reset

### 🏷 Tags
- [x] Create tag
- [x] List tags
- [ ] Attach tags to user
- [ ] User feed filter

### 📦 Items
- [ ] Item CRUD
- [ ] item_tags relation
- [ ] Personalized feed

### 🛡 Security
- [x] Argon2id hashing
- [ ] Rate limiting
- [ ] Session expiration
- [ ] IP-based suspicious login detection

---

# 📜 License

MIT License.  
Do whatever you want — just don't blame us if you break the server.

---

# ⭐ Support the Project

If you're learning C++ backend or Drogon, star the repo — it helps visibility.

```
⭐ Star this repository to support development!
```
