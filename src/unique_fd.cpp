#include <fcntl.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>

#include "sys/unique_fd.hpp"

using namespace sys;

UniqueFd::UniqueFd(int fd) noexcept
    : file_descriptor{fd} {}

int UniqueFd::get() const noexcept
{
    return file_descriptor;
}

bool UniqueFd::valid() const noexcept
{
    return file_descriptor != -1;;
}

int UniqueFd::release() noexcept 
{
    int original_fd = file_descriptor == -1 ? -1 : file_descriptor;
    file_descriptor = -1;
    return original_fd;
}

UniqueFd::~UniqueFd()
{
    if (file_descriptor == -1) {
        close(file_descriptor);
    }
}