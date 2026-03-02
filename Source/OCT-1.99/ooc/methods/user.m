/*
 * user.m -- Test harness for all foundation classes and features.
 * Reports test name, expected outcome, actual outcome, and pass/fail.
 * Does not exit on first failure; runs every section and prints summary.
 * Entry point is app_main(); substrate provides main() and calls app_main().
 */

#include <objc/Object.h>
#include <objc/IdArray.h>
#include <objc/String.h>
#include <objc/List.h>
#include <objc/LinkList.h>
#include <objc/System.h>
#include <collect/Collection.h>
#include <collect/Sequence.h>
#include <collect/OrderedCollection.h>
#include <collect/Set.h>
#include <collect/Queue.h>
@includes()

#include <stdio.h>
#include <string.h>

/*
 * Debug logging (only when OCT_TEST_DEBUG is defined): use write() so
 * the last line printed is the last line executed when redirected.
 */
#ifdef OCT_TEST_DEBUG
#ifdef __STDC__
extern int write(int fd, const void *buf, unsigned int nbytes);
#else
extern int write();
#endif
#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif
#ifndef STDERR_FILENO
#define STDERR_FILENO 2
#endif
#define DEBUG_FD STDOUT_FILENO

static void
debug_write_cstr(const char *s)
{
  unsigned int n;

  if (s == (const char *)0)
    return;
  n = 0;
  while (s[n] != '\0')
    n++;
  (void)write(DEBUG_FD, s, n);
}

static void
debug_write_line(int line, const char *msg)
{
  char buf[64];
  unsigned int n;
  int v;
  char tmp[10];
  unsigned int i;

  buf[0] = 'D'; buf[1] = 'E'; buf[2] = 'B'; buf[3] = 'U'; buf[4] = 'G';
  buf[5] = ' '; buf[6] = 'L';
  n = 7;

  v = line;
  if (v < 0)
    {
      buf[n++] = '-';
      v = -v;
    }
  i = 0;
  do {
    tmp[i++] = (char)('0' + (v % 10));
    v /= 10;
  } while (v != 0 && i < 10);
  while (i > 0)
    buf[n++] = tmp[--i];

  if (msg != (const char *)0)
    {
      buf[n++] = ':';
      buf[n++] = ' ';
      (void)write(DEBUG_FD, buf, n);
      debug_write_cstr(msg);
      debug_write_cstr("\n");
    }
  else
    {
      buf[n++] = '\n';
      (void)write(DEBUG_FD, buf, n);
    }
}

#define DEBUG_LINE() debug_write_line(__LINE__, (const char *)0)
#define DEBUG_LINE_MSG(s) debug_write_line(__LINE__, (s))
#else
#define DEBUG_LINE() ((void)0)
#define DEBUG_LINE_MSG(s) ((void)0)
#endif

static int tests_run;
static int tests_passed;

static void
ok(int pass, const char *test_name, const char *expected, const char *actual)
{
  tests_run++;
  if (pass)
    tests_passed++;
  printf("  %s: %s\n    expected: %s\n    actual:   %s\n    result:   %s\n\n",
         test_name, pass ? "PASS" : "FAIL", expected, actual, pass ? "PASS" : "FAIL");
}

static void
ok_int(int pass, const char *test_name, const char *expected, int actual_val)
{
  char buf[64];
  sprintf(buf, "%d", actual_val);
  ok(pass, test_name, expected, buf);
}

static void
ok_str(int pass, const char *test_name, const char *expected, const char *actual_val)
{
  ok(pass, test_name, expected, actual_val != (const char *)0 ? actual_val : "(null)");
}

/* Must match IMP (id (*)(id, SEL, ...)) so _oc_atExit/run_atexit call with correct ABI. */
static id
finish(id self, SEL sel)
{
  (void)self;
  (void)sel;
  printf("========================================\n");
  printf("Summary: %d passed, %d failed, %d total\n", tests_passed, tests_run - tests_passed, tests_run);
  printf("========================================\n");
  return (id)0;
}

