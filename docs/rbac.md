# 🔐 SignalStream RBAC (Role-Based Access Control)

**Version:** v1.2  
**Roles:** `user`, `moderator`, `admin`

RBAC defines *who is allowed* to perform *which actions*.  
SignalStream uses a **3-level hierarchical permission model**:

```
user  <  moderator  <  admin
```

Higher roles inherit all permissions from lower roles.

---

## 🧑‍💼 Roles Overview

### 1️⃣ User (default)
Basic end-user.

**Can:**
- View own profile.
- Manage own tags.
- View feed.
- View items and tags.

**Cannot:**
- Create tags or items.
- Edit or delete items.
- Access admin endpoints.

---

### 2️⃣ Moderator
Content curator.

**Can:**
- Everything a User can do.
- Create tags.
- Create/edit/delete items.

**Cannot:**
- View all users.
- Change roles.
- Delete users.
- Access administrative tools.

---

### 3️⃣ Admin
Full system authority.

**Can:**
- Everything a Moderator can do.
- List all users.
- Change user roles.
- Access administrative endpoints.
- Manage infrastructure-level features (future).

---

## 📚 Endpoint Permission Matrix

| Endpoint | User | Moderator | Admin |
|---------|------|-----------|-------|
| **/auth/register** | ✔ | ✔ | ✔ |
| **/auth/login** | ✔ | ✔ | ✔ |
| **/auth/logout** | ✔ | ✔ | ✔ |
| **/auth/me** | ✔ | ✔ | ✔ |
| **GET /feed** | ✔ | ✔ | ✔ |
| **GET /tags** | ✔ | ✔ | ✔ |
| **POST /tags** | ❌ | ✔ | ✔ |
| **DELETE /tags/:id** | ❌ | ✔ | ✔ |
| **POST /items** | ❌ | ✔ | ✔ |
| **PUT /items/:id** | ❌ | ✔ | ✔ |
| **DELETE /items/:id** | ❌ | ✔ | ✔ |
| **POST /items/:id/tags** | ❌ | ✔ | ✔ |
| **GET /admin/users** | ❌ | ❌ | ✔ |
| **POST /admin/set-role** | ❌ | ❌ | ✔ |

---

# 🧩 RBAC Implementation Details

RBAC is enforced at the **controller entry level**, not in services or repositories.

### Benefits:
- Blocks unauthorized users early.
- Keeps services clean.
- Prevents duplicated permission checks.

---

# 🧱 RBAC Macros

Defined in `core/RequestContextHelpers.h`.

---

### ✔ REQUIRE_USER

Ensures the request contains authenticated user context.

```cpp
REQUIRE_USER(req, fcb);
```

Used for:
- `/auth/me`
- `/feed`
- `/user/tags/*`

---

### ✔ REQUIRE_ADMIN

Enforces admin-only access.

```cpp
REQUIRE_ADMIN(req, fcb);
```

Used for:
- `/admin/users`
- `/admin/set-role`

---

### ✔ REQUIRE_MOD_OR_ADMIN

For content-management endpoints.

```cpp
REQUIRE_MOD_OR_ADMIN(req, fcb);
```

Used for:
- Editing/creating/deleting items
- Creating/deleting tags
- Item-tag linking

---

# 🔧 Role Representation

### Enum
Located in `core/UserRole.h`:

```cpp
enum class UserRole {
    User,
    Moderator,
    Admin,
    Invalid
};
```

### Converters

```cpp
std::string toString(UserRole);
UserRole fromString(const std::string&);
```

---

# 🧠 Permission Philosophy

SignalStream RBAC follows **capability-based permissions**:

- Roles define capabilities.
- Controllers enforce capabilities.
- Services remain pure and reusable.
- Repositories never check permissions.

---

# 🧱 Role Hierarchy Summary

```
Admin
 └── Moderator
        └── User
```

Admins inherit moderator + user permissions.  
Moderators inherit user permissions.

---

# 🛡 Security Considerations

✔ Prevent admin from demoting themselves  
✔ All errors return consistent structured JSON  
✔ Role parsing is strict  
✔ Unauthorized access is blocked before DB calls

---