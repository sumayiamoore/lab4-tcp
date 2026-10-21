// list/list.c
// 
// Implementation for a singly linked list.
//
// <Author>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

/* Allocates a new empty list on the heap. */
list_t *list_alloc() { 
  list_t* mylist = (list_t *) malloc(sizeof(list_t)); 
  mylist->head = NULL;
  return mylist;
}

/* Frees every node in the list, then frees the list struct itself. */
void list_free(list_t *l) {
  node_t *curr = l->head;
  while (curr != NULL) {
    node_t *temp = curr;
    curr = curr->next;
    free(temp);
  }
  free(l);
}

void list_print(list_t *l) {
  char *s = listToString(l);
  printf("%s\n", s);
  free(s);
}

/* Builds a string representation of the list, e.g. "10->20->NULL".
 * Buffer is dynamically sized to fit the list, growing as needed. */
char * listToString(list_t *l) {
  size_t capacity = 64;
  char *buf = (char *) malloc(capacity);
  buf[0] = '\0';
  size_t len = 0;
  char tbuf[20];

  node_t *curr = l->head;
  while (curr != NULL) {
    int written = sprintf(tbuf, "%d->", curr->value);

    // Grow the buffer if the next piece won't fit.
    if (len + written + 5 > capacity) {  // +5 leaves room for "NULL\0"
      capacity *= 2;
      buf = (char *) realloc(buf, capacity);
    }

    strcat(buf, tbuf);
    len += written;
    curr = curr->next;
  }
  strcat(buf, "NULL");
  return buf;
}

int list_length(list_t *l) {
  int count = 0;
  node_t *curr = l->head;
  while (curr != NULL) {
    count++;
    curr = curr->next;
  }
  return count;
}

void list_add_to_back(list_t *l, elem value) {
  node_t *new_node = getNode(value);

  if (l->head == NULL) {
    l->head = new_node;
    return;
  }

  node_t *curr = l->head;
  while (curr->next != NULL) {
    curr = curr->next;
  }
  curr->next = new_node;
}

void list_add_to_front(list_t *l, elem value) {
  node_t *cur_node = getNode(value);
  node_t *head = l->head;
  cur_node->next = head;
  l->head = cur_node;
}

/* Allocates a new node on the heap with the given value. */
node_t * getNode(elem value) {
  node_t *mynode;
  mynode = (node_t *) malloc(sizeof(node_t));
  mynode->value = value;
  mynode->next = NULL;
  return mynode;
}

/* Inserts at the given index (starting at 1). If index is <= 1, inserts
 * at the front. If index is past the end of the list, inserts at the back
 * rather than rejecting, so callers can always successfully add. */
void list_add_at_index(list_t *l, elem value, int index) {
  if (index <= 1 || l->head == NULL) {
    list_add_to_front(l, value);
    return;
  }

  node_t *curr = l->head;
  int i = 1;
  // Walk until we're right before the target index, or hit the last node.
  while (curr->next != NULL && i < index - 1) {
    curr = curr->next;
    i++;
  }

  node_t *new_node = getNode(value);
  new_node->next = curr->next;
  curr->next = new_node;
}

/* Removes the last element in the list. Returns -1 if the list is empty. */
elem list_remove_from_back(list_t *l) {
  if (l->head == NULL) {
    return -1;
  }

  node_t *curr = l->head;

  // Special case: only one node in the list.
  if (curr->next == NULL) {
    elem value = curr->value;
    free(curr);
    l->head = NULL;
    return value;
  }

  // Walk until curr is the second-to-last node.
  while (curr->next->next != NULL) {
    curr = curr->next;
  }
  elem value = curr->next->value;
  free(curr->next);
  curr->next = NULL;
  return value;
}

/* Removes the first element in the list. Returns -1 if the list is empty. */
elem list_remove_from_front(list_t *l) {
  if (l->head == NULL) {
    return -1;
  }
  node_t *temp = l->head;
  elem value = temp->value;
  l->head = temp->next;
  free(temp);
  return value;
}

/* Removes the element at the given index (starting at 1).
 * Returns -1 if the list is empty or the index is out of range.
 * Walks the list once, tracking length as it goes, instead of
 * calling list_length() separately (avoids a second full traversal). */
elem list_remove_at_index(list_t *l, int index) {
  if (l->head == NULL || index < 1) {
    return -1;
  }

  if (index == 1) {
    return list_remove_from_front(l);
  }

  node_t *curr = l->head;
  int i = 1;
  // Walk until we're right before the target index, or run out of nodes.
  while (curr->next != NULL && i < index - 1) {
    curr = curr->next;
    i++;
  }

  node_t *to_remove = curr->next;
  if (to_remove == NULL) {
    // index was out of range (past the end of the list)
    return -1;
  }

  elem value = to_remove->value;
  curr->next = to_remove->next;
  free(to_remove);
  return value;
}

bool list_is_in(list_t *l, elem value) {
  node_t *curr = l->head;
  while (curr != NULL) {
    if (curr->value == value) {
      return true;
    }
    curr = curr->next;
  }
  return false;
}

/* Returns the element at the given index (starting at 1).
 * Returns -1 if the index is out of range. */
elem list_get_elem_at(list_t *l, int index) {
  node_t *curr = l->head;
  int i = 1;
  while (curr != NULL) {
    if (i == index) {
      return curr->value;
    }
    curr = curr->next;
    i++;
  }
  return -1;
}

/* Returns the index of the given value (starting at 1).
 * Returns -1 if the value is not found. */
int list_get_index_of(list_t *l, elem value) {
  node_t *curr = l->head;
  int i = 1;
  while (curr != NULL) {
    if (curr->value == value) {
      return i;
    }
    curr = curr->next;
    i++;
  }
  return -1;
}
