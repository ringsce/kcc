// ============================================================================
// src/sh2_register_allocator.c - SH-2 Register Allocator Implementation
// ============================================================================

#include "sh2_register_allocator.h"
#include "sh2_instruction_set.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

// Define missing types if not in header
#ifndef VARTYPE_DEFINED
typedef enum {
    VAR_TYPE_INT,
    VAR_TYPE_LONG,
    VAR_TYPE_FLOAT,
    VAR_TYPE_DOUBLE,
    VAR_TYPE_POINTER
} VarType;
#define VARTYPE_DEFINED
#endif

#ifndef ALLOCSTRATEGY_DEFINED
typedef enum {
    ALLOC_STRATEGY_LINEAR_SCAN,
    ALLOC_STRATEGY_GRAPH_COLORING,
    ALLOC_STRATEGY_PRIORITY_BASED
} AllocStrategy;
#define ALLOCSTRATEGY_DEFINED
#endif

// Forward declare the struct and create typedef
typedef struct SH2RegisterAllocator SH2RegisterAllocator;

// Include rest of the original file with struct definition
typedef struct LiveRange {
    int start, end, next_use, spill_cost;
    bool is_spilled;
    int spill_slot, assigned_reg;
    struct LiveRange *next;
} LiveRange;

typedef struct VirtualReg {
    int id;
    VarType type;
    LiveRange *live_range;
    int hint_reg;
    bool is_precolored;
    int color;
    bool needs_spill;
} VirtualReg;

struct SH2RegisterAllocator {
    VirtualReg *virtual_regs;
    int num_vregs, vreg_capacity;
    bool reg_in_use[16];
    int reg_locked[16];
    VirtualReg *reg_assigned[16];
    LiveRange **live_ranges;
    int num_ranges;
    uint8_t **interference;
    int *degree;
    int *spill_slots;
    int num_spill_slots, spill_base_offset;
    AllocStrategy strategy;
    int num_spills, num_reloads, num_moves;
};

static const uint8_t allocatable_regs[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13};
#define NUM_ALLOCATABLE_REGS 14

SH2RegisterAllocator* sh2_regalloc_create(AllocStrategy strategy) {
    SH2RegisterAllocator *alloc = calloc(1, sizeof(SH2RegisterAllocator));
    if (!alloc) return NULL;
    alloc->vreg_capacity = 256;
    alloc->virtual_regs = calloc(alloc->vreg_capacity, sizeof(VirtualReg));
    alloc->live_ranges = calloc(alloc->vreg_capacity, sizeof(LiveRange*));
    alloc->spill_slots = calloc(alloc->vreg_capacity, sizeof(int));
    alloc->strategy = strategy;
    for (int i = 0; i < alloc->vreg_capacity; i++) alloc->spill_slots[i] = -1;
    memset(alloc->reg_in_use, 0, sizeof(alloc->reg_in_use));
    memset(alloc->reg_locked, 0, sizeof(alloc->reg_locked));
    memset(alloc->reg_assigned, 0, sizeof(alloc->reg_assigned));
    alloc->reg_in_use[14] = alloc->reg_in_use[15] = true;
    alloc->reg_locked[14] = alloc->reg_locked[15] = 1;
    alloc->interference = calloc(alloc->vreg_capacity, sizeof(uint8_t*));
    for (int i = 0; i < alloc->vreg_capacity; i++)
        alloc->interference[i] = calloc((alloc->vreg_capacity + 7) / 8, sizeof(uint8_t));
    alloc->degree = calloc(alloc->vreg_capacity, sizeof(int));
    return alloc;
}

void sh2_regalloc_destroy(SH2RegisterAllocator *alloc) {
    if (!alloc) return;
    free(alloc->virtual_regs);
    for (int i = 0; i < alloc->num_vregs; i++) {
        LiveRange *r = alloc->live_ranges[i];
        while (r) { LiveRange *n = r->next; free(r); r = n; }
    }
    free(alloc->live_ranges);
    free(alloc->spill_slots);
    for (int i = 0; i < alloc->vreg_capacity; i++) free(alloc->interference[i]);
    free(alloc->interference);
    free(alloc->degree);
    free(alloc);
}

