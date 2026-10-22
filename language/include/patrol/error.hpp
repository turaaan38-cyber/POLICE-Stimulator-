#pragma once
#include <stdexcept>
#include <string>
namespace patrol {
class ScriptError final : public std::runtime_error {
public:
    ScriptError(int line, int column, const std::string& message)
        : std::runtime_error(message), line_(line), column_(column) {}
    [[nodiscard]] int line() const noexcept { return line_; }
    [[nodiscard]] int column() const noexcept { return column_; }
private:
    int line_, column_;
};
} // namespace patrol
