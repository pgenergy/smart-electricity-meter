/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.9-dev */

#ifndef PB_ELDATA_PB_H_INCLUDED
#define PB_ELDATA_PB_H_INCLUDED
#include <pb.h>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Struct definitions */
typedef struct _ELData {
    char sensorId[128];
    float sensorValue;
} ELData;


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define ELData_init_default                      {"", 0}
#define ELData_init_zero                         {"", 0}

/* Field tags (for use in manual encoding/decoding) */
#define ELData_sensorId_tag                      1
#define ELData_sensorValue_tag                   2

/* Struct field encoding specification for nanopb */
#define ELData_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, STRING,   sensorId,          1) \
X(a, STATIC,   SINGULAR, FLOAT,    sensorValue,       2)
#define ELData_CALLBACK NULL
#define ELData_DEFAULT NULL

extern const pb_msgdesc_t ELData_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define ELData_fields &ELData_msg

/* Maximum encoded size of messages (where known) */
#define ELDATA_PB_H_MAX_SIZE                     ELData_size
#define ELData_size                              135

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
