#ifndef BBB_GPIOLIB_H
#define BBB_GPIOLIB_H

// Enum com todos os códigos de retorno possíveis da biblioteca.
typedef enum {
    RC_OK = 0,                  // Operação bem-sucedida
    RC_ERROR_GENERIC = -1,        // Erro genérico/inesperado
    RC_ERROR_PIN_NOT_FOUND = -2,  // Pino não encontrado na tabela de mapa
    RC_ERROR_NO_GPIO_CAP = -3,  // Pino não tem capacidade de GPIO
    RC_ERROR_NO_PWM_CAP = -4,   // Pino não tem capacidade de PWM
    RC_ERROR_MALLOC_FAILED = -5,  // Falha ao alocar memória
    RC_ERROR_LIBGPIOD = -6,       // Erro interno da libgpiod
    RC_ERROR_SYSFS = -7,          // Falha genérica ao acessar o sysfs
    RC_ERROR_PIN_MODE = -8,       // Falha ao tentar definir o modo do pino (ex: pwm)
    RC_ERROR_EXPORT = -9        // Falha ao exportar um pino/canal
} BBLIB_RC;

// --- Definições Públicas ---
#define OUTPUT 1
#define INPUT  0
#define HIGH   1
#define LOW    0

// Ponteiros opacos para as estruturas internas.
typedef struct GpioPin GpioPin;
typedef struct PwmPin  PwmPin;

// --- Funções GPIO ---

/**
 * @brief Configura um pino para operar como GPIO (entrada ou saída).
 * @param name O nome do pino (ex: "P8_07").
 * @param mode O modo de operação (OUTPUT ou INPUT).
 * @param pin_handle Ponteiro para o ponteiro do pino. Será preenchido com o handle em caso de sucesso.
 * @return Um código de erro BBLIB_RC. RC_OK em caso de sucesso.
 */
int pinMode(const char* name, int mode, GpioPin** pin_handle);
void digitalWrite(GpioPin* pin, int value);
int digitalRead(GpioPin* pin);
void releasePin(GpioPin* pin);

// --- Funções PWM ---

/**
 * @brief Prepara e configura um pino para operar como PWM.
 * @param name O nome do pino (ex: "P9_14").
 * @param pin_handle Ponteiro para o ponteiro do pino. Será preenchido com o handle em caso de sucesso.
 * @return Um código de erro BBLIB_RC. RC_OK em caso de sucesso.
 */
int pwm_setup(const char* name, PwmPin** pin_handle);
int pwm_config(PwmPin* pin, unsigned long period_ns, unsigned long duty_cycle_ns);
int pwm_set_polarity(PwmPin* pin, const char* polarity);
int pwm_enable(PwmPin* pin);
int pwm_disable(PwmPin* pin);
void pwm_release(PwmPin* pin);
int get_pin_mode(const char* pin_name, char* mode_buffer, int buffer_size);

#endif // BBB_GPIOLIB_H
