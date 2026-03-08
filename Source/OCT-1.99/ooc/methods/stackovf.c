/*
 * stackovf.c -- SAS/C stack-overflow handler replacement.
 *
 * On overflow: print message, dump ObjC runtime state (all classes from
 * _oc_allCCB), then backtrace. Use write() only to limit stack.
 * Must call _oc_real_exit (not exit) because objc.h #defines exit->app_exit.
 */

#include <objc/objc.h>

#ifndef STDERR_FILENO
# define STDERR_FILENO 2
#endif
#ifdef __STDC__
extern int write(int fd, const void *buf, unsigned int nbytes);
#else
extern int write();
#endif

extern void _oc_real_exit(int status);

#define LINE_SIZE  120
#define MAX_FRAMES 24
#define HEX_DIGITS 8
static const char hexchars[] = "0123456789abcdef";

static const char msg[] = "OCT: stack overflow detected\n";
static const char objc_header[] = "ObjC runtime state (classes in _oc_allCCB):\n";
static const char no_fp[] = "(no frame pointer for backtrace)\n";
static const char bt_header[] = "backtrace (return addresses):\n";
static const char stack_dump_header[] = "stack dump (words above handler):\n";

extern char *_oc_allCCB[];

static void
write_cstr(int fd, const char *s)
{
  unsigned int n;

  n = 0;
  while (s[n] != '\0')
    n++;
  (void)write(fd, s, n);
}

static unsigned int
strlen_safe(const char *s, unsigned int max)
{
  unsigned int n;

  n = 0;
  while (n < max && s[n] != '\0')
    n++;
  return n;
}

static unsigned int
format_int(char *buf, int val)
{
  unsigned int len, i;
  char tmp[10];

  if (val < 0)
    {
      buf[0] = '-';
      val = -val;
      len = 1;
    }
  else
    len = 0;
  i = 0;
  do
    {
      tmp[i++] = (char)('0' + (val % 10));
      val = val / 10;
    }
  while (val != 0 && i < 10);
  while (i > 0)
    buf[len++] = tmp[--i];
  buf[len] = '\0';
  return len;
}

/* Format ptr as 0xXXXXXXXX into buf; return length. */
static unsigned int
format_ptr(char *buf, void *ptr)
{
  unsigned long v;
  int i;

  v = (unsigned long)ptr;
  buf[0] = '0';
  buf[1] = 'x';
  for (i = HEX_DIGITS - 1; i >= 0; i--)
    buf[2 + (HEX_DIGITS - 1 - i)] = hexchars[(v >> (i * 4)) & 0xf];
  return (unsigned int)(2 + HEX_DIGITS);
}

/* Return 1 if s looks like a valid C identifier (alphanumeric, underscore). */
static int
is_safe_name(const char *s, unsigned int maxlen)
{
  unsigned int n;

  if (s == (const char *)0)
    return 0;
  n = 0;
  while (n < maxlen && s[n] != '\0')
    {
      if ((s[n] >= 'a' && s[n] <= 'z') ||
          (s[n] >= 'A' && s[n] <= 'Z') ||
          (s[n] >= '0' && s[n] <= '9') || s[n] == '_')
        n++;
      else
        return 0;
    }
  return (int)(n > 0);
}

static unsigned int
format_frame(char *line, int n, unsigned long ret)
{
  int i;

  line[0] = ' ';
  line[1] = ' ';
  line[2] = '#';
  line[3] = (char)('0' + (n / 10));
  line[4] = (char)('0' + (n % 10));
  line[5] = ' ';
  line[6] = '0';
  line[7] = 'x';
  for (i = HEX_DIGITS - 1; i >= 0; i--)
    line[8 + (HEX_DIGITS - 1 - i)] = hexchars[(ret >> (i * 4)) & 0xf];
  line[8 + HEX_DIGITS] = '\n';
  return (unsigned int)(8 + HEX_DIGITS + 1);
}

/*
 * Dump all ObjC class state using real struct class_cb and objc_class.
 */
