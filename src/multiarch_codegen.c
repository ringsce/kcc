#include "kcc.h"
#include "multiarch_codegen.h"

// ===== ARCHITECTURE DEFINITIONS =====

// x86_64 registers
static const RegisterInfo x86_64_general_regs[] = {
    {"rax", REG_GENERAL, 8, false}, {"rbx", REG_GENERAL, 8, true},
    {"rcx", REG_GENERAL, 8, false}, {"rdx", REG_GENERAL, 8, false},
    {"rsi", REG_GENERAL, 8, false}, {"rdi", REG_GENERAL, 8, false},
    {"r8",  REG_GENERAL, 8, false}, {"r9",  REG_GENERAL, 8, false},
    {"r10", REG_GENERAL, 8, false}, {"r11", REG_GENERAL, 8, false},
    {"r12", REG_GENERAL, 8, true},  {"r13", REG_GENERAL, 8, true},
    {"r14", REG_GENERAL, 8, true},  {"r15", REG_GENERAL, 8, true},
    {"rsp", REG_SPECIAL, 8, true},  {"rbp", REG_SPECIAL, 8, true}
};

static const RegisterInfo x86_64_float_regs[] = {
    {"xmm0", REG_FLOAT, 16, false}, {"xmm1", REG_FLOAT, 16, false},
    {"xmm2", REG_FLOAT, 16, false}, {"xmm3", REG_FLOAT, 16, false},
    {"xmm4", REG_FLOAT, 16, false}, {"xmm5", REG_FLOAT, 16, false},
    {"xmm6", REG_FLOAT, 16, false}, {"xmm7", REG_FLOAT, 16, false}
};

// ARM64 registers
static const RegisterInfo arm64_general_regs[] = {
    {"x0",  REG_GENERAL, 8, false}, {"x1",  REG_GENERAL, 8, false},
    {"x2",  REG_GENERAL, 8, false}, {"x3",  REG_GENERAL, 8, false},
    {"x4",  REG_GENERAL, 8, false}, {"x5",  REG_GENERAL, 8, false},
    {"x6",  REG_GENERAL, 8, false}, {"x7",  REG_GENERAL, 8, false},
    {"x8",  REG_GENERAL, 8, false}, {"x9",  REG_GENERAL, 8, false},
    {"x10", REG_GENERAL, 8, false}, {"x11", REG_GENERAL, 8, false},
    {"x12", REG_GENERAL, 8, false}, {"x13", REG_GENERAL, 8, false},
    {"x14", REG_GENERAL, 8, false}, {"x15", REG_GENERAL, 8, false},
    {"x19", REG_GENERAL, 8, true},  {"x20", REG_GENERAL, 8, true},
    {"x21", REG_GENERAL, 8, true},  {"x22", REG_GENERAL, 8, true},
    {"x29", REG_SPECIAL, 8, true},  {"x30", REG_SPECIAL, 8, true},
    {"sp",  REG_SPECIAL, 8, true}
};

static const RegisterInfo arm64_float_regs[] = {
    {"v0",  REG_FLOAT, 16, false}, {"v1",  REG_FLOAT, 16, false},
    {"v2",  REG_FLOAT, 16, false}, {"v3",  REG_FLOAT, 16, false},
    {"v4",  REG_FLOAT, 16, false}, {"v5",  REG_FLOAT, 16, false},
    {"v6",  REG_FLOAT, 16, false}, {"v7",  REG_FLOAT, 16, false}
};

// Parameter passing registers
static const char *x86_64_sysv_param_regs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
static const char *x86_64_sysv_return_regs[] = {"rax", "rdx"};

static const char *arm64_param_regs[] = {"x0", "x1", "x2", "x3", "x4", "x5", "x6", "x7"};
static const char *arm64_return_regs[] = {"x0", "x1"};

// ===== TARGET CONFIGURATION =====

TargetArch detect_host_architecture(void) {
#if defined(__x86_64__) || defined(_M_X64)
    return ARCH_X86_64;
#elif defined(__aarch64__) || defined(_M_ARM64)
    return ARCH_ARM64;
#else
    return ARCH_UNKNOWN;
#endif
}

TargetPlatform detect_host_platform(void) {
#if defined(__linux__)
    return PLATFORM_LINUX;
#elif defined(__APPLE__)
    return PLATFORM_MACOS;
#else
    return PLATFORM_UNKNOWN;
#endif
}

