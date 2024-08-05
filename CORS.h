#pragma once
#include "crow_all.h"
#include <unordered_set>

class CORS {
public:
    struct context {};

    void before_handle(crow::request& req, crow::response& res, context& ctx) {
        static const std::unordered_set<std::string> allowed_origins = {
            "http://localhost:3000",
            "https://cutshionframework.github.io/Pionoid/",
            "https://cutshionframework.github.io"
        };

        auto origin = req.get_header_value("Origin");
        if (allowed_origins.find(origin) != allowed_origins.end()) {
            res.add_header("Access-Control-Allow-Origin", origin);
        }

        // Handle preflight OPTIONS requests
        if (req.method == crow::HTTPMethod::Options) {
            res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
            res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
            res.add_header("Access-Control-Allow-Credentials", "true");
            res.code = 204; // No Content
            res.end();
        }
    }

    void after_handle(crow::request& req, crow::response& res, context& ctx) {
        static const std::unordered_set<std::string> allowed_origins = {
            "http://localhost:3000",
            "https://cutshionframework.github.io/Pionoid/",
            "https://cutshionframework.github.io"
        };

        auto origin = req.get_header_value("Origin");
        if (allowed_origins.find(origin) != allowed_origins.end()) {
            res.add_header("Access-Control-Allow-Origin", origin);
        }

        if (req.method != crow::HTTPMethod::Options) {
            res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
            res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
            res.add_header("Access-Control-Allow-Credentials", "true");
        }
    }
};
