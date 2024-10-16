//==============================================================
// Copyright © 2022 Intel Corporation
//
// SPDX-License-Identifier: MIT
// =============================================================
#include <immintrin.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

#define MAX 1024
#define MAX_ROWS 16
#define MAX_COLS 64
#define STRIDE 64
#define ARCH_GET_XCOMP_PERM     0x1022
#define ARCH_REQ_XCOMP_PERM     0x1023
#define XFEATURE_XTILECFG       17
#define XFEATURE_XTILEDATA      18

//Define tile config data structure 
typedef struct __tile_config
{
  uint8_t palette_id;
  uint8_t start_row;
  uint8_t reserved_0[14];
  uint16_t colsb[16]; 
  uint8_t rows[16]; 
} __tilecfg;

/* Initialize tile config */
static void init_tile_config (__tilecfg *tileinfo)
{
  int i;
  tileinfo->palette_id = 1;
  tileinfo->start_row = 0;

  for (i = 0; i < 1; ++i)
  {
    tileinfo->colsb[i] = MAX_ROWS;
    tileinfo->rows[i] =  MAX_ROWS;
  }

  for (i = 1; i < 4; ++i)
  {
    tileinfo->colsb[i] = MAX_COLS;
    tileinfo->rows[i] =  MAX_ROWS;
  }

  _tile_loadconfig (tileinfo);
}

/* Set_tiledata_use() - Invoke syscall to set ARCH_SET_STATE_USE */
static bool set_tiledata_use()
{
   if (syscall(SYS_arch_prctl, ARCH_REQ_XCOMP_PERM, XFEATURE_XTILEDATA)) 
   {
      printf("\n Fail to do XFEATURE_XTILEDATA \n\n");
      return false;
   }
   else
   {
      printf("\n TILE DATA USE SET - OK \n\n");
      return true;
   }

   return true;
}

int main(){

   __tilecfg tile_data = {0};
   int8_t src1[MAX] = {0};
   int8_t src2[MAX] = {1};
   int32_t res[MAX] = {2};
   int rows  = MAX_ROWS;
   int colsb = MAX_COLS;

   struct timespec start, end;
   double elapsed_time;

   // Request permission to linux kernel to run AMX 
   if (!set_tiledata_use())
      exit(-1);

   // Load tile configuration 
   init_tile_config (&tile_data);

   // Load tile rows from memory
   _tile_loadd (2, src1, STRIDE);
   _tile_loadd (3, src2, STRIDE);
   _tile_loadd (1, res, STRIDE);

   // Compute dot-product of bytes in tiles 
    // Medir o tempo antes de chamar a primeira função
    clock_gettime(CLOCK_MONOTONIC, &start);
    #include "./include_tilemultiply.h"
    clock_gettime(CLOCK_MONOTONIC, &end);

    // Calcular o tempo em segundos
    elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("TILE MULTIPLY: Tempo gasto: %f segundos\n", elapsed_time);

}