TargetConfig *target_config_create(TargetArch arch, TargetPlatform platform) {
    TargetConfig *config = malloc(sizeof(TargetConfig));
    if (!config) return NULL;

    config->arch = arch;
    config->platform = platform;

    // Configure based on architecture and platform
    switch (arch) {
        case ARCH_X86_64:
            config->arch_name = "x86_64";
            config->pointer_size = 8;
            config->stack_alignment = 16;
            config->general_regs = x86_64_general_regs;
            config->float_regs = x86_64_float_regs;
            config->num_general_regs = sizeof(x86_64_general_regs) / sizeof(RegisterInfo);
            config->num_float_regs = sizeof(x86_64_float_regs) / sizeof(RegisterInfo);
            config->calling_conv = CALL_CONV_SYSV;
            config->param_regs = x86_64_sysv_param_regs;
            config->return_regs = x86_64_sysv_return_regs;
            config->num_param_regs = 6;
            config->num_return_regs = 2;
            config->syscall_reg = "rax";
            config->syscall_instruction = "syscall";
            config->comment_prefix = "#";
            config->att_syntax = true;
            break;

        case ARCH_ARM64:
            config->arch_name = "arm64";
            config->pointer_size = 8;
            config->stack_alignment = 16;
            config->general_regs = arm64_general_regs;
            config->float_regs = arm64_float_regs;
            config->num_general_regs = sizeof(arm64_general_regs) / sizeof(RegisterInfo);
            config->num_float_regs = sizeof(arm64_float_regs) / sizeof(RegisterInfo);
            config->calling_conv = CALL_CONV_AARCH64;
            config->param_regs = arm64_param_regs;
            config->return_regs = arm64_return_regs;
            config->num_param_regs = 8;
            config->num_return_regs = 2;
            config->syscall_reg = "x8";
            config->syscall_instruction = "svc #0";
            config->comment_prefix = "//";
            config->att_syntax = false;
            break;

        default:
            free(config);
            return NULL;
    }

    // Configure platform-specific settings
    switch (platform) {
        case PLATFORM_LINUX:
            config->platform_name = "linux";
            config->global_directive = ".globl";
            config->section_text = ".text";
            config->section_data = ".data";
            break;

        case PLATFORM_MACOS:
            config->platform_name = "macos";
            config->global_directive = ".globl";
            config->section_text = ".text";
            config->section_data = ".data";
            // macOS uses underscore prefix for symbols
            break;

        default:
            free(config);
            return NULL;
    }

    return config;
}

void target_config_destroy(TargetConfig *config) {
    if (config) {
        free(config);
    }
}

// ===== MULTI-ARCH CODEGEN =====

MultiArchCodegen *multiarch_codegen_create(const char *output_file, TargetArch arch, TargetPlatform platform) {
    MultiArchCodegen *codegen = malloc(sizeof(MultiArchCodegen));
    if (!codegen) return NULL;

    codegen->output_file = fopen(output_file, "w");
    if (!codegen->output_file) {
        free(codegen);
        return NULL;
    }

    codegen->target = target_config_create(arch, platform);
    if (!codegen->target) {
        fclose(codegen->output_file);
        free(codegen);
        return NULL;
    }

    codegen->label_counter = 0;
    codegen->temp_counter = 0;
    codegen->current_function_locals = 0;
    codegen->stack_offset = 0;
    codegen->local_var_count = 0;
    codegen->in_function = false;
    codegen->stack_size = 0;

    return codegen;
}

void multiarch_codegen_destroy(MultiArchCodegen *codegen) {
    if (codegen) {
        if (codegen->output_file) {
            fclose(codegen->output_file);
        }
        target_config_destroy(codegen->target);
        free(codegen);
    }
}

void multiarch_emit(MultiArchCodegen *codegen, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(codegen->output_file, format, args);
    va_end(args);
    fprintf(codegen->output_file, "\n");
}

void multiarch_emit_comment(MultiArchCodegen *codegen, const char *comment) {
    multiarch_emit(codegen, "%s %s", codegen->target->comment_prefix, comment);
}

void multiarch_emit_label(MultiArchCodegen *codegen, const char *label) {
    multiarch_emit(codegen, "%s:", label);
}

void multiarch_emit_directive(MultiArchCodegen *codegen, const char *directive, const char *args) {
    if (args) {
        multiarch_emit(codegen, "%s %s", directive, args);
    } else {
        multiarch_emit(codegen, "%s", directive);
    }
}

// ===== ARCHITECTURE-SPECIFIC IMPLEMENTATIONS =====

void multiarch_load_immediate(MultiArchCodegen *codegen, const char *dest_reg, long value) {
    switch (codegen->target->arch) {
        case ARCH_X86_64:
            multiarch_emit(codegen, "    movq $%ld, %%%s", value, dest_reg);
            break;
        case ARCH_ARM64:
            if (value >= 0 && value < 65536) {
                multiarch_emit(codegen, "    mov %s, #%ld", dest_reg, value);
            } else {
                multiarch_emit(codegen, "    mov %s, #%ld", dest_reg, value & 0xFFFF);
                if (value > 65535) {
                    multiarch_emit(codegen, "    movk %s, #%ld, lsl #16", dest_reg, (value >> 16) & 0xFFFF);
                }
                if (value > 0xFFFFFFFF) {
                    multiarch_emit(codegen, "    movk %s, #%ld, lsl #32", dest_reg, (value >> 32) & 0xFFFF);
                    multiarch_emit(codegen, "    movk %s, #%ld, lsl #48", dest_reg, (value >> 48) & 0xFFFF);
                }
            }
            break;
        default:
            break;
    }
}

void multiarch_add(MultiArchCodegen *codegen, const char *dest, const char *src1, const char *src2) {
    switch (codegen->target->arch) {
        case ARCH_X86_64:
            if (strcmp(dest, src1) == 0) {
                multiarch_emit(codegen, "    addq %%%s, %%%s", src2, dest);
            } else {
                multiarch_emit(codegen, "    movq %%%s, %%%s", src1, dest);
                multiarch_emit(codegen, "    addq %%%s, %%%s", src2, dest);
            }
            break;
        case ARCH_ARM64:
            multiarch_emit(codegen, "    add %s, %s, %s", dest, src1, src2);
            break;
        default:
            break;
    }
}

