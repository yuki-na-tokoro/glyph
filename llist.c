#include <stdio.h>
#include <stdlib.h>

// declarations

typedef struct node {
  int value;
  struct node *succ;
} Node;

typedef struct llist {
  int length;
  struct node *head;
  struct node *tail;
} LinkedList;

// allocates a new llist with head segment
LinkedList* llist_new(int head);

void llist_append(LinkedList*, int);

void llist_prepend(LinkedList*, int);

// frees all allocated segments and the struct
void llist_free(LinkedList*);

// helper function for recursively freeing segments
static void _segfree(Node*);

// definitions

LinkedList* llist_new(int head) {

  Node *node = malloc(sizeof(Node));

  if (node == NULL) {
    perror("#mkllist(): malloc segment");
  }

  node->value = head;

  LinkedList *llist = malloc(sizeof(LinkedList));

  if (llist == NULL) {
    perror("#mkllist(): malloc llist");
  }

  llist->length = 1;
  llist->head = node;
  llist->tail = node;

  return llist;
 
}

void llist_append(LinkedList* llist, int value) {

  Node *seg = malloc(sizeof(Node));

  if (seg == NULL) {
    perror("#append(): malloc segment");
  }

  seg->value = value;

  llist->tail->succ = seg;
  llist->tail = seg;
  llist->length++;
    
}

void llist_prepend(LinkedList* llist, int value) {
  
  Node *seg = malloc(sizeof(Node));

  if (seg == NULL) {
    perror("#prepend(): malloc segment");
  }

  seg->value = value;
  seg->succ = llist->head;

  llist->head = seg;
  llist->length++;

};

void llist_free(LinkedList* llist) {
  _segfree(llist->head);
  free(llist);
}

static void _segfree(Node* node) {
  if (node->succ != NULL) {
    _segfree(node->succ);
  }
  free(node);
}

