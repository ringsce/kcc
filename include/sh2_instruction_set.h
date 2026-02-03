// ============================================================================
// include/sh2_instruction_set.h - Complete SH-2 Instruction Set
// ============================================================================
#ifndef SH2_INSTRUCTION_SET_H
#define SH2_INSTRUCTION_SET_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// ============================================================================
// Data Transfer Instructions
// ============================================================================

// MOV - Move data
void sh2_mov_reg_reg(FILE *out, int dst, int src);
void sh2_mov_imm(FILE *out, int reg, int8_t imm);
void sh2_mov_w_imm(FILE *out, int reg, int16_t imm);
void sh2_mov_l_imm(FILE *out, int reg, int32_t imm);

// MOV with displacement
void sh2_mov_l_disp_reg(FILE *out, int dst, int disp, int src);  // mov.l @(disp,Rn),Rm
void sh2_mov_l_reg_disp(FILE *out, int src, int disp, int dst);  // mov.l Rm,@(disp,Rn)
void sh2_mov_w_disp_reg(FILE *out, int dst, int disp, int src);  // mov.w @(disp,Rn),Rm
void sh2_mov_w_reg_disp(FILE *out, int src, int disp, int dst);  // mov.w Rm,@(disp,Rn)
void sh2_mov_b_disp_reg(FILE *out, int dst, int disp, int src);  // mov.b @(disp,Rn),Rm
void sh2_mov_b_reg_disp(FILE *out, int src, int disp, int dst);  // mov.b Rm,@(disp,Rn)

// MOV with indirect addressing
void sh2_mov_l_indir(FILE *out, int dst, int src);     // mov.l @Rm,Rn
void sh2_mov_l_indir_store(FILE *out, int src, int dst); // mov.l Rm,@Rn
void sh2_mov_w_indir(FILE *out, int dst, int src);     // mov.w @Rm,Rn
void sh2_mov_w_indir_store(FILE *out, int src, int dst); // mov.w Rm,@Rn
void sh2_mov_b_indir(FILE *out, int dst, int src);     // mov.b @Rm,Rn
void sh2_mov_b_indir_store(FILE *out, int src, int dst); // mov.b Rm,@Rn

// MOV with post-increment
void sh2_mov_l_post_inc(FILE *out, int dst, int src);  // mov.l @Rm+,Rn
void sh2_mov_w_post_inc(FILE *out, int dst, int src);  // mov.w @Rm+,Rn
void sh2_mov_b_post_inc(FILE *out, int dst, int src);  // mov.b @Rm+,Rn

// MOV with pre-decrement
void sh2_mov_l_pre_dec(FILE *out, int src, int dst);   // mov.l Rm,@-Rn
void sh2_mov_w_pre_dec(FILE *out, int src, int dst);   // mov.w Rm,@-Rn
void sh2_mov_b_pre_dec(FILE *out, int src, int dst);   // mov.b Rm,@-Rn

// MOV with indexed addressing (R0-based)
void sh2_mov_l_r0_indexed(FILE *out, int dst, int src); // mov.l @(R0,Rm),Rn
void sh2_mov_l_r0_indexed_store(FILE *out, int src, int dst); // mov.l Rm,@(R0,Rn)
void sh2_mov_w_r0_indexed(FILE *out, int dst, int src); // mov.w @(R0,Rm),Rn
void sh2_mov_w_r0_indexed_store(FILE *out, int src, int dst); // mov.w Rm,@(R0,Rn)
void sh2_mov_b_r0_indexed(FILE *out, int dst, int src); // mov.b @(R0,Rm),Rn
void sh2_mov_b_r0_indexed_store(FILE *out, int src, int dst); // mov.b Rm,@(R0,Rn)