void multiarch_sub(MultiArchCodegen *codegen, const char *dest, const char *src1, const char *src2) {
    switch (codegen->target->arch) {
        case ARCH_X86_64:
            if (strcmp(dest, src1) == 0) {
                multiarch_emit(codegen, "    subq %%%s, %%%s", src2, dest);
            } else {
                multiarch_emit(codegen, "    movq %%%s, %%%s", src1, dest);
                multiarch_emit(codegen, "    subq %%%s, %%%s", src2, dest);
            }
            break;
        case ARCH_ARM64:
            multiarch_emit(codegen, "    sub %s, %s, %s", dest, src1, src2);
            break;
        default:
            break;
    }
}

void multiarch_mul(MultiArchCodegen *codegen, const char *dest, const char *src1, const char *src2) {
    switch (codegen->target->arch) {
        case ARCH_X86_64:
            if (strcmp(dest, src1) == 0) {
                multiarch_emit(codegen, "    imulq %%%s, %%%s", src2, dest);
            } else {
                multiarch_emit(codegen, "    movq %%%s, %%%s", src1, dest);
                multiarch_emit(codegen, "    imulq %%%s, %%%s", src2, dest);
            }
            break;
        case ARCH_ARM64:
            multiarch_emit(codegen, "    mul %s, %s, %s", dest, src1, src2);
            break;
        default:
            break;
    }
}

void multiarch_div(MultiArchCodegen *codegen, const char *dest, const char *src1, const char *src2) {
    switch (codegen->target->arch) {
        case ARCH_X86_64:
            // x86_64 division is more complex - needs rax/rdx setup
            multiarch_emit(codegen, "    movq %%%s, %%rax", src1);
            multiarch_emit(codegen, "    cqo");  // Sign extend rax to rdx:rax
            multiarch_emit(codegen, "    idivq %%%s", src2);
            if (strcmp(dest, "rax") != 0) {
                multiarch_emit(codegen, "    movq %%rax, %%%s", dest);
            }
            break;
        case ARCH_ARM64:
            multiarch_emit(codegen, "    sdiv %s, %s, %s", dest, src1, src2);
            break;
        default:
            break;
    }
}

void multiarch_compare(MultiArchCodegen *codegen, const char *reg1, const char *reg2) {
    switch (codegen->target->arch) {
        case ARCH_X86_64:
            multiarch_emit(codegen, "    cmpq %%%s, %%%s", reg2, reg1);
            break;
        case ARCH_ARM64:
            multiarch_emit(codegen, "    cmp %s, %s", reg1, reg2);
            break;
        default:
            break;
    }
}

void multiarch_jump(MultiArchCodegen *codegen, const char *label) {
    switch (codegen->target->arch) {
        case ARCH_X86_64:
            multiarch_emit(codegen, "    jmp %s", label);
            break;
        case ARCH_ARM64:
            multiarch_emit(codegen, "    b %s", label);
            break;
        default:
            break;
    }
}

void multiarch_jump_if_zero(MultiArchCodegen *codegen, const char *label) {
    switch (codegen->target->arch) {
        case ARCH_X86_64:
            multiarch_emit(codegen, "    jz %s", label);
            break;
        case ARCH_ARM64:
            multiarch_emit(codegen, "    b.eq %s", label);
            break;
        default:
            break;
    }
}

void multiarch_jump_if_equal(MultiArchCodegen *codegen, const char *label) {
    multiarch_jump_if_zero(codegen, label);  // Same as jump if zero after compare
}

void multiarch_jump_if_not_equal(MultiArchCodegen *codegen, const char *label) {
    switch (codegen->target->arch) {
        case ARCH_X86_64:
            multiarch_emit(codegen, "    jnz %s", label);
            break;
        case ARCH_ARM64:
            multiarch_emit(codegen, "    b.ne %s", label);
            break;
        default:
            break;
    }
}

void multiarch_jump_if_less(MultiArchCodegen *codegen, const char *label) {
    switch (codegen->target->arch) {
        case ARCH_X86_64:
            multiarch_emit(codegen, "    jl %s", label);
            break;
        case ARCH_ARM64:
            multiarch_emit(codegen, "    b.lt %s", label);
            break;
        default:
            break;
    }
}

void multiarch_jump_if_greater(MultiArchCodegen *codegen, const char *label) {
    switch (codegen->target->arch) {
        case ARCH_X86_64:
            multiarch_emit(codegen, "    jg %s", label);
            break;
        case ARCH_ARM64:
            multiarch_emit(codegen, "    b.gt %s", label);
            break;
        default:
            break;
    }
}

void multiarch_function_prologue(MultiArchCodegen *codegen, const char *func_name, int param_count) {
    codegen->in_function = true;
    strncpy(codegen->current_function, func_name, sizeof(codegen->current_function) - 1);
    codegen->local_var_count = 0;
    codegen->stack_offset = 0;

    // Emit function label
    if (codegen->target->platform == PLATFORM_MACOS) {
        multiarch_emit_label(codegen, func_name);  // macOS may need underscore prefix
    } else {
        multiarch_emit_label(codegen, func_name);
    }

    switch (codegen->target->arch) {
        case ARCH_X86_64:
            multiarch_emit(codegen, "    pushq %%rbp");
            multiarch_emit(codegen, "    movq %%rsp, %%rbp");
            break;
        case ARCH_ARM64:
            multiarch_emit(codegen, "    stp x29, x30, [sp, #-16]!");
            multiarch_emit(codegen, "    mov x29, sp");
            break;
        default:
            break;
    }
}

