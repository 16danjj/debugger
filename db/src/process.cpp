#include <libdb/process.hpp>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <libdb/error.hpp>

std::unique_ptr<db::process> db::process::launch(std::filesystem::path path) {
    pid_t pid;
    if ((pid = fork() < 0)) {
        error::send_errno("fork failed");
    }
    if (pid == 0) {
        if (ptrace(PTRACE_TRACEME, 0, nullptr, nullptr) < 0) {
            error::send_errno("Tracing failed");
        }

        if (execlp(path.c_str(), path.c_str(), nullptr) < 0) {
            error::send_errno("exec failed");
        }
    }

    std::unique_ptr<process> proc (new process(pid, /*terminate_on_end = */ true));
    proc->wait_on_signal();

    return proc;
}

std::unique_ptr<db::process> db::process::attach(pid_t pid) {
    if (pid == 0) {
        error::send("Invalid PID");
    }
    if (ptrace(PTRACE_ATTACH, pid, nullptr, nullptr) < 0) {
        error::send_errno("Could not attach");
    }

    std::unique_ptr<process> proc (new process(pid, /* terminate_on_end =*/false));
    proc->wait_on_signal();

    return proc;
}