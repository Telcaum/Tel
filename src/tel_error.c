#include <stdio.h>
#include <string.h>

#include "tel_error.h"
#include "tel_memory.h"


void Tel_initErrorLogger(Tel_ErrorLogger *self, char const *source) {
  *self = (Tel_ErrorLogger){};
  self->source = source;
  self->logged_an_error = false;
}


/* static void printTokenContent(FILE *stream, Tel_ErrorLogger *self, Tel_Token token) {
  fprintf(stream, "%.*s", token.length, &self->source[token.start]);
} */


void Tel_logError(Tel_ErrorLogger *self, char const *msg, Tel_Token token) {
  self->logged_an_error = true;

  /* fprintf(stderr, "Error line %d near '", token.line);
  printTokenContent(stderr, self, token);
  fprintf(stderr, "': %s\n", msg); */
  fprintf(stderr, "Error line %d: %s\n", token.line, msg);

  char const *line_start = &self->source[token.start];
  char const *line_end = line_start;

  for (;;) {
    if (line_start[0] == '\n') {
      line_start++;
      break;
    }

    if (line_start == self->source) break;

    line_start--;
  }

  for (;;) {
    if (*line_end == '\0' || *line_end == '\n') break;

    line_end++;
  }

  int line_length = line_end - line_start;

  fprintf(stderr, "%05d | %.*s\n", token.line, line_length, line_start);

  char *underline_buffer = TEL_ALLOCN(char, line_length + 1);
  TEL_CHECK_OOM(underline_buffer);

  memset(underline_buffer, ' ', line_length);
  underline_buffer[line_length] = '\0';

  int underline_start = token.start - (line_start - self->source);

  memset(&underline_buffer[underline_start], '^', token.length);

  fprintf(stderr, "      | %s\n", underline_buffer);

  TEL_FREE(underline_buffer);
}
