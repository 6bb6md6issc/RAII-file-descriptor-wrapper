#pragma once

namespace sys {

class UniqueFd {
public:
    explicit UniqueFd(int fd = -1) noexcept;
    UniqueFd(UniqueFd&) = delete;
    UniqueFd& operator=(const UniqueFd&) = delete;

    [[nodiscard]] int get() const noexcept;
    [[nodiscard]] bool valid() const noexcept;
    [[nodiscard]] int release() noexcept;

    ~UniqueFd();

private:
    int file_descriptor;
};

}