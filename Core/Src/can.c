#include "can.h"
#include <stdint.h>

#define RCC_BASE            (0x40021000UL)
#define RCC_APB1ENR_OFFSET  (0x1C)
#define RCC_APB1ENR         *(volatile uint32_t *)(RCC_BASE + RCC_APB1ENR_OFFSET)
#define RCC_APB1ENR_CAN1EN_M (1UL << 25)

#define USB_LP_CAN1_RX0_IRQn   20   // IRQ number cho CAN FIFO0 RX
#define CAN1_RX1_IRQn          21   // IRQ number cho CAN FIFO1 RX
#define USB_HP_CAN1_TX_IRQn    19   // IRQ number cho CAN TX

static void CAN_NVIC_Config(void)
{
    // Bật interrupt cho CAN1 RX0 (FIFO0)
    NVIC_EnableInterrupt(USB_LP_CAN1_RX0_IRQn);

    // Set priority (0 = highest, 15 = lowest nếu 4-bit priority)
    NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 2);
}

void CAN_Init(uint8_t mode, uint32_t baud_rate_prescaler) {
    RCC_APB1ENR |= RCC_APB1ENR_CAN1EN_M;

    CAN1->MCR |= (1UL << 0); // Đặt bit INRQ
    while (!((CAN1->MSR >> 0) & 1)); // Đợi INAK = 1 để xác nhận

    CAN1->BTR &= ~(3UL << 30); // Xóa bit SILM và LBKM
    if (mode == CAN_MODE_SILENT) {
        CAN1->BTR |= (1UL << 31);
    } else if (mode == CAN_MODE_LOOPBACK) {
        CAN1->BTR |= (1UL << 30);
    } else if (mode == CAN_MODE_LOOPBACK_SILENT) {
        CAN1->BTR |= (3UL << 30);
    }

    CAN1->BTR &= ~(0x3FFUL); // Xóa 10 bit BRP
    CAN1->BTR |= (baud_rate_prescaler - 1);

    CAN1->MCR &= ~(1UL << 0); // Xóa bit INRQ
    while ((CAN1->MSR >> 0) & 1); // Đợi INAK = 0 để xác nhận

    // Nhận tất cả ID → filter 0 vào FIFO0
    CAN_FilterInit(0, 0x000, 0x000, 0);

    // Nhận đúng ID 0x123 → filter 1 vào FIFO0
    //CAN_FilterInit(1, 0x123, 0x7FF, 0);

    CAN_NVIC_Config();

}

uint8_t CAN_Transmit(uint32_t id, uint8_t dlc, uint8_t *data) {
    if ((CAN1->TSR >> 26) & 1) { // TME0
        CAN1_TX_MAILBOX_0->TIR = id << 21; // Viết ID vào TIR
        CAN1_TX_MAILBOX_0->TDTR = dlc; // Cấu hình DLC
        CAN1_TX_MAILBOX_0->TDL = *(uint32_t *)data; // Ghi dữ liệu
        CAN1_TX_MAILBOX_0->TDH = *(uint32_t *)(data + 4); // Ghi dữ liệu
        CAN1_TX_MAILBOX_0->TIR |= (1UL << 0); // Đặt TXRQ để yêu cầu truyền
        return 1;
    }
    return 0;
}

uint8_t CAN_Receive(uint32_t *id, uint8_t *dlc, uint8_t *data) {
    if (((CAN1->RF0R >> 0) & 3) > 0) { // FMP0 > 0
        *id = CAN1_RX_MAILBOX_0->RIR >> 21; // Đọc ID từ RIR
        *dlc = CAN1_RX_MAILBOX_0->RDTR & 0xF; // Đọc DLC từ RDTR
        *(uint32_t *)data = CAN1_RX_MAILBOX_0->RDL; // Đọc dữ liệu
        *(uint32_t *)(data + 4) = CAN1_RX_MAILBOX_0->RDH; // Đọc dữ liệu

        CAN1->RF0R |= (1UL << 5); // Đặt bit RFOM0 để giải phóng mailbox
        return 1;
    }
    return 0; // Không có tin nhắn trong FIFO0
}

void CAN_ITConfig(CAN_TypeDef* CANx, uint32_t CAN_IT, FunctionalState NewState)
{
    if (NewState != DISABLE) {
        // Enable interrupt
        CANx->IER |= CAN_IT;
    } else {
        // Disable interrupt
        CANx->IER &= ~CAN_IT;
    }
}

void CAN_FilterInit(uint8_t filterBank, uint32_t id, uint32_t mask, uint8_t fifoAssign)
{
    // Bật chế độ khởi tạo filter
    CAN_FILTER->FMR |= CAN_FMR_FINIT;

    // Chọn scale 32-bit cho filterBank
    CAN_FILTER->FS1R |= (1U << filterBank);

    // Chọn chế độ mask (ID & Mask)
    CAN_FILTER->FM1R &= ~(1U << filterBank);

    // Gán vào FIFO0 hoặc FIFO1
    if (fifoAssign == 0) {
        CAN_FILTER->FFA1R &= ~(1U << filterBank);  // FIFO0
    } else {
        CAN_FILTER->FFA1R |= (1U << filterBank);   // FIFO1
    }

    // Chuẩn 11-bit ID → dịch vào vị trí StdId (bit 21..31)
    uint32_t id_reg   = (id & 0x7FF) << 21;
    uint32_t mask_reg = (mask & 0x7FF) << 21;

    CAN_FILTER->sFilterRegister[filterBank].FR1 = id_reg;
    CAN_FILTER->sFilterRegister[filterBank].FR2 = mask_reg;

    // Bật filter
    CAN_FILTER->FA1R |= (1U << filterBank);

    // Thoát chế độ khởi tạo
    CAN_FILTER->FMR &= ~CAN_FMR_FINIT;
}



void USB_LP_CAN1_RX0_IRQHandler(void)
{
    uint32_t id;
    uint8_t dlc;
    uint8_t data[8];

    if(CAN_Receive(&id, &dlc, data))
    	CANTP_RxIndication(id, data, dlc);
}
