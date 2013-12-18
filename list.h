#include <stdio.h>

typedef struct _cell_t
{
  struct _cell_t *next;
  void *data;
}cell_t;

typedef struct {
	cell_t *first;
	cell_t *current;
	size_t size;
}list_t;

void* listSearch(list_t *list,void *arg,int (*func)(void *,void *));
void addList(list_t *list,void *data);
