#pragma once

#include <optional>

enum class Status {
    Ok,
    Error,
    NotFound,
    InvalidInput,
};

template <typename V>
struct Result {
    Status status;
    std::optional<V> value;

    Result(Status s, V val) : status(s), value(val) {}
    Result(Status s) : status(s), value(std::nullopt) {}

    bool isOk() const { return status == Status::Ok && value.has_value(); }
};