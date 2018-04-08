#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *read_line (char *buf, size_t length, FILE *f)
  /**** Read at most 'length'-1 characters from the file 'f' into
        'buf' and zero-terminate this character sequence. If the
        line contains more characters, discard the rest.
   */
{
  char *p;

  if (p = fgets (buf, length, f)) {
    size_t last = strlen (buf) - 1;

    if (buf[last] == '\n') {
      /**** Discard the trailing newline */
      buf[last] = '\0';
    } else {
      /**** There's no newline in the buffer, therefore there must be
            more characters on that line: discard them!
       */
      fscanf (f, "%*[^\n]");
      /**** And also discard the newline... */
      (void) fgetc (f);
    } /* end if */
  } /* end if */
  return p;
} /* end read_line */

/*
				//I prefer 
				int_ptr_t label(new int(componentID));
				e->from->label = e->to->label = label;
				//to this:
				//e->from->label = e->to->label = make_shared<int>(componentID);
				//because I can switch between int_ptr_t to simple int* during
				//debugging. Alas, make_shared version is more efficient
*/

