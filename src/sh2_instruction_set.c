// ============================================================================
// src/sh2_instruction_set.c - Complete SH-2 Instruction Implementation
// ============================================================================
#include "sh2_instruction_set.h"
#include <stdlib.h>
#include <string.h>

// ============================================================================
// Data Transfer Instructions
// ============================================================================

void sh2_mov_reg_reg(FILE *out, int dst, int src) {
    fprintf(out, "\tmov\tr%d,r%d\n", src, dst);
}

void sh2_mov_imm(FILE *out, int reg, int8_t imm) {
    fprintf(out, "\tmov\t#%d,r%d\n", imm, reg);
}

void sh2_mov_w_imm(FILE *out, int reg, int16_t imm) {
    fprintf(out, "\tmov.w\t.L_const_%d,r%d\n", imm, reg);
}

void sh2_mov_l_imm(FILE *out, int reg, int32_t imm) {
    fprintf(out, "\tmov.l\t.L_const_%d,r%d\n", imm, reg);
}

void sh2_mov_l_disp_reg(FILE *out, int dst, int disp, int src) {
    fprintf(out, "\tmov.l\t@(%d,r%d),r%d\n", disp, src, dst);
}

void sh2_mov_l_reg_disp(FILE *out, int src, int disp, int dst) {
    fprintf(out, "\tmov.l\tr%d,@(%d,r%d)\n", src, disp, dst);
}

void sh2_mov_w_disp_reg(FILE *out, int dst, int disp, int src) {
    fprintf(out, "\tmov.w\t@(%d,r%d),r%d\n", disp, src, dst);
}

void sh2_mov_w_reg_disp(FILE *out, int src, int disp, int dst) {
    fprintf(out, "\tmov.w\tr%d,@(%d,r%d)\n", src, disp, dst);
}

void sh2_mov_b_disp_reg(FILE *out, int dst, int disp, int src) {
    fprintf(out, "\tmov.b\t@(%d,r%d),r%d\n", disp, src, dst);
}

void sh2_mov_b_reg_disp(FILE *out, int src, int disp, int dst) {
    fprintf(out, "\tmov.b\tr%d,@(%d,r%d)\n", src, disp, dst);
}

void sh2_mov_l_indir(FILE *out, int dst, int src) {
    fprintf(out, "\tmov.l\t@r%d,r%d\n", src, dst);
}

void sh2_mov_l_indir_store(FILE *out, int src, int dst) {
    fprintf(out, "\tmov.l\tr%d,@r%d\n", src, dst);
}

void sh2_mov_w_indir(FILE *out, int dst, int src) {
    fprintf(out, "\tmov.w\t@r%d,r%d\n", src, dst);
}

void sh2_mov_w_indir_store(FILE *out, int src, int dst) {
    fprintf(out, "\tmov.w\tr%d,@r%d\n", src, dst);
}

void sh2_mov_b_indir(FILE *out, int dst, int src) {
    fprintf(out, "\tmov.b\t@r%d,r%d\n", src, dst);
}

void sh2_mov_b_indir_store(FILE *out, int src, int dst) {
    fprintf(out, "\tmov.b\tr%d,@r%d\n", src, dst);
}

void sh2_mov_l_post_inc(FILE *out, int dst, int src) {
    fprintf(out, "\tmov.l\t@r%d+,r%d\n", src, dst);
}

void sh2_mov_w_post_inc(FILE *out, int dst, int src) {
    fprintf(out, "\tmov.w\t@r%d+,r%d\n", src, dst);
}

void sh2_mov_b_post_inc(FILE *out, int dst, int src) {
    fprintf(out, "\tmov.b\t@r%d+,r%d\n", src, dst);
}

void sh2_mov_l_pre_dec(FILE *out, int src, int dst) {
    fprintf(out, "\tmov.l\tr%d,@-r%d\n", src, dst);
}

