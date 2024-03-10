/*
 * json_serializer.c
 *
 *  Created on: December 3, 2022
 *      Author: Ben F.
 */

#include "json-builder.h"
#include "json_serializer.h"

#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <time.h>

static char timestamp_s [80]={0};

char * timestamp()
{
    memset(timestamp,0,sizeof(timestamp_s));
    time_t ltime; /* calendar time */
    ltime=time(NULL); /* get current cal time */
    sprintf(timestamp_s, "%s", asctime( localtime(&ltime) ) );
    return timestamp_s;
}


#define P_SUCCESS 1
#define P_FAIL    0

#define LVL_DBG   0x01
#define LVL_INFO  0x02
#define LVL_WRN   0x04
#define LVL_ERR   0x08

/************************************************/
/* set the proper debuger level at compiler time */
/************************************************/
//#define ENABLED_LVL ( LVL_DBG | LVL_WRN | LVL_ERR )
//#define ENABLED_LVL ( LVL_DBG | LVL_INFO | LVL_WRN | LVL_ERR )
//define ENABLED_LVL ( LVL_PRN )
#define ENABLED_LVL ( LVL_WRN | LVL_ERR )

#define TIME_STAMP \
  printf("\n%s - ", timestamp());

#define FILE_NAME_LINE \
  printf(" [%s:%d]",__FILE__,__LINE__);

#define DEBUG_0(i,s) do {\
  if ((i&ENABLED_LVL)==i) {\
  TIME_STAMP printf(s); FILE_NAME_LINE }} while(0);

#define DEBUG_1(i,s,v1) do { \
  if ((i&ENABLED_LVL)==i) { \
  TIME_STAMP printf(s,v1); FILE_NAME_LINE }} while(0);

#define DEBUG_2(i,s,v1,v2) do { \
  if ((i&ENABLED_LVL)==i) { \
  TIME_STAMP printf(s,v1,v2); FILE_NAME_LINE }} while(0);

#define DEBUG_3(i,s,v1,v2,v3) do { \
  if ((i&ENABLED_LVL)==i) { \
  TIME_STAMP printf(s,v1,v2,v3); FILE_NAME_LINE }} while(0);

#define DEBUG_4(i,s,v1,v2,v3,v4) do { \
  if ((i&ENABLED_LVL)==i) { \
  TIME_STAMP printf(s,v1,v2,v3,v4); FILE_NAME_LINE }} while(0);

#define DEBUG_5(i,s,v1,v2,v3,v4,v5) do { \
  if ((i&ENABLED_LVL)==i) { \
  TIME_STAMP printf(s,v1,v2,v3,v4,v5); FILE_NAME_LINE }} while(0);

#define DEBUG_6(i,s,v1,v2,v3,v4,v5,v6) do { \
  if ((i&ENABLED_LVL)==i) { \
  TIME_STAMP printf(s,v1,v2,v3,v4,v5,v6); FILE_NAME_LINE }} while(0);

#define DISPLAY_0(s) do {\
  TIME_STAMP printf(s);} while(0);

#define DISPLAY_1(s,v1) do { \
  TIME_STAMP printf(s,v1);} while(0);

#define DISPLAY_2(s,v1,v2) do { \
  TIME_STAMP printf(s,v1,v2); } while(0);

#define DISPLAY_3(s,v1,v2,v3) do { \
  TIME_STAMP printf(s,v1,v2,v3); } while(0);

#define DISPLAY_4(s,v1,v2,v3,v4) do { \
  TIME_STAMP printf(s,v1,v2,v3,v4); } while(0);

#define DISPLAY_5(s,v1,v2,v3,v4,v5) do { \
  TIME_STAMP printf(s,v1,v2,v3,v4,v5); } while(0);

#define DISPLAY_6(s,v1,v2,v3,v4,v5,v6) do { \
  TIME_STAMP printf(s,v1,v2,v3,v4,v5,v6); } while(0);


#define ENT() DEBUG_0(LVL_DBG,"Entering")

#define RET_VOID() \
  DEBUG_0(LVL_DBG,"Exiting")

#define RET(s) \
  DEBUG_0(LVL_DBG,"Exitingn"); return s

static int check_type_name(char * json_obj_str, char *type, char *name)
{
   char *a = NULL;

   ENT();

   a = strchr(json_obj_str,'@');
   if (a == NULL) {
      //printf("'@' not found : %s",func,line,type);
      RET(P_FAIL);
   }
   if ( memcmp(type,json_obj_str,a-json_obj_str)!=0) {
      //printf("type not matched : %s",func,line,type);
      RET(P_FAIL);
   }
   if (name != NULL ) {
      // name is provided
      int a_len = strlen(a+1);
      int name_len = strlen(name);
      if ((a_len != name_len) || (strncmp(name,a+1,name_len)!=0) ) {
         RET(P_FAIL);
      }
      // check the type
      if ( memcmp(type,json_obj_str,a-json_obj_str)!=0) {
         //printf("name not matched : %s",func,line,name);
         RET(P_FAIL);
      }
   }
   //printf("found the matching type : %s, name : %s",func,line,type,name);
   RET(P_SUCCESS);
}

json_metadata_t * find_json_metadata( const char * name, json_metadata_t *metadata)
{
  json_metadata_t * curr = metadata;
  size_t len = strlen(name);
  size_t meta_len = 0; 

  ENT();

  if ( len <= 0) {
    DEBUG_0(LVL_ERR,"ERROR: invalid meta data name");
    RET(NULL);
  }

  if (curr != NULL) {
    meta_len = strlen(curr->name);
    if ((len == meta_len) && (strncmp(curr->name,name,meta_len) == 0)) {
      DEBUG_1(LVL_DBG,"found the meta data %s",curr->name);
      RET(curr);
    }
    else {
      while ( curr->next != NULL)
      {
        curr = curr->next;
        meta_len = strlen(curr->name);
        if ((len == meta_len) && (strncmp(curr->name,name,meta_len) == 0))
        {
          DEBUG_1(LVL_DBG,"found meta data %s",curr->name);
          RET(curr);
        }
      }
    }
  }
  DEBUG_2(LVL_DBG,"metadata not found : name: %s, size: %lu",name,len);
  RET(NULL);
}

json_metadata_field_t * find_metadata_field_by_name( char *name, json_metadata_t *metadata)
{
  json_metadata_field_t * curr = metadata->field;
  int name_len = strlen(name);

  ENT();

  DEBUG_2(LVL_DBG,"Searching field [%s] in Meta [%s]",name,metadata->name);

  while (curr != NULL)
  {
    int curr_len = strlen(curr->name);
    DEBUG_1(LVL_DBG,"checking curr->field [%s]",curr->name);
    if( (name_len == curr_len) && (strncmp(name,curr->name,curr_len) == 0)) {
      RET(curr);
    }
    // check the format "type@name"
    else if ((strchr(name,'@') != NULL) &&
             (check_type_name(name,curr->type,curr->name)))
    {
      DEBUG_2(LVL_DBG,"Found the field [%s] in Meta [%s]",name,metadata->name);
      RET(curr);
    }
    curr = curr->next;
  }

  RET(NULL);
}