void sh2_regalloc_reset(SH2RegisterAllocator *alloc) {
    if (!alloc) return;
    for (int i = 0; i < alloc->num_vregs; i++) {
        LiveRange *r = alloc->live_ranges[i];
        while (r) { LiveRange *n = r->next; free(r); r = n; }
        alloc->live_ranges[i] = NULL;
    }
    alloc->num_vregs = alloc->num_ranges = alloc->num_spill_slots = 0;
    alloc->num_spills = alloc->num_reloads = alloc->num_moves = 0;
    memset(alloc->reg_in_use, 0, sizeof(alloc->reg_in_use));
    memset(alloc->reg_locked, 0, sizeof(alloc->reg_locked));
    memset(alloc->reg_assigned, 0, sizeof(alloc->reg_assigned));
    alloc->reg_in_use[14] = alloc->reg_in_use[15] = true;
    alloc->reg_locked[14] = alloc->reg_locked[15] = 1;
    for (int i = 0; i < alloc->vreg_capacity; i++) {
        memset(alloc->interference[i], 0, (alloc->vreg_capacity + 7) / 8);
        alloc->degree[i] = 0;
        alloc->spill_slots[i] = -1;
    }
}

int sh2_regalloc_new_vreg(SH2RegisterAllocator *alloc, VarType type) {
    if (alloc->num_vregs >= alloc->vreg_capacity) {
        int new_cap = alloc->vreg_capacity * 2;
        alloc->virtual_regs = realloc(alloc->virtual_regs, new_cap * sizeof(VirtualReg));
        alloc->live_ranges = realloc(alloc->live_ranges, new_cap * sizeof(LiveRange*));
        alloc->spill_slots = realloc(alloc->spill_slots, new_cap * sizeof(int));
        for (int i = 0; i < alloc->vreg_capacity; i++) {
            alloc->interference[i] = realloc(alloc->interference[i], (new_cap + 7) / 8);
            memset(alloc->interference[i] + (alloc->vreg_capacity + 7) / 8, 0,
                   (new_cap - alloc->vreg_capacity + 7) / 8);
        }
        alloc->interference = realloc(alloc->interference, new_cap * sizeof(uint8_t*));
        for (int i = alloc->vreg_capacity; i < new_cap; i++)
            alloc->interference[i] = calloc((new_cap + 7) / 8, sizeof(uint8_t));
        alloc->degree = realloc(alloc->degree, new_cap * sizeof(int));
        for (int i = alloc->vreg_capacity; i < new_cap; i++) {
            alloc->live_ranges[i] = NULL;
            alloc->spill_slots[i] = -1;
            alloc->degree[i] = 0;
        }
        alloc->vreg_capacity = new_cap;
    }
    int id = alloc->num_vregs++;
    VirtualReg *vreg = &alloc->virtual_regs[id];
    vreg->id = id;
    vreg->type = type;
    vreg->live_range = NULL;
    vreg->hint_reg = -1;
    vreg->is_precolored = false;
    vreg->color = -1;
    vreg->needs_spill = false;
    return id;
}

void sh2_regalloc_set_hint(SH2RegisterAllocator *alloc, int vreg, int phys_reg) {
    if (vreg >= 0 && vreg < alloc->num_vregs && phys_reg >= 0 && phys_reg < 16)
        alloc->virtual_regs[vreg].hint_reg = phys_reg;
}

void sh2_regalloc_precolor(SH2RegisterAllocator *alloc, int vreg, int phys_reg) {
    if (vreg >= 0 && vreg < alloc->num_vregs && phys_reg >= 0 && phys_reg < 16) {
        alloc->virtual_regs[vreg].is_precolored = true;
        alloc->virtual_regs[vreg].color = phys_reg;
        alloc->virtual_regs[vreg].hint_reg = phys_reg;
    }
}

