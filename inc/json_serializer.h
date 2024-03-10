/*
 * json_serializer.h
 *
 *  Created on: Feb 3, 2024
 *      Author: Ben F.
 */

#ifndef _JSON_SERIALIZER_
#define _JSON_SERIALIZER_

#include "json-builder.h"

const char J_CHAR[]   = "chr";
const char J_UCHAR[]  = "uchr";
const char J_INT[]    = "int";
const char J_UINT[]   = "uint";
const char J_LONG[]   = "long";
const char J_ULONG[]  = "ulong";
const char J_SHORT[]  = "shrt";
const char J_USHORT[] = "ushrt";
const char J_DOUBLE[] = "dble";
const char J_STRING[] = "strg";

typedef struct json_metadata_field
{
  char *type;
  char *name;
  unsigned int offset;
  size_t size;
  int num_elements;
  struct json_metadata_field *next;
} json_metadata_field_t;

typedef struct json_metadata
{
  char *name;
  size_t size;
  json_metadata_field_t *field;
  struct json_metadata *next;
} json_metadata_t;

/**************************************************
 * Function to search for a metadata by name.
 * Return: NULL if not found.
 *************************************************/
json_metadata_t *find_json_metadata(const char *name,
                                    json_metadata_t *metadata);

/**************************************************
 * Function to search for a metadata field by name.
 * Return: NULL if not found.
 *************************************************/
json_metadata_field_t *find_metadata_field_by_name(char *name,
                                                   json_metadata_t *metadata);

/**************************************************
 * Function to search for a metadata field by type.
 * Return: NULL if not found.
 *************************************************/
json_metadata_field_t *find_metadata_field_by_type(const char *type,
                                                   json_metadata_t *metadata);

/**************************************************************
 * Function to add a new data type to the exiting linked list.
 * Return: the pointer of the created metadata.
 * Note: memory will be allocated.
 *************************************************************/
json_metadata_t *add_json_metadata(const char name[],  // data type name
                                   size_t size,       // size of the data type
                                   json_metadata_t *metadata);

/**************************************************************
 * Function to remove a metatdata field from the metedata.
 * Return: void
 * Note: memory will be deallocated.
 *************************************************************/
void delete_json_metadata_field(json_metadata_field_t *metadata_field);

/**************************************************************
 * Function to add a metatdata field to the metedata.
 * Return: void
 * Note: memory will be allocated.
 *************************************************************/
void add_json_metadata_field(json_metadata_t *metadata,
                             const char type[],
                             char *name,
                             size_t size,
                             int num_elements,
                             unsigned int offset);

/**************************************************************
 * Function to remove a metatdata from the metedata linked list.
 * Return: void
 * Note: memory will be deallocated.
 *************************************************************/
void delete_json_metadata(json_metadata_t *metadata);


/*****************************************************************
 * Function to serialize the target metatdata to the json string.
 * Return: 1 - success, 0 - failure
 ****************************************************************/
int serialize_metadata(json_value *parent_obj,
                       char *type,
                       char *name,
                       json_metadata_t *metadata,
                       void *input);

/**********************************************************************
 * Function to serialize the target metadata field to the json string.
 * Return: 1 - success, 0 - failure
 *********************************************************************/
int serialize_metadata_field(json_value *parent_obj,
                             json_metadata_field_t *field,
                             void *input,
                             json_metadata_t *metadata);

/**********************************************************************
 * Function to deserialize the target metadata from the json object.
 * Return: 1 - success, 0 - failure
 *********************************************************************/
int deserialize_meta(json_value *json_obj,
                     char *json_obj_name,
                     json_object_entry *obj_entry,
                     char *meta_name,
                     json_metadata_t *metadata,
                     void *output);  // data structure to be output

/**************************************************************************
 * Function to deserialize the target metadata field from the json object.
 * Return: 1 - success, 0 - failure
 *************************************************************************/
int deserialize_field(char *json_obj_name,
                      json_value *json_obj,
                      json_metadata_t *meta,
                      json_metadata_field_t *field,
                      void *output); // data structure to be output

/*********************************************************
 * Function to compare the content of two metadata fields
 * Return: 1 - same, 0 - difference(s)
 *********************************************************/
int compare_metadata_field( json_metadata_field_t * field,
                            json_metadata_t * metadata,
                            void * data_a,
                            void * data_b);

/*********************************************************
 * Function to compare the content of two metadata.
 * Return: 1 - same, 0 - difference(s)
 *********************************************************/
int compare_metadata( char * type,
                      json_metadata_t * metadata,
                      void * input_a,
                      void * input_b);

/*********************************************************
 * Function to display the content of the metadata field.
 * Return: void
 * Note: content will be output to the console
 *********************************************************/
void display_metadata_field(json_metadata_t * metadata,
                            json_metadata_field_t *field,
                            void * input,
                            int index);

/*********************************************************
 * Function to display the content of the metadata.
 * Return: void
 * Note: content will be output to the console
 *********************************************************/
void display_metadata(json_metadata_t * metadata,
                      char * type,
                      char * field_name,
                      void * input);

/**************************************************************
 * Function to search of the content for the target data type
 * Return: 1 - success, 0 - failure
 * Note: caller must allocated the memory for the output.
 **************************************************************/
int traverse_metadata(json_metadata_t * from_metadata,
                      char * from_type, /* the type that the input currently is */
                      void * input,
                      char * target_type,
                      char * target_field_name,
                      json_metadata_t ** found_metadata,
                      void ** output);

/**************************************************************
 * Function to serialize a binary object to a json string.
 * Return: 1 - success, 0 - failure
 * Note: caller must allocated the memory for the output.
 **************************************************************/
char * serialize_binary_to_json(json_metadata_t * metadata,
                                char * name_type,
                                void * input,
                                size_t * output_len );

/**************************************************************
 * Function to deserialize a binary object from a json string.
 * Return: 1 - success, 0 - failure
 * Note: caller must allocated the memory for the output.
 **************************************************************/
int deserialize_binary_from_json(char *input,
                                 json_metadata_t *metadata,
                                 char *type_name,
                                 void *output);

#endif