void multiarch_function_epilogue(MultiArchCodegen *codegen) {
    switch (codegen->target->arch) {
        case ARCH_X86_64:
            multiarch_emit(codegen, "    movq %%rbp, %%rsp");
            multiarch_emit(codegen, "    popq %%rbp");
            multiarch_emit(codegen, "    ret");
            break;
        case ARCH_ARM64:
            multiarch_emit(codegen, "    ldp x29, x30, [sp], #16");
            multiarch_emit(codegen, "    ret");
            break;
        default:
            break;
    }

    codegen->in_function = false;
}

void multiarch_function_return(MultiArchCodegen *codegen, bool has_value) {
    if (!has_value) {
        // Set return value to 0 for void functions
        multiarch_load_immediate(codegen, multiarch_get_return_reg(codegen), 0);
    }
    multiarch_function_epilogue(codegen);
}

void multiarch_function_call(MultiArchCodegen *codegen, const char *func_name, int arg_count) {
    switch (codegen->target->arch) {
        case ARCH_X86_64:
            // Arguments should already be in the right registers
            multiarch_emit(codegen, "    call %s", func_name);
            // Clean up stack if needed (for arguments passed on stack)
            if (arg_count > 6) {
                int stack_cleanup = (arg_count - 6) * 8;
                multiarch_emit(codegen, "    addq $%d, %%rsp", stack_cleanup);
            }
            break;
        case ARCH_ARM64:
            multiarch_emit(codegen, "    bl %s", func_name);
            break;
        default:
            break;
    }
}

void multiarch_push(MultiArchCodegen *codegen, const char *reg) {
    switch (codegen->target->arch) {
        case ARCH_X86_64:
            multiarch_emit(codegen, "    pushq %%%s", reg);
            break;
        case ARCH_ARM64:
            multiarch_emit(codegen, "    str %s, [sp, #-16]!", reg);
            break;
        default:
            break;
    }
}

void multiarch_pop(MultiArchCodegen *codegen, const char *reg) {
    switch (codegen->target->arch) {
        case ARCH_X86_64:
            multiarch_emit(codegen, "    popq %%%s", reg);
            break;
        case ARCH_ARM64:
            multiarch_emit(codegen, "    ldr %s, [sp], #16", reg);
            break;
        default:
            break;
    }
}

const char *multiarch_get_param_reg(MultiArchCodegen *codegen, int param_index) {
    if (param_index < codegen->target->num_param_regs) {
        return codegen->target->param_regs[param_index];
    }
    return NULL; // Parameter passed on stack
}

const char *multiarch_get_return_reg(MultiArchCodegen *codegen) {
    return codegen->target->return_regs[0];
}

const char *multiarch_get_temp_reg(MultiArchCodegen *codegen, int index) {
    switch (codegen->target->arch) {
        case ARCH_X86_64:
            switch (index) {
                case 0: return "r10";
                case 1: return "r11";
                default: return "rax";
            }
        case ARCH_ARM64:
            switch (index) {
                case 0: return "x9";
                case 1: return "x10";
                case 2: return "x11";
                default: return "x0";
            }
        default:
            return "unknown";
    }
}

const char *multiarch_get_stack_pointer(MultiArchCodegen *codegen) {
    switch (codegen->target->arch) {
        case ARCH_X86_64:
            return "rsp";
        case ARCH_ARM64:
            return "sp";
        default:
            return "unknown";
    }
}

const char *multiarch_get_frame_pointer(MultiArchCodegen *codegen) {
    switch (codegen->target->arch) {
        case ARCH_X86_64:
            return "rbp";
        case ARCH_ARM64:
            return "x29";
        default:
            return "unknown";
    }
}

void multiarch_load_memory(MultiArchCodegen *codegen, const char *dest_reg, const char *src_addr, int size) {
    switch (codegen->target->arch) {
        case ARCH_X86_64:
            if (size == 8) {
                multiarch_emit(codegen, "    movq %s, %%%s", src_addr, dest_reg);
            } else if (size == 4) {
                multiarch_emit(codegen, "    movl %s, %%%s", src_addr, dest_reg);
            } else if (size == 1) {
                multiarch_emit(codegen, "    movb %s, %%%s", src_addr, dest_reg);
            }
            break;
        case ARCH_ARM64:
            if (size == 8) {
                multiarch_emit(codegen, "    ldr %s, %s", dest_reg, src_addr);
            } else if (size == 4) {
                multiarch_emit(codegen, "    ldr w%s, %s", dest_reg + 1, src_addr); // w register for 32-bit
            } else if (size == 1) {
                multiarch_emit(codegen, "    ldrb w%s, %s", dest_reg + 1, src_addr);
            }
            break;
        default:
            break;
    }
}