void sh2_regalloc_add_use(SH2RegisterAllocator *alloc, int vreg, int position) {
    if (vreg < 0 || vreg >= alloc->num_vregs) return;
    LiveRange *r = alloc->live_ranges[vreg];
    if (!r) {
        r = calloc(1, sizeof(LiveRange));
        r->start = r->end = r->next_use = position;
        r->spill_cost = 10;
        r->is_spilled = false;
        r->spill_slot = r->assigned_reg = -1;
        alloc->live_ranges[vreg] = r;
    } else {
        if (position < r->start) r->start = position;
        if (position > r->end) r->end = position;
        r->spill_cost += 10;
    }
}

void sh2_regalloc_add_def(SH2RegisterAllocator *alloc, int vreg, int position) {
    sh2_regalloc_add_use(alloc, vreg, position);
}

void sh2_regalloc_add_constraint(SH2RegisterAllocator *alloc, int v1, int v2) {
    (void)alloc; (void)v1; (void)v2;
}

static void add_interference_edge(SH2RegisterAllocator *alloc, int v1, int v2) {
    if (v1 == v2 || v1 < 0 || v2 < 0 || v1 >= alloc->num_vregs || v2 >= alloc->num_vregs) return;
    int byte_idx = v2 / 8, bit_idx = v2 % 8;
    if (!(alloc->interference[v1][byte_idx] & (1 << bit_idx))) {
        alloc->interference[v1][byte_idx] |= (1 << bit_idx);
        alloc->interference[v2][v1 / 8] |= (1 << (v1 % 8));
        alloc->degree[v1]++;
        alloc->degree[v2]++;
    }
}

static bool check_interference(SH2RegisterAllocator *alloc, int v1, int v2) {
    if (v1 < 0 || v2 < 0 || v1 >= alloc->num_vregs || v2 >= alloc->num_vregs) return false;
    return (alloc->interference[v1][v2 / 8] & (1 << (v2 % 8))) != 0;
}

void sh2_regalloc_build_interference(SH2RegisterAllocator *alloc) {
    for (int i = 0; i < alloc->num_vregs; i++) {
        LiveRange *r1 = alloc->live_ranges[i];
        if (!r1) continue;
        for (int j = i + 1; j < alloc->num_vregs; j++) {
            LiveRange *r2 = alloc->live_ranges[j];
            if (r2 && !(r1->end < r2->start || r2->end < r1->start))
                add_interference_edge(alloc, i, j);
        }
    }
}

static int select_color(SH2RegisterAllocator *alloc, int vreg, bool *removed);
static int select_spill_candidate(SH2RegisterAllocator *alloc, bool *removed);

bool sh2_regalloc_allocate_registers(SH2RegisterAllocator *alloc) {
    if (alloc->num_vregs == 0) return true;
    sh2_regalloc_build_interference(alloc);
    bool *removed = calloc(alloc->num_vregs, sizeof(bool));
    int *stack = malloc(alloc->num_vregs * sizeof(int));
    int stack_top = 0;
    bool progress = true;
    while (progress) {
        progress = false;
        for (int i = 0; i < alloc->num_vregs; i++) {
            if (removed[i] || alloc->virtual_regs[i].is_precolored) continue;
            if (alloc->degree[i] < NUM_ALLOCATABLE_REGS) {
                removed[i] = true;
                stack[stack_top++] = i;
                for (int j = 0; j < alloc->num_vregs; j++)
                    if (check_interference(alloc, i, j)) alloc->degree[j]--;
                progress = true;
            }
        }
        if (!progress) {
            int remaining = 0;
            for (int i = 0; i < alloc->num_vregs; i++)
                if (!removed[i] && !alloc->virtual_regs[i].is_precolored) remaining++;
            if (remaining > 0) {
                int spill = select_spill_candidate(alloc, removed);
                if (spill >= 0) {
                    removed[spill] = true;
                    stack[stack_top++] = spill;
                    alloc->virtual_regs[spill].needs_spill = true;
                    for (int j = 0; j < alloc->num_vregs; j++)
                        if (check_interference(alloc, spill, j)) alloc->degree[j]--;
                    progress = true;
                }
            }
        }
    }
    bool success = true;
    while (stack_top > 0) {
        int vreg = stack[--stack_top];
        removed[vreg] = false;
        if (alloc->virtual_regs[vreg].is_precolored) continue;
        int color = select_color(alloc, vreg, removed);
        if (color < 0) {
            alloc->virtual_regs[vreg].needs_spill = true;
            alloc->virtual_regs[vreg].color = -1;
            success = false;
        } else alloc->virtual_regs[vreg].color = color;
    }
    free(removed);
    free(stack);
    for (int i = 0; i < alloc->num_vregs; i++)
        if (alloc->virtual_regs[i].needs_spill && alloc->spill_slots[i] < 0)
            alloc->spill_slots[i] = alloc->num_spill_slots++;
    return success;
}

