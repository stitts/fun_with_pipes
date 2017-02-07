//#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PRINT_TO_STREAM(stream, ...) fprintf(stream, "%d: ", fileno(stream)); fprintf(stream, __VA_ARGS__)

struct buffered_pipe {
  int m_fd[2];
  int m_old_fd;
  int m_stream_fd;
  FILE* m_stream;
  size_t m_buffer_len;
  char* m_buffer;

  buffered_pipe(FILE* stream, size_t buffer_len = 4096)
    : m_old_fd(-1),
      m_stream_fd(fileno(stream)),
      m_stream(stream),
      m_buffer_len(buffer_len),
      m_buffer(NULL) {
    m_buffer = new char[buffer_len];
  }

  int init() {
    if (pipe(m_fd) == -1) {
      perror("problem creating pipe: ");
      return 1;
    }
    m_old_fd = dup(m_stream_fd);
    dup2(m_fd[1], m_stream_fd);
    return 0;
  }

  const char* read() {
    ssize_t read_len;
    fflush(m_stream);
    if ((read_len = ::read(m_fd[0], m_buffer, m_buffer_len)) == -1) {
       perror("read error: ");
       return NULL;
    }
    m_buffer[read_len] = '\0';
    return m_buffer;
  }

  ~buffered_pipe() {
    close(m_fd[0]);
    close(m_fd[1]);
    // restore
    dup2(m_old_fd, m_stream_fd);
  }
};


int main() {
  // stdout example
  PRINT_TO_STREAM(stdout, "stdout example: creating a buffered pipe of stdout\n");
  buffered_pipe* b = new buffered_pipe(stdout);
  if (b->init()) {
    fprintf(stderr, "problem creating buffered_pipe\n");
    delete b;
    return 1;
  }

  const char* so_text = "wow some stdout text";
  PRINT_TO_STREAM(stderr, "printing '%s' to stdout\n", so_text);
  PRINT_TO_STREAM(stdout, "%s", so_text);
  PRINT_TO_STREAM(stderr, "buffer contents: '%s'\n", b->read());

  PRINT_TO_STREAM(stderr, "closing buffered pipe\n");
  delete b;
  PRINT_TO_STREAM(stdout, "printing to stdout again\n");

  // stderr name
  printf("\n\n");
  PRINT_TO_STREAM(stderr, "stderr example: creating a buffered pipe on stderr\n");
  b = new buffered_pipe(stderr);
  if (b->init()) {
    fprintf(stderr, "problem creating buffered_pipe\n");
    delete b;
    return 1;
  }
  const char* se_text = "wow some stderr text";
  PRINT_TO_STREAM(stdout, "printing '%s' to stderr\n", se_text);
  PRINT_TO_STREAM(stderr, "%s", se_text);
  PRINT_TO_STREAM(stdout, "buffer contents: '%s'\n", b->read());

  PRINT_TO_STREAM(stdout, "closing buffered pipe\n");
  delete b;
  PRINT_TO_STREAM(stderr, "printing to stderr again\n");


  return 0;
}
