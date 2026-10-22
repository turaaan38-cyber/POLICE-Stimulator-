#pragma once
#include "patrol/parser.hpp"
#include <iosfwd>
#include <memory>
#include <vector>
namespace patrol {
class Runtime final {
public:
    explicit Runtime(std::ostream& output); ~Runtime(); Runtime(Runtime&&) noexcept; Runtime& operator=(Runtime&&) noexcept;
    Runtime(const Runtime&) = delete; Runtime& operator=(const Runtime&) = delete;
    void run(std::vector<StmtPtr>& statements);
private:
    class Implementation; std::unique_ptr<Implementation> implementation_;
};
} // namespace patrol
