#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json_serializer.h"

typedef struct struct_B {
  short some_shorts[5];
  unsigned short some_ushorts[5];
  int int_list[10];
} B_t;

typedef struct struct_A {
  char some_string[10];
  unsigned int some_uinteger;
  long some_long_integer;
  double some_double;
  B_t some_b[2];
} A_t;

int main(int argc, char* argv[])
{
  FILE *iFile = NULL;
  char * json_str = NULL;
  size_t len = 0;
  size_t read_result = 0;
  int ret = 0;
  json_metadata_t *confg_meta = NULL;
  int set_res = 0;
  int i = 0;
  int j = 0;

  // build the JSON meta data from the struct A_t
  json_metadata_t *meta_a = NULL;
  json_metadata_t *meta_b = NULL;

  // create metadata for struct A_t
  meta_a = add_json_metadata("A_t", sizeof(A_t), NULL);
  // create and add the A_t fields to meta_a
  add_json_metadata_field(meta_a, J_STRING, (char *)"some_string", 10, 1, offsetof(A_t, some_string));
  add_json_metadata_field(meta_a, J_UINT, (char *)"some_uinteger", sizeof(unsigned int), 1, offsetof(A_t, some_uinteger));
  add_json_metadata_field(meta_a, J_LONG, (char *)"some_long_integer", sizeof(long), 1, offsetof(A_t, some_long_integer));
  add_json_metadata_field(meta_a, J_DOUBLE, (char *)"some_double", sizeof(double), 1, offsetof(A_t, some_double));

  // create metadata for struct B_t
  meta_b = add_json_metadata("B_t", sizeof(B_t), meta_a);
  // create and add the B_t fields to meta_b
  add_json_metadata_field(meta_b, J_SHORT, (char *)"some_shorts", sizeof(short), 5, offsetof(B_t, some_shorts));
  add_json_metadata_field(meta_b, J_USHORT, (char *)"some_ushorts", sizeof(unsigned short), 5, offsetof(B_t, some_ushorts));
  add_json_metadata_field(meta_b, J_INT, (char *)"int_list", sizeof(int), 10, offsetof(B_t, int_list));

  // add the some_b field to meta_a
  add_json_metadata_field(meta_a, "B_t", (char *)"some_b", sizeof(B_t), 2, offsetof(A_t, some_b));

  A_t a;
  memset( &a, 0, sizeof(A_t));

  iFile = fopen("test.json", "r");
  if (iFile == NULL) {
    perror("Failed to open example.json") ;
    goto done;
  }

  fseek(iFile, 0, SEEK_END);
  len = ftell(iFile);
  if (len < 0)
  {
    perror("ERROR ftell() FAILED");
    goto done;
  }

  /* reset back to the beginning of the file */
  set_res = fseek(iFile, 0, SEEK_SET);
  if (set_res != 0) {
    perror("set_res failed.");
    goto done;
  }

  json_str = (char *)malloc(len);
  read_result = fread(json_str, 1, len, iFile);
  if (read_result < (unsigned long) len )
  {
    perror("ERROR fread()");
    goto done;
  }

  ret = deserialize_binary_from_json(json_str, meta_a, (char *)"A_t", (void *) &a);

  if (!ret) {
    perror("ERROR occurred while deserializing the binary from the json file.");
    goto done;
  }

  printf("a.some_string = %s\n",a.some_string);
  printf("a.some_uinteger = %d\n",a.some_uinteger);
  printf("a.some_long_integer = %ld\n",a.some_long_integer);
  printf("a.somedouble = %f\n",a.some_double);

  for (j=0; j < 2; ++j)
  {
    for (i=0; i < 5; ++i)
    {
      printf("a.some_b[%d].some_shorts[%d] = %d\n", j, i, a.some_b[j].some_shorts[i]);
    }
    for (i=0; i < 5; ++i)
    {
      printf("a.some_b[%d].some_ushorts[%d] = %d\n", j, i, a.some_b[j].some_ushorts[i]);
    }
    for (i=0; i < 10; ++i)
    {
      printf("a.some_b[%d].int_list[%d] = %d\n", j, i, a.some_b[j].int_list[i]);
    }
  }

done:
  if (iFile) {
    fclose(iFile);
  }

  if (json_str) {
    free((void *)json_str);
  }

  delete_json_metadata(meta_a);

  return 0;
}