void sh2_mov_w_pre_dec(FILE *out, int src, int dst) {
    fprintf(out, "\tmov.w\tr%d,@-r%d\n", src, dst);
}

void sh2_mov_b_pre_dec(FILE *out, int src, int dst) {
    fprintf(out, "\tmov.b\tr%d,@-r%d\n", src, dst);
}

void sh2_mov_l_r0_indexed(FILE *out, int dst, int src) {
    fprintf(out, "\tmov.l\t@(r0,r%d),r%d\n", src, dst);
}

void sh2_mov_l_r0_indexed_store(FILE *out, int src, int dst) {
    fprintf(out, "\tmov.l\tr%d,@(r0,r%d)\n", src, dst);
}

void sh2_mov_w_r0_indexed(FILE *out, int dst, int src) {
    fprintf(out, "\tmov.w\t@(r0,r%d),r%d\n", src, dst);
}

void sh2_mov_w_r0_indexed_store(FILE *out, int src, int dst) {
    fprintf(out, "\tmov.w\tr%d,@(r0,r%d)\n", src, dst);
}

void sh2_mov_b_r0_indexed(FILE *out, int dst, int src) {
    fprintf(out, "\tmov.b\t@(r0,r%d),r%d\n", src, dst);
}

void sh2_mov_b_r0_indexed_store(FILE *out, int src, int dst) {
    fprintf(out, "\tmov.b\tr%d,@(r0,r%d)\n", src, dst);
}

void sh2_mov_l_gbr_disp(FILE *out, int reg, int disp) {
    fprintf(out, "\tmov.l\t@(%d,gbr),r%d\n", disp, reg);
}

void sh2_mov_l_gbr_store(FILE *out, int reg, int disp) {
    fprintf(out, "\tmov.l\tr%d,@(%d,gbr)\n", reg, disp);
}

void sh2_mov_w_gbr_disp(FILE *out, int reg, int disp) {
    fprintf(out, "\tmov.w\t@(%d,gbr),r%d\n", disp, reg);
}

void sh2_mov_w_gbr_store(FILE *out, int reg, int disp) {
    fprintf(out, "\tmov.w\tr%d,@(%d,gbr)\n", reg, disp);
}

void sh2_mov_b_gbr_disp(FILE *out, int reg, int disp) {
    fprintf(out, "\tmov.b\t@(%d,gbr),r%d\n", disp, reg);
}

void sh2_mov_b_gbr_store(FILE *out, int reg, int disp) {
    fprintf(out, "\tmov.b\tr%d,@(%d,gbr)\n", reg, disp);
}

void sh2_mova(FILE *out, int disp) {
    fprintf(out, "\tmova\t@(%d,pc),r0\n", disp);
}

void sh2_movt(FILE *out, int reg) {
    fprintf(out, "\tmovt\tr%d\n", reg);
}

void sh2_swap_b(FILE *out, int dst, int src) {
    fprintf(out, "\tswap.b\tr%d,r%d\n", src, dst);
}

void sh2_swap_w(FILE *out, int dst, int src) {
    fprintf(out, "\tswap.w\tr%d,r%d\n", src, dst);
}

void sh2_xtrct(FILE *out, int dst, int src) {
    fprintf(out, "\txtrct\tr%d,r%d\n", src, dst);
}

// ============================================================================
// Arithmetic Instructions
// ============================================================================

void sh2_add(FILE *out, int dst, int src) {
    fprintf(out, "\tadd\tr%d,r%d\n", src, dst);
}

void sh2_add_imm(FILE *out, int reg, int8_t imm) {
    fprintf(out, "\tadd\t#%d,r%d\n", imm, reg);
}

void sh2_addc(FILE *out, int dst, int src) {
    fprintf(out, "\taddc\tr%d,r%d\n", src, dst);
}

void sh2_addv(FILE *out, int dst, int src) {
    fprintf(out, "\taddv\tr%d,r%d\n", src, dst);
}