json_metadata_field_t * find_metadata_field_by_type( const char *type, json_metadata_t *metadata)
{
  json_metadata_field_t * curr = metadata->field;
  int len = strlen(type);

  ENT();

  while (curr != NULL)
  {
    if (strncmp(type,curr->type,len) == 0) {
      RET(curr);
    }
    curr = curr->next;
  }

  RET(NULL);
}

json_metadata_t * add_json_metadata(const char name[], size_t size, json_metadata_t *metadata)
{
  json_metadata_t *curr = metadata;
  json_metadata_t *tmp = NULL;
  size_t len = strlen(name);

  ENT();

  if ((len <= 0) || (size <= 0))
  {
    DEBUG_0(LVL_ERR,"ERROR: invalid meta data");
    return NULL;
  }
  tmp = find_json_metadata(name, curr);
  if (tmp != NULL)
  {
    DEBUG_1(LVL_DBG,"meta data has already existed, returning %s",tmp->name);
    return tmp;
  }

  DEBUG_1(LVL_DBG,"Creating a new meta: %s", name);

  tmp = (json_metadata_t *)malloc(sizeof(json_metadata_t));
  memset(tmp, 0, sizeof(json_metadata_t));
  tmp->name = (char *)malloc(len + 1);
  memset(tmp->name, '\0', len + 1);
  strncpy(tmp->name, name, len);
  tmp->size = size;
  tmp->next = NULL;
  if (curr)
  {
    while (curr->next != NULL)
    {
      curr = curr->next;
    }
    DEBUG_2(LVL_DBG,"appending meta %s to %s", tmp->name, curr->name);
    curr->next = tmp;
  }

  DEBUG_1(LVL_DBG,"Returning meta %s", tmp->name);

  RET(tmp);
}

void delete_json_metadata_field(json_metadata_field_t *metadata_field)
{
  ENT();

  if ( metadata_field == NULL)
    return;

  if ( metadata_field->next != NULL ) {
    delete_json_metadata_field(metadata_field->next);
  }
  if (metadata_field->type) {
    free(metadata_field->type);
    metadata_field->type = NULL;
  }
  free(metadata_field);
  metadata_field=NULL;

  RET_VOID();
}

void add_json_metadata_field(json_metadata_t *metadata,
                             const char type[],
                             char *name,
                             size_t size,
                             int num_elements,
                             unsigned int offset)
{
  json_metadata_field_t * field = NULL;
  json_metadata_field_t * curr = NULL;
  size_t len = 0;

  ENT();

  len = strlen(type);
  if( (type == NULL) || (metadata == NULL)) {
    DEBUG_1(LVL_ERR,"ERROR: invalid data nothing to add, field name: %s",name);
    RET_VOID();
    //return;
  }

  field = (json_metadata_field_t *)malloc(sizeof(json_metadata_field_t));
  memset(field,0,sizeof(json_metadata_field_t));

  if ( len > 0 ) {
    field->type = (char*) malloc(len + 1);
    memset(field->type,'\0',len+1);
    strncpy(field->type,type,len);
  }

  field->size = size;
  field->num_elements = num_elements;
  len = strlen(name);
  if ( len > 0) {
    field->name = (char*) malloc(len + 1);
    memset(field->name,'\0',len+1);
    strncpy(field->name,name,len);
  }
  field->offset = offset;
  field->next = NULL;

  curr = metadata->field;
  if (curr == NULL) {
    DEBUG_3(LVL_DBG,"adding field %s to meta %s, size %lu",
        field->name,metadata->name,field->size);
    metadata->field = field;
  }
  else {
    while ( curr->next != NULL) {
      curr = curr->next;
    }
    DEBUG_3(LVL_DBG,"adding field %s to meta %s, size %lu",
        field->name,metadata->name,field->size);
    curr->next = field;
  }

  RET_VOID();
}

void delete_json_metadata(json_metadata_t *metadata)
{
  ENT();

  if ( metadata == NULL)
    return;

  if ( metadata->next != NULL ) {
    delete_json_metadata(metadata->next);
  }

  if (metadata->name) {
    free (metadata->name);
    metadata->name = NULL;
  }

  delete_json_metadata_field(metadata->field);
  free (metadata);
  metadata=NULL;

  RET_VOID();
}

