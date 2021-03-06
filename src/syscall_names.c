/*
 * Copyright (C) 2019 HLRS, University of Stuttgart
 * <https://www.hlrs.de/>, <https://www.uni-stuttgart.de/>
 *
 * This file is part of iotrace.
 *
 * iotrace is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * iotrace is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with iotrace.  If not, see <https://www.gnu.org/licenses/>.
 *
 * The following people contributed to the project (in alphabetic order
 * by surname):
 *
 * - Nicolas Gross <https://github.com/nicolasgross>
 */


#include "syscall_names.h"


char const *const syscall_names[] = {
	"read",		// nr: 0, abi: common
	"write",		// nr: 1, abi: common
	"open",		// nr: 2, abi: common
	"close",		// nr: 3, abi: common
	"stat",		// nr: 4, abi: common
	"fstat",		// nr: 5, abi: common
	"lstat",		// nr: 6, abi: common
	"poll",		// nr: 7, abi: common
	"lseek",		// nr: 8, abi: common
	"mmap",		// nr: 9, abi: common
	"mprotect",		// nr: 10, abi: common
	"munmap",		// nr: 11, abi: common
	"brk",		// nr: 12, abi: common
	"rt_sigaction",		// nr: 13, abi: 64
	"rt_sigprocmask",		// nr: 14, abi: common
	"rt_sigreturn",		// nr: 15, abi: 64
	"ioctl",		// nr: 16, abi: 64
	"pread64",		// nr: 17, abi: common
	"pwrite64",		// nr: 18, abi: common
	"readv",		// nr: 19, abi: 64
	"writev",		// nr: 20, abi: 64
	"access",		// nr: 21, abi: common
	"pipe",		// nr: 22, abi: common
	"select",		// nr: 23, abi: common
	"sched_yield",		// nr: 24, abi: common
	"mremap",		// nr: 25, abi: common
	"msync",		// nr: 26, abi: common
	"mincore",		// nr: 27, abi: common
	"madvise",		// nr: 28, abi: common
	"shmget",		// nr: 29, abi: common
	"shmat",		// nr: 30, abi: common
	"shmctl",		// nr: 31, abi: common
	"dup",		// nr: 32, abi: common
	"dup2",		// nr: 33, abi: common
	"pause",		// nr: 34, abi: common
	"nanosleep",		// nr: 35, abi: common
	"getitimer",		// nr: 36, abi: common
	"alarm",		// nr: 37, abi: common
	"setitimer",		// nr: 38, abi: common
	"getpid",		// nr: 39, abi: common
	"sendfile",		// nr: 40, abi: common
	"socket",		// nr: 41, abi: common
	"connect",		// nr: 42, abi: common
	"accept",		// nr: 43, abi: common
	"sendto",		// nr: 44, abi: common
	"recvfrom",		// nr: 45, abi: 64
	"sendmsg",		// nr: 46, abi: 64
	"recvmsg",		// nr: 47, abi: 64
	"shutdown",		// nr: 48, abi: common
	"bind",		// nr: 49, abi: common
	"listen",		// nr: 50, abi: common
	"getsockname",		// nr: 51, abi: common
	"getpeername",		// nr: 52, abi: common
	"socketpair",		// nr: 53, abi: common
	"setsockopt",		// nr: 54, abi: 64
	"getsockopt",		// nr: 55, abi: 64
	"clone",		// nr: 56, abi: common
	"fork",		// nr: 57, abi: common
	"vfork",		// nr: 58, abi: common
	"execve",		// nr: 59, abi: 64
	"exit",		// nr: 60, abi: common
	"wait4",		// nr: 61, abi: common
	"kill",		// nr: 62, abi: common
	"uname",		// nr: 63, abi: common
	"semget",		// nr: 64, abi: common
	"semop",		// nr: 65, abi: common
	"semctl",		// nr: 66, abi: common
	"shmdt",		// nr: 67, abi: common
	"msgget",		// nr: 68, abi: common
	"msgsnd",		// nr: 69, abi: common
	"msgrcv",		// nr: 70, abi: common
	"msgctl",		// nr: 71, abi: common
	"fcntl",		// nr: 72, abi: common
	"flock",		// nr: 73, abi: common
	"fsync",		// nr: 74, abi: common
	"fdatasync",		// nr: 75, abi: common
	"truncate",		// nr: 76, abi: common
	"ftruncate",		// nr: 77, abi: common
	"getdents",		// nr: 78, abi: common
	"getcwd",		// nr: 79, abi: common
	"chdir",		// nr: 80, abi: common
	"fchdir",		// nr: 81, abi: common
	"rename",		// nr: 82, abi: common
	"mkdir",		// nr: 83, abi: common
	"rmdir",		// nr: 84, abi: common
	"creat",		// nr: 85, abi: common
	"link",		// nr: 86, abi: common
	"unlink",		// nr: 87, abi: common
	"symlink",		// nr: 88, abi: common
	"readlink",		// nr: 89, abi: common
	"chmod",		// nr: 90, abi: common
	"fchmod",		// nr: 91, abi: common
	"chown",		// nr: 92, abi: common
	"fchown",		// nr: 93, abi: common
	"lchown",		// nr: 94, abi: common
	"umask",		// nr: 95, abi: common
	"gettimeofday",		// nr: 96, abi: common
	"getrlimit",		// nr: 97, abi: common
	"getrusage",		// nr: 98, abi: common
	"sysinfo",		// nr: 99, abi: common
	"times",		// nr: 100, abi: common
	"ptrace",		// nr: 101, abi: 64
	"getuid",		// nr: 102, abi: common
	"syslog",		// nr: 103, abi: common
	"getgid",		// nr: 104, abi: common
	"setuid",		// nr: 105, abi: common
	"setgid",		// nr: 106, abi: common
	"geteuid",		// nr: 107, abi: common
	"getegid",		// nr: 108, abi: common
	"setpgid",		// nr: 109, abi: common
	"getppid",		// nr: 110, abi: common
	"getpgrp",		// nr: 111, abi: common
	"setsid",		// nr: 112, abi: common
	"setreuid",		// nr: 113, abi: common
	"setregid",		// nr: 114, abi: common
	"getgroups",		// nr: 115, abi: common
	"setgroups",		// nr: 116, abi: common
	"setresuid",		// nr: 117, abi: common
	"getresuid",		// nr: 118, abi: common
	"setresgid",		// nr: 119, abi: common
	"getresgid",		// nr: 120, abi: common
	"getpgid",		// nr: 121, abi: common
	"setfsuid",		// nr: 122, abi: common
	"setfsgid",		// nr: 123, abi: common
	"getsid",		// nr: 124, abi: common
	"capget",		// nr: 125, abi: common
	"capset",		// nr: 126, abi: common
	"rt_sigpending",		// nr: 127, abi: 64
	"rt_sigtimedwait",		// nr: 128, abi: 64
	"rt_sigqueueinfo",		// nr: 129, abi: 64
	"rt_sigsuspend",		// nr: 130, abi: common
	"sigaltstack",		// nr: 131, abi: 64
	"utime",		// nr: 132, abi: common
	"mknod",		// nr: 133, abi: common
	"uselib",		// nr: 134, abi: 64
	"personality",		// nr: 135, abi: common
	"ustat",		// nr: 136, abi: common
	"statfs",		// nr: 137, abi: common
	"fstatfs",		// nr: 138, abi: common
	"sysfs",		// nr: 139, abi: common
	"getpriority",		// nr: 140, abi: common
	"setpriority",		// nr: 141, abi: common
	"sched_setparam",		// nr: 142, abi: common
	"sched_getparam",		// nr: 143, abi: common
	"sched_setscheduler",		// nr: 144, abi: common
	"sched_getscheduler",		// nr: 145, abi: common
	"sched_get_priority_max",		// nr: 146, abi: common
	"sched_get_priority_min",		// nr: 147, abi: common
	"sched_rr_get_interval",		// nr: 148, abi: common
	"mlock",		// nr: 149, abi: common
	"munlock",		// nr: 150, abi: common
	"mlockall",		// nr: 151, abi: common
	"munlockall",		// nr: 152, abi: common
	"vhangup",		// nr: 153, abi: common
	"modify_ldt",		// nr: 154, abi: common
	"pivot_root",		// nr: 155, abi: common
	"_sysctl",		// nr: 156, abi: 64
	"prctl",		// nr: 157, abi: common
	"arch_prctl",		// nr: 158, abi: common
	"adjtimex",		// nr: 159, abi: common
	"setrlimit",		// nr: 160, abi: common
	"chroot",		// nr: 161, abi: common
	"sync",		// nr: 162, abi: common
	"acct",		// nr: 163, abi: common
	"settimeofday",		// nr: 164, abi: common
	"mount",		// nr: 165, abi: common
	"umount2",		// nr: 166, abi: common
	"swapon",		// nr: 167, abi: common
	"swapoff",		// nr: 168, abi: common
	"reboot",		// nr: 169, abi: common
	"sethostname",		// nr: 170, abi: common
	"setdomainname",		// nr: 171, abi: common
	"iopl",		// nr: 172, abi: common
	"ioperm",		// nr: 173, abi: common
	"create_module",		// nr: 174, abi: 64
	"init_module",		// nr: 175, abi: common
	"delete_module",		// nr: 176, abi: common
	"get_kernel_syms",		// nr: 177, abi: 64
	"query_module",		// nr: 178, abi: 64
	"quotactl",		// nr: 179, abi: common
	"nfsservctl",		// nr: 180, abi: 64
	"getpmsg",		// nr: 181, abi: common
	"putpmsg",		// nr: 182, abi: common
	"afs_syscall",		// nr: 183, abi: common
	"tuxcall",		// nr: 184, abi: common
	"security",		// nr: 185, abi: common
	"gettid",		// nr: 186, abi: common
	"readahead",		// nr: 187, abi: common
	"setxattr",		// nr: 188, abi: common
	"lsetxattr",		// nr: 189, abi: common
	"fsetxattr",		// nr: 190, abi: common
	"getxattr",		// nr: 191, abi: common
	"lgetxattr",		// nr: 192, abi: common
	"fgetxattr",		// nr: 193, abi: common
	"listxattr",		// nr: 194, abi: common
	"llistxattr",		// nr: 195, abi: common
	"flistxattr",		// nr: 196, abi: common
	"removexattr",		// nr: 197, abi: common
	"lremovexattr",		// nr: 198, abi: common
	"fremovexattr",		// nr: 199, abi: common
	"tkill",		// nr: 200, abi: common
	"time",		// nr: 201, abi: common
	"futex",		// nr: 202, abi: common
	"sched_setaffinity",		// nr: 203, abi: common
	"sched_getaffinity",		// nr: 204, abi: common
	"set_thread_area",		// nr: 205, abi: 64
	"io_setup",		// nr: 206, abi: 64
	"io_destroy",		// nr: 207, abi: common
	"io_getevents",		// nr: 208, abi: common
	"io_submit",		// nr: 209, abi: 64
	"io_cancel",		// nr: 210, abi: common
	"get_thread_area",		// nr: 211, abi: 64
	"lookup_dcookie",		// nr: 212, abi: common
	"epoll_create",		// nr: 213, abi: common
	"epoll_ctl_old",		// nr: 214, abi: 64
	"epoll_wait_old",		// nr: 215, abi: 64
	"remap_file_pages",		// nr: 216, abi: common
	"getdents64",		// nr: 217, abi: common
	"set_tid_address",		// nr: 218, abi: common
	"restart_syscall",		// nr: 219, abi: common
	"semtimedop",		// nr: 220, abi: common
	"fadvise64",		// nr: 221, abi: common
	"timer_create",		// nr: 222, abi: 64
	"timer_settime",		// nr: 223, abi: common
	"timer_gettime",		// nr: 224, abi: common
	"timer_getoverrun",		// nr: 225, abi: common
	"timer_delete",		// nr: 226, abi: common
	"clock_settime",		// nr: 227, abi: common
	"clock_gettime",		// nr: 228, abi: common
	"clock_getres",		// nr: 229, abi: common
	"clock_nanosleep",		// nr: 230, abi: common
	"exit_group",		// nr: 231, abi: common
	"epoll_wait",		// nr: 232, abi: common
	"epoll_ctl",		// nr: 233, abi: common
	"tgkill",		// nr: 234, abi: common
	"utimes",		// nr: 235, abi: common
	"vserver",		// nr: 236, abi: 64
	"mbind",		// nr: 237, abi: common
	"set_mempolicy",		// nr: 238, abi: common
	"get_mempolicy",		// nr: 239, abi: common
	"mq_open",		// nr: 240, abi: common
	"mq_unlink",		// nr: 241, abi: common
	"mq_timedsend",		// nr: 242, abi: common
	"mq_timedreceive",		// nr: 243, abi: common
	"mq_notify",		// nr: 244, abi: 64
	"mq_getsetattr",		// nr: 245, abi: common
	"kexec_load",		// nr: 246, abi: 64
	"waitid",		// nr: 247, abi: 64
	"add_key",		// nr: 248, abi: common
	"request_key",		// nr: 249, abi: common
	"keyctl",		// nr: 250, abi: common
	"ioprio_set",		// nr: 251, abi: common
	"ioprio_get",		// nr: 252, abi: common
	"inotify_init",		// nr: 253, abi: common
	"inotify_add_watch",		// nr: 254, abi: common
	"inotify_rm_watch",		// nr: 255, abi: common
	"migrate_pages",		// nr: 256, abi: common
	"openat",		// nr: 257, abi: common
	"mkdirat",		// nr: 258, abi: common
	"mknodat",		// nr: 259, abi: common
	"fchownat",		// nr: 260, abi: common
	"futimesat",		// nr: 261, abi: common
	"newfstatat",		// nr: 262, abi: common
	"unlinkat",		// nr: 263, abi: common
	"renameat",		// nr: 264, abi: common
	"linkat",		// nr: 265, abi: common
	"symlinkat",		// nr: 266, abi: common
	"readlinkat",		// nr: 267, abi: common
	"fchmodat",		// nr: 268, abi: common
	"faccessat",		// nr: 269, abi: common
	"pselect6",		// nr: 270, abi: common
	"ppoll",		// nr: 271, abi: common
	"unshare",		// nr: 272, abi: common
	"set_robust_list",		// nr: 273, abi: 64
	"get_robust_list",		// nr: 274, abi: 64
	"splice",		// nr: 275, abi: common
	"tee",		// nr: 276, abi: common
	"sync_file_range",		// nr: 277, abi: common
	"vmsplice",		// nr: 278, abi: 64
	"move_pages",		// nr: 279, abi: 64
	"utimensat",		// nr: 280, abi: common
	"epoll_pwait",		// nr: 281, abi: common
	"signalfd",		// nr: 282, abi: common
	"timerfd_create",		// nr: 283, abi: common
	"eventfd",		// nr: 284, abi: common
	"fallocate",		// nr: 285, abi: common
	"timerfd_settime",		// nr: 286, abi: common
	"timerfd_gettime",		// nr: 287, abi: common
	"accept4",		// nr: 288, abi: common
	"signalfd4",		// nr: 289, abi: common
	"eventfd2",		// nr: 290, abi: common
	"epoll_create1",		// nr: 291, abi: common
	"dup3",		// nr: 292, abi: common
	"pipe2",		// nr: 293, abi: common
	"inotify_init1",		// nr: 294, abi: common
	"preadv",		// nr: 295, abi: 64
	"pwritev",		// nr: 296, abi: 64
	"rt_tgsigqueueinfo",		// nr: 297, abi: 64
	"perf_event_open",		// nr: 298, abi: common
	"recvmmsg",		// nr: 299, abi: 64
	"fanotify_init",		// nr: 300, abi: common
	"fanotify_mark",		// nr: 301, abi: common
	"prlimit64",		// nr: 302, abi: common
	"name_to_handle_at",		// nr: 303, abi: common
	"open_by_handle_at",		// nr: 304, abi: common
	"clock_adjtime",		// nr: 305, abi: common
	"syncfs",		// nr: 306, abi: common
	"sendmmsg",		// nr: 307, abi: 64
	"setns",		// nr: 308, abi: common
	"getcpu",		// nr: 309, abi: common
	"process_vm_readv",		// nr: 310, abi: 64
	"process_vm_writev",		// nr: 311, abi: 64
	"kcmp",		// nr: 312, abi: common
	"finit_module",		// nr: 313, abi: common
	"sched_setattr",		// nr: 314, abi: common
	"sched_getattr",		// nr: 315, abi: common
	"renameat2",		// nr: 316, abi: common
	"seccomp",		// nr: 317, abi: common
	"getrandom",		// nr: 318, abi: common
	"memfd_create",		// nr: 319, abi: common
	"kexec_file_load",		// nr: 320, abi: common
	"bpf",		// nr: 321, abi: common
	"execveat",		// nr: 322, abi: 64
	"userfaultfd",		// nr: 323, abi: common
	"membarrier",		// nr: 324, abi: common
	"mlock2",		// nr: 325, abi: common
	"copy_file_range",		// nr: 326, abi: common
	"preadv2",		// nr: 327, abi: 64
	"pwritev2",		// nr: 328, abi: 64
	"pkey_mprotect",		// nr: 329, abi: common
	"pkey_alloc",		// nr: 330, abi: common
	"pkey_free",		// nr: 331, abi: common
	"statx",		// nr: 332, abi: common
	"io_pgetevents",		// nr: 333, abi: common
	"rseq",		// nr: 334, abi: common
	UNDEFINED_SYSCALL,		// nr: 335
	UNDEFINED_SYSCALL,		// nr: 336
	UNDEFINED_SYSCALL,		// nr: 337
	UNDEFINED_SYSCALL,		// nr: 338
	UNDEFINED_SYSCALL,		// nr: 339
	UNDEFINED_SYSCALL,		// nr: 340
	UNDEFINED_SYSCALL,		// nr: 341
	UNDEFINED_SYSCALL,		// nr: 342
	UNDEFINED_SYSCALL,		// nr: 343
	UNDEFINED_SYSCALL,		// nr: 344
	UNDEFINED_SYSCALL,		// nr: 345
	UNDEFINED_SYSCALL,		// nr: 346
	UNDEFINED_SYSCALL,		// nr: 347
	UNDEFINED_SYSCALL,		// nr: 348
	UNDEFINED_SYSCALL,		// nr: 349
	UNDEFINED_SYSCALL,		// nr: 350
	UNDEFINED_SYSCALL,		// nr: 351
	UNDEFINED_SYSCALL,		// nr: 352
	UNDEFINED_SYSCALL,		// nr: 353
	UNDEFINED_SYSCALL,		// nr: 354
	UNDEFINED_SYSCALL,		// nr: 355
	UNDEFINED_SYSCALL,		// nr: 356
	UNDEFINED_SYSCALL,		// nr: 357
	UNDEFINED_SYSCALL,		// nr: 358
	UNDEFINED_SYSCALL,		// nr: 359
	UNDEFINED_SYSCALL,		// nr: 360
	UNDEFINED_SYSCALL,		// nr: 361
	UNDEFINED_SYSCALL,		// nr: 362
	UNDEFINED_SYSCALL,		// nr: 363
	UNDEFINED_SYSCALL,		// nr: 364
	UNDEFINED_SYSCALL,		// nr: 365
	UNDEFINED_SYSCALL,		// nr: 366
	UNDEFINED_SYSCALL,		// nr: 367
	UNDEFINED_SYSCALL,		// nr: 368
	UNDEFINED_SYSCALL,		// nr: 369
	UNDEFINED_SYSCALL,		// nr: 370
	UNDEFINED_SYSCALL,		// nr: 371
	UNDEFINED_SYSCALL,		// nr: 372
	UNDEFINED_SYSCALL,		// nr: 373
	UNDEFINED_SYSCALL,		// nr: 374
	UNDEFINED_SYSCALL,		// nr: 375
	UNDEFINED_SYSCALL,		// nr: 376
	UNDEFINED_SYSCALL,		// nr: 377
	UNDEFINED_SYSCALL,		// nr: 378
	UNDEFINED_SYSCALL,		// nr: 379
	UNDEFINED_SYSCALL,		// nr: 380
	UNDEFINED_SYSCALL,		// nr: 381
	UNDEFINED_SYSCALL,		// nr: 382
	UNDEFINED_SYSCALL,		// nr: 383
	UNDEFINED_SYSCALL,		// nr: 384
	UNDEFINED_SYSCALL,		// nr: 385
	UNDEFINED_SYSCALL,		// nr: 386
	UNDEFINED_SYSCALL,		// nr: 387
	UNDEFINED_SYSCALL,		// nr: 388
	UNDEFINED_SYSCALL,		// nr: 389
	UNDEFINED_SYSCALL,		// nr: 390
	UNDEFINED_SYSCALL,		// nr: 391
	UNDEFINED_SYSCALL,		// nr: 392
	UNDEFINED_SYSCALL,		// nr: 393
	UNDEFINED_SYSCALL,		// nr: 394
	UNDEFINED_SYSCALL,		// nr: 395
	UNDEFINED_SYSCALL,		// nr: 396
	UNDEFINED_SYSCALL,		// nr: 397
	UNDEFINED_SYSCALL,		// nr: 398
	UNDEFINED_SYSCALL,		// nr: 399
	UNDEFINED_SYSCALL,		// nr: 400
	UNDEFINED_SYSCALL,		// nr: 401
	UNDEFINED_SYSCALL,		// nr: 402
	UNDEFINED_SYSCALL,		// nr: 403
	UNDEFINED_SYSCALL,		// nr: 404
	UNDEFINED_SYSCALL,		// nr: 405
	UNDEFINED_SYSCALL,		// nr: 406
	UNDEFINED_SYSCALL,		// nr: 407
	UNDEFINED_SYSCALL,		// nr: 408
	UNDEFINED_SYSCALL,		// nr: 409
	UNDEFINED_SYSCALL,		// nr: 410
	UNDEFINED_SYSCALL,		// nr: 411
	UNDEFINED_SYSCALL,		// nr: 412
	UNDEFINED_SYSCALL,		// nr: 413
	UNDEFINED_SYSCALL,		// nr: 414
	UNDEFINED_SYSCALL,		// nr: 415
	UNDEFINED_SYSCALL,		// nr: 416
	UNDEFINED_SYSCALL,		// nr: 417
	UNDEFINED_SYSCALL,		// nr: 418
	UNDEFINED_SYSCALL,		// nr: 419
	UNDEFINED_SYSCALL,		// nr: 420
	UNDEFINED_SYSCALL,		// nr: 421
	UNDEFINED_SYSCALL,		// nr: 422
	UNDEFINED_SYSCALL,		// nr: 423
	UNDEFINED_SYSCALL,		// nr: 424
	UNDEFINED_SYSCALL,		// nr: 425
	UNDEFINED_SYSCALL,		// nr: 426
	UNDEFINED_SYSCALL,		// nr: 427
	UNDEFINED_SYSCALL,		// nr: 428
	UNDEFINED_SYSCALL,		// nr: 429
	UNDEFINED_SYSCALL,		// nr: 430
	UNDEFINED_SYSCALL,		// nr: 431
	UNDEFINED_SYSCALL,		// nr: 432
	UNDEFINED_SYSCALL,		// nr: 433
	UNDEFINED_SYSCALL,		// nr: 434
	UNDEFINED_SYSCALL,		// nr: 435
	UNDEFINED_SYSCALL,		// nr: 436
	UNDEFINED_SYSCALL,		// nr: 437
	UNDEFINED_SYSCALL,		// nr: 438
	UNDEFINED_SYSCALL,		// nr: 439
	UNDEFINED_SYSCALL,		// nr: 440
	UNDEFINED_SYSCALL,		// nr: 441
	UNDEFINED_SYSCALL,		// nr: 442
	UNDEFINED_SYSCALL,		// nr: 443
	UNDEFINED_SYSCALL,		// nr: 444
	UNDEFINED_SYSCALL,		// nr: 445
	UNDEFINED_SYSCALL,		// nr: 446
	UNDEFINED_SYSCALL,		// nr: 447
	UNDEFINED_SYSCALL,		// nr: 448
	UNDEFINED_SYSCALL,		// nr: 449
	UNDEFINED_SYSCALL,		// nr: 450
	UNDEFINED_SYSCALL,		// nr: 451
	UNDEFINED_SYSCALL,		// nr: 452
	UNDEFINED_SYSCALL,		// nr: 453
	UNDEFINED_SYSCALL,		// nr: 454
	UNDEFINED_SYSCALL,		// nr: 455
	UNDEFINED_SYSCALL,		// nr: 456
	UNDEFINED_SYSCALL,		// nr: 457
	UNDEFINED_SYSCALL,		// nr: 458
	UNDEFINED_SYSCALL,		// nr: 459
	UNDEFINED_SYSCALL,		// nr: 460
	UNDEFINED_SYSCALL,		// nr: 461
	UNDEFINED_SYSCALL,		// nr: 462
	UNDEFINED_SYSCALL,		// nr: 463
	UNDEFINED_SYSCALL,		// nr: 464
	UNDEFINED_SYSCALL,		// nr: 465
	UNDEFINED_SYSCALL,		// nr: 466
	UNDEFINED_SYSCALL,		// nr: 467
	UNDEFINED_SYSCALL,		// nr: 468
	UNDEFINED_SYSCALL,		// nr: 469
	UNDEFINED_SYSCALL,		// nr: 470
	UNDEFINED_SYSCALL,		// nr: 471
	UNDEFINED_SYSCALL,		// nr: 472
	UNDEFINED_SYSCALL,		// nr: 473
	UNDEFINED_SYSCALL,		// nr: 474
	UNDEFINED_SYSCALL,		// nr: 475
	UNDEFINED_SYSCALL,		// nr: 476
	UNDEFINED_SYSCALL,		// nr: 477
	UNDEFINED_SYSCALL,		// nr: 478
	UNDEFINED_SYSCALL,		// nr: 479
	UNDEFINED_SYSCALL,		// nr: 480
	UNDEFINED_SYSCALL,		// nr: 481
	UNDEFINED_SYSCALL,		// nr: 482
	UNDEFINED_SYSCALL,		// nr: 483
	UNDEFINED_SYSCALL,		// nr: 484
	UNDEFINED_SYSCALL,		// nr: 485
	UNDEFINED_SYSCALL,		// nr: 486
	UNDEFINED_SYSCALL,		// nr: 487
	UNDEFINED_SYSCALL,		// nr: 488
	UNDEFINED_SYSCALL,		// nr: 489
	UNDEFINED_SYSCALL,		// nr: 490
	UNDEFINED_SYSCALL,		// nr: 491
	UNDEFINED_SYSCALL,		// nr: 492
	UNDEFINED_SYSCALL,		// nr: 493
	UNDEFINED_SYSCALL,		// nr: 494
	UNDEFINED_SYSCALL,		// nr: 495
	UNDEFINED_SYSCALL,		// nr: 496
	UNDEFINED_SYSCALL,		// nr: 497
	UNDEFINED_SYSCALL,		// nr: 498
	UNDEFINED_SYSCALL,		// nr: 499
	UNDEFINED_SYSCALL,		// nr: 500
	UNDEFINED_SYSCALL,		// nr: 501
	UNDEFINED_SYSCALL,		// nr: 502
	UNDEFINED_SYSCALL,		// nr: 503
	UNDEFINED_SYSCALL,		// nr: 504
	UNDEFINED_SYSCALL,		// nr: 505
	UNDEFINED_SYSCALL,		// nr: 506
	UNDEFINED_SYSCALL,		// nr: 507
	UNDEFINED_SYSCALL,		// nr: 508
	UNDEFINED_SYSCALL,		// nr: 509
	UNDEFINED_SYSCALL,		// nr: 510
	UNDEFINED_SYSCALL,		// nr: 511
	"rt_sigaction",		// nr: 512, abi: x32
	"rt_sigreturn",		// nr: 513, abi: x32
	"ioctl",		// nr: 514, abi: x32
	"readv",		// nr: 515, abi: x32
	"writev",		// nr: 516, abi: x32
	"recvfrom",		// nr: 517, abi: x32
	"sendmsg",		// nr: 518, abi: x32
	"recvmsg",		// nr: 519, abi: x32
	"execve",		// nr: 520, abi: x32
	"ptrace",		// nr: 521, abi: x32
	"rt_sigpending",		// nr: 522, abi: x32
	"rt_sigtimedwait",		// nr: 523, abi: x32
	"rt_sigqueueinfo",		// nr: 524, abi: x32
	"sigaltstack",		// nr: 525, abi: x32
	"timer_create",		// nr: 526, abi: x32
	"mq_notify",		// nr: 527, abi: x32
	"kexec_load",		// nr: 528, abi: x32
	"waitid",		// nr: 529, abi: x32
	"set_robust_list",		// nr: 530, abi: x32
	"get_robust_list",		// nr: 531, abi: x32
	"vmsplice",		// nr: 532, abi: x32
	"move_pages",		// nr: 533, abi: x32
	"preadv",		// nr: 534, abi: x32
	"pwritev",		// nr: 535, abi: x32
	"rt_tgsigqueueinfo",		// nr: 536, abi: x32
	"recvmmsg",		// nr: 537, abi: x32
	"sendmmsg",		// nr: 538, abi: x32
	"process_vm_readv",		// nr: 539, abi: x32
	"process_vm_writev",		// nr: 540, abi: x32
	"setsockopt",		// nr: 541, abi: x32
	"getsockopt",		// nr: 542, abi: x32
	"io_setup",		// nr: 543, abi: x32
	"io_submit",		// nr: 544, abi: x32
	"execveat",		// nr: 545, abi: x32
	"preadv2",		// nr: 546, abi: x32
};

