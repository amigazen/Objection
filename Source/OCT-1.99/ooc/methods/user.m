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

  printf("--- Object: new, name, isKindOf:, isMemberOf: ---\n");
  obj = [Object new];
  ok(obj != nil, "Object new", "non-nil", obj == nil ? "nil" : "non-nil");
  if (obj == nil)
    return;
  name = (char *)[obj name];
  ok(name != (char *)0, "Object name", "non-null", name == (char *)0 ? "null" : "non-null");
  if (name != (char *)0)
    ok_str(strcmp(name, "Object") == 0, "Object name value", "Object", name);
  kind = [obj isKindOf: Object];
  ok(kind == YES, "isKindOf: Object", "YES", kind ? "YES" : "NO");
  kind = [obj isMemberOf: Object];
  ok(kind == YES, "isMemberOf: Object", "YES", kind ? "YES" : "NO");
  kind = [obj isKindOf: IdArray];
  ok(kind == NO, "isKindOf: IdArray (false)", "NO", kind ? "YES" : "NO");
  [obj free];
}

/* ---- String ---- */
static void
run_string_tests(void)
{
  id s1, s2, s3;
  char *p;
  int i;
  unsigned h;

  printf("--- String: str:, str, describe, asInt, compareSTR, isEqual: ---\n");
  s1 = [String str: "hello"];
  ok(s1 != nil, "String str:", "non-nil", s1 == nil ? "nil" : "non-nil");
  if (s1 == nil)
    return;
  p = (char *)[s1 str];
  ok(p != (char *)0 && strcmp(p, "hello") == 0, "String str", "hello", p != (char *)0 ? p : "(null)");
  p = (char *)[s1 describe];
  ok(p != (char *)0, "String describe", "non-null", p == (char *)0 ? "null" : "non-null");
  s2 = [String str: "42"];
  i = [s2 asInt];
  ok_int(i == 42, "String asInt 42", "42", i);
  ok([s1 isEqual: s1] == YES, "String isEqual: self", "YES", [s1 isEqual: s1] ? "YES" : "NO");
  ok([s1 isEqualSTR: "hello"] == YES, "String isEqualSTR:", "YES", [s1 isEqualSTR: "hello"] ? "YES" : "NO");
  h = [s1 hash];
  ok(h == [s1 hash], "String hash stable", "same value", "ok");
  s3 = [String str: "world"];
  [s1 concatSTR: " world"];
  p = (char *)[s1 str];
  ok(p != (char *)0 && strcmp(p, "hello world") == 0, "String concatSTR:", "hello world", p != (char *)0 ? p : "(null)");
  [s1 free];
  [s2 free];
  [s3 free];
}

/* ---- IdArray ---- */
static void
run_idarray_tests(void)
{
  id arr, obj;
  int cap, n;
  int off;

  printf("--- IdArray: new:, with:, count, capacity, at:, add:, offsetOf: ---\n");
  arr = [IdArray new: 4];
  ok(arr != nil, "IdArray new:", "non-nil", arr == nil ? "nil" : "non-nil");
  if (arr == nil)
    return;
  cap = [arr capacity];
  n = [arr count];
  ok_int(cap >= 4, "IdArray capacity", ">= 4", cap);
  ok_int(n == 0, "IdArray count empty", "0", n);
  obj = [Object new];
  [arr add: obj];
  ok_int([arr count] == 1, "IdArray add: count", "1", [arr count]);
  ok([arr at: 0] == obj, "IdArray at: 0", "same as added", [arr at: 0] == obj ? "same" : "different");
  off = [arr offsetOf: obj];
  ok_int(off == 0, "IdArray offsetOf:", "0", off);
  [arr remove: obj];
  ok_int([arr count] == 0, "IdArray remove: count", "0", [arr count]);
  [arr freeContents];
  [arr free];
  [obj free];
}

/* ---- System ---- */
static void
run_system_tests(void)
{
  id sys;
  char *v;

  printf("--- System: getProfileString:, putProfileString: data: ---\n");
  sys = [System new];
  ok(sys != nil, "System new", "non-nil", sys == nil ? "nil" : "non-nil");
  if (sys == nil)
    return;
  [System putProfileString: "testkey" data: "testvalue"];
  v = (char *)[System getProfileString: "testkey"];
  ok(v != (char *)0 && strcmp(v, "testvalue") == 0, "get/putProfileString", "testvalue", v != (char *)0 ? v : "(null)");
  [sys free];
}

