#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstdint>

#define GPIO_BASE_ADDR 0x20200000
#define GPIO_BLOCK_SIZE 0x4096

#define LED_PIN 26
#define BTN_PIN 17

#define GPFSEL0 0
#define GPSET0 7
#define GPCLR0 10

volatile uint32_t *gpio_map = nullptr;

void gpio_init()
{
	int mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (mem_fd < 0) {
		perror("open");
		exit(1);
	}
	
	void *gpio_base = mmap(nullptr, GPIO_BLOCK_SIZE,
		PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd,
		GPIO_BASE_ADDR);
	if (gpio_base == MAP_FAILED) {
		perror("mmap");
		close(mem_fd);
		exit(1);
	}

	gpio_map = (volatile uint32_t *) gpio_base;
	close(mem_fd);
}

void gpio_set_direction(int pin, int direction)
{
	int reg = pin / 10;
	int shift = (pin % 10) * 3;
        gpio_map[reg] &= ~(7 << shift);

	if (direction) {
		gpio_map[reg] |= (1 << shift);
	}
}

void gpio_set(int pin)
{
	gpio_map[GPSET0 + (pin / 32)] = (1 << (pin % 32));
}

void gpio_clear(int pin)
{
	gpio_map[GPCLR0 + (pin / 32)] = (1 << (pin % 32));
}

bool gpio_get_state(int pin)
{
	int reg = 13 + (pin / 32);
	unsigned int level = gpio_map[reg];
	bool state = (level & (1 << pin)) ? true : false;
	return state;
}

int main()
{
	gpio_init();
	gpio_set_direction(LED_PIN, 1);
	gpio_set_direction(BTN_PIN, 0);

	while(true) {

		const bool btn_state = gpio_get_state(BTN_PIN);

		if (btn_state) {
			gpio_set(LED_PIN);
		} else {
			gpio_clear(LED_PIN);
		}
	
		usleep(100000);
	}
	
	return 0;
}




