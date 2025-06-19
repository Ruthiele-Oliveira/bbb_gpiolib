#include "bbb_pin_map.h"
#include <stddef.h> // Para a macro NULL

/*
 * Tabela Mestra de Pinos da BeagleBone Black.
 * A numeração de PWM (pwm_chip) foi corrigida com base na depuração
 * do sistema do usuário (Kernel 5.10.x).
 */
const BbbPin bbb_pin_map[] = {
    // Pino     | GPIO (chip, linha) | PWM (chip, canal, overlay)
    //---------------------------------------------------------------------------------
    // LEDs da Placa
    { "USR0",      1, 21,   -1, -1, NULL           },
    { "USR1",      1, 22,   -1, -1, NULL           },
    { "USR2",      1, 23,   -1, -1, NULL           },
    { "USR3",      1, 24,   -1, -1, NULL           },

    // Header P8
    { "P8_07",     2,  2,   -1, -1, NULL           },
    { "P8_08",     2,  3,   -1, -1, NULL           },
    { "P8_09",     2,  5,   -1, -1, NULL           },
    { "P8_10",     2,  4,   -1, -1, NULL           },
    { "P8_11",     1, 13,   -1, -1, NULL           },
    { "P8_12",     1, 12,   -1, -1, NULL           },
    { "P8_13",     0, 23,    7,  1, "BB-PWM2"      }, // GPIO 23 | eHRPWM2B
    { "P8_14",     0, 26,   -1, -1, NULL           },
    { "P8_15",     1, 15,   -1, -1, NULL           },
    { "P8_16",     1, 14,   -1, -1, NULL           },
    { "P8_17",     0, 27,   -1, -1, NULL           },
    { "P8_18",     2,  1,   -1, -1, NULL           },
    { "P8_19",     0, 22,    7,  0, "BB-PWM2"      }, // GPIO 22 | eHRPWM2A
    { "P8_26",     1, 29,   -1, -1, NULL           },

    // Header P9
    { "P9_11",     0, 30,   -1, -1, NULL           },
    { "P9_12",     1, 28,   -1, -1, NULL           },
    { "P9_13",     0, 31,   -1, -1, NULL           },
    { "P9_14",     1, 18,    5,  0, "BB-PWM1"      }, // GPIO 50 | eHRPWM1A
    { "P9_15",     1, 16,   -1, -1, NULL           },
    { "P9_16",     1, 19,    5,  1, "BB-PWM1"      }, // GPIO 51 | eHRPWM1B
    { "P9_17",     0,  5,   -1, -1, NULL           },
    { "P9_18",     0,  4,   -1, -1, NULL           },
    { "P9_21",     0,  3,    3,  1, "BB-PWM0"      }, // GPIO 3  | eHRPWM0B
    { "P9_22",     0,  2,    3,  0, "BB-PWM0"      }, // GPIO 2  | eHRPWM0A
    { "P9_23",     1, 17,   -1, -1, NULL           },
    { "P9_24",     0, 15,   -1, -1, NULL           },
    { "P9_26",     0, 14,   -1, -1, NULL           },
    { "P9_42",     0,  7,    0,  0, "BB-ECAP-PWM0" }, // GPIO 7  | ECAP0_PWM
};
const int bbb_pin_map_size = sizeof(bbb_pin_map) / sizeof(BbbPin);
