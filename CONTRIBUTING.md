# Contributing to SignalStream

Thank you for your interest in contributing to **SignalStream** — a modern C++ backend built with Drogon, PostgreSQL, Docker, and clean architecture principles.

This document explains how to set up your environment, how we work, and the standards required for contributions.

---

# 🛠 Tech Stack

- **C++20**
- **Drogon** framework
- **PostgreSQL** (Docker)
- **Conan 2** package manager
- **CMake** build system
- **Clean Architecture**  
  (controllers → services → repositories → dto → core)
- **GitHub Issues + GitHub Projects** for backlog

If you are new to Drogon or Conan, browsing the existing code will help.

---

# 📦 Getting Started

## 1. Clone the repository

```bash
git clone https://github.com/<username>/SignalStream.git
cd SignalStream
```

## 2. Install dependencies (Conan 2)

```bash
conan install . --output-folder=build --build=missing
```

## 3. Build the server

```bash
cd build
cmake ..
cmake --build . -j$(nproc)
```

## 4. Run PostgreSQL via Docker

```bash
docker compose up -d
```

## 5. Run the backend

```bash
./server
```

The API will be available on:

```bash
http://localhost:8080
```

# 🧱 Project Structure

```
server/
  controllers/
  services/
  repositories/
  dto/
  core/
  config.json
  CMakeLists.txt
```

- **Controllers** → HTTP layer only (no logic, no SQL)
- **Services** → business logic & validation
- **Repositories** → SQL queries only
- **DTO** → typed models, no JSON
- **Core** → utilities (hashing, tokens, errors, response helpers)
Please follow this architecture strictly.

# - 🧪 Testing
(If tests are not available yet, skip.)
We will expand unit tests with Catch2.

# 🧩 How to Pick Up Tasks
We track all tasks in GitHub Issues and GitHub Project Board:
- Choose an issue that interests you.
Comment:
- I want to take this
- A maintainer will assign the issue to you.
If an issue is unclear, ask questions — don’t guess.
- 
# 🌿 Branching Strategy
Every contribution must be made on a feature branch, not main.
Branch naming convention:

```
feat/<short-description>
fix/<short-description>
chore/<short-description>
refactor/<short-description>
```
Examples:
```
feat/tags-list-endpoint
fix/auth-hash-bug
refactor/user-repo
```

# 📝 Commit Messages (Conventional Commits)
We use Conventional Commits:

```
feat: add user tag repository
fix: handle duplicate email error
refactor: clean auth service
docs: update contributing guide
```

This makes the changelog clean and readable.

# 🔀 Pull Requests
Before opening a PR:
- Make sure your branch is up to date with main
- Run the app and test your endpoint(s)
- Link the PR to an issue:

```
Closes #23
```

This ensures GitHub automation can move the task in the board.

### PR Requirements:
- Small, focused PRs (don’t mix unrelated changes)
- Follow project architecture
- Clean code, no commented-out garbage
- Meaningful names & constants
- Error handling using AppError & ErrorType
- No SQL in controllers

### PR Review Expectations:
We review for:
- architectural correctness
- performance impact
- code safety
- naming consistency
- error handling
- maintainability
Be ready to make changes — reviews are strict but fair.

### 📚 Coding Standards
- ✔ Use std::string_view when correct
- ✔ RAII everywhere
- ✔ Avoid raw pointers
- ✔ No new unless wrapped in smart pointers
- ✔ Follow existing patterns (repository/service)
- ✔ No business logic in controllers
- ✔ No JSON in repositories
- ✔ Prefer const everywhere
- ✔ Always handle async DB errors properly

### 🧩 Adding New Features
Every new feature must include:
- DTO definitions (if needed)
- Repository methods (SQL only)
- Service logic (validation, orchestration)
- Controller handlers (thin, HTTP-only)
- ErrorType mapping
- Updated Project Board / Issue linkage
If you break the architecture, PR will be rejected.

### 🛡 Security Guidelines
- Use Argon2id for password hashing
- Never log passwords or tokens
- Don’t expose raw DB errors
- Validate all user input in Services
- Repositories must catch SQL exceptions
- Tokens must be generated using secure RNG

### 🔔 Need Help?
Open a GitHub Discussion or Issue:
We reply fast.
Contributions are welcome — even small improvements.
Together we’re building a modern, clean, maintainable C++ backend.
Thank you!

---