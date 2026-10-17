/* =====================================================================
   Chapter 10 — Timers, Timing and PWM
   Extra code examples: delay_ms() implementations, non-blocking
   scheduling, and PWM duty-cycle helpers.

   Target: generic Cortex-M (register names follow the STM32-style
   CMSIS conventions used in the slides). Adjust RCC bit positions
   and clock values to match your actual part.
   ===================================================================== */

#include <stdint.h>

/* ---------------------------------------------------------------------
   1. THE BAD delay_ms() — from slide 7, kept here for comparison
   --------------------------------------------------------------------- */
void delay_ms_bad(int n)
{
    for (volatile int i = 0; i < n * 4000; i++)
        ;
    /* Wrong in three ways: drifts with optimisation level and clock
       frequency, burns 100% CPU, and any ISR that fires makes it
       longer than requested. Do not use this in real designs. */
}

/* ---------------------------------------------------------------------
   2. delay_ms() BUILT ON SysTick — blocking, but accurate
   --------------------------------------------------------------------- */
static volatile uint32_t systick_ms = 0;

void SysTick_Handler(void)
{
    systick_ms++;
}

void systick_init(uint32_t core_clock_hz)
{
    SysTick_Config(core_clock_hz / 1000U);   /* 1 ms tick */
}

uint32_t millis(void)
{
    return systick_ms;
}

/* Blocking, but exact regardless of compiler optimisation: it counts
   real elapsed ticks rather than looping a fixed number of times. */
void delay_ms(uint32_t n)
{
    uint32_t start = millis();
    while ((millis() - start) < n)
    {
        __asm volatile ("wfi");   /* sleep the CPU between ticks */
    }
}

/* ---------------------------------------------------------------------
   3. delay_ms() BUILT ON A GENERAL-PURPOSE TIMER (TIM2), polled
      Useful when SysTick is already claimed by an RTOS.
   --------------------------------------------------------------------- */
void tim2_delay_init(void)
{
    RCC->APB1ENR |= (1U << 0);   /* TIM2 clock enable */
    TIM2->PSC     = 15999;       /* 16 MHz -> 1 kHz (1 ms tick) */
    TIM2->ARR     = 0xFFFF;      /* free-running, max period    */
    TIM2->CR1    |= (1U << 0);   /* start counting              */
}

void delay_ms_tim2(uint32_t n)
{
    TIM2->CNT = 0;
    while (TIM2->CNT < n)
        ;
}

/* ---------------------------------------------------------------------
   4. NON-BLOCKING DELAY PATTERN (what Chapter 14 builds on)
      Instead of waiting, remember when something last happened and
      poll whether enough time has passed. The CPU stays free to do
      other work every time through the main loop.
   --------------------------------------------------------------------- */
typedef struct {
    uint32_t last_ms;
    uint32_t period_ms;
} periodic_task_t;

int task_due(periodic_task_t *t)
{
    if ((millis() - t->last_ms) >= t->period_ms)
    {
        t->last_ms += t->period_ms;   /* schedule next, no drift */
        return 1;
    }
    return 0;
}

/* Example: blink an LED every 500 ms without ever blocking */
void non_blocking_blink_demo(void)
{
    static periodic_task_t blink = { .last_ms = 0, .period_ms = 500 };

    for (;;)
    {
        if (task_due(&blink))
        {
            GPIOA->ODR ^= (1U << 5);
        }
        /* ... other work happens here every loop iteration ... */
    }
}

/* ---------------------------------------------------------------------
   5. PWM DUTY-CYCLE HELPER
      Builds on slide 18's PWM configuration. ARR is fixed once (sets
      frequency); this function only touches CCR1 (sets duty), which
      is glitch-free because of the preload register.
   --------------------------------------------------------------------- */
void pwm_init_tim2_ch1(uint32_t psc, uint32_t arr)
{
    RCC->APB1ENR |= (1U << 0);        /* TIM2 clock */

    TIM2->PSC = psc;
    TIM2->ARR = arr;
    TIM2->CCR1 = 0;                   /* start at 0% duty */

    TIM2->CCMR1 |= (6U << 4);         /* PWM mode 1 on channel 1 */
    TIM2->CCMR1 |= (1U << 3);         /* preload enable          */
    TIM2->CCER  |= (1U << 0);         /* enable the output       */
    TIM2->CR1   |= (1U << 0);         /* start                   */
}

/* percent: 0-100 */
void pwm_set_duty_percent(uint8_t percent)
{
    if (percent > 100) percent = 100;
    uint32_t arr = TIM2->ARR;
    TIM2->CCR1 = ((uint32_t)percent * (arr + 1)) / 100U;
}

/* Example: breathe an LED from 0% to 100% and back, non-blocking */
void led_breathe_demo(void)
{
    static periodic_task_t step = { .last_ms = 0, .period_ms = 20 };
    static int duty = 0;
    static int dir  = 1;

    if (task_due(&step))
    {
        duty += dir;
        if (duty >= 100 || duty <= 0) dir = -dir;
        pwm_set_duty_percent((uint8_t)duty);
    }
}

/* ---------------------------------------------------------------------
   6. DESIGN-CHALLENGE SKELETON (slide 21)
      One hardware time base (SysTick at 1 ms) scheduling three
      periodic tasks: sensor @ 10 ms, display @ 100 ms, log @ 1 s.
      Each task's "last_ms" advances by its own period, so a single
      interrupt source drives all three schedules without drift.
   --------------------------------------------------------------------- */
static periodic_task_t sensor_task  = { .last_ms = 0, .period_ms = 10   };
static periodic_task_t display_task = { .last_ms = 0, .period_ms = 100 };
static periodic_task_t log_task     = { .last_ms = 0, .period_ms = 1000};

void read_sensor(void)   { /* ... */ }
void update_display(void){ /* ... */ }
void write_log(void)     { /* ... */ }

void scheduler_loop(void)
{
    for (;;)
    {
        if (task_due(&sensor_task))  read_sensor();
        if (task_due(&display_task)) update_display();
        if (task_due(&log_task))     write_log();
    }
}