void sh2_sub(FILE *out, int dst, int src) {
    fprintf(out, "\tsub\tr%d,r%d\n", src, dst);
}

void sh2_subc(FILE *out, int dst, int src) {
    fprintf(out, "\tsubc\tr%d,r%d\n", src, dst);
}

void sh2_subv(FILE *out, int dst, int src) {
    fprintf(out, "\tsubv\tr%d,r%d\n", src, dst);
}

void sh2_neg(FILE *out, int dst, int src) {
    fprintf(out, "\tneg\tr%d,r%d\n", src, dst);
}

void sh2_negc(FILE *out, int dst, int src) {
    fprintf(out, "\tnegc\tr%d,r%d\n", src, dst);
}

void sh2_mac_l(FILE *out, int src1, int src2) {
    fprintf(out, "\tmac.l\t@r%d+,@r%d+\n", src1, src2);
}

void sh2_mac_w(FILE *out, int src1, int src2) {
    fprintf(out, "\tmac.w\t@r%d+,@r%d+\n", src1, src2);
}

void sh2_mul_l(FILE *out, int src1, int src2) {
    fprintf(out, "\tmul.l\tr%d,r%d\n", src1, src2);
}

void sh2_mulu_w(FILE *out, int src1, int src2) {
    fprintf(out, "\tmulu.w\tr%d,r%d\n", src1, src2);
}

void sh2_muls_w(FILE *out, int src1, int src2) {
    fprintf(out, "\tmuls.w\tr%d,r%d\n", src1, src2);
}

void sh2_div0s(FILE *out, int src1, int src2) {
    fprintf(out, "\tdiv0s\tr%d,r%d\n", src1, src2);
}

void sh2_div0u(FILE *out) {
    fprintf(out, "\tdiv0u\n");
}

void sh2_div1(FILE *out, int src1, int src2) {
    fprintf(out, "\tdiv1\tr%d,r%d\n", src1, src2);
}

void sh2_dmulu_l(FILE *out, int src1, int src2) {
    fprintf(out, "\tdmulu.l\tr%d,r%d\n", src1, src2);
}

void sh2_dmuls_l(FILE *out, int src1, int src2) {
    fprintf(out, "\tdmuls.l\tr%d,r%d\n", src1, src2);
}

void sh2_dt(FILE *out, int reg) {
    fprintf(out, "\tdt\tr%d\n", reg);
}

// ============================================================================
// Logic Instructions
// ============================================================================

void sh2_and(FILE *out, int dst, int src) {
    fprintf(out, "\tand\tr%d,r%d\n", src, dst);
}

void sh2_and_imm(FILE *out, uint8_t imm) {
    fprintf(out, "\tand\t#%u,r0\n", imm);
}

void sh2_and_b_imm(FILE *out, uint8_t imm) {
    fprintf(out, "\tand.b\t#%u,@(r0,gbr)\n", imm);
}

void sh2_or(FILE *out, int dst, int src) {
    fprintf(out, "\tor\tr%d,r%d\n", src, dst);
}

void sh2_or_imm(FILE *out, uint8_t imm) {
    fprintf(out, "\tor\t#%u,r0\n", imm);
}

void sh2_or_b_imm(FILE *out, uint8_t imm) {
    fprintf(out, "\tor.b\t#%u,@(r0,gbr)\n", imm);
}

void sh2_xor(FILE *out, int dst, int src) {
    fprintf(out, "\txor\tr%d,r%d\n", src, dst);
}

void sh2_xor_imm(FILE *out, uint8_t imm) {
    fprintf(out, "\txor\t#%u,r0\n", imm);
}

void sh2_xor_b_imm(FILE *out, uint8_t imm) {
    fprintf(out, "\txor.b\t#%u,@(r0,gbr)\n", imm);
}

void sh2_not(FILE *out, int dst, int src) {
    fprintf(out, "\tnot\tr%d,r%d\n", src, dst);
}

