// ============================================================================
// saturn_sdk/src/smpc.c - SMPC Implementation
// ============================================================================
#include "saturn_smpc.h"

void smpc_init(void) {
    // Initialize SMPC
    SMPC_SF = 0x01;  // Clear status flag
}

void smpc_wait_till_ready(void) {
    while (SMPC_SF & 0x01);
}

void smpc_issue_command(uint8_t cmd) {
    smpc_wait_till_ready();
    SMPC_COMREG = cmd;
}

void smpc_master_on(void) {
    smpc_issue_command(SMPC_CMD_MSHON);
}

void smpc_slave_on(void) {
    smpc_issue_command(SMPC_CMD_SSHON);
}

void smpc_slave_off(void) {
    smpc_issue_command(SMPC_CMD_SSHOFF);
}