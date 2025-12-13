# 📥 Pull Request

## 🎯 Summary
<!-- What does this PR do? Short explanation. -->
Fixes #

---

## 🧩 Changes
<!-- Bullet list of all major changes -->

- [ ]
- [ ]
- [ ]

---

## 🔍 Architecture Impact
<!-- Describe which layers were touched and how -->

- [ ] Controller
- [ ] Service
- [ ] Repository
- [ ] DTO
- [ ] Middleware
- [ ] Core utilities
- [ ] Config / Constants

**Explain why changes were needed:**
<!-- E.g., validation moved to service, repository now async, etc. -->

---

## 🧪 How to Test
<!-- Add step-by-step instructions or Postman requests -->

Example:

1. Start server & DB
2. Hit `/auth/login` with:
```json
{ "email": "test@test.com", "password": "12345" }
```
3. Verify token returned
4. Call protected endpoint with:
```
Authorization: Bearer <token>
```

If new endpoints:

```
POST /items
GET /feed
```

---

## 🔐 Security Considerations
<!-- Ensure no sensitive data leaks, middleware works, validations correct -->

- [ ] Input validation handled in service
- [ ] No SQL in controllers
- [ ] Errors sanitized (`AppError`)
- [ ] TokenMiddleware still functions
- [ ] No sensitive logs added

---

## 🧹 Code Quality Checklist
- [ ] No duplicated strings (uses `Const::*`)
- [ ] Repository methods contain no business logic
- [ ] Service is pure orchestration + validation
- [ ] Controller is thin and clean
- [ ] Naming follows project conventions
- [ ] All new code is async where required
- [ ] No raw SQL in controllers

---

## 📝 Additional Notes
<!-- Optional: anything reviewers must know -->