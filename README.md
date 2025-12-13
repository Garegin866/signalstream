<p align="center">

  <!-- Build / CI -->
  <a href="https://github.com/Garegin866/SignalStream/actions">
    <img src="https://img.shields.io/github/actions/workflow/status/Garegin866/SignalStream/ci.yml?branch=main&label=CI&logo=github&style=for-the-badge" />
  </a>

  <!-- C++20 -->
  <img src="https://img.shields.io/badge/C%2B%2B-20-blue?style=for-the-badge&logo=c%2B%2B" />

  <!-- Drogon Framework -->
  <img src="https://img.shields.io/badge/Framework-Drogon-0A97B0?style=for-the-badge&logo=c%2B%2B" />

  <!-- PostgreSQL -->
  <img src="https://img.shields.io/badge/Database-PostgreSQL-336791?style=for-the-badge&logo=postgresql" />

  <!-- Docker -->
  <img src="https://img.shields.io/badge/Docker-ready-2496ED?style=for-the-badge&logo=docker" />

  <!-- Conan -->
  <img src="https://img.shields.io/badge/Conan-2.x-43A047?style=for-the-badge&logo=conan" />

</p>

# 🔥 SignalStream
A modern backend built with C++20, Drogon, PostgreSQL, Docker, and clean-architecture principles.

SignalStream implements an authentication-first content platform with tagging, personalized feed, async DB operations, and a strict architectural structure designed for growth, clarity, and long-term maintainability.

The codebase uses modern backend engineering practices:
-	Clean Architecture
-	Controller → Service → Repository layering
-	Fully typed DTOs
-	Unified error system
-	Secure Argon2id password hashing
-	Session-based authentication
-	Centralized string constants
-	Authorization & user context middleware
-	GitHub Projects for backlog & contributor workflow

---

# 🚀 Features

### ✅ Authentication Module
- Register users
- Login with email + password
- Secure Argon2id password hashing
- UUID-based session tokens
- TokenMiddleware (Bearer token validation)
- UserContextMiddleware (loads full UserDTO)
- auth/me endpoint
- Logout

### ✅ Tags Module
- Create tags
- List tags
- Attach tags to users
- Get user tags
- Filter content by user tags (feed)

### 📦 Items Module
- 	Create items
- 	List items
- 	Attach tags to items
- 	Personalized feed by tag intersection

### 🧩 Architecture & Engineering
- **No SQL in controllers**
- **No JSON in repositories**
- **Async Postgres queries with Drogon ORM**
- **Separation into controllers → services → repositories → dto → core**
- **Centralized constants (Const::JSON_*, Const::ATTR_*)**
- **Fully typed error system** (ErrorType, AppError)
- **Consistent JSON response layer**
- **Reusable validation rules in services**

---

# 📂 Project Structure

```
server/
  controllers/         # HTTP endpoints only (thin)
  services/            # Business logic & validation
  repositories/        # SQL calls only (async)
  middleware/          # Token + User context
  dto/                 # Data transfer objects
  core/                # Errors, hashing, tokens, constants, responses
CMakeLists.txt
docker-compose.yml     # PostgreSQL container
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

### 🔐 Middleware Flow

```
Client Request
     │
     ▼
┌──────────────────┐
│ TokenMiddleware  │  → Validates Bearer token  
│ (auth required)  │  → Loads session  
└───────▲──────────┘
        │
┌───────┴──────────────┐
│UserContextMiddleware │ → Loads UserDTO  
│      (optional)      │ → Injects into request  
└────────▲─────────────┘
         │
         ▼
     Controller
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

### `GET /auth/me`

Requires:
```
Authorization: Bearer <token>
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

### `POST /user/tags`
```json
{ 
  "tagId": 1
}
```

---

## 📦 Items

### `GET /items`
Lists all items:
```json
{
  "title": "C++ Smart Pointers",
  "description": "A guide",
  "url": "https://example.com"
}
```

### `GET /feed`

Returns personalized feed based on user’s tags.

---

# 🔐 Security

- Passwords hashed via **Argon2id**
- No plaintext passwords
- Sessions stored securely
- Tokens validated via middleware
- Centralized constants (no magic strings)
- Error responses sanitized
- Strict architecture boundaries
- All user input validated in services

---

# 🗂 Project Management

SignalStream uses **GitHub Projects** for backlog & contributors:

- **Issues** → tasks
- **Boards** → workflow
- **Labels** → categorization
- **Milestones** → modules

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

<img src="https://img.shields.io/github/issues/Garegin866/SignalStream?style=for-the-badge" />

---

# 📌 Roadmap

### 🔐 Auth Module
- [x] Register
- [x] Login
- [x] Logout
- [x] /auth/me
- [x] Token middleware
- [ ] Password reset

### 🏷 Tags
- [x] Create tag
- [x] List tags
- [x] Attach tags to user
- [x] User feed filter

### 📦 Items
- [x] Item CRUD
- [x] item_tags relation
- [x] Personalized feed

### 🛡 Security
- [x] Argon2id hashing
- [ ] Rate limiting
- [ ] Session expiration
- [ ] IP-based suspicious login detection

---

# 📜 License

MIT License.  
Do whatever you want — just don't blame us if you break the server.

<img src="https://img.shields.io/badge/license-MIT-green?style=for-the-badge" />
---

# ⭐ Support the Project

If you're learning C++ backend or Drogon, star the repo — it helps visibility.

```
⭐ Star this repository to support development!
```