void multiarch_store_memory(MultiArchCodegen *codegen, const char *src_reg, const char *dest_addr, int size) {
    switch (codegen->target->arch) {
        case ARCH_X86_64:
            if (size == 8) {
                multiarch_emit(codegen, "    movq %%%s, %s", src_reg, dest_addr);
            } else if (size == 4) {
                multiarch_emit(codegen, "    movl %%%s, %s", src_reg, dest_addr);
            } else if (size == 1) {
                multiarch_emit(codegen, "    movb %%%s, %s", src_reg, dest_addr);
            }
            break;
        case ARCH_ARM64:
            if (size == 8) {
                multiarch_emit(codegen, "    str %s, %s", src_reg, dest_addr);
            } else if (size == 4) {
                multiarch_emit(codegen, "    str w%s, %s", src_reg + 1, dest_addr);
            } else if (size == 1) {
                multiarch_emit(codegen, "    strb w%s, %s", src_reg + 1, dest_addr);
            }
            break;
        default:
            break;
    }
}

void multiarch_syscall(MultiArchCodegen *codegen, int syscall_num, int arg_count) {
    multiarch_load_immediate(codegen, codegen->target->syscall_reg, syscall_num);
    multiarch_emit(codegen, "    %s", codegen->target->syscall_instruction);
}

void multiarch_exit_program(MultiArchCodegen *codegen, int exit_code) {
    const char *return_reg = multiarch_get_return_reg(codegen);
    multiarch_load_immediate(codegen, return_reg, exit_code);

    switch (codegen->target->platform) {
        case PLATFORM_LINUX:
            if (codegen->target->arch == ARCH_X86_64) {
                multiarch_syscall(codegen, 60, 1); // sys_exit on Linux x86_64
            } else if (codegen->target->arch == ARCH_ARM64) {
                multiarch_syscall(codegen, 93, 1); // sys_exit on Linux ARM64
            }
            break;
        case PLATFORM_MACOS:
            if (codegen->target->arch == ARCH_X86_64) {
                multiarch_syscall(codegen, 0x2000001, 1); // sys_exit on macOS x86_64
            } else if (codegen->target->arch == ARCH_ARM64) {
                multiarch_syscall(codegen, 1, 1); // sys_exit on macOS ARM64
            }
            break;
        default:
            break;
    }
}

char *multiarch_new_label(MultiArchCodegen *codegen) {
    char *label = malloc(32);
    snprintf(label, 32, "L%d", codegen->label_counter++);
    return label;
}

char *multiarch_new_temp(MultiArchCodegen *codegen) {
    char *temp = malloc(32);
    snprintf(temp, 32, "t%d", codegen->temp_counter++);
    return temp;
}

void multiarch_declare_local_var(MultiArchCodegen *codegen, const char *name, int size) {
    if (codegen->local_var_count >= 256) return;

    // Align to proper boundary
    int alignment = (size >= 8) ? 8 : size;
    codegen->stack_offset = (codegen->stack_offset + alignment - 1) & ~(alignment - 1);
    codegen->stack_offset += size;

    // Store variable info
    strncpy(codegen->local_vars[codegen->local_var_count].name, name, 63);
    codegen->local_vars[codegen->local_var_count].name[63] = '\0';
    codegen->local_vars[codegen->local_var_count].offset = -codegen->stack_offset;
    codegen->local_vars[codegen->local_var_count].size = size;
    codegen->local_var_count++;
}

int multiarch_get_local_var_offset(MultiArchCodegen *codegen, const char *name) {
    for (int i = 0; i < codegen->local_var_count; i++) {
        if (strcmp(codegen->local_vars[i].name, name) == 0) {
            return codegen->local_vars[i].offset;
        }
    }
    return 0; // Not found
}

void multiarch_load_local_var(MultiArchCodegen *codegen, const char *dest_reg, const char *var_name) {
    int offset = multiarch_get_local_var_offset(codegen, var_name);
    const char *fp = multiarch_get_frame_pointer(codegen);

    switch (codegen->target->arch) {
        case ARCH_X86_64:
            multiarch_emit(codegen, "    movq %d(%%%s), %%%s", offset, fp, dest_reg);
            break;
        case ARCH_ARM64:
            multiarch_emit(codegen, "    ldr %s, [%s, #%d]", dest_reg, fp, offset);
            break;
        default:
            break;
    }
}

void multiarch_store_local_var(MultiArchCodegen *codegen, const char *src_reg, const char *var_name) {
    int offset = multiarch_get_local_var_offset(codegen, var_name);
    const char *fp = multiarch_get_frame_pointer(codegen);

    switch (codegen->target->arch) {
        case ARCH_X86_64:
            multiarch_emit(codegen, "    movq %%%s, %d(%%%s)", src_reg, offset, fp);
            break;
        case ARCH_ARM64:
            multiarch_emit(codegen, "    str %s, [%s, #%d]", src_reg, fp, offset);
            break;
        default:
            break;
    }
}

// ===== AST CODE GENERATION =====

bool multiarch_codegen_generate(MultiArchCodegen *codegen, struct ASTNode *ast) {
    if (!codegen || !ast) return false;

    // Emit header
    multiarch_emit_comment(codegen, "Generated by KCC Multi-Architecture Compiler");
    char target_info[128];
    snprintf(target_info, sizeof(target_info), "Target: %s-%s",
             codegen->target->arch_name, codegen->target->platform_name);
    multiarch_emit_comment(codegen, target_info);
    multiarch_emit(codegen, "");

    // Emit sections
    multiarch_emit_directive(codegen, codegen->target->section_text, NULL);
    multiarch_emit(codegen, "");

    // Generate code for the AST
    multiarch_codegen_program(codegen, ast);

    // Add program entry point
    if (codegen->target->platform == PLATFORM_LINUX) {
        multiarch_emit(codegen, "");
        multiarch_emit_directive(codegen, codegen->target->global_directive, "_start");
        multiarch_emit_label(codegen, "_start");
        multiarch_function_call(codegen, "main", 0);
        multiarch_exit_program(codegen, 0);
    } else if (codegen->target->platform == PLATFORM_MACOS) {
        multiarch_emit(codegen, "");
        multiarch_emit_directive(codegen, codegen->target->global_directive, "_main");
        // macOS handles entry differently
    }

    return true;
}

