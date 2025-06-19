#ifndef BBB_PIN_MAP_H
#define BBB_PIN_MAP_H

// Estrutura Mestra que descreve as capacidades de um pino.
typedef struct {
    const char* name;
    int gpio_chip;
    int gpio_line;
    int pwm_chip;
    int pwm_channel;
    const char* pwm_overlay;
} BbbPin;

// Declaração da tabela e seu tamanho para outros arquivos usarem.
extern const BbbPin bbb_pin_map[];
extern const int bbb_pin_map_size;

#endif // BBB_PIN_MAP_H
