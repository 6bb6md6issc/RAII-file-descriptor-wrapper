#pragma once

namespace sys {

class UniqueFd {
public:
    explicit UniqueFd(int fd = -1) noexcept;
    UniqueFd(const UniqueFd&) = delete;
    UniqueFd& operator=(const UniqueFd&) = delete;
    UniqueFd(UniqueFd&& original_fd) noexcept;
    UniqueFd& operator=(UniqueFd&& original_fd) noexcept;


    [[nodiscard]] int get() const noexcept;
    [[nodiscard]] bool valid() const noexcept;
    [[nodiscard]] int release() noexcept;

    ~UniqueFd();

private:
    int file_descriptor;
};

}