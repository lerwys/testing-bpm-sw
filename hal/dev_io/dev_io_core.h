/*
 * Copyright (C) 2014 LNLS (www.lnls.br)
 * Author: Lucas Russo <lucas.russo@lnls.br>
 *
 * Released according to the GNU LGPL, version 3 or any later version.
 */

#ifndef _DEV_IO_CORE_H_
#define _DEV_IO_CORE_H_

#include "czmq.h"
#include "mdp.h"

#include "hal_utils.h"
#include "dispatch_table.h"
#include "dev_io_err.h"
#include "ll_io.h"
/* #include "sm_io.h" */

/* #include "sm_io_thsafe_codes.h" */
/* #include "sm_io_bootstrap.h" */

/* SMIO hash key length in chars */
#define SMIO_HKEY_LEN                   8
#define NODES_MAX_LEN                   20

struct _smio_thsafe_server_ops_t;

struct _devio_t {
    /* General information */
    /*mdp_worker_t *worker;*/           /* zeroMQ Majordomo Worker */
    zctx_t *ctx;                        /* zeroMQ Context */
    void **pipes;                       /* Address nodes using this array of pipes */
    zpoller_t *poller;                  /* Poller structure to multiplex threads messages */
    unsigned int nnodes;                /* Number of actual nodes */
    char *name;                         /* Identification of this worker instance */
    char *endpoint_broker;              /* Broker location to connect to */
    int verbose;                        /* Print activity to stdout */

    /* ll_io instance for Low-Level operations*/
    llio_t *llio;
    /* Server part of the llio operations. This is the bridge between the
     * smio client part of the llio operations and the de-facto
     * llio operations */
    const struct _smio_thsafe_server_ops_t *thsafe_server_ops;
    /* Hash containg all the sm_io objects that
     * this dev_io can handle. It is composed
     * of key (10-char ID) / value (sm_io instance) */
    zhash_t *sm_io_h;
    /* Dispatch table containg all the sm_io thsafe operations
     * that we need to handle. It is composed
     * of key (4-char ID) / value (pointer to funtion) */
    disp_table_t *disp_table_thsafe_ops;
};

struct _smio_thsafe_server_ops_t {
    disp_table_func_fp thsafe_server_open;              /* Open device */
    disp_table_func_fp thsafe_server_release;           /* Release device */
    disp_table_func_fp thsafe_server_read_16;           /* Read 16-bit data */
    disp_table_func_fp thsafe_server_read_32;           /* Read 32-bit data */
    disp_table_func_fp thsafe_server_read_64;           /* Read 64-bit data */
    disp_table_func_fp thsafe_server_write_16;          /* Write 16-bit data */
    disp_table_func_fp thsafe_server_write_32;          /* Write 32-bit data */
    disp_table_func_fp thsafe_server_write_64;          /* Write 64-bit data */
    disp_table_func_fp thsafe_server_read_block;        /* Read arbitrary block size data,
                                                            parameter size in bytes */
    disp_table_func_fp thsafe_server_write_block;       /* Write arbitrary block size data,
                                                            parameter size in bytes */
    disp_table_func_fp thsafe_server_read_dma;          /* Read arbitrary block size data via DMA,
                                                            parameter size in bytes */
    disp_table_func_fp thsafe_server_write_dma;         /* Write arbitrary block size data via DMA,
                                                            parameter size in bytes */
    /*disp_table_func_fp_read_info_fp thsafe_server_read_info; Moved to dev_io */
    /* Read device information data */
};

/* Opaque llio_th_safe_ops structure */
typedef struct _smio_thsafe_server_ops_t smio_thsafe_server_ops_t;
/* Opaque class structure */
typedef struct _devio_t devio_t;

/***************** Our methods *****************/

/* Creates a new instance of Device Information */
devio_t * devio_new (char *name, char *endpoint_dev,
        llio_type_e type, char *endpoint_broker, int verbose);
/* Destroy an instance of the Device Information */
devio_err_e devio_destroy (devio_t **self_p);

/* Read specific information about the device. Typically,
 * this is stored in the SDB structure inside the device */
devio_err_e devio_print_info (devio_t *self);
/* Register an specific sm_io module to this device */
devio_err_e devio_register_sm (devio_t *self, uint32_t smio_id, void *priv);
/* Register all sm_io module that this device can handle,
 * according to the device information stored in the SDB */
devio_err_e devio_register_all_sm (devio_t *self);
devio_err_e devio_unregister_sm (devio_t *self, uint32_t smio_id);
devio_err_e devio_unregister_all_sm (devio_t *self);
/* Initilize poller with all of the initialized PIPE sockets */
devio_err_e devio_init_poller_sm (devio_t *self);
/* Poll all PIPE sockets */
devio_err_e devio_poll_all_sm (devio_t *self);
/* Router for all the opcodes registered for this dev_io */
/* devio_err_e devio_do_op (devio_t *self, uint32_t opcode, int nargs, ...); */
/* Router for all of the low-level operations for this dev_io */
devio_err_e devio_do_smio_op (devio_t *self, void *msg);

/********* Low-level generic methods API *********/

/* Open device */
void *smio_thsafe_server_open (void *owner, void *args);
/* Release device */
void *smio_thsafe_server_release (void *owner, void *args);
/* Read data from device */
void *smio_thsafe_server_read_16 (void *owner, void *args);
void *smio_thsafe_server_read_32 (void *owner, void *args);
void *smio_thsafe_server_read_64 (void *owner, void *args);
/* Write data to device */
void *smio_thsafe_server_write_16 (void *owner, void *args);
void *smio_thsafe_server_write_32 (void *owner, void *args);
void *smio_thsafe_server_write_64 (void *owner, void *args);
/* Read data block from device, size in bytes */
void *smio_thsafe_server_read_block (void *owner, void *args);
/* Write data block from device, size in bytes */
void *smio_thsafe_server_write_block (void *owner, void *args);
/* Read data block via DMA from device, size in bytes */
void *smio_thsafe_server_read_dma (void *owner, void *args);
/* Write data block via DMA from device, size in bytes */
void *smio_thsafe_server_write_dma (void *owner, void *args);
/* Read device information */
/* void *smio_thsafe_server_read_info (void *owner, void *args); */

#endif
