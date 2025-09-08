#include <stdint.h>
#include <stddef.h>
#include "Clock.h"
#include "Crc.h"
#include "Nvic.h"

/* Chọn một IRQ hợp lệ của STM32F1 để test bit NVIC (ví dụ TIM2 = 28) */
#define TEST_IRQ_NUM 28u

/* Optional: redirect printf() ra SWO ITM (bật SWV trong IDE trước khi dùng) */
int _write(int fd, const char *ptr, int len) {
    (void)fd;
    /* ITM_SendChar là intrinsic do CMSIS cung cấp (core_cm3.h), đảm bảo IDE bật SWV ITM Port 0 */
    for (int i = 0; i < len; ++i) {
        /* Nếu chưa có ITM_SendChar, có thể bỏ toàn bộ _write và không dùng printf */
        extern int ITM_SendChar (int ch);
        ITM_SendChar(ptr[i]);
    }
    return len;
}

/* Dummy delay nhỏ để dễ quan sát trong debugger */
static void delay(volatile uint32_t t) {
    while (t--) __asm__("nop");
}

/* Tùy chọn: dummy handler nếu vector table đã map đúng TIM2_IRQHandler */
__attribute__((weak)) void TIM2_IRQHandler(void) {
    /* Chỉ chạy qua rồi thoát để có thể thấy IABR thay đổi khi vào ISR */
    for (volatile int i = 0; i < 1000; ++i) { __asm__("nop"); }
}

int main(void) {
    /* 1) Khởi tạo clock theo driver (HSI làm SYSCLK như code hiện có) */
    clock_init(); /* Có thể để nguyên HSI cho đơn giản khi smoke test */ /*[21]*/

    /* 2) Test CRC: enable clock + reset + ghi dữ liệu vào DR và đọc kết quả */
    CRC_Init(); /* Enable RCC AHB CRC + reset unit theo driver */ /*[16]*/
    static const uint32_t test_data[] = {
        0x12345678u, 0xAAAAAAAAu, 0x00000000u, 0xFFFFFFFFu, 0xDEADBEEFu
    };
    uint32_t crc_val = CRC_Calculate(test_data, sizeof(test_data)/sizeof(test_data)); /*[16]*/

    /* 3) Test NVIC: set priority (4-bit high nibble), enable, set pending, đọc active, rồi clear */
    NVIC_SetPriority(TEST_IRQ_NUM, 5u);        /* Ưu tiên mức 5 (trên F1 có 16 mức ưu tiên) */ /*[21][18]*/
    NVIC_EnableInterrupt(TEST_IRQ_NUM);        /* ISER */ /*[21]*/
    NVIC_SetPending(TEST_IRQ_NUM);             /* ISPR */ /*[21]*/
    delay(50000);
    uint8_t is_active = NVIC_GetActive(TEST_IRQ_NUM);  /* IABR */ /*[21]*/
    uint8_t prio_read = NVIC_GetPriority(TEST_IRQ_NUM);/* IPR */ /*[21]*/
    NVIC_ClearPending(TEST_IRQ_NUM);           /* ICPR */ /*[21]*/
    NVIC_DisableInterrupt(TEST_IRQ_NUM);       /* ICER */ /*[21]*/

    /* 4) Quan sát qua debugger: đặt breakpoint, xem biến crc_val, is_active, prio_read
          Hoặc in ra SWO nếu đã enable SWV ITM trong IDE */
    /* Lưu ý: Muốn thấy printf, cần bật SWV ITM và port 0 trong Debug Configuration của IDE */
    /* Ví dụ: printf("CRC=0x%08lX, active=%u, prio=%u\n", crc_val, is_active, prio_read); */

    while (1) {
        /* Có thể in định kỳ nếu đã bật SWO/semihosting */
        /* printf("CRC=0x%08lX, active=%u, prio=%u\n", crc_val, is_active, prio_read); */ /*[13]*/
        delay(100000);
    }
}
