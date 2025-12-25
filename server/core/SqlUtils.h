#pragma once

#include <vector>
#include <string>

namespace SqlUtils {

    inline std::string toPgIntArrayLiteral(std::vector<int> ids) {
        std::sort(ids.begin(), ids.end());
        ids.erase(std::unique(ids.begin(), ids.end()), ids.end());

        std::string s = "{";
        for (size_t i = 0; i < ids.size(); ++i) {
            if (i) s += ",";
            s += std::to_string(ids[i]);
        }
        s += "}";
        return s;
    }

} // namespace SqlUtils