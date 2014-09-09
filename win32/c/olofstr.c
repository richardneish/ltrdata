#include <string.h>

#include <olofstr.h>

char *
strltrm(char *ptr)
{
  if (ptr == NULL)
    return NULL;

  while (ptr[0] == ' ')
    ++ptr;

  return ptr;
}

char *
strrtrm(char *ptr)
{
  size_t len;

  if (ptr == NULL)
    return NULL;

  len = strlen(ptr);

  while (ptr[--len] == ' ')
    ptr[len] = 0;

  return ptr;
}

char *
strtrm(char *ptr)
{
  size_t len;

  if (ptr == NULL)
    return NULL;

  while (ptr[0] == ' ')
    ++ptr;

  len = strlen(ptr);

  while (ptr[--len] == ' ')
    ptr[len] = 0;

  return ptr;
}
