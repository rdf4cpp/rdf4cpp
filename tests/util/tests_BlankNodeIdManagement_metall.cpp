#include "BlankNodeIdManagement_metall_common.hpp"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <random>

#include <cassert>
#include <unistd.h>
#include <sys/wait.h>

TEST_CASE("BlankNodeIdManagement metall") {
    std::string const path{"/tmp/" + std::to_string(std::random_device{}())};

    std::cout << "storage path: " << path << std::endl;

    auto pid = fork();
    assert(pid >= 0);

    if (pid == 0) {
        int st = execl("BlankNodeIdManagement_metall_phase1", "BlankNodeIdManagement_metall_phase1", path.data(), nullptr);
        assert(st == 0);
    } else {
        int rc;
        int st = waitpid(pid, &rc, 0);
        assert(st >= 0);
        assert(WIFEXITED(rc));
        assert(WEXITSTATUS(rc) == 0);
    }

    pid = fork();
    assert(pid >= 0);

    if (pid == 0) {
        int st = execl("BlankNodeIdManagement_metall_phase2", "BlankNodeIdManagement_metall_phase2", path.data(), nullptr);
        assert(st == 0);
    } else {
        int rc;
        int st = waitpid(pid, &rc, 0);
        assert(st >= 0);
        assert(WIFEXITED(rc));
        assert(WEXITSTATUS(rc) == 0);
    }
}
