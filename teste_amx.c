#include <immintrin.h>  // Suporte para intrínsecos
#include <stdint.h>     // Suporte para tipos de dados como uint16_t
#include <stdio.h>

void get_cache_info() {
    unsigned int eax, ebx, ecx, edx;

    // Chamada CPUID com EAX = 4, ECX = 1 (cache nível 1)
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
}

int main() {
    get_cache_info();
    return 0;
}

//    _tile_dpbf16ps_intnernal(void,void,void);
