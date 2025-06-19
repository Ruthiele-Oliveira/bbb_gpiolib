#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <gpiod.h>
#include <ctype.h>
#include "bbb_gpiolib.h"
#include "bbb_pin_map.h"

#define CONSUMER "bbb_lib"

// --- Estruturas Internas ---
struct GpioPin { struct gpiod_chip* chip; struct gpiod_line* line; };
struct PwmPin { int chip; int channel; char period_path[64]; char duty_cycle_path[64]; char enable_path[64]; char polarity_path[64]; };

// --- Funções Auxiliares Internas ---

// Encontra um pino na tabela mestra pelo nome.
static const BbbPin* find_pin_by_name(const char* name) {
    for (int i = 0; i < bbb_pin_map_size; i++) {
        if (strcmp(name, bbb_pin_map[i].name) == 0) {
            return &bbb_pin_map[i];
        }
    }
    return NULL;
}

// Escreve uma string de texto em um arquivo do sysfs.
static int write_to_sysfs(const char* path, const char* value) {
    FILE* fp = fopen(path, "w");
    if (!fp) return RC_ERROR_SYSFS;
    fprintf(fp, "%s", value);
    fclose(fp);
    return RC_OK;
}

// Lê uma string de texto de um arquivo do sysfs.
static int read_from_sysfs(const char* path, char* buffer, int buffer_size) {
    if (buffer_size <= 0) return RC_ERROR_GENERIC;
    buffer[0] = '\0';
    FILE* fp = fopen(path, "r");
    if (!fp) return RC_ERROR_SYSFS;
    if (fgets(buffer, buffer_size, fp) == NULL) { fclose(fp); return RC_ERROR_GENERIC; }
    fclose(fp);
    buffer[strcspn(buffer, "\n")] = '\0';
    return RC_OK;
}

// Define o modo de multiplexação de um pino.
static int set_pin_mode(const char* pin_name, const char* mode) {
    char path[128];
    sprintf(path, "/sys/devices/platform/ocp/ocp:%s_pinmux/state", pin_name);
    return write_to_sysfs(path, mode);
}

//================================= Seção GPIO =================================

int pinMode(const char* name, int mode, GpioPin** pin_handle) {
    const BbbPin* pin_info = find_pin_by_name(name);
    if (!pin_info) return RC_ERROR_PIN_NOT_FOUND;
    if (pin_info->gpio_chip == -1) return RC_ERROR_NO_GPIO_CAP;

    GpioPin* new_pin = malloc(sizeof(GpioPin));
    if (!new_pin) return RC_ERROR_MALLOC_FAILED;

    char chip_name[32];
    sprintf(chip_name, "gpiochip%d", pin_info->gpio_chip);
    new_pin->chip = gpiod_chip_open_by_name(chip_name);
    if (!new_pin->chip) { free(new_pin); return RC_ERROR_LIBGPIOD; }
    
    new_pin->line = gpiod_chip_get_line(new_pin->chip, pin_info->gpio_line);
    if (!new_pin->line) { gpiod_chip_close(new_pin->chip); free(new_pin); return RC_ERROR_LIBGPIOD; }
    
    int ret = (mode == OUTPUT) ? gpiod_line_request_output(new_pin->line, CONSUMER, 0) : gpiod_line_request_input(new_pin->line, CONSUMER);
    if (ret < 0) { gpiod_line_release(new_pin->line); gpiod_chip_close(new_pin->chip); free(new_pin); return RC_ERROR_LIBGPIOD; }
    
    *pin_handle = new_pin;
    return RC_OK;
}

void digitalWrite(GpioPin* pin, int value) {
    if (pin && pin->line) {
        gpiod_line_set_value(pin->line, value);
    }
}

int digitalRead(GpioPin* pin) {
    if (pin && pin->line) {
        return gpiod_line_get_value(pin->line);
    }
    return -1;
}