void multiarch_codegen_program(MultiArchCodegen *codegen, struct ASTNode *node) {
    if (node->type != AST_PROGRAM) return;

    for (int i = 0; i < node->data.program.declaration_count; i++) {
        struct ASTNode *decl = node->data.program.declarations[i];

        if (decl->type == AST_FUNCTION_DECLARATION) {
            multiarch_codegen_function_declaration(codegen, decl);
        } else if (decl->type == AST_VAR_DECL) {
            multiarch_codegen_variable_declaration(codegen, decl);
        }
    }
}

void multiarch_codegen_function_declaration(MultiArchCodegen *codegen, struct ASTNode *node) {
    if (node->type != AST_FUNCTION_DECLARATION) return;

    multiarch_emit(codegen, "");
    char func_comment[128];
    snprintf(func_comment, sizeof(func_comment), "Function: %s", node->data.function_decl.name);
    multiarch_emit_comment(codegen, func_comment);

    // Make function global if it's main or explicitly requested
    if (strcmp(node->data.function_decl.name, "main") == 0) {
        if (codegen->target->platform == PLATFORM_MACOS) {
            multiarch_emit_directive(codegen, codegen->target->global_directive, "_main");
        } else {
            multiarch_emit_directive(codegen, codegen->target->global_directive, "main");
        }
    }

    multiarch_function_prologue(codegen, node->data.function_decl.name, node->data.function_decl.parameter_count);

    // Allocate space for local variables if needed
    if (codegen->stack_offset > 0) {
        multiarch_stack_alloc(codegen, codegen->stack_offset);
    }

    // Generate function body
    if (node->data.function_decl.body) {
        multiarch_codegen_statement(codegen, node->data.function_decl.body);
    }

    // Add default return if needed
    multiarch_function_return(codegen, false);
}

void multiarch_codegen_variable_declaration(MultiArchCodegen *codegen, struct ASTNode *node) {
    if (node->type != AST_VAR_DECL) return;

    char var_comment[128];
    snprintf(var_comment, sizeof(var_comment), "Variable: %s", node->data.var_decl.name);
    multiarch_emit_comment(codegen, var_comment);

    // For local variables, declare space on stack
    if (codegen->in_function) {
        int size = 8; // Default size for int
        multiarch_declare_local_var(codegen, node->data.var_decl.name, size);

        if (node->data.var_decl.initializer) {
            // Generate code for initializer
            multiarch_codegen_expression(codegen, node->data.var_decl.initializer);
            // Store result in variable
            multiarch_store_local_var(codegen, multiarch_get_return_reg(codegen), node->data.var_decl.name);
        }
    }
}

void multiarch_codegen_statement(MultiArchCodegen *codegen, struct ASTNode *node) {
    switch (node->type) {
        case AST_COMPOUND_STATEMENT:
            for (int i = 0; i < node->data.compound_stmt.statement_count; i++) {
                multiarch_codegen_statement(codegen, node->data.compound_stmt.statements[i]);
            }
            break;

        case AST_EXPRESSION_STATEMENT:
            if (node->data.expression_stmt.expression) {
                multiarch_codegen_expression(codegen, node->data.expression_stmt.expression);
            }
            break;

        case AST_RETURN_STATEMENT:
            multiarch_codegen_return_stmt(codegen, node);
            break;

        case AST_IF_STATEMENT:
            multiarch_codegen_if_stmt(codegen, node);
            break;

        case AST_WHILE_STATEMENT:
            multiarch_codegen_while_stmt(codegen, node);
            break;

        case AST_VAR_DECL:
            multiarch_codegen_variable_declaration(codegen, node);
            break;

        default:
            multiarch_emit_comment(codegen, "Unsupported statement type");
            break;
    }
}

void multiarch_codegen_expression(MultiArchCodegen *codegen, struct ASTNode *node) {
    switch (node->type) {
        case AST_NUMBER_LITERAL:
            multiarch_load_immediate(codegen, multiarch_get_return_reg(codegen), node->data.number.value);
            break;

        case AST_IDENTIFIER:
            multiarch_load_local_var(codegen, multiarch_get_return_reg(codegen), node->data.identifier.name);
            break;

        case AST_BINARY_OP:
            multiarch_codegen_binary_expr(codegen, node);
            break;

        case AST_FUNCTION_CALL:
            multiarch_codegen_call_expr(codegen, node);
            break;

        default:
            multiarch_emit_comment(codegen, "Unsupported expression type");
            break;
    }
}

