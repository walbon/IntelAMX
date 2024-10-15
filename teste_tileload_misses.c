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

void get_cache_info() {
    unsigned int eax, ebx, ecx, edx;

    // Chamada CPUID com EAX = 4, ECX = 0 (cache nível 0)
    eax = 4;
    ecx = 0; // Cache L1d (ECX = 0)(cache de dados L1)
             // Cache L1i (ECX = 1)(cache de instrucoes  L1)

    __asm__ __volatile__(
        "cpuid"
        : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
        : "a"(eax), "c"(ecx)
    );

    // O tamanho do cache está codificado nos bits de EBX
    unsigned int cache_line_size = (ebx & 0xFFF) + 1;
    unsigned int partitions = ((ebx >> 12) & 0x3FF) + 1;
    unsigned int ways_of_associativity = ((ebx >> 22) & 0x3FF) + 1;
    unsigned int sets = ecx + 1;

    unsigned int cache_size = cache_line_size * partitions * ways_of_associativity * sets;

    printf("Cache L1 size: %u bytes\n", cache_size);
    printf("Cache line L1: %u bytes\n", cache_line_size);
}

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

    get_cache_info();

    init_tile_config(&tile_data);

    int8_t address[2*1024*1024] = {0}; //  2MB

   // Medir o tempo antes de chamar a primeira função
    clock_gettime(CLOCK_MONOTONIC, &start);
    #include "./include_tileload_misses.h"
    clock_gettime(CLOCK_MONOTONIC, &end);

    // Calcular o tempo em segundos
    elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.0;
     printf("TILELOADS: Tempo gasto: %f segundos\n", elapsed_time);

    return 0;
}

