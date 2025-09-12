#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include "driver/twai.h"
#include "TWAI_ISO.h"

// ================= WiFi Config =================
const char* ssid = "ESP32_AP";         // Tên WiFi AP do ESP32 phát ra
const char* password = "12345678";     // Mật khẩu WiFi
WebServer server(80);                  // Khởi tạo web server cổng 80

// ================= CAN Config =================
#define CAN_TX GPIO_NUM_5              // Chân TX CAN trên ESP32
#define CAN_RX GPIO_NUM_4              // Chân RX CAN trên ESP32
#define TESTER_CAN_ID 0x7E0            // CAN ID của tester (ESP32)
#define ECU_CAN_ID    0x7E8            // CAN ID của ECU (STM32)

IsoTpLink_t isoLink;                   // Cấu trúc quản lý ISO-TP
uint8_t rxBuffer[512];                 // Bộ đệm nhận ISO-TP

// ================= UDS State Machine =================
enum UDS_State {
  IDLE,
  WAIT_RD_RESPONSE,
  WAIT_TD_RESPONSE,
  WAIT_EXIT_RESPONSE,
  COMPLETED,
  ERROR_STATE
};
UDS_State udsState = IDLE;             // Trạng thái hiện tại của UDS
uint8_t blockCounter = 1;              // Đếm số block truyền
size_t offset = 0;                     // Vị trí hiện tại trong file
size_t firmwareSize = 0;               // Kích thước file firmware

// ================= CRC32 Calculation =================
// Tính CRC32 cho dữ liệu firmware, dùng để kiểm tra toàn vẹn khi kết thúc truyền
uint32_t calculateCRC32(const uint8_t* data, size_t length) {
  uint32_t crc = 0xFFFFFFFF;
  for (size_t i = 0; i < length; i++) {
    crc ^= (uint32_t)data[i] << 24;
    for (int j = 0; j < 8; j++) {
      if (crc & 0x80000000) crc = (crc << 1) ^ 0x04C11DB7;
      else crc <<= 1;
    }
  }
  return ~crc;
}

// ================== WiFi + WebServer ==================
// Xử lý upload file từ Winform lên ESP32 qua HTTP
File uploadFile;
void handleFileUpload() {
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    String filename = "/firmware.bin";
    if (SPIFFS.exists(filename)) SPIFFS.remove(filename); // Xóa file cũ nếu có
    uploadFile = SPIFFS.open(filename, FILE_WRITE);       // Mở file mới để ghi
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (uploadFile) uploadFile.write(upload.buf, upload.currentSize); // Ghi dữ liệu vào file
  } else if (upload.status == UPLOAD_FILE_END) {
    if (uploadFile) uploadFile.close();                   // Đóng file khi upload xong
    Serial.printf("Upload End: %u bytes\n", upload.totalSize);
    firmwareSize = upload.totalSize;                      // Lưu kích thước file
    udsState = IDLE;                                      // Đặt lại trạng thái UDS
  }
}

// Khởi tạo WiFi AP và WebServer
void setupWiFi() {
  WiFi.softAP(ssid, password);                            // Phát WiFi AP
  Serial.print("ESP32 AP IP: ");
  Serial.println(WiFi.softAPIP());
  // Trang chủ: form upload file
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", "<form method='POST' action='/upload' enctype='multipart/form-data'>"
                                  "<input type='file' name='firmware'>"
                                  "<input type='submit' value='Upload'></form>");
  });
  // Xử lý upload file
  server.on("/upload", HTTP_POST, []() { server.send(200, "text/plain", "Upload complete!"); }, handleFileUpload);
  server.begin();
}

// ================== CAN + ISO-TP ==================
// Khởi tạo CAN (TWAI) và ISO-TP
void setupCAN() {
  twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(CAN_TX, CAN_RX, TWAI_MODE_NORMAL);
  twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
  twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
  if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK && twai_start() == ESP_OK) {
    Serial.println("CAN started");
  } else {
    Serial.println("CAN init failed!");
    while (1);
  }
  isoTp_initLink(&isoLink, rxBuffer, sizeof(rxBuffer), 1000, 1000, 1000, 1000, 0, 150, 0, 0);
}

// ================== UDS Send Functions ==================
// Gửi RequestDownload (0x34) để bắt đầu truyền firmware
void sendUDS_RequestDownload(size_t fwSize) {
  uint8_t req[] = {
    0x34, 0x00, 0x44,
    0x00, 0x00, 0x10, 0x00, // Địa chỉ ghi (0x1000)
    0x00, 0x00, (uint8_t)((fwSize >> 8) & 0xFF), (uint8_t)(fwSize & 0xFF) // Độ dài firmware
  };
  bool sent = isoTp_send(&isoLink, TESTER_CAN_ID, ECU_CAN_ID, req, sizeof(req));
  Serial.println(sent ? ">> Sent RequestDownload (0x34)" : "!! Failed to send RequestDownload");
  if (sent) udsState = WAIT_RD_RESPONSE;
}

