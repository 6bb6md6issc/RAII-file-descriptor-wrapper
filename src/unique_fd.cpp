#include <fcntl.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>

#include "sys/unique_fd.hpp"

using namespace sys;

UniqueFd::UniqueFd(int fd) noexcept
    : file_descriptor{fd} {}

UniqueFd::UniqueFd(UniqueFd&& original_fd) noexcept 
    : file_descriptor{original_fd.release()} {}

UniqueFd& UniqueFd::operator=(UniqueFd&& original_fd) noexcept {
    if (this != &original_fd){
        if (file_descriptor != -1) {
            close(file_descriptor);
        }
        file_descriptor = original_fd.release();
    }
    return *this;
}

int UniqueFd::get() const noexcept
{
    return file_descriptor;
}

bool UniqueFd::valid() const noexcept
{
    return file_descriptor != -1;
}

int UniqueFd::release() noexcept 
{
    int original_fd = file_descriptor;
    file_descriptor = -1;
    return original_fd;
}

UniqueFd::~UniqueFd()
{
    if (file_descriptor != -1) {
        close(file_descriptor);
    }
}