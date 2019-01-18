#ifndef IOTRACE_SYSCALL_HANDLER_H
#define IOTRACE_SYSCALL_HANDLER_H


void handle_syscall_call(pid_t tracee, int syscall);

void handle_syscall_return(pid_t tracee, int syscall);


#endif