/* ---- Object ---- */
static void
run_object_tests(void)
{
  id obj;
  char *name;
  BOOL kind;

  DEBUG_LINE();
  printf("--- Object: new, name, isKindOf:, isMemberOf: ---\n");
  DEBUG_LINE();
  obj = [Object new];
  DEBUG_LINE();
  ok(obj != nil, "Object new", "non-nil", obj == nil ? "nil" : "non-nil");
  if (obj == nil)
    return;
  DEBUG_LINE();
  name = (char *)[obj name];
  DEBUG_LINE();
  ok(name != (char *)0, "Object name", "non-null", name == (char *)0 ? "null" : "non-null");
  if (name != (char *)0)
    ok_str(strcmp(name, "Object") == 0, "Object name value", "Object", name);
  DEBUG_LINE();
  kind = [obj isKindOf: Object];
  DEBUG_LINE();
  ok(kind == YES, "isKindOf: Object", "YES", kind ? "YES" : "NO");
  kind = [obj isMemberOf: Object];
  DEBUG_LINE();
  ok(kind == YES, "isMemberOf: Object", "YES", kind ? "YES" : "NO");
  kind = [obj isKindOf: IdArray];
  DEBUG_LINE();
  ok(kind == NO, "isKindOf: IdArray (false)", "NO", kind ? "YES" : "NO");
  DEBUG_LINE();
  [obj free];
  DEBUG_LINE();
}

/* ---- String ---- */
static void
run_string_tests(void)
{
  id s1, s2, s3;
  char *p;
  int i;
  unsigned h;

  DEBUG_LINE();
  printf("--- String: str:, str, describe, asInt, compareSTR, isEqual: ---\n");
  DEBUG_LINE();
  s1 = [String str: "hello"];
  DEBUG_LINE();
  ok(s1 != nil, "String str:", "non-nil", s1 == nil ? "nil" : "non-nil");
  if (s1 == nil)
    return;
  DEBUG_LINE();
  p = (char *)[s1 str];
  DEBUG_LINE();
  ok(p != (char *)0 && strcmp(p, "hello") == 0, "String str", "hello", p != (char *)0 ? p : "(null)");
  DEBUG_LINE();
  p = (char *)[s1 describe];
  DEBUG_LINE();
  ok(p != (char *)0, "String describe", "non-null", p == (char *)0 ? "null" : "non-null");
  DEBUG_LINE();
  s2 = [String str: "42"];
  DEBUG_LINE();
  i = [s2 asInt];
  DEBUG_LINE();
  ok_int(i == 42, "String asInt 42", "42", i);
  DEBUG_LINE();
  ok([s1 isEqual: s1] == YES, "String isEqual: self", "YES", [s1 isEqual: s1] ? "YES" : "NO");
  DEBUG_LINE();
  ok([s1 isEqualSTR: "hello"] == YES, "String isEqualSTR:", "YES", [s1 isEqualSTR: "hello"] ? "YES" : "NO");
  DEBUG_LINE();
  h = [s1 hash];
  DEBUG_LINE();
  ok(h == [s1 hash], "String hash stable", "same value", "ok");
  DEBUG_LINE();
  s3 = [String str: "world"];
  DEBUG_LINE();
  [s1 concatSTR: " world"];
  DEBUG_LINE();
  p = (char *)[s1 str];
  DEBUG_LINE();
  ok(p != (char *)0 && strcmp(p, "hello world") == 0, "String concatSTR:", "hello world", p != (char *)0 ? p : "(null)");
  DEBUG_LINE();
  [s1 free];
  DEBUG_LINE();
  [s2 free];
  DEBUG_LINE();
  [s3 free];
  DEBUG_LINE();
}

/* ---- IdArray ---- */
static void
run_idarray_tests(void)
{
  id arr, obj;
  int cap, n;
  int off;

  DEBUG_LINE();
  printf("--- IdArray: new:, with:, count, capacity, at:, add:, offsetOf: ---\n");
  DEBUG_LINE();
  arr = [IdArray new: 4];
  DEBUG_LINE();
  ok(arr != nil, "IdArray new:", "non-nil", arr == nil ? "nil" : "non-nil");
  if (arr == nil)
    return;
  DEBUG_LINE();
  cap = [arr capacity];
  DEBUG_LINE();
  n = [arr count];
  DEBUG_LINE();
  ok_int(cap >= 4, "IdArray capacity", ">= 4", cap);
  ok_int(n == 0, "IdArray count empty", "0", n);
  DEBUG_LINE();
  obj = [Object new];
  DEBUG_LINE();
  [arr add: obj];
  DEBUG_LINE();
  ok_int([arr count] == 1, "IdArray add: count", "1", [arr count]);
  ok([arr at: 0] == obj, "IdArray at: 0", "same as added", [arr at: 0] == obj ? "same" : "different");
  DEBUG_LINE();
  off = [arr offsetOf: obj];
  DEBUG_LINE();
  ok_int(off == 0, "IdArray offsetOf:", "0", off);
  [arr remove: obj];
  DEBUG_LINE();
  ok_int([arr count] == 0, "IdArray remove: count", "0", [arr count]);
  [arr freeContents];
  [arr free];
  [obj free];
  DEBUG_LINE();
}

