#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "memory.h"
#include "harmonicCluster.h"
#include "error.h"

void* listSearch(list_t *list,void *arg,int (*func)(void *,void *)){
	cell_t *cur;
	for(cur=list->first;cur;cur=cur->next){
		if(func(arg,cur->data)) return cur->data;
	}
	return NULL;
}
void addList(list_t *list,void *data){
	cell_t *tmp;

	tmp=calloc(1,sizeof(cell_t));
	tmp->data=data;
	if(list->current){
		list->current->next=tmp;
	}else
		list->first=tmp;
	list->current=tmp;
	list->size++;
}
/*!
 ******************************************************************************
 * \fn int setList(cell_t* p, void* data)
 * \param *p 挿入したいセルのポインタ（この直後にセルを挿入する）
 * \param data データ
 * \
 *
 ******************************************************************************
 */
cell_t*
setList(cell_t* p, void* data)
{
  cell_t *tmp;
  dmalloc(tmp,sizeof(cell_t));

  tmp->next = p->next;
  tmp->data = data;
  p->next = tmp;
  return tmp;
}

/*!
 ******************************************************************************
 * \fn int deleteList(cell_t* p)
 * \brief 引数で受け取ったセルの次のポインタのデータを削除
 * \param *p 削除したいセルを指しているポインタ
 * \return -1:エラー 0:正常終了
 *
 ******************************************************************************
 */
int
deleteList(cell_t* p)
{
  cell_t *tmp;
  if(p->next == NULL)
  {
    return(-1);
  }
  tmp = p->next;
  p->next = tmp->next;
  free(tmp);
  return(0);
}


/*!
 ******************************************************************************
 * \fn void allDeleteList(cell_t *p)
 * \brief データの全削除
 * \param *p 先頭のセルのポインタ
 * \return None
 *
 ******************************************************************************
 */
void
allDeleteList(cell_t *p)
{
  cell_t *tmp;
  while (p->next != NULL)
  {
    tmp = p->next;
    p->next = tmp->next;
    free(tmp);
  }
}