void multiarch_codegen_binary_expr(MultiArchCodegen *codegen, struct ASTNode *node) {
    if (node->type != AST_BINARY_OP) return;

    // Generate left operand
    multiarch_codegen_expression(codegen, node->data.binary_expr.left);
    multiarch_push(codegen, multiarch_get_return_reg(codegen));

    // Generate right operand
    multiarch_codegen_expression(codegen, node->data.binary_expr.right);
    const char *right_reg = multiarch_get_return_reg(codegen);
    const char *temp_reg = multiarch_get_temp_reg(codegen, 0);
    multiarch_pop(codegen, temp_reg);

    // Perform operation
    switch (node->data.binary_expr.operator) {
        case TOKEN_PLUS:
            multiarch_add(codegen, right_reg, temp_reg, right_reg);
            break;
        case TOKEN_MINUS:
            multiarch_sub(codegen, right_reg, temp_reg, right_reg);
            break;
        case TOKEN_CHAR:
            multiarch_mul(codegen, right_reg, temp_reg, right_reg);
            break;
        case TOKEN_DIVIDE:
            multiarch_div(codegen, right_reg, temp_reg, right_reg);
            break;
        default:
            multiarch_emit_comment(codegen, "Unsupported binary operator");
            break;
    }
}

void multiarch_codegen_return_stmt(MultiArchCodegen *codegen, struct ASTNode *node) {
    if (node->data.return_stmt.expression) {
        multiarch_codegen_expression(codegen, node->data.return_stmt.expression);
    }
    multiarch_function_return(codegen, node->data.return_stmt.expression != NULL);
}

void multiarch_codegen_call_expr(MultiArchCodegen *codegen, struct ASTNode *node) {
    if (node->type != AST_FUNCTION_CALL) return;

    // Generate arguments and place in parameter registers
    // FIX: Changed .arg_count to .argument_count and .args to .arguments
    for (int i = 0; i < node->data.call_expr.argument_count && i < codegen->target->num_param_regs; i++) {
        multiarch_codegen_expression(codegen, node->data.call_expr.arguments[i]);
        const char *param_reg = multiarch_get_param_reg(codegen, i);
        const char *return_reg = multiarch_get_return_reg(codegen);

        if (strcmp(param_reg, return_reg) != 0) {
            // Move result to parameter register
            switch (codegen->target->arch) {
                case ARCH_X86_64:
                    multiarch_emit(codegen, "    movq %%%s, %%%s", return_reg, param_reg);
                    break;
                case ARCH_ARM64:
                    multiarch_emit(codegen, "    mov %s, %s", param_reg, return_reg);
                    break;
                default:
                    break;
            }
        }
    }
    // FIX: Changed undeclared 'arg_count' to the correct struct member
    multiarch_function_call(codegen, node->data.call_expr.function_name, node->data.call_expr.argument_count);
}

void multiarch_codegen_if_stmt(MultiArchCodegen *codegen, struct ASTNode *node) {
    if (node->type != AST_IF_STATEMENT) return;

    char *else_label = multiarch_new_label(codegen);
    char *end_label = multiarch_new_label(codegen);

    // Generate condition
    multiarch_codegen_expression(codegen, node->data.if_stmt.condition);

    // Test result and jump to else if false
    const char *return_reg = multiarch_get_return_reg(codegen);
    switch (codegen->target->arch) {
        case ARCH_X86_64:
            multiarch_emit(codegen, "    testq %%%s, %%%s", return_reg, return_reg);
            break;
        case ARCH_ARM64:
            multiarch_emit(codegen, "    cmp %s, #0", return_reg);
            break;
        default:
            break;
    }

    multiarch_jump_if_zero(codegen, else_label);

    // Generate then statement
    multiarch_codegen_statement(codegen, node->data.if_stmt.then_stmt);
    multiarch_jump(codegen, end_label);

    // Generate else statement
    multiarch_emit_label(codegen, else_label);
    if (node->data.if_stmt.else_stmt) {
        multiarch_codegen_statement(codegen, node->data.if_stmt.else_stmt);
    }

    multiarch_emit_label(codegen, end_label);

    free(else_label);
    free(end_label);
}

void multiarch_codegen_while_stmt(MultiArchCodegen *codegen, struct ASTNode *node) {
    if (node->type != AST_WHILE_STATEMENT) return;

    char *loop_label = multiarch_new_label(codegen);
    char *end_label = multiarch_new_label(codegen);

    multiarch_emit_label(codegen, loop_label);

    // Generate condition
    multiarch_codegen_expression(codegen, node->data.while_stmt.condition);

    // Test and jump if false
    const char *return_reg = multiarch_get_return_reg(codegen);
    switch (codegen->target->arch) {
        case ARCH_X86_64:
            multiarch_emit(codegen, "    testq %%%s, %%%s", return_reg, return_reg);
            break;
        case ARCH_ARM64:
            multiarch_emit(codegen, "    cmp %s, #0", return_reg);
            break;
        default:
            break;
    }

    multiarch_jump_if_zero(codegen, end_label);

    // Generate body
    multiarch_codegen_statement(codegen, node->data.while_stmt.body);
    multiarch_jump(codegen, loop_label);

    multiarch_emit_label(codegen, end_label);

    free(loop_label);
    free(end_label);
}

void multiarch_stack_alloc(MultiArchCodegen *codegen, int bytes) {
    // Align to stack alignment
    bytes = (bytes + codegen->target->stack_alignment - 1) & ~(codegen->target->stack_alignment - 1);

    switch (codegen->target->arch) {
        case ARCH_X86_64:
            multiarch_emit(codegen, "    subq $%d, %%rsp", bytes);
            break;
        case ARCH_ARM64:
            multiarch_emit(codegen, "    sub sp, sp, #%d", bytes);
            break;
        default:
            break;
    }
}

