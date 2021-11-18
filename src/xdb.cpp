#include <vector>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sstream>
#include <iostream>

#include "linenoise.h"
#include "debugger.h"

using namespace xdb;

// split and is_prefix are a couple of small helper functions
std::vector<std::string> split(const std::string &s, char delimiter) {
  std::vector<std::string> out{};
  std::stringstream ss {s};
  std::string item;
 
  while (std::getline(ss, item, delimiter)) {
    out.push_back(item);
  }
 
  return out;
}

bool is_prefix(const std::string &s, const std::string &of) {
  if (s.size() > of.size())
    return false;
  return std::equal(s.begin(), s.end(), of.begin());
}

void debugger::handle_command(const std::string &line) {
  auto args = split(line, ' ');
  auto command = args[0];
 
  if (is_prefix(command, "continue")) {
    continue_execution();
  } else {
    std::cerr << "Unknown command\n";
  }
}

void debugger::run() {
  int wait_status;
  auto options = 0;
  waitpid(m_pid, &wait_status, options);
  char* line = nullptr;
 
  // get input until receive ctrl+d
  while ((line = linenoise("(xdb) ")) != nullptr) {
    handle_command(line);
    linenoiseHistoryAdd(line);
    // linenoiseFree(line);
  }
}

void debugger::continue_execution() {
  ptrace(PTRACE_CONT, m_pid, nullptr, nullptr);
  int wait_status;
  auto options = 0;
  waitpid(m_pid, &wait_status, options);
}

void execute_debugee(const std::string &prog_name) {
  if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) {
    std::cerr << "Error in ptrace\n";
    return;
  }
  execl(prog_name.c_str(), prog_name.c_str(), nullptr);
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Executable is not specified\n";
    return -1;
  }
  
  auto prog = argv[1];
  auto pid = fork();

  if (pid == 0) {
    // child process, debuggee
    // launch the debugee program with classic fork/exec pattern
    execute_debugee(prog);
  } else if (pid >= 1) {
    // parent process, debugger
    std::cout << "Started debugging process " << pid << '\n';
    debugger dbg{prog, pid}; // create class debugger to monit user input loop
    dbg.run();
  }
}