void sh2_tst(FILE *out, int src1, int src2) {
    fprintf(out, "\ttst\tr%d,r%d\n", src1, src2);
}

void sh2_tst_imm(FILE *out, uint8_t imm) {
    fprintf(out, "\ttst\t#%u,r0\n", imm);
}

void sh2_tst_b_imm(FILE *out, uint8_t imm) {
    fprintf(out, "\ttst.b\t#%u,@(r0,gbr)\n", imm);
}

// ============================================================================
// Shift Instructions
// ============================================================================

void sh2_shal(FILE *out, int reg) {
    fprintf(out, "\tshal\tr%d\n", reg);
}

void sh2_shar(FILE *out, int reg) {
    fprintf(out, "\tshar\tr%d\n", reg);
}

void sh2_shll(FILE *out, int reg) {
    fprintf(out, "\tshll\tr%d\n", reg);
}

void sh2_shlr(FILE *out, int reg) {
    fprintf(out, "\tshlr\tr%d\n", reg);
}

void sh2_shll2(FILE *out, int reg) {
    fprintf(out, "\tshll2\tr%d\n", reg);
}

void sh2_shlr2(FILE *out, int reg) {
    fprintf(out, "\tshlr2\tr%d\n", reg);
}

void sh2_shll8(FILE *out, int reg) {
    fprintf(out, "\tshll8\tr%d\n", reg);
}

void sh2_shlr8(FILE *out, int reg) {
    fprintf(out, "\tshlr8\tr%d\n", reg);
}

void sh2_shll16(FILE *out, int reg) {
    fprintf(out, "\tshll16\tr%d\n", reg);
}

void sh2_shlr16(FILE *out, int reg) {
    fprintf(out, "\tshlr16\tr%d\n", reg);
}

void sh2_rotl(FILE *out, int reg) {
    fprintf(out, "\trotl\tr%d\n", reg);
}

void sh2_rotr(FILE *out, int reg) {
    fprintf(out, "\trotr\tr%d\n", reg);
}

void sh2_rotcl(FILE *out, int reg) {
    fprintf(out, "\trotcl\tr%d\n", reg);
}

void sh2_rotcr(FILE *out, int reg) {
    fprintf(out, "\trotcr\tr%d\n", reg);
}

// ============================================================================
// Branch Instructions
// ============================================================================

void sh2_bra(FILE *out, const char *label) {
    fprintf(out, "\tbra\t%s\n", label);
}

void sh2_braf(FILE *out, int reg) {
    fprintf(out, "\tbraf\tr%d\n", reg);
}

void sh2_bsr(FILE *out, const char *label) {
    fprintf(out, "\tbsr\t%s\n", label);
}

void sh2_bsrf(FILE *out, int reg) {
    fprintf(out, "\tbsrf\tr%d\n", reg);
}

void sh2_bt(FILE *out, const char *label) {
    fprintf(out, "\tbt\t%s\n", label);
}

void sh2_bf(FILE *out, const char *label) {
    fprintf(out, "\tbf\t%s\n", label);
}

void sh2_bt_s(FILE *out, const char *label) {
    fprintf(out, "\tbt/s\t%s\n", label);
}

void sh2_bf_s(FILE *out, const char *label) {
    fprintf(out, "\tbf/s\t%s\n", label);
}

void sh2_jmp(FILE *out, int reg) {
    fprintf(out, "\tjmp\t@r%d\n", reg);
}

void sh2_jsr(FILE *out, int reg) {
    fprintf(out, "\tjsr\t@r%d\n", reg);
}

void sh2_rts(FILE *out) {
    fprintf(out, "\trts\n");
}

void sh2_rte(FILE *out) {
    fprintf(out, "\trte\n");
}

// ============================================================================
// Compare Instructions
// ============================================================================

void sh2_cmp_eq(FILE *out, int src1, int src2) {
    fprintf(out, "\tcmp/eq\tr%d,r%d\n", src1, src2);
}

