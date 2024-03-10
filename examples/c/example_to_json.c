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
  strcpy(&a.some_string[0],"HELLO.");
  a.some_uinteger = 1234;
  a.some_long_integer = 24689123456;
  a.some_double = 0.52;
  a.some_b[0].some_shorts[0] = -1;
  a.some_b[0].some_shorts[1] = -2;
  a.some_b[0].some_shorts[2] = -3;
  a.some_b[0].some_shorts[3] = -4;
  a.some_b[0].some_shorts[4] = -5;
  a.some_b[1].some_shorts[0] = -11;
  a.some_b[1].some_shorts[1] = -12;
  a.some_b[1].some_shorts[2] = -13;
  a.some_b[1].some_shorts[3] = -14;
  a.some_b[1].some_shorts[4] = -15;
  a.some_b[0].some_ushorts[0] = 1;
  a.some_b[0].some_ushorts[1] = 2;
  a.some_b[0].some_ushorts[2] = 3;
  a.some_b[0].some_ushorts[3] = 4;
  a.some_b[0].some_ushorts[4] = 5;
  a.some_b[1].some_ushorts[0] = 11;
  a.some_b[1].some_ushorts[1] = 12;
  a.some_b[1].some_ushorts[2] = 13;
  a.some_b[1].some_ushorts[3] = 14;
  a.some_b[1].some_ushorts[4] = 15;
  a.some_b[0].int_list[0] = -10;
  a.some_b[0].int_list[1] = -20;
  a.some_b[0].int_list[2] = -30;
  a.some_b[0].int_list[3] = -40;
  a.some_b[0].int_list[4] = -50;
  a.some_b[0].int_list[5] = -60;
  a.some_b[0].int_list[6] = -70;
  a.some_b[0].int_list[7] = -80;
  a.some_b[0].int_list[8] = -90;
  a.some_b[0].int_list[9] = -100;
  a.some_b[1].int_list[0] = 10;
  a.some_b[1].int_list[1] = 20;
  a.some_b[1].int_list[2] = 30;
  a.some_b[1].int_list[3] = 40;
  a.some_b[1].int_list[4] = 50;
  a.some_b[1].int_list[5] = 60;
  a.some_b[1].int_list[6] = 70;
  a.some_b[1].int_list[7] = 80;
  a.some_b[1].int_list[8] = 90;
  a.some_b[1].int_list[9] = 100;

  json_str = serialize_binary_to_json(meta_a, (char *)"A_t", (void *) &a, &len);

  printf("%s\n",json_str);

  iFile = fopen("test.json", "wr");
  if (iFile == NULL) {
    perror("Failed to open example.json") ;
    goto done;
  }

  if( !fwrite(json_str,1,len,iFile) )
  {
    perror("fwrite failed");
    goto done;
  }

done:

  if (iFile) {
    fclose(iFile);
  }

  if (json_str)
  {
    free((void *)json_str);
  }

  delete_json_metadata(meta_a);

  return 0;

}