/* ---- List ---- */
static void
run_list_tests(void)
{
  id list, obj;
  unsigned idx;
  int n;

  printf("--- List: new, addObject:, count, objectAt:, indexOf:, removeObject: ---\n");
  list = [List new];
  ok(list != nil, "List new", "non-nil", list == nil ? "nil" : "non-nil");
  if (list == nil)
    return;
  obj = [String str: "item1"];
  [list addObject: obj];
  [list addObject: [String str: "item2"]];
  n = [list count];
  ok_int(n == 2, "List count after addObject:", "2", n);
  ok([list objectAt: 0] == obj, "List objectAt: 0", "first object", [list objectAt: 0] == obj ? "same" : "different");
  idx = [list indexOf: obj];
  ok_int(idx == 0, "List indexOf:", "0", (int)idx);
  [list removeObject: obj];
  ok_int([list count] == 1, "List count after removeObject:", "1", [list count]);
  [list freeObjects];
  [list free];
  [obj free];
}

/* ---- LinkList ---- */
static void
run_linklist_tests(void)
{
  id ll, a, b;
  id first;
  int n;
  BOOL empty;

  printf("--- LinkList: new, add:, first, next, toFirst, count, isEmpty ---\n");
  ll = [LinkList new];
  ok(ll != nil, "LinkList new", "non-nil", ll == nil ? "nil" : "non-nil");
  if (ll == nil)
    return;
  empty = [ll isEmpty];
  ok(empty == YES, "LinkList isEmpty empty", "YES", empty ? "YES" : "NO");
  a = [String str: "a"];
  [ll add: a];
  [ll add: [String str: "b"]];
  n = [ll count];
  ok_int(n == 2, "LinkList count", "2", n);
  [ll toFirst];
  first = [ll first];
  ok(first != nil, "LinkList first", "non-nil", first == nil ? "nil" : "non-nil");
  ok([ll next] != nil, "LinkList next", "non-nil", [ll next] != nil ? "non-nil" : "nil");
  [ll freeContents];
  [ll free];
}

/* ---- OrderedCollection ---- */
static void
run_ordered_collection_tests(void)
{
  id col, other, first, last;
  int n, cap;
  id it;

  printf("--- OrderedCollection: new, add:, count, at:, firstElement, lastElement, remove: ---\n");
  col = [OrderedCollection new];
  ok(col != nil, "OrderedCollection new", "non-nil", col == nil ? "nil" : "non-nil");
  if (col == nil)
    return;
  [col add: [Object new]];
  [col add: [String str: "hello"]];
  [col add: [String str: "world"]];
  n = [col count];
  cap = [col capacity];
  ok_int(n == 3, "OrderedCollection count", "3", n);
  ok_int(cap >= 3, "OrderedCollection capacity", ">= 3", cap);
  first = [col firstElement];
  last = [col lastElement];
  ok(first != nil, "OrderedCollection firstElement", "non-nil", first == nil ? "nil" : "non-nil");
  ok(last != nil, "OrderedCollection lastElement", "non-nil", last == nil ? "nil" : "non-nil");
  it = [col at: 1];
  ok(it != nil, "OrderedCollection at: 1", "non-nil", it == nil ? "nil" : "non-nil");
  [col remove: it];
  ok_int([col count] == 2, "OrderedCollection count after remove:", "2", [col count]);
  other = [col copy];
  ok(other != nil && [other count] == 2, "OrderedCollection copy", "non-nil count 2", other != nil ? "ok" : "nil");
  if (other != nil)
    [other free];
  [col freeContents];
  [col free];
}

/* ---- Set ---- */
static void
run_set_tests(void)
{
  id set, s1, s2, found;
  int n;
  BOOL cont;

  printf("--- Set: new:, add:, count, contains:, find: ---\n");
  set = [Set new: 8];
  ok(set != nil, "Set new:", "non-nil", set == nil ? "nil" : "non-nil");
  if (set == nil)
    return;
  s1 = [String str: "alpha"];
  s2 = [String str: "beta"];
  [set add: s1];
  [set add: s2];
  [set add: s1];
  n = [set count];
  ok_int(n == 2, "Set count (no duplicates)", "2", n);
  cont = [set contains: s1];
  ok(cont == YES, "Set contains:", "YES", cont ? "YES" : "NO");
  found = [set find: s1];
  ok(found == s1, "Set find:", "same object", found == s1 ? "same" : "different");
  [set remove: s1];
  ok_int([set count] == 1, "Set count after remove:", "1", [set count]);
  [set freeContents];
  [set free];
  [s1 free];
  [s2 free];
}