void releasePin(GpioPin* pin) {
    if (pin) {
        if (pin->line) {
            gpiod_line_set_value(pin->line, 0);
            gpiod_line_release(pin->line);
        }
        if (pin->chip) gpiod_chip_close(pin->chip);
        free(pin);
    }
}

//================================== Seção PWM =================================

int get_pin_mode(const char* pin_name, char* mode_buffer, int buffer_size) {
    char path[128];
    sprintf(path, "/sys/devices/platform/ocp/ocp:%s_pinmux/state", pin_name);
    return read_from_sysfs(path, mode_buffer, buffer_size);
}

int pwm_setup(const char* name, PwmPin** pin_handle) {
    const BbbPin* pin_info = find_pin_by_name(name);
    if (!pin_info) return RC_ERROR_PIN_NOT_FOUND;
    if (pin_info->pwm_chip == -1) return RC_ERROR_NO_PWM_CAP;

    if (set_pin_mode(name, "pwm") != RC_OK) {
        return RC_ERROR_PIN_MODE;
    }
    usleep(100000);

    char path[64], value_str[10];
    sprintf(path, "/sys/class/pwm/pwmchip%d/export", pin_info->pwm_chip);
    sprintf(value_str, "%d", pin_info->pwm_channel);
    if (write_to_sysfs(path, value_str) != RC_OK) return RC_ERROR_EXPORT;
    usleep(100000);

    PwmPin* pin = malloc(sizeof(PwmPin));
    if (!pin) return RC_ERROR_MALLOC_FAILED;
    
    pin->chip = pin_info->pwm_chip;
    pin->channel = pin_info->pwm_channel;
    sprintf(pin->period_path, "/sys/class/pwm/pwmchip%d/pwm%d/period", pin->chip, pin->channel);
    sprintf(pin->duty_cycle_path, "/sys/class/pwm/pwmchip%d/pwm%d/duty_cycle", pin->chip, pin->channel);
    sprintf(pin->enable_path, "/sys/class/pwm/pwmchip%d/pwm%d/enable", pin->chip, pin->channel);
    sprintf(pin->polarity_path, "/sys/class/pwm/pwmchip%d/pwm%d/polarity", pin->chip, pin->channel);

    *pin_handle = pin;
    return RC_OK;
}

int pwm_config(PwmPin* pin, unsigned long period_ns, unsigned long duty_cycle_ns) {
    if (!pin) return RC_ERROR_GENERIC;
    char value_str[32];

    sprintf(value_str, "%lu", 0UL);
    if (write_to_sysfs(pin->duty_cycle_path, value_str) != RC_OK) return RC_ERROR_SYSFS;

    sprintf(value_str, "%lu", period_ns);
    if (write_to_sysfs(pin->period_path, value_str) != RC_OK) return RC_ERROR_SYSFS;

    sprintf(value_str, "%lu", duty_cycle_ns);
    if (write_to_sysfs(pin->duty_cycle_path, value_str) != RC_OK) return RC_ERROR_SYSFS;
    
    return RC_OK;
}

int pwm_set_polarity(PwmPin* pin, const char* polarity) {
    if (!pin) return RC_ERROR_GENERIC;
    return write_to_sysfs(pin->polarity_path, polarity);
}

int pwm_enable(PwmPin* pin) { 
    return (pin) ? write_to_sysfs(pin->enable_path, "1") : RC_ERROR_GENERIC; 
}

int pwm_disable(PwmPin* pin) { 
    return (pin) ? write_to_sysfs(pin->enable_path, "0") : RC_ERROR_GENERIC; 
}

void pwm_release(PwmPin* pin) { 
    if (!pin) return; 
    char path[64], val[10]; 
    sprintf(path, "/sys/class/pwm/pwmchip%d/unexport", pin->chip); 
    sprintf(val, "%d", pin->channel); 
    write_to_sysfs(path, val); 
    free(pin); 
}