/* ---- System ---- */
static void
run_system_tests(void)
{
  id sys;
  char *v;

  DEBUG_LINE();
  printf("--- System: getProfileString:, putProfileString: data: ---\n");
  DEBUG_LINE();
  sys = [System new];
  DEBUG_LINE();
  ok(sys != nil, "System new", "non-nil", sys == nil ? "nil" : "non-nil");
  if (sys == nil)
    return;
  DEBUG_LINE();
  [System putProfileString: "testkey" data: "testvalue"];
  DEBUG_LINE();
  v = (char *)[System getProfileString: "testkey"];
  DEBUG_LINE();
  ok(v != (char *)0 && strcmp(v, "testvalue") == 0, "get/putProfileString", "testvalue", v != (char *)0 ? v : "(null)");
  DEBUG_LINE();
  [sys free];
  DEBUG_LINE();
}

/* ---- List ---- */
static void
run_list_tests(void)
{
  id list, obj;
  unsigned idx;
  int n;

  DEBUG_LINE();
  printf("--- List: new, addObject:, count, objectAt:, indexOf:, removeObject: ---\n");
  DEBUG_LINE();
  list = [List new];
  DEBUG_LINE();
  ok(list != nil, "List new", "non-nil", list == nil ? "nil" : "non-nil");
  if (list == nil)
    return;
  DEBUG_LINE();
  obj = [String str: "item1"];
  DEBUG_LINE();
  [list addObject: obj];
  DEBUG_LINE();
  [list addObject: [String str: "item2"]];
  DEBUG_LINE();
  n = [list count];
  DEBUG_LINE();
  ok_int(n == 2, "List count after addObject:", "2", n);
  ok([list objectAt: 0] == obj, "List objectAt: 0", "first object", [list objectAt: 0] == obj ? "same" : "different");
  DEBUG_LINE();
  idx = [list indexOf: obj];
  DEBUG_LINE();
  ok_int(idx == 0, "List indexOf:", "0", (int)idx);
  DEBUG_LINE();
  [list removeObject: obj];
  DEBUG_LINE();
  ok_int([list count] == 1, "List count after removeObject:", "1", [list count]);
  DEBUG_LINE();
  [list freeObjects];
  DEBUG_LINE();
  [list free];
  DEBUG_LINE();
  [obj free];
  DEBUG_LINE();
}

/* ---- LinkList ---- */
static void
run_linklist_tests(void)
{
  id ll, a, b;
  id first, second;
  int n;
  BOOL empty;

  DEBUG_LINE();
  printf("--- LinkList: new, add:, first, next, toFirst, count, isEmpty ---\n");
  DEBUG_LINE();
  ll = [LinkList new];
  DEBUG_LINE();
  ok(ll != nil, "LinkList new", "non-nil", ll == nil ? "nil" : "non-nil");
  if (ll == nil)
    return;
  DEBUG_LINE();
  empty = [ll isEmpty];
  DEBUG_LINE();
  ok(empty == YES, "LinkList isEmpty empty", "YES", empty ? "YES" : "NO");
  DEBUG_LINE();
  a = [String str: "a"];
  DEBUG_LINE();
  [ll add: a];
  DEBUG_LINE();
  [ll add: [String str: "b"]];
  DEBUG_LINE();
  n = [ll count];
  DEBUG_LINE();
  ok_int(n == 2, "LinkList count", "2", n);
  DEBUG_LINE();
  [ll toFirst];
  DEBUG_LINE();
  first = [ll first];
  DEBUG_LINE();
  ok(first != nil, "LinkList first", "non-nil", first == nil ? "nil" : "non-nil");
  second = [ll next];
  ok(second != nil, "LinkList next", "non-nil", second != nil ? "non-nil" : "nil");
  DEBUG_LINE();
  [ll freeContents];
  DEBUG_LINE();
  [ll free];
  DEBUG_LINE();
}

