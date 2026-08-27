 # 8-Bit Custom CPU - Instruction Set Architecture

## 1. Overview

This is a custom designed 8-bit CPU architecture built for a software emulator project. This ISA is intended to be implemented later on through an FPGA/Verilog hardware implementation. This ISA is designed from scratch rather than copying an existing architecture (e.g. RISC-V or ARM) in order to experience architectural decision making for myself. 

This ISA is validated against 3 test programs: Fibonacci sequence generation, factorial computation and a standard bubble sort. These effectively show the functionality of the ISA, demonstrating  arithmetic capability, loop-based control flow and memory/array access patterns respectively. The bubble sort in particular influenced the decision to include a signed offset in indirect addressing, spoken about in Section 3.

### Basic design decision
- **Data width:** 8-bit
- **Instruction width:** 16-bit, fixed length
- **Memory:** 64KB, byte-addressable
- **Philosophy:** RISC-style, fixed length instructions with simple decode and load/store architecture. ALU operations only work on registers. Memory is only accessed using load and store instructions, not ALU operations.

**Fixed length over variable length:** Although variable-length instructions, such as **x86**, allow denser code, it requires a much more complex decoder that has to determine instruction length before it can be parsed. Fixed-length instructions allow every instruction to be decoded in an identical matter regardless of type, resulting in a simple fetch-decode-execute cycle. This decision RISC vs CISC decision in real architectures.

---

## 2. Register File

- **8 General-Purpose Registers:** R0 - R7, 3-bit register addressing

### Special-purpose registers
 
- **Program Counter (PC):** 16 bits wide — must be wider than the 8-bit general-purpose registers, since it needs to address the full 64KB memory space. Not part of the 8-register general-purpose file and not addressable by any instruction's register fields. It's updated implicitly by instruction fetch (PC += 2 each cycle, since instructions are 16 bits) and explicitly by jump-type instructions (Section 3).

- **Flags/Status Register:** 4 bits (Z, N, C, V) - fully explained in Section 6. Also not part of the general-purpose register fil. Written implicitly by ADD/SUB/ADDI and read implicitly by conditional jump instructions (JEQ/JNE/JLT/JGE).

- **Stack Pointer:** Not included as none of the three test programs (fibonacci, factorial, bubble sort) require function calls, subroutines, or nested returns as currently scoped. A stack pointer would be added if the ISA were extended to support subroutine calls.

### Rejected alternatives
- **16 registers:** With a 3-operand R type format (destination, source1, source2), 16 registers require 4 bits x 3 = 12 bits for just the register fields. In conjunction with 5-bit opcodes, there would be 0 bits remaining for addressing modes or immediates, meaning instructions won't fit in the 16 bit budget. 8 registers require 3 bits x 3 = 9 bits, leaving space for opcode and other fields to comfortably fit.

- **4 registers:** Allows for more bits per instruction, but could result in unnecessary register pressure (frequent spills to memory) for even moderately complicated test programs. Using 8 registers strikes a balance between encoding cost and having enough working registers for fibonacci, factorial and bubble sort without constant memory traffic.

---

## 3. Instruction Format

Each instruction is 16 bits. The fields the non-opcode bits represents are dependent on the instruction type. First, the decoder reads the opcode to determine which of the 4 formats to interpret the rest oc the instruction as. This pattern occurs in real life ISAs, such as MIPS R-type/I-type/J-type (Register, Immediates, Jumps).

### R-type (register-direct ALU operations, e.g. ADD Rd, Rs1, Rs2)
```
[15:11] opcode           (5 bits)
[10:8]  destination      (3 bits)
[7:5]   source1          (3 bits)
[4:2]   source2          (3 bits)
[1:0]   unused           (2 bits)
```

### I-type (immediate load, e.g. LOADI Rd, imm)
```
[15:11] opcode            (5 bits)
[10:8]  destination       (3 bits)
[7:0]   immediate         (8 bits, signed two's complement)
```
Uses all 16 bits with zero waste. Immediate width of 8 bits maximises 16 bit instruction length with the opcode length of 5 bits and destination register width 3 bit width. Gives a signed range of -128 to 127, sufficient for loop counters and constants in the planned test programs.