// MOV with GBR
void sh2_mov_l_gbr_disp(FILE *out, int reg, int disp);  // mov.l @(disp,GBR),R0
void sh2_mov_l_gbr_store(FILE *out, int reg, int disp); // mov.l R0,@(disp,GBR)
void sh2_mov_w_gbr_disp(FILE *out, int reg, int disp);  // mov.w @(disp,GBR),R0
void sh2_mov_w_gbr_store(FILE *out, int reg, int disp); // mov.w R0,@(disp,GBR)
void sh2_mov_b_gbr_disp(FILE *out, int reg, int disp);  // mov.b @(disp,GBR),R0
void sh2_mov_b_gbr_store(FILE *out, int reg, int disp); // mov.b R0,@(disp,GBR)

// MOVA - Move effective address
void sh2_mova(FILE *out, int disp);  // mova @(disp,PC),R0

// MOVT - Move T bit to register
void sh2_movt(FILE *out, int reg);

// SWAP - Swap register halves
void sh2_swap_b(FILE *out, int dst, int src);  // swap.b
void sh2_swap_w(FILE *out, int dst, int src);  // swap.w

// XTRCT - Extract
void sh2_xtrct(FILE *out, int dst, int src);

// ============================================================================
// Arithmetic Instructions
// ============================================================================

// ADD - Addition
void sh2_add(FILE *out, int dst, int src);           // add Rm,Rn
void sh2_add_imm(FILE *out, int reg, int8_t imm);    // add #imm,Rn
void sh2_addc(FILE *out, int dst, int src);          // addc Rm,Rn (with carry)
void sh2_addv(FILE *out, int dst, int src);          // addv Rm,Rn (with overflow)

// SUB - Subtraction
void sh2_sub(FILE *out, int dst, int src);           // sub Rm,Rn
void sh2_subc(FILE *out, int dst, int src);          // subc Rm,Rn (with borrow)
void sh2_subv(FILE *out, int dst, int src);          // subv Rm,Rn (with overflow)

// NEG - Negate
void sh2_neg(FILE *out, int dst, int src);           // neg Rm,Rn
void sh2_negc(FILE *out, int dst, int src);          // negc Rm,Rn (with borrow)

// Multiply and Accumulate
void sh2_mac_l(FILE *out, int src1, int src2);       // mac.l @Rm+,@Rn+
void sh2_mac_w(FILE *out, int src1, int src2);       // mac.w @Rm+,@Rn+

// Multiply
void sh2_mul_l(FILE *out, int src1, int src2);       // mul.l Rm,Rn
void sh2_mulu_w(FILE *out, int src1, int src2);      // mulu.w Rm,Rn (unsigned)
void sh2_muls_w(FILE *out, int src1, int src2);      // muls.w Rm,Rn (signed)

// Divide
void sh2_div0s(FILE *out, int src1, int src2);       // div0s Rm,Rn
void sh2_div0u(FILE *out);                           // div0u
void sh2_div1(FILE *out, int src1, int src2);        // div1 Rm,Rn

// Multiply and accumulate special
void sh2_dmulu_l(FILE *out, int src1, int src2);     // dmulu.l Rm,Rn (double-length unsigned)
void sh2_dmuls_l(FILE *out, int src1, int src2);     // dmuls.l Rm,Rn (double-length signed)

// Decimal adjust
void sh2_dt(FILE *out, int reg);                     // dt Rn (decrement and test)

// ============================================================================
// Logic Instructions
// ============================================================================

// AND
void sh2_and(FILE *out, int dst, int src);           // and Rm,Rn
void sh2_and_imm(FILE *out, uint8_t imm);            // and #imm,R0
void sh2_and_b_imm(FILE *out, uint8_t imm);          // and.b #imm,@(R0,GBR)

// OR
void sh2_or(FILE *out, int dst, int src);            // or Rm,Rn
void sh2_or_imm(FILE *out, uint8_t imm);             // or #imm,R0
void sh2_or_b_imm(FILE *out, uint8_t imm);           // or.b #imm,@(R0,GBR)

// XOR
void sh2_xor(FILE *out, int dst, int src);           // xor Rm,Rn
void sh2_xor_imm(FILE *out, uint8_t imm);            // xor #imm,R0
void sh2_xor_b_imm(FILE *out, uint8_t imm);          // xor.b #imm,@(R0,GBR)