int serialize_metadata_field(json_value * parent_obj,
                             json_metadata_field_t * field,
                             void * input,
                             json_metadata_t * metadata)
{
  int i = 0;

  ENT();

  DEBUG_2(LVL_INFO,"serialize field, name: %s, total size: %lu",
          field->name,field->size);

  if (field && parent_obj) {
    DEBUG_1(LVL_INFO,"number of elements: %d",field->num_elements);

    unsigned char non_type = 0;
    char typeName[128] = {0};
    void * start = (unsigned char *)input + field->offset;
    //int data_size = field->size/field->num_elements;
    int data_size = field->size;
    json_value * array_obj  = NULL;

    if (field->num_elements > 1) {
      array_obj  = json_array_new (0);
    }

    for (i = 0; i < field->num_elements; ++i) {
      void * curr = (unsigned char *)start + data_size*i;

      if (strcmp(field->type,J_UCHAR) == 0)
      {
        json_int_t tmp  = 0;
        unsigned char *p = (unsigned char *) curr;

        // make sure the size is correct
        if ( data_size > sizeof(unsigned char)) {
          DEBUG_4(LVL_WRN,"WARNING - the field name: %s size: %d > the actual data type %s's size [%lu], skipping...",
                  field->name,data_size,field->type,sizeof(unsigned char));
          continue;
        }

        tmp = *p;
        DEBUG_2(LVL_INFO,"field J_UCHAR detected, size: %d, data: 0x%x",
                data_size,(unsigned char)tmp);

        json_value * int_obj = json_integer_new (tmp);
        if (array_obj) {
          json_array_push(array_obj,int_obj);
        }
        else {
          json_object_push( parent_obj,field->name,int_obj);
        }
        break;
      }
      //else if (strcmp(field->type,J_CHAR) == 0)
      else if (strcmp(field->type,J_CHAR) == 0)
      {
        json_int_t tmp  = 0;
        char *p = (char *) curr;

        // make sure the size is correct
        if ( data_size > sizeof(char)) {
          DEBUG_4(LVL_WRN,"WARNING - the field name %s size: %d > the actual data type %s's size [%lu], skipping...",
                  field->name,data_size,field->type,sizeof(char));
          continue;
        }

        tmp = *p;
        DEBUG_2(LVL_INFO,"field char detected, size: %d, data: 0x%x",
                data_size,(char)tmp);

        json_value * int_obj = json_integer_new (tmp);
        if (array_obj) {
          json_array_push(array_obj,int_obj);
        }
        else {
          json_object_push( parent_obj,field->name,int_obj);
        }
      }
      else if( strcmp(field->type,J_SHORT) == 0)
      {
        json_int_t tmp  = 0;
        short *p = (short *) curr;

        // make sure the size is correct
        if ( data_size > sizeof(short)) {
          DEBUG_4(LVL_WRN,"WARNING - the field name %s size: %d > the actual data type %s's size [%lu], skipping...",
                  field->name,data_size,field->type,sizeof(short));
          continue;
        }

        tmp =  *p;
        DEBUG_2(LVL_INFO,"field short detected, size: %d, data: %lld",data_size,tmp);

        json_value * int_obj = json_integer_new (tmp);
        if (array_obj) {
          json_array_push(array_obj,int_obj);
        }
        else {
          json_object_push( parent_obj,field->name,int_obj);
        }
      }
      else if( strcmp(field->type,J_USHORT) == 0)
      {
        json_int_t tmp  = 0;
        unsigned short *p = (unsigned short *) curr;

        // make sure the size is correct
        if ( data_size > sizeof(unsigned short)) {
          DEBUG_4(LVL_WRN,"WARNING - the field name %s size: %d > the actual data type %s's size [%lu], skipping...",
                  field->name,data_size,field->type,sizeof(unsigned short));
          continue;
        }

        tmp =  *p;
        DEBUG_2(LVL_INFO,"field ushort detected, size: %d, data: %lld",data_size,tmp);

        json_value * int_obj = json_integer_new (tmp);
        if (array_obj) {
          json_array_push(array_obj,int_obj);
        }
        else {
          json_object_push( parent_obj,field->name,int_obj);
        }
      }
      else if( strcmp(field->type,J_INT) == 0)
      {
        json_int_t tmp  = 0;
        int *p = (int *) curr;

        /*
        // make sure the size is correct
        if ( data_size > sizeof(int)) {
            DEBUG_4(LVL_WRN,"WARNING - the field name %s size: %d > the actual data type %s's size [%d], skipping...",
                    field->name,data_size,field->type,sizeof(int));
            continue;
        }
        */

        tmp =  *p;
        DEBUG_2(LVL_INFO,"field int detected, size: %d, data: %lld",data_size,tmp);

        json_value * int_obj = json_integer_new (tmp);
        if (array_obj) {
          json_array_push(array_obj,int_obj);
        }
        else {
          json_object_push( parent_obj,field->name,int_obj);
        }
      }
      else if( strcmp(field->type,J_UINT) == 0)
      {
        json_int_t tmp  = 0;
        unsigned int *p = (unsigned int *) curr;

        /*
        // make sure the size is correct
        if ( data_size > sizeof(unsigned int)) {
            DEBUG_4(LVL_WRN,"WARNING - the field name %s size: %d > the actual data type %s's size [%d], skipping...",
                    field->name,data_size,field->type,sizeof(unsigned int));
            continue;
        }
        */

        tmp =  *p;
        DEBUG_2(LVL_INFO,"field unsigned int detected, size: %d, data: %lld",data_size,tmp);

        json_value * uint_obj = json_integer_new (tmp);
        if (array_obj) {
          json_array_push(array_obj,uint_obj);
        }
        else {
          json_object_push( parent_obj,field->name,uint_obj);
        }
      }
      else if (strcmp(field->type,J_LONG) == 0)
      {
        json_int_t tmp  = 0;
        long *p = (long *) curr;

        // make sure the size is correct
        if ( data_size > sizeof(long)) {
          DEBUG_4(LVL_WRN,"WARNING - the field name %s size: %d > the actual data type %s's size [%lu], skipping...",
                  field->name,data_size,field->type,sizeof(long));
          continue;
        }

        tmp = *p;
        DEBUG_2(LVL_INFO,"field long detected, size: %d, data: %lld",data_size,tmp);

        json_value * int_obj = json_integer_new (tmp);
        if (array_obj) {
          json_array_push(array_obj,int_obj);
        }
        else {
          json_object_push( parent_obj,field->name,int_obj);
        }
      }
      else if (strcmp(field->type,J_ULONG) == 0)
      {
        json_int_t tmp  = 0;
        unsigned long *p = (unsigned long *) curr;

        // make sure the size is correct
        if ( data_size > sizeof(unsigned long)) {
          DEBUG_4(LVL_WRN,"WARNING - the field name %s size: %d > the actual data type %s's size [%lu], skipping...",
                  field->name,data_size,field->type,sizeof(unsigned long));
          continue;
        }

        tmp = *p;
        DEBUG_2(LVL_INFO,"field unsigned long detected, size: %d, data: %lld",data_size,tmp);

        json_value * uint_obj = json_integer_new (tmp);
        if (array_obj) {
          json_array_push(array_obj,uint_obj);
        }
        else {
          json_object_push( parent_obj,field->name,uint_obj);
        }
      }
      else if( strcmp(field->type,J_DOUBLE) == 0)
      {
        double tmp  = 0;

        // make sure the size is correct
        if ( data_size != sizeof(double)) {
          DEBUG_4(LVL_WRN,"WARNING - the field name %s size: %d > the actual data type %s's size [%lu], skipping...",
                  field->name,data_size,field->type,sizeof(double));
          continue;
        }

        memcpy( &tmp, curr, data_size);
        DEBUG_2(LVL_INFO,"field integer detected, size: %d, data: %f",data_size,tmp);
        json_value * dbl_obj = json_double_new (tmp);
        if (array_obj) {
          json_array_push(array_obj,dbl_obj);
        }
        else {
          json_object_push( parent_obj,field->name,dbl_obj);
        }
      }
      else if( strcmp(field->type,J_STRING) == 0)
      {
        int len = data_size;
        char * tmp = (char * )malloc(len+1);
        memset(tmp,'\0',len);
        memcpy( tmp, curr, data_size);
        tmp[data_size] = '\0'; // just to make sure it has null terminator
        DEBUG_3(LVL_INFO,"field string detected: %s, data_size: %d, curr data size: %lu",
                tmp,data_size,strlen((char*) curr));
        json_value * str_obj = json_string_new (tmp);
        if (array_obj) {
          json_value * obj = json_object_new (0);
          json_object_push(obj,field->name,str_obj);
          json_array_push( array_obj,obj);
        }
        else {
          DEBUG_1(LVL_DBG,"pushing string to the parent object %s",field->name);
          json_object_push( parent_obj,field->name,str_obj);
        }

        if (tmp) {
          free(tmp);
          tmp = NULL;
        }
      }
      else {
        // non json type
        non_type = 1;
        DEBUG_2(LVL_INFO,"detected non json type: %s, for field: %s",field->type,field->name);

        memset(typeName,'\0',128);
        snprintf(typeName, 128, "%s@%s",field->type,field->name);
        DEBUG_1(LVL_DBG,"typeName: %s",typeName);

        if (array_obj) {
          serialize_metadata(array_obj, field->type, typeName, metadata, curr);
        }
        else {
          serialize_metadata(parent_obj,field->type, typeName, metadata, curr);
        }
      }
    }
    if (array_obj) {
      // push the array to the parent
      if ( non_type == 1) {
        // need to write the custom data type name to json
        json_object_push( parent_obj,typeName,array_obj);
      }
      else {
        json_object_push( parent_obj,field->name,array_obj);
      }
    }
  }
  RET(P_SUCCESS);
}

