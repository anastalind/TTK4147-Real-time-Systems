#include <asf.h>
#include <board.h>
#include <gpio.h>
#include <sysclk.h>
#include "busy_delay.h"

#define CONFIG_USART_IF (AVR32_USART2)

// defines for BRTT interface
#define TEST_A      AVR32_PIN_PA31
#define RESPONSE_A  AVR32_PIN_PA30
#define TEST_B      AVR32_PIN_PA29
#define RESPONSE_B  AVR32_PIN_PA28
#define TEST_C      AVR32_PIN_PA27
#define RESPONSE_C  AVR32_PIN_PB00

uint8_t flag_A;
uint8_t flag_B;
uint8_t flag_C;

__attribute__((__interrupt__)) static void interrupt_J3(void);

void test_pins_init(){
    gpio_configure_pin(TEST_A, (GPIO_DIR_INPUT | GPIO_PULL_UP));
	gpio_configure_pin(TEST_B, (GPIO_DIR_INPUT | GPIO_PULL_UP));
	gpio_configure_pin(TEST_C, (GPIO_DIR_INPUT | GPIO_PULL_UP));
	
	gpio_configure_pin(RESPONSE_A, (GPIO_DIR_OUTPUT | GPIO_INIT_HIGH));
	gpio_configure_pin(RESPONSE_B, (GPIO_DIR_OUTPUT | GPIO_INIT_HIGH));
	gpio_configure_pin(RESPONSE_C, (GPIO_DIR_OUTPUT | GPIO_INIT_HIGH));
}

void interrupt_pins_init(){
	gpio_enable_pin_interrupt(TEST_A, GPIO_FALLING_EDGE);
	gpio_enable_pin_interrupt(TEST_B, GPIO_FALLING_EDGE);
	gpio_enable_pin_interrupt(TEST_C, GPIO_FALLING_EDGE);
	
	flag_A = 0;
	flag_B = 0;
	flag_C = 0;
}

void init(){
    sysclk_init();
    board_init();
	
	test_pins_init();
	interrupt_pins_init();
	
    busy_delay_init(BOARD_OSC0_HZ);
    
    cpu_irq_disable();
    INTC_init_interrupts();
    INTC_register_interrupt(&interrupt_J3, AVR32_GPIO_IRQ_3, AVR32_INTC_INT1);
    cpu_irq_enable();
    
    stdio_usb_init(&CONFIG_USART_IF);

    #if defined(__GNUC__) && defined(__AVR32__)
        setbuf(stdout, NULL);
        setbuf(stdin,  NULL);
    #endif
}


__attribute__((__interrupt__)) static void interrupt_J3(void){ 
	if (gpio_get_pin_interrupt_flag(TEST_A)){
		flag_A = 1;
		gpio_clear_pin_interrupt_flag(TEST_A);
	}
	
	if (gpio_get_pin_interrupt_flag(TEST_B)){
		flag_B = 1;
		gpio_clear_pin_interrupt_flag(TEST_B);
	}
	
	if (gpio_get_pin_interrupt_flag(TEST_C)){
		flag_C = 1;
		gpio_clear_pin_interrupt_flag(TEST_C);
	}

}

/*
__attribute__((__interrupt__)) static void interrupt_J3(void){
	if (gpio_get_pin_interrupt_flag(TEST_A)){
		gpio_clear_pin_interrupt_flag(TEST_A);
		
		gpio_set_pin_low(RESPONSE_A);
		
		busy_delay_us(5);
		
		gpio_set_pin_high(RESPONSE_A);
	}
	
	if (gpio_get_pin_interrupt_flag(TEST_B)){
		gpio_clear_pin_interrupt_flag(TEST_B);
		
		busy_delay_us(100);
		
		gpio_set_pin_low(RESPONSE_B);
		
		busy_delay_us(5);
		
		gpio_set_pin_high(RESPONSE_B);
	}
	
	if (gpio_get_pin_interrupt_flag(TEST_C)){
		gpio_clear_pin_interrupt_flag(TEST_C);
		
		gpio_set_pin_low(RESPONSE_C);
		
		busy_delay_us(5);
		
		gpio_set_pin_high(RESPONSE_C);
	}

}
*/

void busy_wait_A() {
	while(1){
		while(gpio_get_pin_value(TEST_A) != 0) {};
			
		gpio_set_pin_low(RESPONSE_A);
		
		busy_delay_us(5);
		
		gpio_set_pin_high(RESPONSE_A);
	}
}

void busy_wait_ABC() {
	while(1){
		while((gpio_get_pin_value(TEST_A) != 0) && (gpio_get_pin_value(TEST_B) != 0) && (gpio_get_pin_value(TEST_C) != 0)) {};
			
		gpio_set_pin_low(RESPONSE_A);
		gpio_set_pin_low(RESPONSE_B);
		gpio_set_pin_low(RESPONSE_C);
		
		busy_delay_us(5);
		
		gpio_set_pin_high(RESPONSE_A);
		gpio_set_pin_high(RESPONSE_B);
		gpio_set_pin_high(RESPONSE_C);
	}
}

void interrupt_big_while(){
	if (flag_A){
		flag_A = 0;		
		gpio_set_pin_low(RESPONSE_A);
		
		busy_delay_us(5);
		
		gpio_set_pin_high(RESPONSE_A);
	}
	
	if (flag_B){
		flag_B = 0;
		busy_delay_us(100);
		
		gpio_set_pin_low(RESPONSE_B);
		
		busy_delay_us(5);
		
		gpio_set_pin_high(RESPONSE_B);
	}
	
	if (flag_C){
		flag_C = 0;
		gpio_set_pin_low(RESPONSE_C);
		
		busy_delay_us(5);
		
		gpio_set_pin_high(RESPONSE_C);
	}
}

int main (void){
    init();
	
	/* Testing the AVR32 */
	/*
	while(1){
		gpio_toggle_pin(LED0_GPIO);

		printf("tick\n");
		
		busy_delay_ms(500);
		
	}
	*/
	
	/* TASK A - Test of type A */
	//busy_wait_A();
	
	/* TASK B - Test of type A+B+C */
	//busy_wait_ABC();
	
	/* TASK C - Interrupt handler */
	//while(1){};
		
	/* TASK D - Deferred interrupt handler*/
	while(1){
		interrupt_big_while();	
	}
	
}
