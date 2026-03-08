/*
 * user.m -- Test harness for OpenStep Foundation classes.
 * Entry point is app_main(); substrate provides main() and calls app_main().
 */

#include <OpenStep/Foundation.h>
#include <OpenStep/NSRange.h>
#include <OpenStep/NSRuntime.h>

@includes

static int failed;
static int passed;

#define FORMAT_INT_MAX 16
static char format_int_buf[FORMAT_INT_MAX];

/* Format unsigned int as decimal; returns static buffer. */
static const char *
format_int(unsigned int n)
{
  char *p;
  unsigned int d;

  p = format_int_buf + FORMAT_INT_MAX - 1;
  *p = '\0';
  if (n == 0)
    {
      p--;
      *p = '0';
      return p;
    }
  while (n != 0)
    {
      d = n % 10;
      n = n / 10;
      p--;
      *p = (char)('0' + d);
    }
  return p;
}

static const char *
format_ptr(id p)
{
  return (p == (id)0) ? "nil" : "non-nil";
}

static void
ok_expect(int cond, const char *what, const char *expected, const char *actual)
{
  OpenStep_Printf("  Test: %s\n", what);
  OpenStep_Printf("  Expected: %s\n", expected);
  OpenStep_Printf("  Got: %s\n", actual);
  if (cond)
    {
      passed++;
      OpenStep_Printf("  PASS\n");
    }
  else
    {
      failed++;
      OpenStep_Printf("  FAIL\n");
    }
}

/* For int comparisons: expected and actual are formatted from integers. */
static void
ok_expect_int(int cond, const char *what, unsigned int expected_val, unsigned int actual_val)
{
  OpenStep_Printf("  Test: %s\n", what);
  OpenStep_Printf("  Expected: %s\n", format_int(expected_val));
  OpenStep_Printf("  Got: %s\n", format_int(actual_val));
  if (cond)
    {
      passed++;
      OpenStep_Printf("  PASS\n");
    }
  else
    {
      failed++;
      OpenStep_Printf("  FAIL\n");
    }
}

static void
section(const char *title)
{
  OpenStep_Printf("\n--- %s ---\n", title);
}