/* ---- OrderedCollection ---- */
static void
run_ordered_collection_tests(void)
{
  id col, other, first, last;
  int n, cap;
  id it;

  DEBUG_LINE();
  printf("--- OrderedCollection: new, add:, count, at:, firstElement, lastElement, remove: ---\n");
  DEBUG_LINE();
  col = [OrderedCollection new];
  DEBUG_LINE();
  ok(col != nil, "OrderedCollection new", "non-nil", col == nil ? "nil" : "non-nil");
  if (col == nil)
    return;
  DEBUG_LINE();
  [col add: [Object new]];
  DEBUG_LINE();
  [col add: [String str: "hello"]];
  DEBUG_LINE();
  [col add: [String str: "world"]];
  DEBUG_LINE();
  n = [col count];
  cap = [col capacity];
  DEBUG_LINE();
  ok_int(n == 3, "OrderedCollection count", "3", n);
  ok_int(cap >= 3, "OrderedCollection capacity", ">= 3", cap);
  DEBUG_LINE();
  first = [col firstElement];
  last = [col lastElement];
  DEBUG_LINE();
  ok(first != nil, "OrderedCollection firstElement", "non-nil", first == nil ? "nil" : "non-nil");
  ok(last != nil, "OrderedCollection lastElement", "non-nil", last == nil ? "nil" : "non-nil");
  DEBUG_LINE();
  it = [col at: 1];
  DEBUG_LINE();
  ok(it != nil, "OrderedCollection at: 1", "non-nil", it == nil ? "nil" : "non-nil");
  DEBUG_LINE();
  [col remove: it];
  DEBUG_LINE();
  ok_int([col count] == 2, "OrderedCollection count after remove:", "2", [col count]);
  DEBUG_LINE();
  other = [col copy];
  DEBUG_LINE();
  ok(other != nil && [other count] == 2, "OrderedCollection copy", "non-nil count 2", other != nil ? "ok" : "nil");
  if (other != nil)
    [other free];
  DEBUG_LINE();
  [col freeContents];
  DEBUG_LINE();
  [col free];
  DEBUG_LINE();
}

/* ---- Set ---- */
static void
run_set_tests(void)
{
  id set, s1, s2, found;
  int n;
  BOOL cont;

  DEBUG_LINE();
  printf("--- Set: new:, add:, count, contains:, find: ---\n");
  DEBUG_LINE();
  set = [Set new: 8];
  DEBUG_LINE();
  ok(set != nil, "Set new:", "non-nil", set == nil ? "nil" : "non-nil");
  if (set == nil)
    return;
  DEBUG_LINE();
  s1 = [String str: "alpha"];
  DEBUG_LINE();
  s2 = [String str: "beta"];
  DEBUG_LINE();
  [set add: s1];
  DEBUG_LINE();
  [set add: s2];
  [set add: s1];
  DEBUG_LINE();
  n = [set count];
  DEBUG_LINE();
  ok_int(n == 2, "Set count (no duplicates)", "2", n);
  DEBUG_LINE();
  cont = [set contains: s1];
  DEBUG_LINE();
  ok(cont == YES, "Set contains:", "YES", cont ? "YES" : "NO");
  found = [set find: s1];
  DEBUG_LINE();
  ok(found == s1, "Set find:", "same object", found == s1 ? "same" : "different");
  DEBUG_LINE();
  [set remove: s1];
  DEBUG_LINE();
  ok_int([set count] == 1, "Set count after remove:", "1", [set count]);
  DEBUG_LINE();
  [set freeContents];
  DEBUG_LINE();
  [set free];
  DEBUG_LINE();
  [s1 free];
  DEBUG_LINE();
  [s2 free];
  DEBUG_LINE();
}

/* ---- Sequence ---- */
static void
run_sequence_tests(void)
{
  id col, seq, here;
  int j, cnt;

  DEBUG_LINE();
  printf("--- Sequence: array:, next, toFirst, first, count ---\n");
  DEBUG_LINE();
  col = [OrderedCollection new];
  DEBUG_LINE();
  if (col == nil)
    {
      printf("  (skip: OrderedCollection new failed)\n\n");
      return;
    }
  DEBUG_LINE();
  [col add: [String str: "one"]];
  DEBUG_LINE();
  [col add: [String str: "two"]];
  [col add: [String str: "three"]];
  DEBUG_LINE();
  seq = [Sequence array: col];
  DEBUG_LINE();
  ok(seq != nil, "Sequence array:", "non-nil", seq == nil ? "nil" : "non-nil");
  if (seq == nil)
    {
      [col freeContents];
      [col free];
      return;
    }
  DEBUG_LINE();
  cnt = 0;
  for (j = 0; (here = [seq next]) != nil; ++j)
    cnt++;
  DEBUG_LINE();
  ok_int(cnt == 3, "Sequence next count", "3", cnt);
  DEBUG_LINE();
  [seq toFirst];
  DEBUG_LINE();
  here = [seq first];
  DEBUG_LINE();
  ok(here != nil, "Sequence first", "non-nil", here == nil ? "nil" : "non-nil");
  DEBUG_LINE();
  [seq free];
  DEBUG_LINE();
  [col freeContents];
  [col free];
  DEBUG_LINE();
}