static int select_color(SH2RegisterAllocator *alloc, int vreg, bool *removed) {
    bool avail[NUM_ALLOCATABLE_REGS];
    memset(avail, true, sizeof(avail));
    for (int i = 0; i < alloc->num_vregs; i++) {
        if (removed[i] || !check_interference(alloc, vreg, i)) continue;
        int color = alloc->virtual_regs[i].color;
        if (color >= 0)
            for (int j = 0; j < NUM_ALLOCATABLE_REGS; j++)
                if (allocatable_regs[j] == (uint8_t)color) { avail[j] = false; break; }
    }
    for (int i = 0; i < NUM_ALLOCATABLE_REGS; i++)
        if (avail[i]) return allocatable_regs[i];
    return -1;
}

static int select_spill_candidate(SH2RegisterAllocator *alloc, bool *removed) {
    double min_p = 1e9;
    int cand = -1;
    for (int i = 0; i < alloc->num_vregs; i++) {
        if (removed[i] || alloc->virtual_regs[i].is_precolored) continue;
        LiveRange *r = alloc->live_ranges[i];
        if (!r) continue;
        double p = (double)r->spill_cost / (alloc->degree[i] + 1);
        if (p < min_p) { min_p = p; cand = i; }
    }
    return cand;
}

int sh2_regalloc_get_register(SH2RegisterAllocator *alloc, int vreg) {
    return (vreg >= 0 && vreg < alloc->num_vregs) ? alloc->virtual_regs[vreg].color : -1;
}

bool sh2_regalloc_is_spilled(SH2RegisterAllocator *alloc, int vreg) {
    return (vreg >= 0 && vreg < alloc->num_vregs) ? alloc->virtual_regs[vreg].needs_spill : false;
}

int sh2_regalloc_get_spill_slot(SH2RegisterAllocator *alloc, int vreg) {
    return (vreg >= 0 && vreg < alloc->num_vregs) ? alloc->spill_slots[vreg] : -1;
}

int sh2_regalloc_get_spill_offset(SH2RegisterAllocator *alloc, int vreg) {
    int slot = sh2_regalloc_get_spill_slot(alloc, vreg);
    return slot < 0 ? -1 : alloc->spill_base_offset - ((slot + 1) * 4);
}

int sh2_regalloc_get_num_spill_slots(SH2RegisterAllocator *alloc) {
    return alloc->num_spill_slots;
}

void sh2_regalloc_emit_spill(SH2RegisterAllocator *alloc, FILE *out, int vreg, int temp_reg) {
    if (!sh2_regalloc_is_spilled(alloc, vreg)) return;
    fprintf(out, "\t! Spill v%d to stack\n", vreg);
    sh2_mov_l_reg_disp(out, temp_reg, sh2_regalloc_get_spill_offset(alloc, vreg), 14);
    alloc->num_spills++;
}

void sh2_regalloc_emit_reload(SH2RegisterAllocator *alloc, FILE *out, int vreg, int temp_reg) {
    if (!sh2_regalloc_is_spilled(alloc, vreg)) return;
    fprintf(out, "\t! Reload v%d from stack\n", vreg);
    sh2_mov_l_disp_reg(out, temp_reg, sh2_regalloc_get_spill_offset(alloc, vreg), 14);
    alloc->num_reloads++;
}

bool sh2_regalloc_can_coalesce(SH2RegisterAllocator *alloc, int v1, int v2) {
    if (v1 < 0 || v2 < 0 || v1 >= alloc->num_vregs || v2 >= alloc->num_vregs) return false;
    if (check_interference(alloc, v1, v2)) return false;
    return (alloc->degree[v1] + alloc->degree[v2]) < NUM_ALLOCATABLE_REGS;
}