### Indirect-type (register-indirect memory access with offset, e.g. LOAD Rd, [Rs + offset])
```
[15:11] opcode           (5 bits)
[10:8]  destination      (3 bits)
[7:5]   Source Address   (3 bits)
[4:0]   offset           (5 bits, signed two's complement, range -16 to +15)
```
Uses all 16 bits with zero waste.

**Base + offset instead of simple register-indirect:** A standard register indirect format leaves 5 bits unused in the indirect-type instruction. Rather than leaving them reserved, they're used as a signed offset, enabling `LOAD Rd, [Rs + offset]` instead of just `LOAD Rd, [Rs]`. This directly supports bubble sort, which needs to compare adjacent array elements (`arr[i]`, `arr[i+1]`). An offset field allows for two LOADs with offsets 0 and +1 against a single base register, rather than recomputing the address with a separate ADD every iteration.

### Jump-type (PC-relative jump, e.g. JEQ offset)
```
[15:11] opcode   (5 bits)
[10:8]  unused   (3 bits)
[7:0]   offset   (8 bits, signed two's complement, PC-relative)
```
Reuses I-type's physical bit layout exactly (the "destination register" field is simply unused, following the same degenerate-reuse pattern as HALT and MOV) rather than introducing a fourth format. The offset counts in instruction-units, not bytes - the decoder computes `new PC = current PC + (offset * 2)` since each instruction occupies 2 bytes. Counting in instruction-units rather than raw bytes doubles the effective jump range for the same 8 bits.
 
**Why PC-relative instead of absolute jump target:** an absolute target would need 16 bits to address the full 64KB memory space. This doesn't fit alongside a 5-bit opcode in a 16-bit instruction.
 

---

## 4. Addressing Modes

| Mode | Example | Description |
|---|---|---|
| Immediate | `LOADI Rd, #42` | Operand is a literal value encoded in the instruction |
| Register-direct | `ADD Rd, Rs1, Rs2` | Operands are values held in registers |
| Register-indirect (base+offset) | `LOAD Rd, [Rs + offset]` | Operand is in memory, at address = value in Rs plus signed offset |

**Exclusion of absolute/direct addressing:** A direct-addressing instruction requires enough bits to encode a full memory address (16 bits for a 64KB address space), which doesn't fit inside a 16-bit instruction alongside an opcode and destination register. To implement this, the addressable range would need to be shrunk (zero-page style) or a variable-length instruction format would be required.

**Effective addressable range for register-indirect addressing:** General-purpose registers are 8 bits wide, so a register used as the base address in `LOAD`/`STORE` can only hold a value 0–255. Therefore a full 16 bit address can't be represented. Combined with the ±16 signed offset, register-indirect addressing can reach approximately addresses 0–270 of the full 64KB space.This is a direct consequence of register width, and the limitation was accepted rather than resolved (e.g. with 16-bit register pairing) because none of the three test programs (fibonacci, factorial, bubble sort) require data or arrays stored beyond this range, and register pairing would add real implementation and decode complexity without a driving requirement. `LOADI Rn, addr` followed by `LOAD Rd, [Rn]` doesn't provide a general workaround for this, since `LOADI` only populates an 8-bit register and cannot hold a full 16-bit address either. This substitute was originally the reason for omitting direct addressing, however the final justification is that no test program needs memory access outside the reachable range.

---

## 5. Instruction Set Table