// NOT
void sh2_not(FILE *out, int dst, int src);           // not Rm,Rn

// TEST
void sh2_tst(FILE *out, int src1, int src2);         // tst Rm,Rn
void sh2_tst_imm(FILE *out, uint8_t imm);            // tst #imm,R0
void sh2_tst_b_imm(FILE *out, uint8_t imm);          // tst.b #imm,@(R0,GBR)

// ============================================================================
// Shift Instructions
// ============================================================================

// Shift Logical
void sh2_shal(FILE *out, int reg);                   // shal Rn (shift arithmetic left)
void sh2_shar(FILE *out, int reg);                   // shar Rn (shift arithmetic right)
void sh2_shll(FILE *out, int reg);                   // shll Rn (shift logical left)
void sh2_shlr(FILE *out, int reg);                   // shlr Rn (shift logical right)

// Shift Logical by 2
void sh2_shll2(FILE *out, int reg);                  // shll2 Rn
void sh2_shlr2(FILE *out, int reg);                  // shlr2 Rn

// Shift Logical by 8
void sh2_shll8(FILE *out, int reg);                  // shll8 Rn
void sh2_shlr8(FILE *out, int reg);                  // shlr8 Rn

// Shift Logical by 16
void sh2_shll16(FILE *out, int reg);                 // shll16 Rn
void sh2_shlr16(FILE *out, int reg);                 // shlr16 Rn

// Rotate
void sh2_rotl(FILE *out, int reg);                   // rotl Rn
void sh2_rotr(FILE *out, int reg);                   // rotr Rn
void sh2_rotcl(FILE *out, int reg);                  // rotcl Rn (with carry)
void sh2_rotcr(FILE *out, int reg);                  // rotcr Rn (with carry)

// ============================================================================
// Branch Instructions
// ============================================================================

// Unconditional branches
void sh2_bra(FILE *out, const char *label);          // bra label
void sh2_braf(FILE *out, int reg);                   // braf Rn
void sh2_bsr(FILE *out, const char *label);          // bsr label (branch to subroutine)
void sh2_bsrf(FILE *out, int reg);                   // bsrf Rn

// Conditional branches (based on T bit)
void sh2_bt(FILE *out, const char *label);           // bt label (branch if true)
void sh2_bf(FILE *out, const char *label);           // bf label (branch if false)
void sh2_bt_s(FILE *out, const char *label);         // bt/s label (with delay slot)
void sh2_bf_s(FILE *out, const char *label);         // bf/s label (with delay slot)

// Jump
void sh2_jmp(FILE *out, int reg);                    // jmp @Rn
void sh2_jsr(FILE *out, int reg);                    // jsr @Rn (jump to subroutine)

// Return
void sh2_rts(FILE *out);                             // rts (return from subroutine)
void sh2_rte(FILE *out);                             // rte (return from exception)

// ============================================================================
// Compare Instructions
// ============================================================================

// Compare/Conditional set T bit
void sh2_cmp_eq(FILE *out, int src1, int src2);      // cmp/eq Rm,Rn
void sh2_cmp_hs(FILE *out, int src1, int src2);      // cmp/hs Rm,Rn (higher or same, unsigned)
void sh2_cmp_ge(FILE *out, int src1, int src2);      // cmp/ge Rm,Rn (greater or equal, signed)
void sh2_cmp_hi(FILE *out, int src1, int src2);      // cmp/hi Rm,Rn (higher, unsigned)
void sh2_cmp_gt(FILE *out, int src1, int src2);      // cmp/gt Rm,Rn (greater than, signed)
void sh2_cmp_pz(FILE *out, int reg);                 // cmp/pz Rn (positive or zero)
void sh2_cmp_pl(FILE *out, int reg);                 // cmp/pl Rn (positive)
void sh2_cmp_str(FILE *out, int src1, int src2);     // cmp/str Rm,Rn (string compare)

