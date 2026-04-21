/* NOTAS
- VÊ BRANCO: output = 0
- VÊ PRETO: output = 1
- É o meio da parte transparente que detecta a linha
- IN1 = motor esquerda para frente
- IN2 = motor esquerda para trás
- IN3 = motor direita para frente
- IN4 = motor direita para trás
- Sensor do pino 20 = esquerda
- Sensor do pino 21 = direita
PARA OTIMIZAR O CÓDIGO
- Usar o FPGIO
- Colocar a parte do loop na SRAM (acesso mais rápido, minuto 12 do video de memoria)
 */

#include <zephyr/kernel.h>             // Funções básicas do Zephyr (ex: k_msleep, k_thread, etc.)
#include <zephyr/device.h>             // A'PI para obter e utilizar dispositivos do sistema
#include <zephyr/drivers/gpio.h>       // API para controle de pinos de entrada/saída (GPIO)
#include <pwm_z42.h>                // Biblioteca personalizada com funções de controle do TPM (Timer/PWM Module)

// Define o valor do regAistrador MOD do TPM para configurar o período do PWM
#define TPM_MODULE 1000         // Define a frequência do PWM fpwm = (TPM_CLK / (TPM_MODULE * PS))
#define SLEEP_TIME_MS 1000
#define INPUT_PORT DT_NODELABEL(gpioa)
#define INPUT_PIN1 1
#define INPUT_PIN2 2

void sensor_interrupt() {
    
}

int main(void)
{
    // Configura os sensores
    const struct device *input_dev;
    int esquerda, direita, ret1, ret2;
	input_dev = DEVICE_DT_GET(INPUT_PORT);
    if (!input_dev) {
        printk("Erro ao acessar porta\n");
        return 1;
    }

    // Configura os sensores e o interrupt
	gpio_pin_configure(input_dev, INPUT_PIN1, GPIO_INPUT);
	gpio_pin_configure(input_dev, INPUT_PIN2, GPIO_INPUT);
    gpio_pin_interrupt_configure(input_dev, INPUT_PIN1, GPIO_INT_EDGE_TO_ACTIVE);
    gpio_pin_interrupt_configure(input_dev, INPUT_PIN2, GPIO_INT_EDGE_TO_ACTIVE);

    // Configura os motores
    pwm_tpm_Init(TPM0, TPM_PLLFLL, TPM_MODULE, TPM_CLK, PS_8, CENTER_PWM);
    pwm_tpm_Ch_Init(TPM0, 3, TPM_PWM_H,GPIOD,3);
    pwm_tpm_Ch_Init(TPM0, 2, TPM_PWM_H,GPIOD,2);
    pwm_tpm_Ch_Init(TPM0, 0, TPM_PWM_H,GPIOD,0);
    pwm_tpm_Ch_Init(TPM0, 5, TPM_PWM_H,GPIOD,5);

    while (1) {
        esquerda = gpio_pin_get(input_dev, INPUT_PIN1);
		direita = gpio_pin_get(input_dev, INPUT_PIN2);

        if (esquerda == 0 && direita == 0) {
            pwm_tpm_CnV(TPM0, 3, 800);
            pwm_tpm_CnV(TPM0, 2, 0);
            pwm_tpm_CnV(TPM0, 0, 800);
            pwm_tpm_CnV(TPM0, 5, 0);
        }
        else if (esquerda == 1 && direita == 0) {
            pwm_tpm_CnV(TPM0, 3, 0);
            pwm_tpm_CnV(TPM0, 2, 0);
            pwm_tpm_CnV(TPM0, 0, 1000);
            pwm_tpm_CnV(TPM0, 5, 0);
        }
        else if (esquerda == 0 && direita == 1) {
            pwm_tpm_CnV(TPM0, 3, 1000);
            pwm_tpm_CnV(TPM0, 2, 0);
            pwm_tpm_CnV(TPM0, 0, 0);
            pwm_tpm_CnV(TPM0, 5, 0);

        }
        else if (esquerda == 1 && direita == 1) {
            pwm_tpm_CnV(TPM0, 3, 800);
            pwm_tpm_CnV(TPM0, 2, 0);
            pwm_tpm_CnV(TPM0, 0, 800);
            pwm_tpm_CnV(TPM0, 5, 0);
        }
    }
}