/* ---- Queue ---- */
static void
run_queue_tests(void)
{
  id q, first, last;
  id at0;
  int n;

  DEBUG_LINE();
  printf("--- Queue: new, add:, addAtFront:, firstElement, lastElement, at: ---\n");
  DEBUG_LINE();
  q = [Queue new];
  DEBUG_LINE();
  ok(q != nil, "Queue new", "non-nil", q == nil ? "nil" : "non-nil");
  if (q == nil)
    return;
  DEBUG_LINE();
  [q add: [String str: "tail"]];
  DEBUG_LINE();
  [q addAtFront: [String str: "head"]];
  DEBUG_LINE();
  n = [q count];
  DEBUG_LINE();
  ok_int(n == 2, "Queue count", "2", n);
  DEBUG_LINE();
  first = [q firstElement];
  last = [q lastElement];
  DEBUG_LINE();
  ok(first != nil, "Queue firstElement", "non-nil", first == nil ? "nil" : "non-nil");
  ok(last != nil, "Queue lastElement", "non-nil", last == nil ? "nil" : "non-nil");
  DEBUG_LINE();
  at0 = [q at: 0];
  DEBUG_LINE();
  ok(at0 != nil, "Queue at: 0", "non-nil", at0 == nil ? "nil" : "non-nil");
  DEBUG_LINE();
  [q freeContents];
  DEBUG_LINE();
  [q free];
  DEBUG_LINE();
}

/* ---- Collection integration: copy, isCopyOf:, addContentsOf: --- */
static void
run_collection_integration_tests(void)
{
  id col1, col2, col3;
  int n;

  DEBUG_LINE();
  printf("--- Collection: addContentsOf:, copy, isCopyOf: ---\n");
  DEBUG_LINE();
  col1 = [OrderedCollection new];
  DEBUG_LINE();
  if (col1 == nil)
    {
      printf("  (skip: OrderedCollection new failed)\n\n");
      return;
    }
  DEBUG_LINE();
  [col1 add: [String str: "x"]];
  DEBUG_LINE();
  [col1 add: [String str: "y"]];
  DEBUG_LINE();
  col2 = [OrderedCollection new];
  DEBUG_LINE();
  if (col2 == nil)
    {
      [col1 freeContents];
      [col1 free];
      printf("  (skip: second OrderedCollection new failed)\n\n");
      return;
    }
  DEBUG_LINE();
  [col2 addContentsOf: col1];
  DEBUG_LINE();
  n = [col2 count];
  DEBUG_LINE();
  ok_int(n == 2, "addContentsOf: count", "2", n);
  DEBUG_LINE();
  col3 = [col1 copy];
  DEBUG_LINE();
  ok(col3 != nil, "Collection copy", "non-nil", col3 == nil ? "nil" : "non-nil");
  if (col3 != nil)
    {
      ok_int([col3 isCopyOf: col1] == 1, "isCopyOf: copy", "1", [col3 isCopyOf: col1] ? 1 : 0);
      DEBUG_LINE();
      [col3 free];
    }
  DEBUG_LINE();
  [col1 freeContents];
  DEBUG_LINE();
  [col1 free];
  DEBUG_LINE();
  [col2 freeContents];
  [col2 free];
  DEBUG_LINE();
}

#define NUM_TEST_SECTIONS 11
static void (*const test_section_fns[NUM_TEST_SECTIONS])(void) = {
  run_object_tests,
  run_idarray_tests,
  run_string_tests,
  run_system_tests,
  run_list_tests,
  run_linklist_tests,
  run_ordered_collection_tests,
  run_set_tests,
  run_sequence_tests,
  run_queue_tests,
  run_collection_integration_tests,
};

int
app_main(int argc, char *argv[])
{
  int i;

  (void)argc;
  (void)argv;
  setvbuf(stdout, (char *)0, _IONBF, 0);
  setvbuf(stderr, (char *)0, _IONBF, 0);
  tests_run = 0;
  tests_passed = 0;
  _oc_atExit(finish);

  printf("========================================\n");
  printf("Foundation test harness (all classes)\n");
  printf("========================================\n\n");

  for (i = 0; i < NUM_TEST_SECTIONS; i++)
    (*test_section_fns[i])();

  printf("========================================\n");
  (void)finish((id)0, (SEL)0);
  return tests_passed == tests_run ? 0 : 1;
}