// Gửi TransferData (0x36) từng block 6 byte
void sendUDS_TransferDataBlock(File &f) {
  if (offset >= firmwareSize) {
    // Đã gửi hết, gửi RequestTransferExit (0x37) kèm CRC32
    f.seek(0, SeekSet);
    uint8_t* fwBuf = (uint8_t*)malloc(firmwareSize);
    f.read(fwBuf, firmwareSize);
    uint32_t crcValue = calculateCRC32(fwBuf, firmwareSize);
    free(fwBuf);
    uint8_t req[5] = {0x37, (uint8_t)(crcValue >> 24), (uint8_t)(crcValue >> 16), (uint8_t)(crcValue >> 8), (uint8_t)crcValue};
    bool sent = isoTp_send(&isoLink, TESTER_CAN_ID, ECU_CAN_ID, req, sizeof(req));
    Serial.printf(sent ? ">> Sent RequestTransferExit (CRC=0x%08X)\n" : "!! Failed to send RequestTransferExit\n", crcValue);
    if (sent) udsState = WAIT_EXIT_RESPONSE;
    return;
  }
  // Gửi block TransferData
  size_t chunk = min((size_t)6, firmwareSize - offset);   // Mỗi block tối đa 6 byte
  uint8_t req[8] = {0x36, blockCounter, 0,0,0,0,0,0};
  f.seek(offset, SeekSet);
  f.read(&req[2], chunk);                                 // Đọc dữ liệu từ file
  bool sent = isoTp_send(&isoLink, TESTER_CAN_ID, ECU_CAN_ID, req, 2 + chunk);
  Serial.printf(sent ? ">> Sent TransferData block %u\n" : "!! Failed to send TransferData block %u\n", blockCounter);
  if (sent) udsState = WAIT_TD_RESPONSE;
}

// Xử lý phản hồi UDS từ STM32 (ECU)
void handleUDSResponse(File &f, uint8_t* buf, uint16_t len) {
  if (len == 0) return;
  uint8_t sid = buf[0];
  if (sid == 0x7F) { // Negative Response
    if (len >= 3) Serial.printf("<< Negative Response: for SID=0x%02X, NRC=0x%02X\n", buf[1], buf[2]);
    udsState = ERROR_STATE;
    return;
  }
  switch (sid) {
    case 0x74: // Positive Response to RequestDownload
      if (udsState == WAIT_RD_RESPONSE) {
        Serial.println("<< Positive Response: RequestDownload (0x74)");
        blockCounter = 1; offset = 0;
        sendUDS_TransferDataBlock(f); // Gửi block đầu tiên
      }
      break;
    case 0x76: // Positive Response to TransferData
      if (udsState == WAIT_TD_RESPONSE) {
        Serial.println("<< Positive Response: TransferData (0x76)");
        offset += 6; blockCounter++;
        sendUDS_TransferDataBlock(f); // Gửi block tiếp theo
      }
      break;
    case 0x77: // Positive Response to RequestTransferExit
      if (udsState == WAIT_EXIT_RESPONSE) {
        Serial.println("<< Positive Response: RequestTransferExit (0x77)");
        Serial.println("=== Firmware Update Completed Successfully ===");
        udsState = COMPLETED;
        
      }
      break;
    default:
      Serial.printf("<< Unknown Response SID=0x%02X\n", sid);
      break;
  }
}

// ================== Setup & Loop ==================
// Hàm setup: khởi tạo SPIFFS, WiFi, CAN
void setup() {
  Serial.begin(115200);
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed!");
    return;
  }
  setupWiFi();
  setupCAN();
}

// Vòng lặp chính
void loop() {
  server.handleClient(); // Xử lý HTTP client (upload file)

  static bool sending = false; // Đã bắt đầu truyền UDS chưa
  static File fwFile;

  // Khi upload xong, bắt đầu gửi UDS
  if (!sending && SPIFFS.exists("/firmware.bin")) {
    fwFile = SPIFFS.open("/firmware.bin", FILE_READ);
    if (fwFile) {
      firmwareSize = fwFile.size();
      Serial.printf("Firmware size: %u bytes\n", firmwareSize);
      sendUDS_RequestDownload(firmwareSize); // Gửi yêu cầu bắt đầu truyền
      sending = true;
      udsState = WAIT_RD_RESPONSE;
      blockCounter = 1;
      offset = 0;
    }
  }

  // Poll ISO-TP để xử lý truyền nhận frame
  isoTp_poll(&isoLink);

  // Nhận CAN và xử lý UDS response
  twai_message_t rxMsg;
  if (twai_receive(&rxMsg, pdMS_TO_TICKS(10)) == ESP_OK) {
    uint16_t len;
    uint8_t* buf;
    if (isoTp_receive(&isoLink, &rxMsg, &len, &buf)) {
      Serial.printf("<< ECU Response, len=%u: ", len);
      for (int i = 0; i < len; i++) Serial.printf("%02X ", buf[i]);
      Serial.println();
      handleUDSResponse(fwFile, buf, len); // Xử lý phản hồi từ STM32
    }
  }

  // Khi hoàn thành hoặc lỗi, dọn dẹp
  if (sending && (udsState == COMPLETED || udsState == ERROR_STATE)) {
    fwFile.close();
    SPIFFS.remove("/firmware.bin");
    sending = false;
    Serial.println("=== Done or Error, ready for next upload ===");
  }

  delay(10); // Giảm tải CPU
}