// Compare immediate
void sh2_cmp_eq_imm(FILE *out, int8_t imm);          // cmp/eq #imm,R0

// ============================================================================
// System Control Instructions
// ============================================================================

// Status Register Control
void sh2_ldc(FILE *out, int src, const char *ctrl);  // ldc Rm,SR/GBR/VBR
void sh2_ldc_l(FILE *out, int src, const char *ctrl);// ldc.l @Rm+,SR/GBR/VBR
void sh2_stc(FILE *out, const char *ctrl, int dst);  // stc SR/GBR/VBR,Rn
void sh2_stc_l(FILE *out, const char *ctrl, int dst);// stc.l SR/GBR/VBR,@-Rn

// MAC Register Control
void sh2_lds(FILE *out, int src, const char *ctrl);  // lds Rm,MACH/MACL/PR
void sh2_lds_l(FILE *out, int src, const char *ctrl);// lds.l @Rm+,MACH/MACL/PR
void sh2_sts(FILE *out, const char *ctrl, int dst);  // sts MACH/MACL/PR,Rn
void sh2_sts_l(FILE *out, const char *ctrl, int dst);// sts.l MACH/MACL/PR,@-Rn

// Special
void sh2_clrmac(FILE *out);                          // clrmac (clear MAC register)
void sh2_clrt(FILE *out);                            // clrt (clear T bit)
void sh2_sett(FILE *out);                            // sett (set T bit)
void sh2_ldtlb(FILE *out);                           // ldtlb (load PTEH/PTEL into TLB)
void sh2_nop(FILE *out);                             // nop
void sh2_rte_nop(FILE *out);                         // rte with delay slot nop
void sh2_sleep(FILE *out);                           // sleep

// ============================================================================
// Sign/Zero Extension
// ============================================================================

void sh2_exts_b(FILE *out, int dst, int src);        // exts.b Rm,Rn (sign extend byte)
void sh2_exts_w(FILE *out, int dst, int src);        // exts.w Rm,Rn (sign extend word)
void sh2_extu_b(FILE *out, int dst, int src);        // extu.b Rm,Rn (zero extend byte)
void sh2_extu_w(FILE *out, int dst, int src);        // extu.w Rm,Rn (zero extend word)

// ============================================================================
// Pseudo Instructions (for convenience)
// ============================================================================

void sh2_push(FILE *out, int reg);                   // push Rn
void sh2_pop(FILE *out, int reg);                    // pop Rn
void sh2_call(FILE *out, const char *label);         // call label
void sh2_ret(FILE *out);                             // ret
void sh2_label(FILE *out, const char *label);        // label:
void sh2_comment(FILE *out, const char *comment);    // ! comment

// Load 32-bit immediate (pseudo-instruction using literal pool)
void sh2_load_imm32(FILE *out, int reg, uint32_t value);

// ============================================================================
// Literal Pool Management
// ============================================================================

typedef struct {
    uint32_t value;
    const char *label;
    int ref_count;
} LiteralPoolEntry;

typedef struct {
    LiteralPoolEntry *entries;
    int count;
    int capacity;
    int pool_counter;
} LiteralPool;

LiteralPool* sh2_literal_pool_create(void);
void sh2_literal_pool_destroy(LiteralPool *pool);
const char* sh2_literal_pool_add(LiteralPool *pool, uint32_t value);
void sh2_literal_pool_emit(LiteralPool *pool, FILE *out);
void sh2_literal_pool_clear(LiteralPool *pool);

// ============================================================================
// Instruction Encoding Helpers (for binary output)
// ============================================================================

typedef struct {
    uint16_t opcode;
    const char *mnemonic;
    uint8_t size;  // in bytes
} SH2Instruction;

uint16_t sh2_encode_mov_reg_reg(int dst, int src);
uint16_t sh2_encode_add(int dst, int src);
uint16_t sh2_encode_cmp_eq(int src1, int src2);
// ... more encoding functions

#endif // SH2_INSTRUCTION_SET_H