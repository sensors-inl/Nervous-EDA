/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.8-dev */

#ifndef PB_RENFORCE_PB_H_INCLUDED
#define PB_RENFORCE_PB_H_INCLUDED
#include <pb.h>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Struct definitions */
/* ** Date/Time message to set RTC clock and get timestamps */
typedef struct _Timestamp {
    uint64_t time; /* POSIX timestamp (seconds elapsed since 1st January 1970 0:00) */
    uint32_t us; /* Microseconds */
} Timestamp;

/* ** ECG Sensor ** */
typedef struct _EcgBuffer {
    pb_byte_t data[200];
    int32_t lodpn;
    bool has_timestamp;
    Timestamp timestamp;
} EcgBuffer;

/* ** EDA Sensor ** */
typedef struct _Impedance {
    float real;
    float imag;
} Impedance;

typedef struct _EdaBuffer {
    Impedance data[16];
    bool has_timestamp;
    Timestamp timestamp;
} EdaBuffer;


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define Timestamp_init_default                   {0, 0}
#define EcgBuffer_init_default                   {{0}, 0, false, Timestamp_init_default}
#define Impedance_init_default                   {0, 0}
#define EdaBuffer_init_default                   {{Impedance_init_default, Impedance_init_default, Impedance_init_default, Impedance_init_default, Impedance_init_default, Impedance_init_default, Impedance_init_default, Impedance_init_default, Impedance_init_default, Impedance_init_default, Impedance_init_default, Impedance_init_default, Impedance_init_default, Impedance_init_default, Impedance_init_default, Impedance_init_default}, false, Timestamp_init_default}
#define Timestamp_init_zero                      {0, 0}
#define EcgBuffer_init_zero                      {{0}, 0, false, Timestamp_init_zero}
#define Impedance_init_zero                      {0, 0}
#define EdaBuffer_init_zero                      {{Impedance_init_zero, Impedance_init_zero, Impedance_init_zero, Impedance_init_zero, Impedance_init_zero, Impedance_init_zero, Impedance_init_zero, Impedance_init_zero, Impedance_init_zero, Impedance_init_zero, Impedance_init_zero, Impedance_init_zero, Impedance_init_zero, Impedance_init_zero, Impedance_init_zero, Impedance_init_zero}, false, Timestamp_init_zero}

/* Field tags (for use in manual encoding/decoding) */
#define Timestamp_time_tag                       1
#define Timestamp_us_tag                         2
#define EcgBuffer_data_tag                       1
#define EcgBuffer_lodpn_tag                      2
#define EcgBuffer_timestamp_tag                  3
#define Impedance_real_tag                       1
#define Impedance_imag_tag                       2
#define EdaBuffer_data_tag                       1
#define EdaBuffer_timestamp_tag                  2

/* Struct field encoding specification for nanopb */
#define Timestamp_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UINT64,   time,              1) \
X(a, STATIC,   SINGULAR, UINT32,   us,                2)
#define Timestamp_CALLBACK NULL
#define Timestamp_DEFAULT NULL

#define EcgBuffer_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, FIXED_LENGTH_BYTES, data,              1) \
X(a, STATIC,   SINGULAR, INT32,    lodpn,             2) \
X(a, STATIC,   OPTIONAL, MESSAGE,  timestamp,         3)
#define EcgBuffer_CALLBACK NULL
#define EcgBuffer_DEFAULT NULL
#define EcgBuffer_timestamp_MSGTYPE Timestamp

#define Impedance_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, FLOAT,    real,              1) \
X(a, STATIC,   SINGULAR, FLOAT,    imag,              2)
#define Impedance_CALLBACK NULL
#define Impedance_DEFAULT NULL

#define EdaBuffer_FIELDLIST(X, a) \
X(a, STATIC,   FIXARRAY, MESSAGE,  data,              1) \
X(a, STATIC,   OPTIONAL, MESSAGE,  timestamp,         2)
#define EdaBuffer_CALLBACK NULL
#define EdaBuffer_DEFAULT NULL
#define EdaBuffer_data_MSGTYPE Impedance
#define EdaBuffer_timestamp_MSGTYPE Timestamp

extern const pb_msgdesc_t Timestamp_msg;
extern const pb_msgdesc_t EcgBuffer_msg;
extern const pb_msgdesc_t Impedance_msg;
extern const pb_msgdesc_t EdaBuffer_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define Timestamp_fields &Timestamp_msg
#define EcgBuffer_fields &EcgBuffer_msg
#define Impedance_fields &Impedance_msg
#define EdaBuffer_fields &EdaBuffer_msg

/* Maximum encoded size of messages (where known) */
#define EcgBuffer_size                           233
#define EdaBuffer_size                           211
#define Impedance_size                           10
#define Timestamp_size                           17

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
