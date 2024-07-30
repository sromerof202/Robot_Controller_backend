#pragma once
#include "crow_all.h"

class CORS {
public:
    struct context {};

    void before_handle(crow::request& req, crow::response& res, context& ctx) {
        // No action needed before handling the request
    }

    void after_handle(crow::request& req, crow::response& res, context& ctx) {
        res.add_header("Access-Control-Allow-Origin", "http://localhost:3000");
        res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.add_header("Access-Control-Allow-Credentials", "true");
    }
};