void sh2_cmp_hs(FILE *out, int src1, int src2) {
    fprintf(out, "\tcmp/hs\tr%d,r%d\n", src1, src2);
}

void sh2_cmp_ge(FILE *out, int src1, int src2) {
    fprintf(out, "\tcmp/ge\tr%d,r%d\n", src1, src2);
}

void sh2_cmp_hi(FILE *out, int src1, int src2) {
    fprintf(out, "\tcmp/hi\tr%d,r%d\n", src1, src2);
}

void sh2_cmp_gt(FILE *out, int src1, int src2) {
    fprintf(out, "\tcmp/gt\tr%d,r%d\n", src1, src2);
}

void sh2_cmp_pz(FILE *out, int reg) {
    fprintf(out, "\tcmp/pz\tr%d\n", reg);
}

void sh2_cmp_pl(FILE *out, int reg) {
    fprintf(out, "\tcmp/pl\tr%d\n", reg);
}

void sh2_cmp_str(FILE *out, int src1, int src2) {
    fprintf(out, "\tcmp/str\tr%d,r%d\n", src1, src2);
}

void sh2_cmp_eq_imm(FILE *out, int8_t imm) {
    fprintf(out, "\tcmp/eq\t#%d,r0\n", imm);
}

// ============================================================================
// System Control Instructions
// ============================================================================

void sh2_ldc(FILE *out, int src, const char *ctrl) {
    fprintf(out, "\tldc\tr%d,%s\n", src, ctrl);
}

void sh2_ldc_l(FILE *out, int src, const char *ctrl) {
    fprintf(out, "\tldc.l\t@r%d+,%s\n", src, ctrl);
}

void sh2_stc(FILE *out, const char *ctrl, int dst) {
    fprintf(out, "\tstc\t%s,r%d\n", ctrl, dst);
}

void sh2_stc_l(FILE *out, const char *ctrl, int dst) {
    fprintf(out, "\tstc.l\t%s,@-r%d\n", ctrl, dst);
}

void sh2_lds(FILE *out, int src, const char *ctrl) {
    fprintf(out, "\tlds\tr%d,%s\n", src, ctrl);
}

void sh2_lds_l(FILE *out, int src, const char *ctrl) {
    fprintf(out, "\tlds.l\t@r%d+,%s\n", src, ctrl);
}

void sh2_sts(FILE *out, const char *ctrl, int dst) {
    fprintf(out, "\tsts\t%s,r%d\n", ctrl, dst);
}

void sh2_sts_l(FILE *out, const char *ctrl, int dst) {
    fprintf(out, "\tsts.l\t%s,@-r%d\n", ctrl, dst);
}

void sh2_clrmac(FILE *out) {
    fprintf(out, "\tclrmac\n");
}

void sh2_clrt(FILE *out) {
    fprintf(out, "\tclrt\n");
}

void sh2_sett(FILE *out) {
    fprintf(out, "\tsett\n");
}

void sh2_ldtlb(FILE *out) {
    fprintf(out, "\tldtlb\n");
}

void sh2_nop(FILE *out) {
    fprintf(out, "\tnop\n");
}

void sh2_rte_nop(FILE *out) {
    fprintf(out, "\trte\n");
    fprintf(out, "\tnop\n");
}

void sh2_sleep(FILE *out) {
    fprintf(out, "\tsleep\n");
}

// ============================================================================
// Sign/Zero Extension
// ============================================================================

void sh2_exts_b(FILE *out, int dst, int src) {
    fprintf(out, "\texts.b\tr%d,r%d\n", src, dst);
}

void sh2_exts_w(FILE *out, int dst, int src) {
    fprintf(out, "\texts.w\tr%d,r%d\n", src, dst);
}

void sh2_extu_b(FILE *out, int dst, int src) {
    fprintf(out, "\textu.b\tr%d,r%d\n", src, dst);
}