int serialize_metadata(json_value * parent_obj,
                        char * type,
                        char * name, /* name in the format of "type@name" */
                        json_metadata_t * metadata,
                        void * input)
{
  json_value * this_obj = NULL;
  json_metadata_t * this_metadata = find_json_metadata(type,metadata);

  ENT();

  if ( this_metadata == NULL ) {
    DEBUG_2(LVL_ERR,"ERROR: invalid metadata name: %s, type: %s",name,type);
    RET(P_FAIL);
  }

  this_obj = json_object_new (0);
  DEBUG_1(LVL_INFO,"serializing meta %s",this_metadata->name);
  json_metadata_field_t * curr_field = this_metadata->field;
  while (curr_field) {
    serialize_metadata_field(this_obj,curr_field,input,metadata);
    curr_field = curr_field->next;
  }
  if (parent_obj->type == json_array) {
    json_array_push(parent_obj,this_obj);
  }
  else {
    //json_object_push( parent_obj,this_metadata->name,this_obj);
    json_object_push(parent_obj,name,this_obj);
  }

  RET(P_SUCCESS);

}

int compare_metadata_field( json_metadata_field_t * field,
                            json_metadata_t * metadata,
                            void * data_a, void * data_b)
{
  int i = 0;

  ENT();

  if (field)
  {
    void * start_a = (unsigned char *)data_a + field->offset;
    void * start_b = (unsigned char *)data_b + field->offset;
    int data_size = field->size;

    DEBUG_2(LVL_INFO, "comparing field: %s, size: %lu",field->name,field->size);
    DEBUG_1(LVL_INFO, "number of elements: %d", field->num_elements);

    for (i = 0; i < field->num_elements; ++i)
    {
      void * curr_a = (unsigned char *)start_a + data_size*i;
      void * curr_b = (unsigned char *)start_b + data_size*i;

      if (strcmp(field->type,J_UCHAR) == 0)
      {
        unsigned char *a = (unsigned char *) curr_a;
        unsigned char *b = (unsigned char *) curr_b;

        // make sure the size is correct
        /* if ( data_size > sizeof(unsigned char)) {
          DEBUG_3(LVL_WRN,"WARNING - the field size: %d > the actual data type %s's size [%d]",
                  data_size,field->type,sizeof(unsigned char));
          //continue;
        }
        */

        if (*a != *b) {
            DISPLAY_3("field %s is different, 0x%x != 0x%x",
                    field->name,*a, *b);
            RET(P_FAIL);
        }
      }
      else if (strcmp(field->type,J_CHAR) == 0)
      {
        char *a = (char *) curr_a;
        char *b = (char *) curr_b;

        // make sure the size is correct
        /* if ( data_size > sizeof(char)) {
          DEBUG_3(LVL_WRN,"WARNING - the field size: %d > the actual data type %s's size [%d]",
                  data_size,field->type,sizeof(char));
          //continue;
        }
        */

        if (*a != *b) {
            DISPLAY_3("field %s is different, 0x%x != 0x%x",
                    field->name,*a, *b);
            RET(P_FAIL);
        }
      }
      else if( strcmp(field->type,J_SHORT) == 0)
      {
        short *a = (short *) curr_a;
        short *b = (short *) curr_b;

        if (*a != *b) {
            DISPLAY_3("field %s is different, %d != %d",
                    field->name,*a, *b);
            RET(P_FAIL);
        }

        // make sure the size is correct
        /*
           if ( data_size > sizeof(int)) {
           printf("WARNING - the field size [%d] > the actual data type [%s] size [%d], skipping...",
           __FUNCTION__,__LINE__,data_size,field->type,sizeof(int));
           continue;
           }
           */
      }
      else if( strcmp(field->type,J_USHORT) == 0)
      {
        unsigned short *a = (unsigned short *) curr_a;
        unsigned short *b = (unsigned short *) curr_b;

        if (*a != *b) {
            DISPLAY_3("field %s is different, %u != %u",
                    field->name,*a, *b);
            RET(P_FAIL);
        }

        // make sure the size is correct
        /*
           if ( data_size > sizeof(int)) {
           printf("WARNING - the field size [%d] > the actual data type [%s] size [%d], skipping...",
           __FUNCTION__,__LINE__,data_size,field->type,sizeof(int));
           continue;
           }
           */
      }
      else if( strcmp(field->type,J_INT) == 0)
      {
        int *a = (int *) curr_a;
        int *b = (int *) curr_b;

        if (*a != *b) {
            DISPLAY_3("field %s is different, %d != %d",
                    field->name,*a, *b);
            RET(P_FAIL);
        }

        // make sure the size is correct
        /*
           if ( data_size > sizeof(int)) {
           printf("WARNING - the field size [%d] > the actual data type [%s] size [%d], skipping...",
           __FUNCTION__,__LINE__,data_size,field->type,sizeof(int));
           continue;
           }
           */
      }
      else if( strcmp(field->type,J_UINT) == 0)
      {
        unsigned int *a = (unsigned int *) curr_a;
        unsigned int *b = (unsigned int *) curr_b;

        if (*a != *b) {
            DISPLAY_3("field %s is different, %u != %u",
                    field->name,*a, *b);
            RET(P_FAIL);
        }

        // make sure the size is correct
        /*
           if ( data_size > sizeof(unsigned int)) {
           printf("WARNING - the field size [%d] > the actual data type [%s] size [%d], skipping...",
           __FUNCTION__,__LINE__,data_size,field->type,sizeof(unsigned int));
           continue;
           }
           */
      }
      else if (strcmp(field->type,J_LONG) == 0)
      {
        long *a = (long *) curr_a;
        long *b = (long *) curr_b;

        if (*a != *b) {
            DISPLAY_3("field %s is different, %ld != %ld",
                    field->name,*a, *b);
            RET(P_FAIL);
        }

        // make sure the size is correct
        //if ( data_size > sizeof(long)) {
        //  DEBUG_3(LVL_WRN,"WARNING - the field size: %d > the actual data type %s's size: %d, skipping...",data_size,field->type,sizeof(long));
        //  continue;
        // }
      }
      else if (strcmp(field->type,J_ULONG) == 0)
      {
        unsigned long *a = (unsigned long *) curr_a;
        unsigned long *b = (unsigned long *) curr_b;

        if (*a != *b) {
            DISPLAY_3("field %s is different, %lu != %lu",
                    field->name,*a, *b);
            RET(P_FAIL);
        }
      }
      else if( strcmp(field->type,J_DOUBLE) == 0)
      {
        double *a = (double *) curr_a;
        double *b = (double *) curr_b;

        if (*a != *b) {
            DISPLAY_3("field %s is different, %f != %f",
                    field->name,*a, *b);
            RET(P_FAIL);
        }
      }
      else if( strcmp(field->type,J_STRING) == 0)
      {
        char *a = (char *) curr_a;
        char *b = (char *) curr_b;
        int len = data_size;

        if (strncmp(a,b,len) != 0)
        {
          DISPLAY_3( "field %s is different, \"%s\" != \"%s\"",
                  field->name, a, b);
          RET(P_FAIL);
        }
      }
      else
      {
        // non json type
        DEBUG_2(LVL_INFO,"detected non json type: %s, field: %s",field->type,field->name);

        if (!compare_metadata(field->type, metadata, curr_a, curr_b))
        {
            DISPLAY_1( "Meta data [%s] is different", field->type);
            RET(P_FAIL);
        }
      }
    }
  }
  else
  {
    DEBUG_0(LVL_ERR, "field is invalid");
    RET(P_FAIL);
  }

  RET(P_SUCCESS);
}