void multiarch_stack_dealloc(MultiArchCodegen *codegen, int bytes) {
    // Align to stack alignment
    bytes = (bytes + codegen->target->stack_alignment - 1) & ~(codegen->target->stack_alignment - 1);

    switch (codegen->target->arch) {
        case ARCH_X86_64:
            multiarch_emit(codegen, "    addq $%d, %%rsp", bytes);
            break;
        case ARCH_ARM64:
            multiarch_emit(codegen, "    add sp, sp, #%d", bytes);
            break;
        default:
            break;
}


}
void multiarch_codegen_switch_stmt(MultiArchCodegen *codegen, struct ASTNode *node) {
    if (node->type != AST_SWITCH_STATEMENT) return;

    // Generate switch expression
    multiarch_codegen_expression(codegen, node->data.switch_stmt.expression);
    const char *switch_reg = multiarch_get_return_reg(codegen);

    // Create labels
    char *end_label = multiarch_new_label(codegen);
    char **case_labels = malloc(sizeof(char*) * node->data.switch_stmt.case_count);

    // Generate labels for each case
    for (int i = 0; i < node->data.switch_stmt.case_count; i++) {
        case_labels[i] = multiarch_new_label(codegen);
    }

    // Generate comparison code for each case
    for (int i = 0; i < node->data.switch_stmt.case_count; i++) {
        ASTNode *case_node = node->data.switch_stmt.cases[i];

        if (!case_node->data.case_stmt.is_default) {
            // Compare switch value with case value
            multiarch_codegen_expression(codegen, case_node->data.case_stmt.value);
            const char *case_reg = multiarch_get_temp_reg(codegen, 1);

            switch (codegen->target->arch) {
                case ARCH_X86_64:
                    multiarch_emit(codegen, "    movq %%%s, %%%s",
                                 multiarch_get_return_reg(codegen), case_reg);
                    multiarch_emit(codegen, "    cmpq %%%s, %%%s", case_reg, switch_reg);
                    break;
                case ARCH_ARM64:
                    multiarch_emit(codegen, "    mov %s, %s",
                                 case_reg, multiarch_get_return_reg(codegen));
                    multiarch_emit(codegen, "    cmp %s, %s", switch_reg, case_reg);
                    break;
                default:
                    break;
            }

            multiarch_jump_if_equal(codegen, case_labels[i]);
        }
    }

    // Jump to default case if present, otherwise jump to end
    int default_index = -1;
    for (int i = 0; i < node->data.switch_stmt.case_count; i++) {
        if (node->data.switch_stmt.cases[i]->data.case_stmt.is_default) {
            default_index = i;
            break;
        }
    }

    if (default_index >= 0) {
        multiarch_jump(codegen, case_labels[default_index]);
    } else {
        multiarch_jump(codegen, end_label);
    }

    // Generate code for each case
    for (int i = 0; i < node->data.switch_stmt.case_count; i++) {
        ASTNode *case_node = node->data.switch_stmt.cases[i];

        multiarch_emit_label(codegen, case_labels[i]);

        // Generate statements for this case
        for (int j = 0; j < case_node->data.case_stmt.statement_count; j++) {
            multiarch_codegen_statement(codegen, case_node->data.case_stmt.statements[j]);
        }
    }

    multiarch_emit_label(codegen, end_label);

    // Clean up
    for (int i = 0; i < node->data.switch_stmt.case_count; i++) {
        free(case_labels[i]);
    }
    free(case_labels);
    free(end_label);
}

void multiarch_codegen_call_expr(MultiArchCodegen *codegen, struct ASTNode *node) {
    if (node->type != AST_FUNCTION_CALL) return;

    const char *func_name = node->data.call_expr.function_name;
    int arg_count = node->data.call_expr.argument_count;

    // Check if this is a built-in function
    bool is_builtin = is_builtin_function(func_name);

    if (is_builtin) {
        multiarch_emit_comment(codegen, "Call to standard library function");
        char comment[128];
        snprintf(comment, sizeof(comment), "Function: %s", func_name);
        multiarch_emit_comment(codegen, comment);
    }

    // Generate arguments and place in parameter registers
    for (int i = 0; i < arg_count && i < codegen->target->num_param_regs; i++) {
        multiarch_codegen_expression(codegen, node->data.call_expr.arguments[i]);
        const char *param_reg = multiarch_get_param_reg(codegen, i);
        const char *return_reg = multiarch_get_return_reg(codegen);

        if (strcmp(param_reg, return_reg) != 0) {
            // Move result to parameter register
            switch (codegen->target->arch) {
            case ARCH_X86_64:
                multiarch_emit(codegen, "    movq %%%s, %%%s", return_reg, param_reg);
                break;
            case ARCH_ARM64:
                multiarch_emit(codegen, "    mov %s, %s", param_reg, return_reg);
                break;
            default:
                break;
            }
        }
    }

    // For macOS, prepend underscore to external symbols
    char mangled_name[256];
    if (codegen->target->platform == PLATFORM_MACOS && is_builtin) {
        snprintf(mangled_name, sizeof(mangled_name), "_%s", func_name);
    } else {
        snprintf(mangled_name, sizeof(mangled_name), "%s", func_name);
    }

    multiarch_function_call(codegen, mangled_name, arg_count);
}
