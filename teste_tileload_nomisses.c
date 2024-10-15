#include <immintrin.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define MAX 1024
#define MAX_ROWS 16
#define MAX_COLS 64
#define STRIDE 64
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
      printf("Fail to do XFEATURE_XTILEDATA\n");
      return false;
   }
   else
   {
      printf("TILE DATA USE SET - OK \n");
      return true;
   }

   return true;
}

int main() {
   __tilecfg tile_data = {0};
   int8_t src1[MAX];
   int8_t src2[MAX];
   int32_t res[MAX/4];
   int rows  = MAX_ROWS;
   int colsb = MAX_COLS;

   struct timespec start, end;
   double elapsed_time;

   // Request permission to linux kernel to run AMX 
   if (!set_tiledata_use())
      exit(-1);

    init_tile_config(&tile_data);

    int8_t address[10*1024] = {0}; //  10KB

   // Medir o tempo antes de chamar a primeira função
    clock_gettime(CLOCK_MONOTONIC, &start);
    #include "./include_tileload_nomisses.h"
    clock_gettime(CLOCK_MONOTONIC, &end);

    // Calcular o tempo em segundos
    elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.0;
     printf("TILELOADS(NO Misses): Tempo gasto: %f segundos\n", elapsed_time);

    return 0;
}

