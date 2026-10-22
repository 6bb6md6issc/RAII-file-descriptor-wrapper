#include <fcntl.h>
#include <sys/socket.h>
#include <errno.h>

export module UniqueFd

export class UniqueFd {
public:
    explicit UniqueFd(int fd = -1) nonexcept;
    UniqueFd(UniqueFd&) = delete;
    UniqueFd& operator=() = delete;

    [[nodiscard]] int get() const noexcept;
    [[nodiscard]] bool valid() const noexcept;
    [[nodiscard]] int release() noexcept;

    ~UniqueFd();

private:
    int file_descriptor;
}

UniqueFd::UniqueFd(int fd = -1)
    : file_descriptor{fd} {}

UniqueFd::get() 
{
    return this.file_descriptor;
}

UniqueFd::valid()
{
    if(fcntl(this.file_descriptor, F_GETFD) < 0) {
        if (errno == EBADF) {
            return false;
        }
    }
    return true;
}

UniqueFd::release()
{
    int original_fd = this.unique_fd == -1 ? -1 : this.unique_fd;
    this.unique_fd = -1;
    return original_fd;
}

UniqueFd::~UniqueFd()
{
    if (this.valid()) {
        close(this.unique_fd);
    }
}