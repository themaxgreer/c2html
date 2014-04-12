/*
 * Filter that excludes bytes larger than 126.
 */
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  int c, n = 0;
  while ((c = fgetc(stdin)) != EOF)
    if (c <= 126)  /* only 7-bit ASCII allowed */
      fputc(c, stdout);
    else
      n++;
  if (n > 0)
    fprintf(stderr, "%d bytes filtered out!\n", n);
  return 0;
}
