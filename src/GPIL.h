#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>

#ifdef WINDOWS
#include <Windows.h>
#endif

#define ip reg[0]
#define sp reg[1]
#define bp reg[2]

#define REGISTERS 10
//65535
#define RAM 8192

#define false 0
#define true 1

typedef int bool;

int reg[REGISTERS];

#define I64(x) (x) & 0xff, (x >> 8) & 0xff, (x >> 16) & 0xff, (x >> 24) & 0xff, (x >> 32) & 0xff , (x >> 40) & 0xff, (x >> 48) & 0xff, (x >> 56) & 0xff


typedef unsigned int UINT;
typedef unsigned long long UINT64;

enum {
    string_t,
    int_t,
    pointer_t,
    function_t,
}
typedef Type;

struct {
    UINT addr;
    UINT len;
    Type type;
}
typedef PDBEntry; // Debug Info

PDBEntry* pdb;
int pdb_len;


bool pdb_loaded, res_loaded;

__declspec(dllexport) void test()
{
    printf("Test Function\n");
}

#ifdef LOADER
char* read(const char* file)
{
    FILE* fp;
    fp = fopen(file, "r");

    int len;

    char* program;

    fseek(fp, 0, SEEK_SET);
    fread(&len, sizeof(int), 1, fp);

    program = calloc(len, sizeof(char));

    fseek(fp, sizeof(int), SEEK_SET);
    fread(program, sizeof(char), len, fp);

    fclose(fp);
    res_loaded = true;
    return program;
}

void LoadInto(const char* file, char* loc)
{
    FILE* fp;
    fp = fopen(file, "r");

    int len;


    fseek(fp, 0, SEEK_SET);
    fread(&len, sizeof(int), 1, fp);

    fseek(fp, sizeof(int), SEEK_SET);
    fread(loc, sizeof(char), len, fp);

    fclose(fp);
}

void save(const char* file, const char* program, int len)
{

    FILE* fp = fopen(file, "w");

    fwrite(&len, sizeof(int), 1, fp);
    fwrite(program, sizeof(char), len, fp);

    fclose(fp);
}

void loadpdb(const char* file)
{
    FILE* fp;
    fp = fopen(file, "r");


    fseek(fp, 0, SEEK_SET);
    fread(&pdb_len, sizeof(int), 1, fp);

    pdb = calloc(pdb_len, sizeof(PDBEntry));

    fseek(fp, sizeof(int), SEEK_SET);
    fread(pdb, sizeof(PDBEntry), pdb_len, fp);

    fclose(fp);

    pdb_loaded = true;
}

