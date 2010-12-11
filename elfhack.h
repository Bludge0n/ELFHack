#ifndef _ELFHACK_H
#define _ELFHACK_H

#define EI_NIDENT 16

#define ELF_R_SYM(info) ((info) >> 8)
#define ELF_R_TYPE(info) ((unsigned char)(info))
#define ELF_R_INFO(s, t) ((s) << 8 + (unsigned char)(t))

#define SHT_NULL        0
#define SHT_PROGBITS    1
#define SHT_SYMTAB      2
#define SHT_STRTAB      3
#define SHT_RELA        4
#define SHT_HASH        5
#define SHT_DYNAMIC     6
#define SHT_NOTE        7
#define SHT_NOBITS      8
#define SHT_REL         9
#define SHT_SHLIB       10
#define SHT_DYNSYM      11
#define SHT_LOPROC      0x70000000
#define SHT_HIPROC      0x7fffffff
#define SHT_LOUSER      0x80000000
#define SHT_HIUSER      0xffffffff

typedef unsigned int Elf32_Addr;
typedef unsigned short int Elf32_Half;
typedef unsigned int Elf32_Off;
typedef signed int Elf32_Sword;
typedef unsigned int Elf32_Word;
typedef unsigned char byte;

typedef struct {
    unsigned char e_ident[EI_NIDENT];
    Elf32_Half e_type;
    Elf32_Half e_machine;
    Elf32_Word e_version;
    Elf32_Addr e_entry;
    Elf32_Off e_phoff;
    Elf32_Off e_shoff;
    Elf32_Word e_flags;
    Elf32_Half e_ehsize;
    Elf32_Half e_phentsize;
    Elf32_Half e_phnum;
    Elf32_Half e_shentsize;
    Elf32_Half e_shnum;
    Elf32_Half e_shstrndx;
} Elf32_Ehdr;

typedef struct {
    Elf32_Word sh_name;
    Elf32_Word sh_type;
    Elf32_Word sh_flags;
    Elf32_Addr sh_addr;
    Elf32_Off sh_offset;
    Elf32_Word sh_size;
    Elf32_Word sh_link;
    Elf32_Word sh_info;
    Elf32_Word sh_addralign;
    Elf32_Word sh_entsize;
} Elf32_Shdr;

typedef struct {
    Elf32_Word st_name;
    Elf32_Addr st_value;
    Elf32_Word st_size;
    unsigned char st_info;
    unsigned char st_other;
    Elf32_Half st_shndx;
} Elf32_Sym;

typedef struct {
    Elf32_Addr r_offset;
    Elf32_Word r_info;
} Elf32_Rel;

typedef struct {
    Elf32_Addr r_offset;
    Elf32_Word r_info;
    Elf32_Sword r_addend;
} Elf32_Rela;

/* ELF header */
Elf32_Ehdr * ehdr = NULL;
/* Section Header */
Elf32_Shdr * shdr = NULL;
/* ELF File */
FILE * fp = NULL;
/* Section Header String Table */
char * shstrtab = NULL;
/* String Table */
char * strtab = NULL;
/* Symbol Table */
Elf32_Sym * symtab = NULL;
unsigned int n_symtab = 0;
/* Relocation Entry */
Elf32_Rel * relent = NULL;
unsigned int n_relent = 0;

/* Write section headers back to file */
void ELFWriteShdr();
/* Write symbol table back to file */
void ELFWriteSymtab();
/* Should be called before any other ELFfuncs, ELF file path should be passed. */
void ELFInit(char * path);
/* Should be called at the end of ELF hack */
void ELFDeInit();
/* It's an example, make a symbol in ELF file from local to global (symbol ends with '\0') */
void ELFMakeGlobal(char * symbol);

#endif
