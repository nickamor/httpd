/*
 * childpid.c
 *
 *  Created on: Oct 11, 2011
 *      Author: nick
 */

#include <stdlib.h>
#include <stdio.h>
#include "childpid.h"

struct childpid_list_t * childpid_list = NULL;

void
add_childpid(pid_t childpid)
{
  /* add the new child to the child manager */
  struct childpid_list_t * new_childpid = calloc(1,
      sizeof(struct childpid_list_t));
  new_childpid->pid = childpid;
  new_childpid->next = NULL;

  if (childpid_list)
    {
      struct childpid_list_t * tail_childpid = childpid_list;
      while (tail_childpid->next)
        {
          tail_childpid = tail_childpid->next;
        }
      tail_childpid->next = new_childpid;
    }
  else
    {
      childpid_list = new_childpid;
    }
}

void
remove_childpid(pid_t childpid)
{
  /* find the child to remove */
  struct childpid_list_t * childpid_to_remove = NULL, *prev_childpid = NULL;

  childpid_to_remove = childpid_list;
  while (childpid_to_remove && childpid_to_remove->pid != childpid)
    {
      prev_childpid = childpid_to_remove;
      childpid_to_remove = childpid_to_remove->next;
    }

  /* remove the child from the child manager */
  if (childpid_to_remove)
    {
      struct childpid_list_t * next_childpid = childpid_to_remove->next;

      if (prev_childpid)
        {
          prev_childpid->next = next_childpid;
        }

      if (childpid_list == childpid_to_remove)
        {
          childpid_list = childpid_to_remove->next;
        }

      free(childpid_to_remove);
    }
}