int compare_metadata( char * type,
                      json_metadata_t * metadata,
                      void * input_a,
                      void * input_b)
{
  json_metadata_t * this_metadata = find_json_metadata(type,metadata);

  ENT();

  if ( this_metadata == NULL ) {
    DEBUG_1(LVL_ERR,"ERROR: invalid metadata %s",type);
    RET(P_FAIL);
  }

  DEBUG_1(LVL_DBG,"Comparing data: %s",this_metadata->name);
  json_metadata_field_t * curr_field = this_metadata->field;

  while (curr_field) {
    if (!compare_metadata_field(curr_field,metadata,input_a,input_b) )
    {
      DEBUG_2(LVL_WRN,"The field [%s] in Metedata [%s] is different",
              curr_field->name, this_metadata->name);
      RET(P_FAIL);
    }
    curr_field = curr_field->next;
  }

  DEBUG_1(LVL_DBG,"Data are equal: %s",this_metadata->name);

  RET(P_SUCCESS);
}

void display_metadata_field(json_metadata_t * metadata,
                            json_metadata_field_t *field, void * input, int index)
{
  int i = 0;

  ENT();
  if (field)
  {
    int data_size = field->size;
    void * start = (unsigned char *)input + field->offset;
    unsigned char is_array = (field->num_elements>1)?1:0;

    DEBUG_2(LVL_DBG, "display field, name: %s, total size: %lu",
            field->name,field->size);
    DEBUG_1(LVL_INFO, "number of elements: %d", field->num_elements);

    int num = field->num_elements;
    int start_idx = 0;
    if( index > -1 && index < num) {
      start_idx = index;
      num = start_idx+1;
    }

    for ( i = start_idx; i < num; ++i)
    {
      void * curr = (unsigned char *)start + data_size*i;

      if ((strcmp(field->type,J_UCHAR) == 0) ||
          (strcmp(field->type,J_CHAR) == 0) )
      {
        unsigned char *data = (unsigned char *) curr;
        if (is_array) {
          DISPLAY_3( "%s[%d] - [0x%x]", field->name, i, *data);
        }
        else {
          DISPLAY_2( "%s [0x%x]", field->name, *data);
        }
      }
      else if( strcmp(field->type,J_SHORT) == 0)
      {
        short *data = (short *) curr;
        if (is_array) {
          DISPLAY_3( "%s[%d] - [%d]", field->name, i, *data);
        }
        else {
          DISPLAY_2( "%s [%d]", field->name, *data);
        }
      }
      else if( strcmp(field->type,J_USHORT) == 0)
      {
        unsigned short *data = (unsigned short *) curr;
        if (is_array) {
          DISPLAY_3( "%s[%d] - [%u]", field->name, i, *data);
        }
        else {
          DISPLAY_2( "%s [%u]", field->name, *data);
        }
      }
      else if( strcmp(field->type,J_INT) == 0)
      {
        int *data = (int *) curr;
        if (is_array) {
          DISPLAY_3( "%s[%d] - [%d]", field->name, i, *data);
        }
        else {
          DISPLAY_2( "%s [%d]", field->name, *data);
        }
      }
      else if( strcmp(field->type,J_UINT) == 0)
      {
        unsigned int *data = (unsigned int *) curr;
        if (is_array) {
          DISPLAY_3( "%s[%d] - [%u]", field->name, i, *data);
        }
        else {
          DISPLAY_2( "%s [%u]", field->name, *data);
        }
      }
      else if (strcmp(field->type,J_LONG) == 0)
      {
        long *data = (long *) curr;
        if (is_array) {
          DISPLAY_3( "%s[%d] - [%ld]", field->name, i, *data);
        }
        else {
          DISPLAY_2( "%s [%ld]", field->name, *data);
        }
      }
      else if (strcmp(field->type,J_ULONG) == 0)
      {
        unsigned long *data = (unsigned long *) curr;
        if (is_array) {
          DISPLAY_3( "%s[%d] - [%lu]", field->name, i, *data);
        }
        else {
          DISPLAY_2( "%s [%lu]", field->name, *data);
        }
      }
      else if( strcmp(field->type,J_DOUBLE) == 0)
      {
        double *data = (double *) curr;
        if (is_array) {
          DISPLAY_3( "%s[%d] - [%f]", field->name, i, *data);
        }
        else {
          DISPLAY_2( "%s [%f]", field->name, *data);
        }
      }
      else if( strcmp(field->type,J_STRING) == 0)
      {
        char *data = (char *) curr;
        int len = data_size;
        char *tmp = (char * )malloc(len+1);
        tmp[len] = '\0';

        strncpy(tmp,data,len);
        if (is_array) {
          DISPLAY_3( "%s[%d] - [%s]", field->name, i, tmp);
        }
        else {
          DISPLAY_2( "%s [%s]", field->name, tmp);
        }
        if (tmp) {
          free(tmp);
          tmp = NULL;
        }
      }
      else
      {
        // non json type
        DEBUG_2(LVL_INFO,"detected non json type: %s, field: %s",field->type,field->name);
        if (is_array) {
          DISPLAY_2( "[%d] - %s:", i, field->name);
        }
        else {
          DISPLAY_1( "%s:", field->name);
        }
        display_metadata(metadata, field->type, field->name, curr);
      }
    }
  }
  else
  {
    DEBUG_0(LVL_ERR, "field is invalid");
  }

  RET_VOID();
}