void savepdb(const char* file, const PDBEntry* pdb, int len)
{

    FILE* fp = fopen(file, "w");

    fwrite(&len, sizeof(int), 1, fp);
    fwrite(pdb, sizeof(PDBEntry), len, fp);

    fclose(fp);
}
#endif
#ifdef WINDOWS
int exec(char* ram, HMODULE module)
#else
int exec(char* ram)
#endif
{


    bool runing = true;
    int code = 0;

    int cmp = 0;

    sp = 1024;
    while (runing)
    {
        UINT64 instr = ((UINT64*)ram)[reg[0]];

        unsigned op;
        unsigned rs;
        unsigned rt;
        unsigned rd;
        unsigned im;


        op = (instr & 0xFF00000000000000) >> 56;
        rs = (instr & 0x00FF000000000000) >> 48;
        rt = (instr & 0x0000FF0000000000) >> 40;
        rd = (instr & 0x000000FF00000000) >> 32;
        im = (instr & 0x00000000FFFFFFFF);


        switch (op)
        {
        case 0x00: // halt
            runing = false;
            break;
        case 0x01: // set exit code
            code = im;
            break;
        case 0x02: // j
            ip = im;
            break;
        case 0x03: // call (Not Implemented)
            *((int*)(ram + sp)) = ip + 1;
            sp -= 4;
            ip = im;
            break;
        #ifdef WINDOWS
        case 0x04: // call c func         
            ((void(*)())GetProcAddress(module, ram + im))();
            break;
        #endif 
        case 0x05: // lw
            reg[rs] = *((int*)(ram + reg[rt]));
            break;
        case 0x06: // sw
            *((int*)(ram + reg[rs])) = reg[rt];
            break;
        case 0x07: // li
            reg[rs] = im;
            break;
        case 0x08: // add
            reg[rs] = reg[rt] + reg[rd];
            break;
        case 0x09: // sub
            reg[rs] = reg[rt] - reg[rd];
            break;
        case 0x0A: // mul
            reg[rs] = reg[rt] * reg[rd];
            break;
        case 0x0B: // div
            reg[rs] = reg[rt] / reg[rd];
            break;
        case 0x0C: // cmp
            cmp = reg[rs] - reg[rt];
            break;
        case 0x0D: // j
            reg[0] = im;
            break;
        case 0x0E: // jz
            if (cmp == 0)
                reg[0] = im;
            break;
        case 0x0F: // jg
            if (cmp > 0)
                reg[0] = im;
            break;
        case 0x10: // jl
            if (cmp < 0)
                reg[0] = im;
            break;
        case 0x11: // inc
            reg[rs]++;
            break;
        case 0x12: // dec
            reg[rs]--;
            break;
        case 0x13: // mov
            reg[rs] = reg[rt];
            break;
        case 0x14: // pushr
            *((int*)(ram + sp)) = reg[rs];
            sp -= 4;
            break;
        case 0x15: // pushi
            *((int*)(ram + sp)) = im;
            sp -= 4;
            break;
        case 0x16: // pop
            sp += 4;
            reg[rs] = *((int*)(ram + sp));
            break;
        case 0x17: // stackalloc
            sp -= reg[rs];
            break;
        case 0x18: // lb
            LoadInto(ram + im, ram + reg[rs]);
            printf("Loaded Module '%s' at 0x%x\n", ram + im, reg[rs]);
            break;
        case 0x19: // ret
            sp += 4;
            ip = *((int*)(ram + sp));
            break;
        case 0xFF: // test
            printf("test\n");
            break;
        }
        reg[0]++;
    }
#ifdef DEBUG

    {
        for (size_t i = 0; i < REGISTERS; i++)
        {
            printf("R%d=0x%x, ", i, reg[i]);
        }
        if (pdb_loaded)
        {
            printf("\n\n");
            for (size_t i = 0; i < pdb_len; i++)
            {
                PDBEntry info = pdb[i];//252, 119, 3  \x1b[38;2;252;119;3m
                if (info.type == string_t)
                    printf("string (\x1b[38;2;71;237;201m0x%p\x1b[38;2;204;204;204m) = \x1b[38;2;252;119;3m'%s'\x1b[38;2;204;204;204m\n", info.addr, ram + info.addr);
                else if (info.type == int_t)
                    printf("int (\x1b[38;2;71;237;201m0x%p\x1b[38;2;204;204;204m) = \x1b[38;2;71;237;201m%d\x1b[38;2;204;204;204m\n", info.addr, *((int*)(ram + info.addr)));
                else if (info.type == pointer_t)
                    printf("pointer (\x1b[38;2;71;237;201m0x%p\x1b[38;2;204;204;204m) = \x1b[38;2;71;237;201m%p\x1b[38;2;204;204;204m\n", info.addr, *((int*)(ram + info.addr)));

            }
        }
    }
#endif // DEBUG
    if (pdb_loaded)
    {
        free(pdb);//28, 252, 3
#ifdef DEBUG
        printf("\x1b[38;2;28;252;3mFreed PDB\x1b[38;2;204;204;204m\n");
#endif // DEBUG

    }
    if (res_loaded)
    {
        free(ram);
#ifdef DEBUG
        printf("\x1b[38;2;28;252;3mFreed Resources\x1b[38;2;204;204;204m\n");
#endif // DEBUG

    }
    /*if (prog_loaded)
    {
        free(rom);
        if (debug)
        printf("\x1b[38;2;28;252;3mFreed Program\x1b[38;2;204;204;204m\n");
    }*/
    printf("\nExited with code: \x1b[38;2;71;237;201m0x%x\x1b[38;2;204;204;204m\n", code);
    return 0;
}