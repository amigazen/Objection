/*
 * tree.c -- List/Node support for non-Amiga OCT runtime.
 * On Amiga these come from exec.library; on other platforms implemented here.
 * objc-runtime.h and code using it expect struct List and struct Node.
 */

#ifndef MCH_AMIGA

#include <octhead.h>
#include <string.h>

void
NewList(struct List *list)
{
  if (list == (struct List *)0)
    return;
  list->lh_Head = (struct Node *)&list->lh_Tail;
  list->lh_Tail = (struct Node *)0;
  list->lh_TailPred = (struct Node *)&list->lh_Head;
  list->lh_Type = 0;
  list->l_pad = 0;
}

void
Remove(struct Node *node)
{
  if (node == (struct Node *)0)
    return;
  node->ln_Succ->ln_Pred = node->ln_Pred;
  node->ln_Pred->ln_Succ = node->ln_Succ;
}

void
AddHead(struct List *list, struct Node *node)
{
  if (list == (struct List *)0 || node == (struct Node *)0)
    return;
  node->ln_Succ = list->lh_Head;
  node->ln_Pred = (struct Node *)&list->lh_Head;
  list->lh_Head->ln_Pred = node;
  list->lh_Head = node;
}

void
AddTail(struct List *list, struct Node *node)
{
  if (list == (struct List *)0 || node == (struct Node *)0)
    return;
  node->ln_Succ = (struct Node *)&list->lh_Tail;
  node->ln_Pred = list->lh_TailPred;
  list->lh_TailPred->ln_Succ = node;
  list->lh_TailPred = node;
}

struct Node *
RemTail(struct List *list)
{
  struct Node *node;

  if (list == (struct List *)0)
    return (struct Node *)0;
  node = list->lh_TailPred;
  if (node == (struct Node *)&list->lh_Head)
    return (struct Node *)0;
  Remove(node);
  return node;
}

struct Node *
FindName(struct List *list, char *name)
{
  struct Node *node;

  if (list == (struct List *)0)
    return (struct Node *)0;
  for (node = list->lh_Head; node->ln_Succ != (struct Node *)0; node = node->ln_Succ)
    if (node->ln_Name != (char *)0 && name != (char *)0 &&
        strcmp(node->ln_Name, name) == 0)
      return node;
  return (struct Node *)0;
}

#endif /* !MCH_AMIGA */
