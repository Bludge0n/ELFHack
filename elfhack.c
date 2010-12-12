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
    shdr = (Elf32_Shdr *)malloc(ehdr->e_shnum * ehdr->e_shentsize);
    fread((void *)shdr, ehdr->e_shentsize, ehdr->e_shnum, fp);
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

static void init_relent() {
    int i;
    for (i = 0; i < ehdr->e_shnum; i++)
        if (!strncmp(".rel.\0", shstrtab + shdr[i].sh_name, 5))
            break;
    fseek(fp, shdr[i].sh_offset, SEEK_SET);
    relent = (Elf32_Rel *)malloc(shdr[i].sh_size);
    fread((void *)relent, shdr[i].sh_size, 1, fp);
    n_relent = shdr[i].sh_size / sizeof(Elf32_Rel);
}

static void init_phdr() {
    fseek(fp, ehdr->e_phoff, SEEK_SET);
    phdr = (Elf32_Phdr *)malloc(ehdr->e_phentsize * ehdr->e_phnum);
    fread((void *)phdr, ehdr->e_phentsize, ehdr->e_phnum, fp);
}

static void print_note_name() {
    /* It's only a test of knowledge about NOTE */
    int i, sz;
    char * buf;
    for (i = 0; i < ehdr->e_shnum; i++)
        if (shdr[i].sh_type == 7)
            break;
    fseek(fp, shdr[i].sh_offset, SEEK_SET);
    fread((void *)&sz, 4, 1, fp);
    fseek(fp, 8, SEEK_CUR);
    buf = (char *)malloc(sizeof(char) * sz);
    fread((void *)buf, 1, sz, fp);
    printf("%s\n", buf);
    free(buf);
}

void ELFWriteShdr() {
    fseek(fp, ehdr->e_shoff, SEEK_SET);
    fwrite((void *)shdr, ehdr->e_shentsize, ehdr->e_shnum, fp);
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
    init_relent();
    init_phdr();
}

void ELFDeInit() {
    /* free memory in heap */
    free(ehdr);
    free(shdr);
    free(symtab);
    free(shstrtab);
    free(strtab);
    free(relent);
    free(phdr);
    fclose(fp);
}

void ELFMakeGlobal(char * symbol) {
    int i;
    Elf32_Sym tmp;
    Elf32_Sym * tgt;
    tgt = ELFGetSymByName(symbol);
    if (tgt == NULL)
        return;
    /* make it from local to global */
    tgt->st_info = (0x01 << 4) + (tgt->st_info & 0xf);
    /* global symbols should be after local ones */
    tmp = *tgt;
    *tgt = symtab[n_symtab - 1];
    symtab[n_symtab - 1] = tmp;
    ELFGetShdrByName(".symtab")->sh_info -= 1;
    ELFWriteShdr();
    ELFWriteSymtab();
}

Elf32_Shdr * ELFGetShdrByName(char * name) {
    int i;
    for (i = 0; i < ehdr->e_shnum; i++)
        if (!strcmp(name, shdr[i].sh_name + shstrtab))
            return shdr + i;
    return NULL;
}

Elf32_Sym * ELFGetSymByName(char * name) {
    int i;
    for (i = 0; i < n_symtab; i++)
        if (!strcmp(name, symtab[i].st_name + strtab))
            return symtab + i;
    return NULL;
}

int main(int argc, char * argv[]) {
    int i;
    ELFInit(argv[1]);
    print_note_name();

    ELFDeInit();
    return 0;
}