void sh2_extu_w(FILE *out, int dst, int src) {
    fprintf(out, "\textu.w\tr%d,r%d\n", src, dst);
}

// ============================================================================
// Pseudo Instructions
// ============================================================================

void sh2_push(FILE *out, int reg) {
    sh2_mov_l_pre_dec(out, reg, 15);
}

void sh2_pop(FILE *out, int reg) {
    sh2_mov_l_post_inc(out, reg, 15);
}

void sh2_call(FILE *out, const char *label) {
    fprintf(out, "\tmov.l\t.L_%s,r0\n", label);
    fprintf(out, "\tjsr\t@r0\n");
    fprintf(out, "\tnop\n");
}

void sh2_ret(FILE *out) {
    fprintf(out, "\trts\n");
    fprintf(out, "\tnop\n");
}

void sh2_label(FILE *out, const char *label) {
    fprintf(out, "%s:\n", label);
}

void sh2_comment(FILE *out, const char *comment) {
    fprintf(out, "\t! %s\n", comment);
}

void sh2_load_imm32(FILE *out, int reg, uint32_t value) {
    if (value <= 127 && value >= 0) {
        sh2_mov_imm(out, reg, (int8_t)value);
    } else {
        fprintf(out, "\tmov.l\t.L_const_%u,r%d\n", value, reg);
    }
}

// ============================================================================
// Literal Pool Management
// ============================================================================

LiteralPool* sh2_literal_pool_create(void) {
    LiteralPool *pool = malloc(sizeof(LiteralPool));
    pool->capacity = 64;
    pool->count = 0;
    pool->pool_counter = 0;
    pool->entries = malloc(sizeof(LiteralPoolEntry) * pool->capacity);
    return pool;
}

void sh2_literal_pool_destroy(LiteralPool *pool) {
    if (pool) {
        for (int i = 0; i < pool->count; i++) {
            free((void*)pool->entries[i].label);
        }
        free(pool->entries);
        free(pool);
    }
}

const char* sh2_literal_pool_add(LiteralPool *pool, uint32_t value) {
    // Check if value already exists
    for (int i = 0; i < pool->count; i++) {
        if (pool->entries[i].value == value) {
            pool->entries[i].ref_count++;
            return pool->entries[i].label;
        }
    }

    // Add new entry
    if (pool->count >= pool->capacity) {
        pool->capacity *= 2;
        pool->entries = realloc(pool->entries,
                               sizeof(LiteralPoolEntry) * pool->capacity);
    }

    char *label = malloc(32);
    snprintf(label, 32, ".L_const_%d", pool->pool_counter++);

    pool->entries[pool->count].value = value;
    pool->entries[pool->count].label = label;
    pool->entries[pool->count].ref_count = 1;
    pool->count++;

    return label;
}

void sh2_literal_pool_emit(LiteralPool *pool, FILE *out) {
    if (pool->count == 0) return;

    fprintf(out, "\n\t.align 4\n");
    for (int i = 0; i < pool->count; i++) {
        fprintf(out, "%s:\n", pool->entries[i].label);
        fprintf(out, "\t.long\t0x%08X\n", pool->entries[i].value);
    }
    fprintf(out, "\n");
}

void sh2_literal_pool_clear(LiteralPool *pool) {
    for (int i = 0; i < pool->count; i++) {
        free((void*)pool->entries[i].label);
    }
    pool->count = 0;
}

// ============================================================================
// Instruction Encoding (for binary output)
// ============================================================================

uint16_t sh2_encode_mov_reg_reg(int dst, int src) {
    return 0x6003 | (dst << 8) | (src << 4);
}

uint16_t sh2_encode_add(int dst, int src) {
    return 0x300C | (dst << 8) | (src << 4);
}

uint16_t sh2_encode_cmp_eq(int src1, int src2) {
    return 0x3000 | (src2 << 8) | (src1 << 4);
}