| Mnemonic | Opcode (5-bit) | Format | Description |
|---|---|---|---|
| ADD | `00000` | R-type | Rd = Rs1 + Rs2 |
| SUB | `00001` | R-type | Rd = Rs1 - Rs2 |
| HALT | `00010` | R-type (degenerate - dest/src1/src2 fields unused) | Stop execution |
| MOV | `00011` | R-type (degenerate - src2 field unused) | Rd = Rs1 |
| LOADI | `01000` | I-type | Rd = immediate (signed, -128 to 127) |
| ADDI | `01001` | I-type (destination doubles as source) | Rd = Rd + immediate |
| LOAD | `10000` | Indirect-type | Rd = memory[Rs + offset] |
| STORE | `10001` | Indirect-type | memory[Rs + offset] = Rd |
| JMP | `10010` | Jump-type | PC = PC + (offset × 2), unconditional |
| JEQ | `10011` | Jump-type | if Z == 1: PC = PC + (offset × 2) |
| JNE | `10100` | Jump-type | if Z == 0: PC = PC + (offset × 2) |
| JLT | `10101` | Jump-type | if (N XOR V) == 1: PC = PC + (offset × 2) - signed less-than |
| JGE | `10110` | Jump-type | if (N XOR V) == 0: PC = PC + (offset × 2) - signed greater-or-equal |
| *(reserved)* | `00100`–`00111` | R-type range | Reserved for future R-type ops (e.g. bitwise logic, if extended later) |
| *(reserved)* | `01010`–`01111` | I-type range | Reserved for future I-type ops |
| *(reserved)* | `10111`–`11111` | Jump-type range | Reserved for future jump conditions (e.g. JLE, JGT) if extended |

---

## 6. Flags / Status Register
 
A dedicated 4-bit register will be used to represent the status of the flag variables as opposed to any of the 8 general purpose registers.

```
Bit 3: Z (Zero)
Bit 2: N (Negative)
Bit 1: C (Carry)
Bit 0: V (Overflow)
```
 
### Flag definitions:
 
- **Z (Zero):** set if the instruction's result equals 0. Needed for loop termination and equality checks (`while`, `if a == b`) across all three test programs.

- **N (Negative):** set if bit 7 (the sign bit) of the result is 1. Required for ordering comparisons: bubble sort's `arr[i] > arr[i+1]` is implemented as `SUB` followed by testing N (and Z, for the equal case).

- **C (Carry):** set if an addition's **unsigned** interpretation overflows 8 bits, or equivalently for subtraction, if a borrow occurred. Essentially detects unsigned wraparound.

- **V (Overflow):** set if a **signed** operation produces a result outside the representable signed range (-128 to 127) - specifically, when both operands share a sign and the result's sign differs from theirs. Essentially detects signed wraparound: applies to factorial program, which exceeds 8-bit range quickly (6! = 720) and would otherwise silently wrap to an incorrect value with no indication anything went wrong.

**Why all four flags were included, not just Z/N:** Although only flags Z and N were strictly required for the test programs to function correctly, flags C and V were also included to verify that arithmetic operations were occurring correctly, not necessarily add new capability. This is particularly applicable to the factorial program for the reason given in the overflow section.
 
### Which instructions set flags
 
- **ADD, SUB, ADDI** - all four flags updated based on the result.
- **LOADI, MOV, LOAD, STORE, HALT** - flags unchanged as these operations don't compute an arithmetic result where the meaning can be stored in the flags.

### Set logic (ADD Rd, Rs1, Rs2)
 
- `Z = 1` if result == 0
- `N = 1` if bit 7 of result == 1
- `C = 1` if `(unsigned)Rs1 + (unsigned)Rs2 > 255` (computed using a widened intermediate before truncating to 8 bits)
- `V = 1` if Rs1 and Rs2 have the same sign **and** the result's sign differs from theirs (same-sign operands produces a different-sign result)

### Set logic (SUB Rd, Rs1, Rs2)
 
Implemented internally as `Rs1 + (~Rs2 + 1)` meaning the addition of Rs1 and the two's-complement negation of Rs2. This mirrors real ALU design, where a single adder circuit handles both addition and subtraction via a negate control line, rather than requiring separate hardware. C and V reuse the exact same logic as ADD, applied to this negated form.
 
---


## 7. Design Decisions Log

