#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

void init_shstrtab() {
    shstrtab = (char *)malloc(sh_list[header->e_shstrndx].sh_size);
    fseek(fp, sh_list[header->e_shstrndx].sh_offset, SEEK_SET);
    fread((void *)shstrtab, sh_list[header->e_shstrndx].sh_size, 1, fp);
}

void init_strtab() {
    int index = header->e_shnum - 1;
    strtab = (char *)malloc(sh_list[index].sh_size);
    fseek(fp, sh_list[index].sh_offset, SEEK_SET);
    fread((void *)strtab, sh_list[index].sh_size, 1, fp);
}

void init_symtab() {
    /* returns entry number in symbol table */
    int i;
    for (i = 0; i < header->e_shnum; i++)
        if (sh_list[i].sh_type == SHT_SYMTAB)
            break;
    symtab = (Elf32_Sym *)malloc(sh_list[i].sh_size);
    fseek(fp, sh_list[i].sh_offset, SEEK_SET);
    fread((void *)symtab, sh_list[i].sh_size, 1, fp);
    n_symtab = sh_list[i].sh_size / sizeof(Elf32_Sym);
}


void init() {
    fp = fopen("test.o", "rb");
    init_elf_header();
    init_section_list();
    init_shstrtab();
    init_strtab();
    init_symtab();
}

void deinit() {
    /* free memory in heap */
    free(header);
    free(sh_list);
    free(symtab);
    free(shstrtab);
    free(strtab);
    fclose(fp);
}

int main(int argc, char * argv) {
    int i;
    init();

    for (i = 0; i < n_symtab; i++)
        if (strcmp("hello\0", strtab + symtab[i].st_name))
            printf("%d\t%s\n", symtab[i].st_info, strtab + symtab[i].st_name);

    deinit();
    return 0;
}
