#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "elfhack.h"

static void init_ehdr() {
    ehdr = (Elf32_Ehdr *)malloc(sizeof(Elf32_Ehdr));
    fread((void *)ehdr , sizeof(Elf32_Ehdr), 1, fp);
}

static void init_shdr() {
    fseek(fp, ehdr->e_shoff, SEEK_SET);
    shdr = (Elf32_Shdr *)malloc(ehdr->e_shnum * sizeof(Elf32_Shdr));
    fread((void *)shdr, sizeof(Elf32_Shdr), ehdr->e_shnum, fp);
}

static void init_shstrtab() {
    shstrtab = (char *)malloc(shdr[ehdr->e_shstrndx].sh_size);
    fseek(fp, shdr[ehdr->e_shstrndx].sh_offset, SEEK_SET);
    fread((void *)shstrtab, shdr[ehdr->e_shstrndx].sh_size, 1, fp);
}

static void init_strtab() {
    int index = ehdr->e_shnum - 1;
    strtab = (char *)malloc(shdr[index].sh_size);
    fseek(fp, shdr[index].sh_offset, SEEK_SET);
    fread((void *)strtab, shdr[index].sh_size, 1, fp);
}

static void init_symtab() {
    /* returns entry number in symbol table */
    int i;
    for (i = 0; i < ehdr->e_shnum; i++)
        if (shdr[i].sh_type == SHT_SYMTAB)
            break;
    symtab = (Elf32_Sym *)malloc(shdr[i].sh_size);
    fseek(fp, shdr[i].sh_offset, SEEK_SET);
    fread((void *)symtab, shdr[i].sh_size, 1, fp);
    n_symtab = shdr[i].sh_size / sizeof(Elf32_Sym);
}

void ELFWriteShdr() {
    fseek(fp, ehdr->e_shoff, SEEK_SET);
    fwrite((void *)shdr, sizeof(Elf32_Shdr), ehdr->e_shnum, fp);
    fflush(fp);
}

void ELFWriteSymtab() {
    int i;
    for (i = 0; i < ehdr->e_shnum; i++)
        if (shdr[i].sh_type == SHT_SYMTAB)
            break;
    fseek(fp, shdr[i].sh_offset, SEEK_SET);
    fwrite((void *)symtab, shdr[i].sh_size, 1, fp);
    fflush(fp);
}

void ELFInit(char * path) {
    fp = fopen(path, "r+");
    init_ehdr();
    init_shdr();
    init_shstrtab();
    init_strtab();
    init_symtab();
}

void ELFDeInit() {
    /* free memory in heap */
    free(ehdr);
    free(shdr);
    free(symtab);
    free(shstrtab);
    free(strtab);
    fclose(fp);
}

void ELFMakeGlobal(char * symbol) {
    int i;
    Elf32_Sym tmp;
    for (i = 0; i < n_symtab; i++)
        if (!strcmp(symbol, strtab + symtab[i].st_name))
            break;
    symtab[i].st_info = (0x01 << 4) + (symtab[i].st_info & 0xf);
    tmp = symtab[i];
    symtab[i] = symtab[n_symtab - 1];
    symtab[n_symtab - 1] = tmp;
    for (i = 0; i < ehdr->e_shnum; i++) {
        if (!strcmp(".symtab\0", shstrtab + shdr[i].sh_name))
            break;
    }
    shdr[i].sh_info -= 1;
    ELFWriteShdr();
    ELFWriteSymtab();
}

int main(int argc, char * argv[]) {
    int i;
    ELFInit(argv[1]);
    ELFMakeGlobal("hello\0");
    ELFDeInit();
    return 0;
}
