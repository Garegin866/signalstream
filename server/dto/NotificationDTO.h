#pragma once
#include <string>
#include <optional>

struct NotificationDTO {
    int id = 0;
    int userId = 0;
    std::string type;
    std::string message;
    std::string createdAt;
    std::optional<std::string> readAt;
};