void display_metadata(json_metadata_t * metadata,
                      char * type, char * field_name, void * input)
{
  json_metadata_t * this_metadata = find_json_metadata(type,metadata);
  json_metadata_field_t * curr_field = NULL;
  unsigned char done = 0;

  ENT();
  if ( this_metadata == NULL ) {
    DEBUG_1(LVL_ERR,"ERROR: invalid metadata %s",type);
    RET_VOID();
  }

  curr_field = this_metadata->field;
  while (curr_field != NULL && !done)
  {
    if ((field_name != NULL) &&
        (strncmp(curr_field->type,type,strlen(curr_field->type))==0) &&
        (strncmp(curr_field->name,field_name,strlen(curr_field->name))==0))
    {
      done = 1;
    }
    display_metadata_field(metadata, curr_field, input,-1);
    curr_field = curr_field->next;
  }

  RET_VOID();

}

int traverse_metadata(json_metadata_t * from_metadata,
                      char * from_type, /* type the input currently is */
                      void * input,
                      char * target_type,
                      char * target_field_name,
                      json_metadata_t ** found_metadata,
                      void ** output)
{
  //void * tmp = input;
  unsigned char found = 0;
  json_metadata_field_t * curr_field = NULL;
  json_metadata_t * this_metadata = NULL;

  ENT();

  // get the metadata from the from_type
  this_metadata = find_json_metadata(from_type,from_metadata);
  if ( this_metadata == NULL ) {
    //DEBUG_0(LVL_ERR,"ERROR: invalid metadata");
    RET(0);
  }

  // first pass, check if any field matches in this metadata
  curr_field = this_metadata->field;
  while (curr_field != NULL)
  {
    if ((target_field_name != NULL) &&
        (strncmp(curr_field->type,target_type,strlen(curr_field->type))==0) &&
        (strncmp(curr_field->name,target_field_name,strlen(curr_field->name))==0))
    {
      DEBUG_2(LVL_DBG,"Found the target meta %s and field %s",
              this_metadata->name,curr_field->name);
      found = 1;
      *output = input+curr_field->offset;
      *found_metadata = this_metadata;
      break;
    }
    curr_field = curr_field->next;
  }

  if (!found)
  {
    // second pass
    curr_field = this_metadata->field;
    while (curr_field != NULL)
    {
      if (traverse_metadata(from_metadata,
                            curr_field->type,
                            input+curr_field->offset,
                            target_type,
                            target_field_name,
                            found_metadata,
                            output))
      {
        found = 1;
        break;
      }
      curr_field = curr_field->next;
    }
  }

  RET(found);

}

char* serialize_binary_to_json(json_metadata_t * metadata,
                                char * name_type,
                                void * input,
                                size_t *output_len )
{
  json_metadata_t * meta = NULL;
  json_value * root = NULL;
  json_value * ptr = NULL;
  size_t measured = 0;
  size_t serialized = 0;
  char * buf = NULL;

  ENT();

  DEBUG_1(LVL_INFO,"serializing %s",name_type);

  meta = find_json_metadata(name_type, metadata);
  if (meta == NULL) {
    DEBUG_1(LVL_ERR,"ERROR: metadata invalid, %s",name_type);
    RET(NULL);
  }
  // TO BE INFO
  DEBUG_1(LVL_INFO,"Found serialize_profile for meta %s",meta->name);

  root = json_object_new (0);

  if (serialize_metadata(root, name_type, meta->name, metadata, input) == P_FAIL)
  {
    RET(NULL);
  };

  json_serialize_opts serialize_opts = {
    json_serialize_mode_multiline,
    0, //json_serialize_opt_CRLF,
    3
  };

  ptr=root;
  measured = json_measure_ex (ptr, serialize_opts);
  DEBUG_1(LVL_INFO,"root len: %d", (int) measured);

  buf = (char *) malloc (measured);
  memset(buf,'\0',measured);
  json_serialize_ex (buf, ptr, serialize_opts);

  serialized = (int) strlen (buf); // + 1;
  if (serialized > measured) {
    serialized = measured;
  }
  buf[serialized] = '\0';
  *output_len = serialized;

  //DISPLAY_1("measured len: %d", measured);
  //DISPLAY_1("serialized len: %d", serialized);

  json_builder_free(root);
  //json_value_free (root);
  //free(root);
  return buf;
}

