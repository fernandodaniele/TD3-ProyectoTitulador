//Configuración de PWM
#define DUTY_US     50
#define PWM_FREQ    10000

mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, PIN_PASO);    //Set GPIO PIN_PASO as PWM0A, to which servo is connected

mcpwm_config_t pwm_config;
pwm_config.frequency = PWM_FREQ;
pwm_config.cmpr_a = 0;    //duty cycle of PWMxA = 0
pwm_config.cmpr_b = 0;    //duty cycle of PWMxb = 0
pwm_config.counter_mode = MCPWM_UP_COUNTER;
pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);    //Configure PWM0A & PWM0B with above settings

//Para inyectar 0,1 mL tiempoInyectado = 1150
mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, DUTY_US);
vTaskDelay(tiempoInyectado /portTICK_PERIOD_MS);
mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 0);