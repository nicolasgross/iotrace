#ifndef IOTRACE_SYSCALL_HANDLER_H
#define IOTRACE_SYSCALL_HANDLER_H

#include "fd_table.h"


void handle_syscall_call(pid_t tracee, int sc);

void handle_syscall_return(pid_t tracee, int sc, GHashTable *syscall_table);


#endif

