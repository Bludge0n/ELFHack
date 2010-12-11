#include <stdio.h>
#include <stdlib.h>
#include "elfhack.h"

void init_elf_header() {
    header = (Elf32_Ehdr *)malloc(sizeof(Elf32_Ehdr));
    fread((void *)header, sizeof(Elf32_Ehdr), 1, fp);
}

void init_section_list() {
    fseek(fp, header->e_shoff, SEEK_SET);
    sh_list = (Elf32_Shdr *)malloc(header->e_shnum * sizeof(Elf32_Shdr));
    fread((void *)sh_list, sizeof(Elf32_Shdr), header->e_shnum, fp);
}

void init_strtab() {
    strtab = (char *)malloc(sh_list[header->e_shstrndx].sh_size);
    fseek(fp, sh_list[header->e_shstrndx].sh_offset, SEEK_SET);
    fread((void *)strtab, sh_list[header->e_shstrndx].sh_size, 1, fp);
}


void init() {
    fp = fopen("main", "rb");
    init_elf_header();
    init_section_list();
    init_strtab();
}

void deinit() {
    /* free memory in heap */
    free(header);
    free(sh_list);
}

int main(int argc, char * argv) {
    int i;
    init();

    for (i = 0; i < header->e_shnum; i++)
        printf("%s\n", strtab + sh_list[i].sh_name);

    
    deinit();
    return 0;
}
