# iotrace
iotrace is a tracing tool that can provide information about the I/O behavior
of a program. It builds upon the ptrace interface of the Linux kernel and is
capable of collecting the following statistics:
- The number of times a file was opened/closed/read/written.
- The time spent waiting for open/close/read/write to complete for each file.
- The read/write speed for each file.
- The block sizes read/written for each file.
- The time spent waiting for other syscalls to complete.

[iotrace-GUI](https://github.com/nicolasgross/iotrace-GUI) is a complementary
tool for viewing/filtering/merging the trace results.


iotrace was developed by [Nicolas Gross](https://github.com/nicolasgross) in
the course of a student assistant job at the [HLRS](https://www.hlrs.de) under
[Christoph Niethammer](https://github.com/cniethammer).


## Dependencies
- iotrace currently runs only on 64-bit Linux
- cmake
- pkg-config
- libglib-2.0
- libjson-glib-1.0


## Build
1. Clone repository:
	- `git clone https://github.com/nicolasgross/iotrace`
2. Create build directory and `cd` into it:
	- `mkdir YOUR_BUILD_DIR`
	- `cd YOUR_BUILD_DIR`
3. Build iotrace:
	- `cmake PATH_TO_CLONED_REPOSITORY`
	- `make iotrace`
4. Done, there should be an executable called `iotrace` in the build directory.


## Usage
- `[IOTRACE_OPTIONS]`: Options for iotrace.
- `TRACE_ID`: The ID for this trace, choose one. (String)
- `YOUR_PROGRAM`: The program that should be traced.
- `[YOUR_PROGRAM_ARGUMENTS]`: The arguments for the program that should be traced.
- `[MPIRUN_ARGUMENTS]`: The arguments for `mpirun`.

#### Trace normal program
`iotrace [IOTRACE_OPTIONS] TRACE_ID YOUR_PROGRAM [YOUR_PROGRAM_ARGUMENTS]`

#### Trace MPI program
`mpirun [MPIRUN_ARGUMENTS] iotrace [IOTRACE_OPTIONS] TRACE_ID YOUR_PROGRAM [YOUR_PROGRAM_ARGUMENTS]`


#### Options
You can use the `-v` or `--verbose` flag to print the trace results also to the
command line.


## Output
iotrace generates JSON files, which contain the results of the trace. They are
named in the scheme `TRACE-ID_HOSTNAME_RANK.json` and can be
viewed/filtered/merged with
[iotrace-GUI](https://github.com/nicolasgross/iotrace-GUI).

The output files are formatted as follows:
- open : [ 'count', 'total nanosecs', 'min nanosecs', 'max nanosecs' ]
- close : [ 'count', 'total nanosecs', 'min nanosecs', 'max nanosecs' ]
- read : [ 'total bytes', 'total nanosecs', 'min byte/sec', 'max byte/sec' ]
- read-blocks : [ [ 'number of bytes', 'count' ], ... ]
- write : [ 'total bytes', 'total nanosecs', 'min byte/sec', 'max byte/sec' ]
- write-blocks : [ [ 'number of bytes', 'count' ], ... ]

#### Example:
```json
{
  "trace-id" : "testls",
  "hostname" : "l380y-arch-ng",
  "rank" : "4",
  "file statistics" : [
    {
      "filename" : "/usr/lib/openmpi/pmix/mca_pshmem_mmap.so",
      "open" : [
        5,
        55180,
        6577,
        14269
      ],
      "close" : [
        5,
        37835,
        4461,
        10151
      ],
      "read" : [
        832,
        27466,
        30291997.37857715,
        30291997.37857715
      ],
      "read-blocks" : [
        [
          832,
          1
        ]
      ],
      "write" : [
        122,
        70351,
        1633882.2323316459,
        1814140.0720545775
      ],
      "write-blocks" : [
        [
          51,
          1
        ],
        [
          71,
          1
        ]
      ]
    },
    {
      "filename" : "sys/bus/pci/devices/0000:00:1c.2/subsystem_device",
      "open" : [
        1,
        8829,
        8829,
        8829
      ],
      "close" : [
        1,
        5432,
        5432,
        5432
      ],
      "read" : [
        7,
        5765,
        1214223.7640936687,
        1214223.7640936687
      ],
      "read-blocks" : [
        [
          7,
          1
        ]
      ],
      "write" : [
        0,
        0,
        9999999999999.0,
        0.0
      ],
      "write-blocks" : [
      ]
    }
  ],
  "unmatched syscalls" : [
    {
      "syscall" : "brk",
      "count" : 3,
      "total ns" : 76477
    },
    {
      "syscall" : "fstat",
      "count" : 6,
      "total ns" : 152305
    }
  ]
}
```


## Resources
These are some resources that have been very helpful during development:
- [Mapping of syscall arguments to registers](http://blog.rchapman.org/posts/Linux_System_Call_Table_for_x86_64/)
- [Stackoverflow ptrace multithreaded application](https://stackoverflow.com/questions/5477976/how-to-ptrace-a-multi-threaded-application)
- [ptrace blog post](https://www.cyphar.com/blog/post/20160703-remainroot-ptrace-hell)
- [ptrace tips from the strace developers](https://github.com/strace/strace/blob/master/README-linux-ptrace)


## License
Copyright © 2019 HLRS, University of Stuttgart. iotrace is published under
the terms of GPL3.

