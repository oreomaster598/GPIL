## What is GPIL
GPIL is a light weight [MIPS](https://en.wikipedia.org/wiki/MIPS_architecture) based interpreted language,
That is designed to be ran on any platform.
It is dependency free so that it can be use with any platform that can use C(Basically every platform).

## GPIL Instruction Format
| Opcode | Register 1 | Register 2| Register 3| Immediate |
|---|---|---|---|---|
| 0000 | 0000 | 0000 | 0000 | 0000 0000 |
| short | short | short | short | int32 |

## GPIL Instruction Set Reference

| Mnemonic | Opcode | Type | Usage | Action |
|--|--|--|--|--|
| halt | 0x00 | I | halt | halts execution |
| code | 0x01 | I | code [exit code] | sets exit code |
| invoke | 0x04 | I | invoke[function] | invokes a c function |
| j | 0x02 | I | j [location] | jump to location |
| jz | 0x0E | I | jz [location] | jump to location if ZF = 0 |
| jg | 0x0F | I | jg [location] | jump to location if ZF > 0 |
| jl | 0x10 | I | jl [location] | jump to location if ZF < 0 |
| cmp | 0x0C | R | cmp [r0] [r1] | compairs two registers |
| lw | 0x05 | I | lw [r0] [address] | loads word into register |
| sw | 0x06 | I | sw [r0] [address] | saves word from register |
| li | 0x07 | I | li [r0] [const] | loads immediate into registers |
| add | 0x08 | R | add [dest] [r1] [r2] | adds two registers |
| sub | 0x09 | R | sub [dest] [r1] [r2] | subtracts two registers |
| mul| 0x0A | R | mul [dest] [r1] [r2] | multiplies two registers |
| div| 0x0B | R | div [dest] [r1] [r2] | divides two registers |
| pushr| 0x14 | R | pushr [r1] | push register onto stack |
| pushi| 0x15 | I | pushi [im] | push immediate onto stack |
| pop| 0x16 | R | pop [dest] | pops value off of stack into register |
| ret | 0x19 | I | ret | returns from function |
| call | 0x03 | I | call | call a function |
| stackalloc | 0x17 | R | stackalloc [r1] | Allocates memory from stack|

## Using GPIL
Executing a GPIL program is as simple as passing its ram into ```exec(char* ram)```

```C
 char ram[RAM] = {
      ...
  };
  
  exec(&ram);
```

Some GPIL programs require access to the interpreter (Currently this is only possible on Windows).
```C
 #define WINDOWS
 ...

 char ram[RAM] = {
      ...
  };
  
  exec(&ram, GetModuleHandle(NULL));
```
# Examples
## Jumping and Calling
```C
 char ram[RAM] = {
      I64(0x0300000000000003),// call test
      I64(0x0200000000000002),// goto end

      //label end
      I64(0x0100000000000000),// code 0x0a
      I64(0x0000000000000000),// halt

      //function test
      I64(0x0400000000000030),// invoke "test" (0)
      I64(0x1900000000000000),// ret

      //constants
      "test"
  };
  
  exec(&ram, GetModuleHandle(NULL));
```
