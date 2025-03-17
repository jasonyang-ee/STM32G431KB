# LED


LED is the 2nd debugging interface. Running it in breathing / blink / rapid mode will help you to determine if system is at fault.

And it is fun to stare at LED anyway.

## Machnism

LED is controlled by PWM. PWM is controlled by `period` and `duty cycle`. `period` is the time it takes to complete one cycle. `duty cycle` is the time it takes to complete one cycle in high state.

## PWM

- `Prescaler` x `Overflow` = `Clock Source` = `System Clock` / `Desired PWM Frequency`

- If wanting period be 100 for ease of duty cycle setting:
- 32Mhz / 1KHz / 100 = 320
- => `prescaler`: 320-1, `period`: 100-1, `PWM Frequency`: 1KHz
- If wanting max precision with using max period:
- 32Mhz / 1KHz / (if > 65535) then divide 65535 (16bit period)
- => `prescaler`: 1-1, `period`: 32000-1, `PWM Frequency`: 1KHz
- Use `clock source`: `internal clock (APBx)`

## Interrupt Via PWM OC (output compare) signal

- Enable `PWM global interrupt`
- Run `HAL_TIM_PWM_Start_IT(&htimX, TIM_CHANNEL_X)` in `main()`
- Define `void PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)`
- Run `led_user.scheduler()` in the `PWM_PulseFinishedCallback`

## LED Class
  - Init object `LED led_user{period, dimmer, Interrupt Frequency}`
  - Passthogh channel CCR with `led_user.setCCR(&htimX.Instance->CCRX);`
  - Ready to use `on()`, `off()`, `toggle()`, `set()`, `breath()`, `blink()`, `rapid()`.