static void
dump_objc_state(int fd)
{
  struct class_cb *cb;
  struct objc_class *inst;
  char *name;
  int i;
  unsigned int n, len;
  static char line[LINE_SIZE];

  (void)write(fd, objc_header, sizeof(objc_header) - 1);
  for (i = 0; _oc_allCCB[i] != (char *)0; i++)
    {
      cb = (struct class_cb *)_oc_allCCB[i];
      if (cb == (struct class_cb *)0)
        continue;
      inst = cb->instance;
      if (inst == (struct objc_class *)0)
        continue;
      line[0] = ' ';
      line[1] = ' ';
      len = 2;
      len += format_int(line + len, i);
      line[len++] = ':';
      line[len++] = ' ';
      name = inst->name;
      if (name != (char *)0 && is_safe_name(name, 32u))
        {
          n = strlen_safe(name, 24u);
          if (len + n >= (unsigned int)LINE_SIZE)
            n = (unsigned int)LINE_SIZE - len - 1;
          for (; n > 0; n--, len++)
            line[len] = *name++;
        }
      else
        len += format_ptr(line + len, (void *)name);
      line[len++] = ' ';
      line[len++] = 'i';
      line[len++] = 's';
      line[len++] = 'a';
      line[len++] = '=';
      if (inst->isa != (Class)0 && inst->isa->name != (char *)0 &&
          is_safe_name(inst->isa->name, 32u))
        {
          name = inst->isa->name;
          n = strlen_safe(name, 20u);
          for (; n > 0; n--, len++)
            line[len] = *name++;
        }
      else
        len += format_ptr(line + len, (void *)inst->isa);
      line[len++] = ' ';
      line[len++] = 's';
      line[len++] = 'u';
      line[len++] = 'p';
      line[len++] = '=';
      if (inst->super_class != (Class)0 &&
          inst->super_class->name != (char *)0 &&
          is_safe_name(inst->super_class->name, 32u))
        {
          name = inst->super_class->name;
          n = strlen_safe(name, 20u);
          for (; n > 0; n--, len++)
            line[len] = *name++;
        }
      else
        len += format_ptr(line + len, (void *)inst->super_class);
      line[len++] = '\n';
      (void)write(fd, line, len);
    }
}

void
_CXOVF(void)
{
  unsigned long *fp;
  unsigned long ret;
  int fd;
  int i;
  unsigned int len;
  static char line[LINE_SIZE];

  fd = STDERR_FILENO;
  (void)write(fd, msg, sizeof(msg) - 1);

  dump_objc_state(fd);

#if defined(__GNUC__) && (defined(__mc68000__) || defined(__M68000__))
  {
    register unsigned long *a6 asm("a6");
    fp = a6;
  }
#elif defined(__builtin_frame_address)
  fp = (unsigned long *)__builtin_frame_address(0);
#elif defined(__SASC) || defined(__SASC__)
  /* SAS/C: try to get A6 via inline. Fallback to 0 if not available. */
  fp = (unsigned long *)0;
#else
  fp = (unsigned long *)0;
#endif

  if (fp != (unsigned long *)0)
    {
      write_cstr(fd, bt_header);
      for (i = 0; i < MAX_FRAMES; i++)
        {
          ret = fp[1];
          if (ret == 0)
            break;
          len = format_frame(line, i, ret);
          (void)write(fd, line, len);
          fp = (unsigned long *)fp[0];
          if (fp == (unsigned long *)0)
            break;
        }
    }
  else
    {
      write_cstr(fd, no_fp);
      /* Dump raw stack words below current frame (may contain return addrs). */
      {
        unsigned long *sp;
        int j;

        sp = (unsigned long *)((char *)&fd - 32);
        (void)write(fd, stack_dump_header, sizeof(stack_dump_header) - 1);
        for (j = 0; j < 16; j++)
          {
            line[0] = ' ';
            line[1] = ' ';
            len = 2 + format_int(line + 2, j);
            line[len++] = ':';
            line[len++] = ' ';
            len += format_ptr(line + len, (void *)sp);
            line[len++] = ' ';
            len += format_ptr(line + len, (void *)(*sp));
            line[len++] = '\n';
            (void)write(fd, line, len);
            sp++;
          }
      }
    }

  _oc_real_exit(1);
}
