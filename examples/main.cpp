#include "sys/unique_fd.hpp"
#include <fcntl.h>
#include <unistd.h>

int main() {
    sys::UniqueFd file{
        open("/dev/null", O_RDONLY)
    };

    if (!file.valid()) {
        return 1;
    }
}