void sh2_regalloc_coalesce(SH2RegisterAllocator *alloc, int v1, int v2) {
    if (!sh2_regalloc_can_coalesce(alloc, v1, v2)) return;
    LiveRange *r1 = alloc->live_ranges[v1], *r2 = alloc->live_ranges[v2];
    if (r1 && r2) {
        if (r2->start < r1->start) r1->start = r2->start;
        if (r2->end > r1->end) r1->end = r2->end;
        r1->spill_cost += r2->spill_cost;
    }
    for (int i = 0; i < alloc->num_vregs; i++)
        if (check_interference(alloc, v2, i)) add_interference_edge(alloc, v1, i);
    alloc->num_moves++;
}

int sh2_regalloc_compute_pressure(SH2RegisterAllocator *alloc, int position) {
    int p = 0;
    for (int i = 0; i < alloc->num_vregs; i++) {
        LiveRange *r = alloc->live_ranges[i];
        if (r && position >= r->start && position <= r->end) p++;
    }
    return p;
}

bool sh2_regalloc_needs_spilling(SH2RegisterAllocator *alloc, int position) {
    return sh2_regalloc_compute_pressure(alloc, position) > NUM_ALLOCATABLE_REGS;
}

void sh2_regalloc_lock_register(SH2RegisterAllocator *alloc, int phys_reg) {
    if (phys_reg >= 0 && phys_reg < 16) {
        alloc->reg_locked[phys_reg]++;
        alloc->reg_in_use[phys_reg] = true;
    }
}

void sh2_regalloc_unlock_register(SH2RegisterAllocator *alloc, int phys_reg) {
    if (phys_reg >= 0 && phys_reg < 16 && alloc->reg_locked[phys_reg] > 0) {
        alloc->reg_locked[phys_reg]--;
        if (alloc->reg_locked[phys_reg] == 0) alloc->reg_in_use[phys_reg] = false;
    }
}

bool sh2_regalloc_is_locked(SH2RegisterAllocator *alloc, int phys_reg) {
    return (phys_reg >= 0 && phys_reg < 16) ? alloc->reg_locked[phys_reg] > 0 : false;
}

int sh2_regalloc_find_free_temp(SH2RegisterAllocator *alloc) {
    for (int i = 0; i < 4; i++)
        if (!alloc->reg_in_use[i] && !alloc->reg_locked[i]) return i;
    return -1;
}

int sh2_regalloc_find_free_saved(SH2RegisterAllocator *alloc) {
    for (int i = 8; i <= 13; i++)
        if (!alloc->reg_in_use[i] && !alloc->reg_locked[i]) return i;
    return -1;
}

void sh2_regalloc_print_allocation(SH2RegisterAllocator *alloc, FILE *out) {
    fprintf(out, "\n! Register Allocation: %d vregs, %d spill slots\n",
            alloc->num_vregs, alloc->num_spill_slots);
}

void sh2_regalloc_get_stats(SH2RegisterAllocator *alloc, int *s, int *r, int *m) {
    if (s) *s = alloc->num_spills;
    if (r) *r = alloc->num_reloads;
    if (m) *m = alloc->num_moves;
}

void sh2_regalloc_dump_interference(SH2RegisterAllocator *alloc, FILE *out) {
    fprintf(out, "! Interference graph for %d vregs\n", alloc->num_vregs);
}

bool sh2_regalloc_verify(SH2RegisterAllocator *alloc) {
    for (int i = 0; i < alloc->num_vregs; i++) {
        if (alloc->virtual_regs[i].needs_spill) continue;
        int c1 = alloc->virtual_regs[i].color;
        if (c1 < 0) return false;
        for (int j = i + 1; j < alloc->num_vregs; j++) {
            if (alloc->virtual_regs[j].needs_spill) continue;
            if (check_interference(alloc, i, j) && alloc->virtual_regs[j].color == c1)
                return false;
        }
    }
    return true;
}

bool sh2_regalloc_linear_scan(SH2RegisterAllocator *alloc) {
    (void)alloc;
    return true;  // Stub for linear scan algorithm
}