#include <iostream>
#include <unistd.h>
#include <string_view>
#include <sys/ptrace.h>
#include <editline/readline.h>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <sys/types.h>
#include <sys/wait.h>
#include <libdb/process.hpp>

namespace {

    std::vector<std::string> split(std::string_view str, char delimiter) {
        std::vector<std::string> out{};
        std::stringstream ss {std::string{str}};
        std::string item;

        while (std::getline(ss, item, delimiter)) {
            out.push_back(item);
        }

        return out;
    }

    bool is_prefix(std::string_view str, std::string_view of) {
        if (str.size() > of.size()) return false;
        return std::equal(str.begin(), str.end(), of.begin());

    }

    void print_stop_reason(const db::process& process, db::stop_reason reason){
        std::cout << "Process " << process.pid() << ' ';

        switch(reason.reason) {
            case db::process_state::exited:
                std::cout << "exited with status " << static_cast<int>(reason.info);
                break;
            
            case db::process_state::terminated:
                std::cout << "terminated with signal " << sigabbrev_np(reason.info);
                break;
            
            case db::process_state::stopped:
                std::cout << "stopped with signal " << sigabbrev_np(reason.info);
                break;
        }

        std::cout << std::endl;
    }

    void handle_command(std::unique_ptr<db::process>& process, std::string_view line) {
        auto args = split(line, ' ');
        auto command = args[0];

        if(is_prefix(command, "continue")) {
            process->resume();
            process->wait_on_signal();
        } 
        else {
            std::cerr << "Unknown command\n";
        }
    }

    std::unique_ptr<db::process> attach(int argc, const char** argv) {
        pid_t pid = 0;
        //Passing PID
        if (argc == 3 && argv[1] == std::string_view("-p")) {
            pid = std::atoi(argv[2]);
            return db::process::attach(pid);
        }
        //Passing program name
        else {
            const char* program_path = argv[1];
            return db::process::launch(program_path);
        }
    }
}

int main(int argc, const char** argv) {
    if (argc == 1) {
        std::cerr << "No arguments given \n";
        return -1;
    }

    pid_t pid = attach(argc, argv);

    int wait_status;
    int options = 0;
    if (waitpid(pid, &wait_status, options) < 0) {
        std::perror("waitpid failed");
    }

    char* line = nullptr;
    while ((line = readline("db> ")) != nullptr) {
        std::string line_str;

        if (line == std::string_view("")) {
            free(line);
            if (history_length > 0) {
                line_str = history_list()[history_length - 1]->line;
            }
        }
        else {
            line_str = line;
            add_history(line);
            free(line);
        }

        if (!line_str.empty()) {
            handle_command(pid, line_str);
        }
    }
}