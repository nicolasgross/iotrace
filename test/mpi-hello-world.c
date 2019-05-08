/*
 * Copyright (c) 2016-2017  Christoph Niethammer <niethammer@hlrs.de>
 * hello-world [stdout|mpi-io] [prefix]
 * hello-world [prefix]
 */

#include <stdio.h>
#include <string.h>

#include <mpi.h>


enum output_target_types { STDOUT, FILE_PER_PROCESS, MPI_FILE_IO };

#define MAX_HELLO_WORLD_STRING_LENGTH 256

int main(int argc, char *argv[]) {
  MPI_Comm comm = MPI_COMM_WORLD;
  int rank, size, ret;
  enum output_target_types output_target = STDOUT;
  FILE *fh = stdout;
  MPI_File mpi_fh;
  int mpi_amode = MPI_MODE_RDWR | MPI_MODE_CREATE;
  char mpi_datarep[128] = "native";
  char prefix[128];
  char filename[256];
  memset(prefix, '\0', 128);
  strncpy(prefix, "hello-world", 127);
  memset(filename, '\0', 256);

  MPI_Init(&argc, &argv);

  if (argc > 1) {
    if (strcmp("stdout", argv[1]) == 0) {
      output_target = STDOUT;
    } else if (strcmp("mpi-io", argv[1]) == 0) {
      output_target = MPI_FILE_IO;
      if (argc > 2) {
        strncpy(prefix, argv[2], 127);
      }
      if (argc > 3) {
          strncpy(mpi_datarep, argv[3], 127);
      }
    } else {
      output_target = FILE_PER_PROCESS;
      strncpy(prefix, argv[1], 127);
      prefix[127] = '\0';
    }
  }

  MPI_Comm_size(comm, &size);
  MPI_Comm_rank(comm, &rank);

  switch (output_target) {
  case STDOUT:
    fh = stdout;
    break;
  case FILE_PER_PROCESS:
    snprintf(filename, 255, "%s-r%d.txt", prefix, rank);
    printf("Rank %d of %d open file '%s'\n", rank, size, filename);
    fh = fopen(filename, "w+");
    break;
  case MPI_FILE_IO:
    snprintf(filename, 255, "%s-all.txt", prefix);
    printf("Rank %d of %d MPI_File_open '%s'\n", rank, size, filename);
    ret = MPI_File_open(comm, filename, mpi_amode, MPI_INFO_NULL, &mpi_fh);
    break;
  }

  switch (output_target) {
  case STDOUT:
  case FILE_PER_PROCESS:
    fprintf(fh, "Rank %d of %d: Hello World\n", rank, size);
    fflush(fh);
    break;
  case MPI_FILE_IO:
    if (MPI_SUCCESS == ret) {
      char my_hello_world_string[MAX_HELLO_WORLD_STRING_LENGTH];
      memset(my_hello_world_string, '\0', 256);
#if 0
      int disp = rank * MAX_HELLO_WORLD_STRING_LENGTH;

      MPI_File_set_view(mpi_fh, disp, MPI_CHAR, MPI_CHAR, mpi_datarep,
                        MPI_INFO_NULL);
# else
      int blocklength = MAX_HELLO_WORLD_STRING_LENGTH;
      int stride = blocklength * size;
      printf("stride: %d\n", stride);
      MPI_Datatype etype = MPI_CHAR;
      MPI_Datatype filetype;

      MPI_Type_contiguous(blocklength, etype, &filetype);
      MPI_Type_commit(&filetype);
      MPI_Offset disp = (MPI_Offset) blocklength * rank * sizeof(char);
      printf("[%d] disp: %lld\n", rank, disp);
      printf("[%d] datarep: %s\n", rank, mpi_datarep);
      int err = MPI_File_set_view(mpi_fh, disp, etype, filetype, mpi_datarep, MPI_INFO_NULL);
      printf("[%d] MPI_File_set_view returned %d\n", rank, err);
#endif
      int write_count = snprintf(my_hello_world_string, MAX_HELLO_WORLD_STRING_LENGTH,
                           "Rank %d of %d: Hello World\n", rank, size);
      printf("[%d] write count: %d\n", rank, write_count);
      write_count = write_count + 1; /* for \0 at end of string */
      MPI_File_write(mpi_fh, my_hello_world_string, write_count, MPI_CHAR, MPI_STATUS_IGNORE);
    }
    break;
  }
  MPI_Barrier(comm);

  switch (output_target) {
  case STDOUT:
    break;
  case FILE_PER_PROCESS:
    fclose(fh);
    break;
  case MPI_FILE_IO:
    MPI_File_close(&mpi_fh);
    break;
  }
  MPI_Finalize();

  return 0;
}
