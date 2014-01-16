/// \file string.h
/*
  -----------------------------------
  Create date: 2013-11-24 00:07
  Modified date: 2014-01-09 20:33
  Author: Sen1993
  Email: 1730806439@qq.com
  -----------------------------------
*/
PUBLIC void * memcpy(void *p_dst,void* p_src,int size);
PUBLIC void   memset(void *p_dst,char ch,int size);

#define phys_copy memcpy
#define phys_set  memset