This is a running log of decisions in the order that they were made, with the alternative considered and the constraint that forced the choice. Relevant sections can be referred to for more detailed reasoning. 

1. **Fixed-length 16-bit instructions** used instead of variable-length: simpler to decode and more appropriate for first custom ISA. **(Section 1)**

2. **8 general-purpose registers with 3 bit addressing** over 16: bit budget doesn't support 16 registers in a 3-operand R-type format. **(Section 2)**

3. **Three addressing modes (immediate, register-direct, register-indirect)** and omitting absolute/direct: direct addressing doesn't fit the bit budget and is not required given the other three modes. **(Section 4)**

4. **Multiple instruction formats selected by opcode (R-type/I-type/Indirect-type)** instead of one uniform format: avoids wasting bits on unused fields. **(Section 3)**

5. **8-bit immediate field:** derived directly from remaining bits after opcode + destination register. **(Section 3)**

6. **5-bit signed offset added to indirect addressing:** makes use of unused bits. Influenced by bubble sort's need to access adjacent array elements without recomputing addresses. **(Section 3)**

7. **No dedicated MUL, no shift instructions (LSL/LSR/ASR/XSR):**  factorial's multiplication is implemented through repeated addition using ADD. Shift-and-add multiplication is a real hardware technique but adds unrequired opcode and implementation complexity. **(Section 5)**

8. **No bitwise logic (AND/OR/XOR/NOT):** none of the test programs need bit manipulation, therefore not added. **(Section 5)**

9. **No NOP:** not currently requirement (no pipeline/timing constraints in a pure emulator). Can be added if the project is extended toward hardware timing simulation. **(Section 5)**

10. **HALT and MOV treated as degenerate R-type instructions** rather than introducing a fourth zero/one-operand format: keeps the decoder handling only three instruction shapes. **(Section 5)**

11. **ADDI added late, after discovering the gap:** initial instruction list had no way to do register-immediate arithmetic (e.g. loop increment/decrement) or register-to-register copy (MOV). These are the most frequently executed operations across all three test programs (every loop iteration needs increment/decrement), so they were added once I realised they were required. **(Section 5)**

12. **Opcodes grouped by format** (`000xx`= R-type, `010xx`= I-type, `10xxx`= Indirect-type/jump) rather than assigned sequentially, so the opcode's high bits alone can indicate instruction format before full decode. **(Section 5)**

13. **Jump-type reuses I-type's physical layout** rather than a dedicated fourth format: same degenerate-reuse pattern as HALT/MOV, minimizes decoder complexity. **(Section 3)**

14. **PC-relative jumping, offset counted in instruction-units (not bytes):** an absolute jump target needs 16 bits and doesn't fit the instruction budget. Counting the 8-bit offset in instruction-units rather than bytes doubles effective jump range. **(Section 3)**

15. **JLT/JGE use the fully correct N XOR V signed-comparison test**, not the simpler N-alone approximation: chosen for correctness even though the current test programs' value ranges wouldn't have exposed the edge case where N alone gives a wrong answer **(Section 6)**

16. **Only including the following 5 jump instructions - JMP, JEQ, JNE, JLT, JGE:** JLT and JGE are required by bubble sort's ordering comparison (`arr[i] > arr[i+1]`); JEQ/JNE cover equality-based loop termination; JMP is required for unconditional control transfer (such as skipping parts of an if-statement) that no conditional jump can express regardless of flag state. Additional conditions (JLE, JGT) were deliberately not added as no test program requires them. **(Section 5)**

17. **Stack pointer omitted:** none of the three test programs require function calls, subroutines, or nested returns as currently scoped. Can be added if the ISA were extended to support subroutine calls. **(Section 2)**

18. **Corrected Section 4's justification for omitting direct addressing.** Originally I thought  `LOADI Rn, addr` + `LOAD Rd, [Rn]` could substitute for direct addressing. This doesn't work as `LOADI` only populates an 8-bit register and cannot hold a full 16-bit address. Therefore register-indirect addressing is structurally limited to roughly addresses 0–270 (8-bit register base + 5-bit signed offset) due to register width rather than a deliberate design choice. I accepted this limitation as of right now since no test program requires data beyond this range. It can be resolved in the future with 16-bit register pairing. **(Section 4)**

