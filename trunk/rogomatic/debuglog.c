#include <stdarg.h>
#include <stdio.h>

#define MAXLINE 4096

FILE *debug = NULL;

static void
err_doit(int errnoflag, int error, const char *fmt, va_list ap)
{
  char buf[MAXLINE];

  vsnprintf(buf, MAXLINE, fmt, ap);
  if (debug != NULL)
    {
      fputs(buf, debug);
      fflush (debug);
    }
  else
    {
      fputs (buf, stderr);
      fflush (stderr);
    }
}

void debuglog_open (const char *log)
{
  debug = fopen (log, "w");
}

void debuglog_close (void)
{
  fclose (debug);
}

void debuglog (const char *fmt, ...)
{
  va_list  ap;
  char buf[MAXLINE];

  va_start (ap, fmt);
  err_doit (0, 0, fmt, ap);
  va_end (ap);
}


