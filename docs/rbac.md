# 🔐 SignalStream RBAC (Role-Based Access Control)

**Roles:** `user`, `moderator`, `admin`

RBAC defines *who is allowed* to perform *which actions*.  
SignalStream uses a **3-level hierarchical permission model**:

```
user  <  moderator  <  admin
```

Higher roles inherit **all permissions** from lower roles.

---

## 🧑‍💼 Roles Overview

### 1️⃣ User (default)
Basic end-user.

**Can:**
- View own profile (`/auth/me`)
- Login / logout
- Manage own followed tags
- View feed
- View items and tags
- Receive and read notifications

**Cannot:**
- Register new users
- Create tags
- Create, edit, or delete items
- Access admin endpoints

---

### 2️⃣ Moderator
Content curator.

**Can:**
- Everything a **User** can do
- Create, edit, and delete items
- Attach tags to items
- Moderate content quality

**Cannot:**
- Create tags
- View all users
- Change roles
- Access administrative endpoints

---

### 3️⃣ Admin
Full system authority.

**Can:**
- Everything a **Moderator** can do
- Register new users
- Create tags
- List all users
- Promote/demote users
- View moderators
- Access all administrative endpoints
- Manage infrastructure-level features (future)

---

## 📚 Endpoint Permission Matrix

| Endpoint | User | Moderator | Admin |
|--------|------|-----------|-------|
| **POST /auth/register** | ❌ | ❌ | ✔ |
| **POST /auth/login** | ✔ | ✔ | ✔ |
| **POST /auth/logout** | ✔ | ✔ | ✔ |
| **GET /auth/me** | ✔ | ✔ | ✔ |
| **GET /feed** | ✔ | ✔ | ✔ |
| **GET /tags** | ✔ | ✔ | ✔ |
| **POST /tags** | ❌ | ❌ | ✔ |
| **POST /user/tags** | ✔ | ✔ | ✔ |
| **GET /user/tags** | ✔ | ✔ | ✔ |
| **POST /items** | ❌ | ✔ | ✔ |
| **PUT /items/:id** | ❌ | ✔ | ✔ |
| **DELETE /items/:id** | ❌ | ✔ | ✔ |
| **GET /items** | ✔ | ✔ | ✔ |
| **GET /items/:id** | ✔ | ✔ | ✔ |
| **GET /notifications** | ✔ | ✔ | ✔ |
| **POST /notifications/read** | ✔ | ✔ | ✔ |
| **GET /admin/users** | ❌ | ❌ | ✔ |
| **GET /admin/moderators** | ❌ | ❌ | ✔ |
| **POST /admin/set-role** | ❌ | ❌ | ✔ |

---

## 🧩 RBAC Enforcement Strategy

RBAC is enforced **only at controller entry level**.

✔ Controllers validate permissions  
✔ Services contain **zero role logic**  
✔ Repositories never check permissions

---

## 🧱 RBAC Macros

Defined in `core/RequestContextHelpers.h`.

### REQUIRE_AUTH_USER
```cpp
REQUIRE_AUTH_USER(req, cb, user);
```

### REQUIRE_ADMIN
```cpp
REQUIRE_ADMIN(req, cb);
```

### REQUIRE_MOD_OR_ADMIN
```cpp
REQUIRE_MOD_OR_ADMIN(req, cb);
```

---

## 🔧 Role Representation

```cpp
enum class UserRole {
    User,
    Moderator,
    Admin,
    Invalid
};
```

---

## 🛡 Security Guarantees

✔ Admin cannot demote themselves  
✔ Unauthorized access rejected before DB calls  
✔ Consistent JSON errors  
✔ No role strings outside RoleService