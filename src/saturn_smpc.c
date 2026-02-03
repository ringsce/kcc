// ============================================================================
// src/saturn_smpc.c - Saturn SMPC Implementation
// ============================================================================
#include "saturn_smpc.h"

void smpc_init(void) {
    // Wait for SMPC to be ready
    smpc_wait_ready();
}

void smpc_wait_ready(void) {
    // Wait until SF flag is clear
    while (SMPC_SR & SMPC_SR_SF);
}

void smpc_command(uint8_t cmd) {
    smpc_wait_ready();
    SMPC_COMREG = cmd;
}

void smpc_master_on(void) {
    smpc_command(SMPC_CMD_MSHON);
}

void smpc_slave_on(void) {
    smpc_command(SMPC_CMD_SSHON);
}

void smpc_slave_off(void) {
    smpc_command(SMPC_CMD_SSHOFF);
}

void smpc_sound_on(void) {
    smpc_command(SMPC_CMD_SNDON);
}

void smpc_sound_off(void) {
    smpc_command(SMPC_CMD_SNDOFF);
}

void smpc_cd_on(void) {
    smpc_command(SMPC_CMD_CDON);
}

void smpc_cd_off(void) {
    smpc_command(SMPC_CMD_CDOFF);
}

void smpc_intback(void) {
    smpc_command(SMPC_CMD_INTBACK);
}

void smpc_get_peripheral_data(uint8_t *data, int length) {
    smpc_wait_ready();
    for (int i = 0; i < length && i < 32; i++) {
        data[i] = SMPC_OREG(i);
    }
}

void smpc_get_time(SmpcDateTime *dt) {
    if (!dt) return;

    smpc_wait_ready();

    // Read time from output registers after appropriate command
    dt->year = SMPC_OREG(0);
    dt->month = SMPC_OREG(1);
    dt->day = SMPC_OREG(2);
    dt->hour = SMPC_OREG(3);
    dt->minute = SMPC_OREG(4);
    dt->second = SMPC_OREG(5);
    dt->day_of_week = SMPC_OREG(6);
}

void smpc_set_time(const SmpcDateTime *dt) {
    if (!dt) return;

    smpc_wait_ready();

    SMPC_IREG(0) = dt->year;
    SMPC_IREG(1) = dt->month;
    SMPC_IREG(2) = dt->day;
    SMPC_IREG(3) = dt->hour;
    SMPC_IREG(4) = dt->minute;
    SMPC_IREG(5) = dt->second;
    SMPC_IREG(6) = dt->day_of_week;

    smpc_command(SMPC_CMD_SETTIME);
}

void smpc_reset_enable(void) {
    smpc_command(SMPC_CMD_RESENAB);
}

void smpc_reset_disable(void) {
    smpc_command(SMPC_CMD_RESDISA);
}

void smpc_nmi_request(void) {
    smpc_command(SMPC_CMD_NMIREQ);
}