int
app_main(int argc, char *argv[])
{
  id s;
  id arr;
  id marr;
  id dict;
  id num;
  id dt;
  id ms;
  id tmp;
  id ex;
  id n;
  id r;
  id is;
  id mis;
  id obja;
  id objb;
  const char *cstr;
  NSRange idxRange;
  unsigned int len;
  unsigned int cnt;
  int eq;

  (void)argc;
  (void)argv;
  failed = 0;
  passed = 0;

  section("NSString");
  s = [NSString stringWithCString: "hello"];
  ok_expect(s != (id)0, "stringWithCString returns", "non-nil", format_ptr(s));
  len = [s length];
  ok_expect_int(len == 5, "length", 5, len);
  cstr = [s cString];
  ok_expect(cstr != (const char *)0 && OpenStep_Strcmp(cstr, "hello") == 0, "cString content", "hello", cstr != (const char *)0 ? cstr : "nil");
  eq = [s isEqualToString: s] ? 1 : 0;
  ok_expect(eq, "isEqualToString self", "true", eq ? "true" : "false");
  [s free];

  section("NSArray");
  obja = [NSString stringWithCString: "a"];
  objb = [NSString stringWithCString: "b"];
  arr = [NSArray arrayWithObject: obja withObject: objb];
  ok_expect(arr != (id)0, "arrayWithObject:withObject: returns", "non-nil", format_ptr(arr));
  cnt = [arr count];
  ok_expect_int(cnt == 2, "array count", 2, cnt);
  ok_expect([arr objectAtIndex: 0] != (id)0, "objectAtIndex 0", "non-nil", format_ptr([arr objectAtIndex: 0]));
  [arr free];
  [obja free];
  [objb free];

  section("NSMutableArray");
  marr = [NSMutableArray arrayWithCapacity: 4];
  ok_expect(marr != (id)0, "arrayWithCapacity returns", "non-nil", format_ptr(marr));
  [marr addObject: [NSString stringWithCString: "x"]];
  cnt = [marr count];
  ok_expect_int(cnt == 1, "addObject count", 1, cnt);
  [marr free];

  section("NSNumber");
  num = [NSNumber numberWithInt: 42];
  ok_expect(num != (id)0, "numberWithInt returns", "non-nil", format_ptr(num));
  len = (unsigned int)[num intValue];
  ok_expect_int(len == 42, "intValue", 42, len);
  [num free];

  section("NSDictionary");
  dict = [NSDictionary dictionaryWithObject: [NSString stringWithCString: "v"]
                                     forKey: [NSString stringWithCString: "k"]];
  ok_expect(dict != (id)0, "dictionaryWithObject:forKey: returns", "non-nil", format_ptr(dict));
  cnt = [dict count];
  ok_expect_int(cnt == 1, "dict count", 1, cnt);
  s = [dict objectForKey: [NSString stringWithCString: "k"]];
  ok_expect(s != (id)0, "objectForKey", "non-nil", format_ptr(s));
  [dict free];

  section("NSDate");
  dt = [NSDate date];
  ok_expect(dt != (id)0, "date returns", "non-nil", format_ptr(dt));
  [dt free];

  section("NSMutableString");
  ms = [NSMutableString stringWithCapacity: 8];
  ok_expect(ms != (id)0, "stringWithCapacity returns", "non-nil", format_ptr(ms));
  tmp = [NSString stringWithCString: "foo"];
  [ms setString: tmp];
  [tmp free];
  len = [ms length];
  ok_expect_int(len == 3, "setString length", 3, len);
  tmp = [NSString stringWithCString: "bar"];
  [ms appendString: tmp];
  [tmp free];
  cstr = [ms cString];
  eq = (cstr != (const char *)0 && OpenStep_Strcmp(cstr, "foobar") == 0) ? 1 : 0;
  ok_expect(eq, "appendString", "foobar", cstr != (const char *)0 ? cstr : "nil");
  [ms free];

  section("NSException");
  n = [NSString stringWithCString: "TestException"];
  r = [NSString stringWithCString: "test"];
  ex = [NSException exceptionWithName: n reason: r userInfo: (id)0];
  ok_expect(ex != (id)0, "exceptionWithName:reason:userInfo: returns", "non-nil", format_ptr(ex));
  ok_expect([ex name] != (id)0, "exception name", "non-nil", format_ptr([ex name]));
  ok_expect([ex reason] != (id)0, "exception reason", "non-nil", format_ptr([ex reason]));
  [ex free];
  [n free];
  [r free];

  section("NSIndexSet");
  is = [NSIndexSet indexSetWithIndex: 3];
  cnt = (is != (id)0) ? [is count] : 0;
  ok_expect(is != (id)0 && cnt == 1, "indexSetWithIndex count", "1", format_int(cnt));
  len = [is firstIndex];
  eq = [is containsIndex: 3] ? 1 : 0;
  ok_expect(len == 3 && eq, "firstIndex and containsIndex", "3, true", len == 3 && eq ? "3, true" : "fail");
  [is free];
  idxRange.location = 1;
  idxRange.length = 4;
  is = [NSIndexSet indexSetWithIndexesInRange: idxRange];
  cnt = (is != (id)0) ? [is count] : 0;
  ok_expect(is != (id)0 && cnt == 4, "indexSetWithIndexesInRange count", "4", format_int(cnt));
  len = [is indexGreaterThanIndex: 2];
  eq = [is containsIndex: 2] ? 1 : 0;
  ok_expect(eq && len == 3, "containsIndex 2 and indexGreaterThanIndex", "true, 3", eq && len == 3 ? "true, 3" : "fail");
  [is free];

  section("NSMutableIndexSet");
  mis = [NSMutableIndexSet indexSetWithCapacity: 4];
  ok_expect(mis != (id)0, "indexSetWithCapacity returns", "non-nil", format_ptr(mis));
  [mis addIndex: 1];
  [mis addIndex: 3];
  cnt = [mis count];
  eq = ([mis containsIndex: 1] && [mis containsIndex: 3]) ? 1 : 0;
  ok_expect(cnt == 2 && eq, "addIndex count", "2, contains 1 and 3", cnt == 2 && eq ? "2, true" : format_int(cnt));
  [mis removeIndex: 1];
  cnt = [mis count];
  eq = [mis containsIndex: 1] ? 0 : 1;
  ok_expect(cnt == 1 && eq, "removeIndex", "count 1, does not contain 1", cnt == 1 && eq ? "1, true" : "fail");
  [mis free];

  OpenStep_Printf("\nPassed: %ld  Failed: %ld\n", (long)passed, (long)failed);
  return failed != 0 ? 1 : 0;
}