int deserialize_field(char* json_obj_name,
                      json_value *json_obj,
                      json_metadata_t *meta,
                      json_metadata_field_t *field,
                      void *output)
{
   char *field_name = field->name;
   int len = strlen(field_name);
   int i = 0;
   int num = field->num_elements;
   unsigned char is_array = 0;

   ENT();

   // get the json value name
   DEBUG_2(LVL_INFO,"field name [%s], type [%s]",field->name,field->type);
   DEBUG_1(LVL_DBG,"json object name [%s]",json_obj_name);

   if( strncmp(field_name,json_obj_name,len) != 0) {
      // check the format "type@name"
      if (!check_type_name(json_obj_name,field->type,field_name)) {
         RET(P_FAIL);
      }
   }
   DEBUG_1(LVL_DBG,"matched field : %s",field_name);

   if (num > 1) {
      DEBUG_1(LVL_DBG,"array size: %d",num);
      // make sure it is the array type in json obj
      int length = json_obj->u.object.length;
      if (num != length) {
         DEBUG_4(LVL_ERR,"ERROR: object name %s, field %s - sizes different, field array size: %d but json array size: %d",
                json_obj_name,field_name,num,length);
         RET(P_FAIL);
      }
      if (json_obj->type != json_array) {
         DEBUG_0(LVL_WRN,"WARNING: not a json array - skipping");
         RET(P_SUCCESS); // continue to deserialize another field
      }
      is_array = 1;
   }
   else if (json_obj->type == json_array)
   {
     DEBUG_2(LVL_WRN,"WARNING: %s is a json array but the object %s is not - skipping",json_obj_name,field_name);
     RET(P_SUCCESS);
   }

   DEBUG_1(LVL_INFO,"Total field size : %lu",field->size);
   int field_sz = field->size;
   for (i = 0; i < num; ++i)
   {
      // adjust the output position
      void *pos = (unsigned char*)output+field->offset + (field_sz*i);

      memset(pos,0,field_sz);

      DEBUG_2(LVL_DBG,"index [%d], size : %d",i,field_sz);

      if( strcmp(field->type,J_SHORT) == 0) {
         short data = 0;
         if (is_array==0) {
            data = (short)json_obj->u.integer;
         } else {
            data = (short)json_obj->u.array.values[i]->u.integer;
         }
         DEBUG_3(LVL_DBG,"%s[%d] = %d",field->name,i,data);
         // TODO: verify the field_sz before copying
         // make sure the size is correct before doing memcpy
         if ( field_sz == sizeof(short)) {
             memcpy(pos,&data,field_sz);
         }
         else if ( field_sz > sizeof(short)) {
             short *p = (short *)pos;
             *p = data;
         }
         else {
             DEBUG_4(LVL_ERR,"WARNING: object name %s, field %s size %u < data types(short) size: %lu, skipping... ",
                     json_obj_name,field_name,field_sz,sizeof(short));
         }
      }
      else if( strcmp(field->type,J_USHORT) == 0) {
         unsigned short data = 0;
         if (is_array==0) {
            data = (unsigned short) json_obj->u.integer;
         } else {
            data = (unsigned short) json_obj->u.array.values[i]->u.integer;
         }
         DEBUG_3(LVL_DBG,"%s[%d] = %d",field->name,i,data);
         // TODO: verify the field_sz before copying
         // memcpy(pos,&data,field_sz);
         // make sure the size is correct before doing memcpy
         if ( field_sz == sizeof(unsigned short)) {
             memcpy(pos,&data,field_sz);
         }
         else if ( field_sz > sizeof(unsigned short)) {
             unsigned short *p = (unsigned short *)pos;
             *p = data;
         }
         else {
             DEBUG_4(LVL_ERR,"WARNING: object name %s, field %s size %u < data types(unsigned short) size: %lu, skipping... ",
                     json_obj_name,field_name,field_sz,sizeof(unsigned short));
         }
      }
      else if( strcmp(field->type,J_INT) == 0) {
         long data = 0;
         if (is_array==0) {
            data = (int) json_obj->u.integer;
         } else {
            data = (int) json_obj->u.array.values[i]->u.integer;
         }
         DEBUG_3(LVL_DBG,"%s[%d] = %ld",field->name,i,data);
         // TODO: verify the field_sz before copying
         // memcpy(pos,&data,field_sz);
         // make sure the size is correct before doing memcpy
         if ( field_sz == sizeof(int)) {
             memcpy(pos,&data,field_sz);
         }
         else if ( field_sz > sizeof(int)) {
             int *p = (int *)pos;
             *p = data;
         }
         else {
             DEBUG_4(LVL_ERR,"WARNING: object name %s, field %s size %u < data types(int) size: %lu, skipping... ",
                     json_obj_name,field_name,field_sz,sizeof(int));
         }
      }
      else if( strcmp(field->type,J_UINT) == 0) {
         unsigned int data = 0;
         if (is_array==0) {
            data = (unsigned int) json_obj->u.integer;
         } else {
            data = (unsigned int) json_obj->u.array.values[i]->u.integer;
         }
         DEBUG_3(LVL_DBG,"%s[%d] = %u",field->name,i,data);
         // TODO: verify the field_sz before copying
         // memcpy(pos,&data,field_sz);
         // make sure the size is correct before doing memcpy
         if ( field_sz == sizeof(unsigned int)) {
             memcpy(pos,&data,field_sz);
         }
         else if ( field_sz > sizeof(unsigned int)) {
             unsigned int *p = (unsigned int *)pos;
             *p = data;
         }
         else {
             DEBUG_4(LVL_ERR,"WARNING: object name %s, field %s size %u < data types(unsigned int) size: %lu, skipping... ",
                     json_obj_name,field_name,field_sz,sizeof(unsigned int));
         }
      }
      else if( strcmp(field->type,J_LONG) == 0) {
         long data = 0;
         if (is_array==0) {
            data = (long) json_obj->u.integer;
         } else {
            data = (long) json_obj->u.array.values[i]->u.integer;
         }
         DEBUG_3(LVL_DBG,"%s[%d] = %ld",field->name,i,data);
         // TODO: verify the field_sz before copying
         // memcpy(pos,&data,field_sz);
         // make sure the size is correct before doing memcpy
         if ( field_sz == sizeof(long)) {
             memcpy(pos,&data,field_sz);
         }
         else if ( field_sz > sizeof(long)) {
             long *p = (long *)pos;
             *p = data;
         }
         else {
             DEBUG_4(LVL_ERR,"WARNING: object name %s, field %s size %u < data types(long) size: %lu, skipping... ",
                     json_obj_name,field_name,field_sz,sizeof(long));
         }
      }
      else if( strcmp(field->type,J_ULONG) == 0) {
         unsigned long data = 0;
         if (is_array==0) {
            data = (unsigned long) json_obj->u.integer;
         } else {
            data = (unsigned long) json_obj->u.array.values[i]->u.integer;
         }
         DEBUG_3(LVL_DBG,"%s[%d] = %lu",field->name,i,data);
         // TODO: verify the field_sz before copying
         // memcpy(pos,&data,field_sz);
         // make sure the size is correct before doing memcpy
         if ( field_sz == sizeof(unsigned long)) {
             memcpy(pos,&data,field_sz);
         }
         else if ( field_sz > sizeof(unsigned long)) {
             unsigned long *p = (unsigned long *)pos;
             *p = data;
         }
         else {
             DEBUG_4(LVL_ERR,"WARNING: object name %s, field %s size %u < data types(unsigned long) size: %lu, skipping... ",
                     json_obj_name,field_name,field_sz,sizeof(unsigned long));
         }
      }
      else if( strcmp(field->type,J_DOUBLE) == 0) {
         double data = .0;
         if (is_array==0) {
            data = (double) json_obj->u.dbl;
         } else {
            data = (double) json_obj->u.array.values[i]->u.dbl;
         }
         DEBUG_3(LVL_DBG,"%s[%d] = %lf",field->name,i,data);
         // TODO: verify the field_sz before copying
         // memcpy(pos,&data,field_sz);
         // make sure the size is correct before doing memcpy
         if ( field_sz == sizeof(double)) {
             memcpy(pos,&data,field_sz);
         }
         else if ( field_sz > sizeof(double)) {
             double *p = (double *)pos;
             *p = data;
         }
         else {
             DEBUG_4(LVL_ERR,"WARNING: object name %s, field %s size %d < data types(double) size: %lu, skipping... ",
                     json_obj_name,field_name,field_sz,sizeof(double));
         }
      }
      else if( strcmp(field->type,J_UCHAR) == 0) {
         unsigned char data = 0;
         if (is_array==0) {
            data = (unsigned char) json_obj->u.integer;
         } else {
            data = (unsigned char) json_obj->u.array.values[i]->u.integer;
         }
         DEBUG_4(LVL_DBG," %s[%d] = %u, field size: %d",field->name,i,data,field_sz);
         // TODO: verify the field_sz before copying
         // memcpy(pos,&data,field_sz);
         // make sure the size is correct before doing memcpy
         if ( field_sz == sizeof(unsigned char)) {
             memcpy(pos,&data,field_sz);
         }
         else if ( field_sz > sizeof(unsigned char)) {
             unsigned char *p = (unsigned char *)pos;
             *p = data;
         }
         else {
             DEBUG_4(LVL_ERR,"WARNING: object name %s, field %s size %u < data types(unsigned char) size: %lu, skipping... ",
                     json_obj_name,field_name,field_sz,sizeof(unsigned char));
         }
      }
      else if( strcmp(field->type,J_CHAR) == 0) {
         char data = 0;
         if (is_array==0) {
            data = (char) json_obj->u.integer;
         } else {
            data = (char) json_obj->u.array.values[i]->u.integer;
         }
         DEBUG_4(LVL_DBG," %s[%d] = %u, field size: %d",field->name,i,data,field_sz);
         // TODO: verify the field_sz before copying
         // memcpy(pos,&data,field_sz);
         // make sure the size is correct before doing memcpy
         if ( field_sz == sizeof(char)) {
             memcpy(pos,&data,field_sz);
         }
         else if ( field_sz > sizeof(char)) {
             char *p = (char *)pos;
             *p = data;
         }
         else {
             DEBUG_4(LVL_ERR,"WARNING: object name %s, field %s size %u < data types(char) size: %lu, skipping... ",
                     json_obj_name,field_name,field_sz,sizeof(char));
         }
      }
      else if( strcmp(field->type,J_STRING) == 0) {
         char * data = NULL;
         int data_len = 0;
         int obj_type =0;
         if (is_array==0) {
            data = (char *)json_obj->u.string.ptr;
            data_len = json_obj->u.string.length;
            obj_type = json_obj->type;
         }
         else {
            //data = (char *)json_obj->u.array.values[i]->u.string.ptr;
            //data_len = json_obj->u.array.values[i]->u.string.length;
            //obj_type = json_obj->u.array.values[i]->type;
            data = (char *)json_obj->u.array.values[i]->u.object.values[0].value->u.string.ptr;
            data_len = json_obj->u.array.values[i]->u.object.values[0].value->u.string.length;
         }

         DEBUG_1(LVL_DBG,"object type %d",obj_type);
         /*
         if( obj_type != json_string)
         {
            DEBUG_5(LVL_WRN,"json object type %d is not a string, meta name [%s] field name [%s] ",
                    __FUNCTION__,__LINE__,obj_type, meta->name, field_name);
         }
         */
         DEBUG_2(LVL_INFO,"data len %d, field_sz: %d",data_len,field_sz);

         if (field_sz < data_len) {
           data_len = field_sz;
           data[field_sz] = '\0';
         }

         DEBUG_3(LVL_DBG,"%s[%d] = %s",field->name,i,data);

         memset(pos,'\0',field_sz);
         memcpy(pos,data,data_len);
      }
      else {
         // other custom data struct
         DEBUG_2(LVL_INFO,"custome data type[%s], size[%d] ",field->type,field_sz);

         json_value *obj = NULL;
         json_object_entry *json_entry = NULL;
         if (is_array==0) {
            obj = json_obj;
            json_entry = &json_obj->u.object.values[i];
            deserialize_meta(json_obj,json_obj_name,json_entry,field->type,meta,pos);
         }
         else {
            obj = json_obj->u.array.values[i];
            json_entry = &obj->u.object.values[i];
            deserialize_meta(obj,json_obj_name,json_entry,field->type,meta,pos);
         }
      }
   }

   RET(P_SUCCESS);
}