/* ---- Sequence ---- */
static void
run_sequence_tests(void)
{
  id col, seq, here;
  int j, cnt;

  printf("--- Sequence: array:, next, toFirst, first, count ---\n");
  col = [OrderedCollection new];
  if (col == nil)
    {
      printf("  (skip: OrderedCollection new failed)\n\n");
      return;
    }
  [col add: [String str: "one"]];
  [col add: [String str: "two"]];
  [col add: [String str: "three"]];
  seq = [Sequence array: col];
  ok(seq != nil, "Sequence array:", "non-nil", seq == nil ? "nil" : "non-nil");
  if (seq == nil)
    {
      [col freeContents];
      [col free];
      return;
    }
  cnt = 0;
  for (j = 0; (here = [seq next]) != nil; ++j)
    cnt++;
  ok_int(cnt == 3, "Sequence next count", "3", cnt);
  [seq toFirst];
  here = [seq first];
  ok(here != nil, "Sequence first", "non-nil", here == nil ? "nil" : "non-nil");
  [seq free];
  [col freeContents];
  [col free];
}

/* ---- Queue ---- */
static void
run_queue_tests(void)
{
  id q, first, last;
  id at0;
  int n;

  printf("--- Queue: new, add:, addAtFront:, firstElement, lastElement, at: ---\n");
  q = [Queue new];
  ok(q != nil, "Queue new", "non-nil", q == nil ? "nil" : "non-nil");
  if (q == nil)
    return;
  [q add: [String str: "tail"]];
  [q addAtFront: [String str: "head"]];
  n = [q count];
  ok_int(n == 2, "Queue count", "2", n);
  first = [q firstElement];
  last = [q lastElement];
  ok(first != nil, "Queue firstElement", "non-nil", first == nil ? "nil" : "non-nil");
  ok(last != nil, "Queue lastElement", "non-nil", last == nil ? "nil" : "non-nil");
  at0 = [q at: 0];
  ok(at0 != nil, "Queue at: 0", "non-nil", at0 == nil ? "nil" : "non-nil");
  [q freeContents];
  [q free];
}

/* ---- Collection integration: copy, isCopyOf:, addContentsOf: --- */
static void
run_collection_integration_tests(void)
{
  id col1, col2, col3;
  int n;

  printf("--- Collection: addContentsOf:, copy, isCopyOf: ---\n");
  col1 = [OrderedCollection new];
  if (col1 == nil)
    {
      printf("  (skip: OrderedCollection new failed)\n\n");
      return;
    }
  [col1 add: [String str: "x"]];
  [col1 add: [String str: "y"]];
  col2 = [OrderedCollection new];
  if (col2 == nil)
    {
      [col1 freeContents];
      [col1 free];
      printf("  (skip: second OrderedCollection new failed)\n\n");
      return;
    }
  [col2 addContentsOf: col1];
  n = [col2 count];
  ok_int(n == 2, "addContentsOf: count", "2", n);
  col3 = [col1 copy];
  ok(col3 != nil, "Collection copy", "non-nil", col3 == nil ? "nil" : "non-nil");
  if (col3 != nil)
    {
      ok_int([col3 isCopyOf: col1] == 1, "isCopyOf: copy", "1", [col3 isCopyOf: col1] ? "1" : "0");
      [col3 free];
    }
  [col1 freeContents];
  [col1 free];
  [col2 freeContents];
  [col2 free];
}

int
app_main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;
  tests_run = 0;
  tests_passed = 0;
  _oc_atExit(finish);

  printf("========================================\n");
  printf("Foundation test harness (all classes)\n");
  printf("========================================\n\n");

  run_object_tests();
  run_string_tests();
  run_idarray_tests();
  run_system_tests();
  run_list_tests();
  run_linklist_tests();
  run_ordered_collection_tests();
  run_set_tests();
  run_sequence_tests();
  run_queue_tests();
  run_collection_integration_tests();

  printf("========================================\n");
  (void)finish((id)0, (SEL)0);
  return tests_passed == tests_run ? 0 : 1;
}