19. **Memory layout separates the structural data ceiling (271 bytes) from actual data usage (30 bytes, once all three test programs were decided to store their results via STORE/LOAD rather than staying register-only):** declaring a fixed data-region size equal to either number would misrepresent one as the other. Instructions were placed starting at `0x0100` rather than immediately after the last used or last reachable data byte, therefore trading 15 bytes of technically reachable data  for round region boundaries. Little-endian was chosen for instruction word storage to match x86/RISC-V convention. **(Section 8)**

20. **Testing was centred around highest risk instructions rather than done equally across all 13:** ADD, ADDI and SUB had full results and flag verification tests completed. LOAD and STORE were tested with positive and negative offsets to ensure appropriate sign extension. JLT and JGE had extensive testing since they are logically the most complex instruction. JMP, JEQ and JNE didn't receive dedicated testing as their logic was simpler. **(Section 5)**


---

## 8. Memory Model

- **Size:** 64KB, byte-addressable, 16-bit address space (0x0000–0xFFFF).
- **Architecture:** Von Neumann — instructions and data share the same address space rather than separate instruction/data memories. The PC (16-bit) can address the full 64KB for instruction fetch, while register-indirect LOAD/STORE (8-bit base + 5-bit signed offset) can only reach the first 271 bytes for data.
- **Endianness:** Little-endian. Since instructions are 16 bits but memory is byte-addressed, fetching an instruction word means reading two consecutive bytes and combining them — little-endian was chosen (low byte at the lower address) to match the convention used by the most common instruction-set families (x86, RISC-V).
- **PC increment:** +2 per fetch, consistent with 16-bit instructions in byte-addressable memory.

### Structural data ceiling vs. actual data usage

Section 4 established that register-indirect addressing is structurally limited to around addresses 0–270 (271 bytes), due to the 8-bit base register and 5-bit signed offset. This is the **ceiling**: the maximum data range the ISA can ever reach with its current addressing mode.

Separately, the **actual data usage** of the three test programs was calculated by deciding that each program stores its results into memory (exercising STORE/LOAD, rather than staying register-only):

| Program | What's stored | Bytes | Reasoning |
|---|---|---|---|
| Fibonacci | Full sequence, fib(0) to fib(13) | 14 | fib(14) = 377 overflows the 8-bit register, so the sequence is capped at fib(13) = 233 |
| Factorial | Full table, 0! to 5! | 6 | 6! = 720 overflows 8 bits, so the table is capped at 5! = 120 |
| Bubble sort | Array to be sorted | 10 | Free choice, not a constraint — sized to be large enough to show multiple passes/swaps, small enough to trace by hand. |
| **Total** | | **30** | |

30 bytes is well within the 271-byte structural ceiling, with a good amount of headroom (241 bytes) unused. 

### Memory layout

- **Data region:** starts at `0x0000`. No fixed size is declared — usage is ~30 bytes in practice, structurally bounded at 271 bytes. The unused space between actual usage and the structural ceiling is left available (e.g. for extending a test program later).
- **Instruction region:** starts at `0x0100` (256 decimal). This was chosen over starting immediately at address 271 (the first byte past the structural ceiling) for two reasons: 256 is word-aligned for instruction fetch (PC increments by 2, and an even start address keeps every subsequent instruction address even too), and it leaves a clean, round boundary between the two regions rather than an odd one-byte-past-the-limit boundary. The 256–270 byte range is technically inside the addressable-by-register-indirect ceiling but is reserved as instruction space and not used for data — this is a deliberate trade-off (15 bytes of otherwise-reachable data range given up) in exchange for a clean, easy-to-reason-about split.
- Programs (instructions) occupy `0x0100` onward; with three small test programs (well under a few hundred bytes combined), this leaves the majority of the 64KB space unused.