int deserialize_meta(json_value *json_obj,
                     char * json_obj_name,
                     json_object_entry *obj_entry,
                     char *meta_name,
                     json_metadata_t *metadata,
                     void *output)
{
   int name_len = 0;
   int obj_len = 0;
   json_metadata_field_t * field = NULL;
   int k = 0;

   ENT();

   if (metadata == NULL) {
      DEBUG_0(LVL_ERR,"ERROR: invalid metadata");
      RET(P_FAIL);
   }

   DEBUG_1(LVL_INFO,"deserialize json object: %s", json_obj_name);

   // get the meta type from the json object
   name_len = strlen(meta_name);
   if ( (strncmp(meta_name,json_obj_name,name_len)!=0) &&
        (check_type_name(json_obj_name,meta_name,NULL) == 0))
   {
      DEBUG_2(LVL_ERR,"ERROR: meta name: %s not matching with json object name: %s !",
              meta_name,json_obj_name);
      RET(P_FAIL);
   }

   /* Get the meta type */
   json_metadata_t *m = find_json_metadata( meta_name, metadata);
   if ( m == NULL) {
      DEBUG_1(LVL_ERR,"ERROR: meta: %s not found!",meta_name);
      RET(P_FAIL);
   }

   /*
   For each json object, find the matching field from the meta type
   and deserialize the matching field.

   In case of no matching field, continue next json object
   until all the json objects in this meta type are done.
   */
   obj_len = json_obj->u.object.length;
   for (k = 0; k < obj_len; k++) {

      json_object_entry *obj_entry = &json_obj->u.object.values[k];

      DEBUG_1(LVL_INFO,"json object name: %s",obj_entry->name);
      json_value* obj = obj_entry->value;

      field = find_metadata_field_by_name(obj_entry->name,m);

      if (field == NULL) {
         DEBUG_1(LVL_WRN,"WARNING - field[%s] not found in the meta, perhaps the old field was deleted?", obj_entry->name);
      }
      else if (!deserialize_field(obj_entry->name,obj,metadata,field,output)) {
         DEBUG_1(LVL_ERR,"ERROR: error occures when deserializing field[%s], perhaps the old field was modified?. Continue...", obj_entry->name);
      }
   } // for

   RET(P_SUCCESS);
}

int deserialize_binary_from_json(char *input,
                                 json_metadata_t *metadata,
                                 char *type_name,
                                 void *output)
{
  int obj_len = 0;
  int i = 0;
  json_value *value = NULL;
  char error[2048] = {0};
  int ret = 0;

  ENT();

  //value = json_parse(input,strlen(input));

  json_settings settings = { 0 };
  settings.settings = json_enable_comments;
  //value = json_parse_ex (&settings, input, strlen(input), 0);
  value = json_parse_ex (&settings, input, strlen(input), error);

  if (metadata == NULL || output == NULL)
  {
    DEBUG_0(LVL_ERR,"ERROR: invalid metadata or output object");
    RET(P_FAIL);
  }

  if (value == NULL)
  {
    DEBUG_1(LVL_ERR,"Encountered error from the parser: %s, returning...", error);
    //DEBUG_0(LVL_ERR,"No more json object to process, returning...");
    RET(P_FAIL);
  }

  obj_len = value->u.object.length;
  for (i = 0; i < obj_len; i++)
  {
    json_object_entry *json_entry = &value->u.object.values[i];

    DEBUG_2(LVL_INFO,"Start deserializing json object[%d]%s",i,json_entry->name);
    ret = deserialize_meta(json_entry->value,
                           json_entry->name,
                           json_entry,
                           type_name,
                           metadata,
                           output);
    if( ret == P_FAIL)
    {
      DEBUG_2(LVL_ERR,"ERROR: deserialize_mata failed for json_entry : %s, type : %s", json_entry->name,type_name);
    }
  }

  json_value_free (value);
  //if ( value != NULL)
  //{
  //  free (value);
  //}
  value = NULL;

  RET(ret);
}
