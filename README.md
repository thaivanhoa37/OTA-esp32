# Dự Án Quét WiFi & Cập Nhật OTA cho ESP32

## Tổng Quan
Dự án này triển khai hệ thống quét WiFi và cập nhật firmware qua mạng (OTA) sử dụng ESP32 với màn hình OLED và các nút điều khiển. Hệ thống cho phép người dùng quét các mạng WiFi có sẵn, hiển thị chúng trên màn hình OLED, và thực hiện cập nhật firmware không dây thông qua giao diện menu.

## Linh Kiện Phần Cứng
- Bo mạch ESP32 (phiên bản 38 chân)
- Màn hình OLED 0.96" (giao tiếp I2C)
- Các nút nhấn để điều hướng
- Nguồn điện (cáp USB hoặc nguồn ngoài)
- Breadboard và dây jumper

## Sơ Đồ Kết Nối
```
ESP32 -> Màn Hình OLED
- 3.3V -> VCC
- GND  -> GND
- GPIO21 (SDA) -> SDA
- GPIO22 (SCL) -> SCL

ESP32 -> Các Nút Điều Hướng
- GPIO12 -> Nút LÊN
- GPIO14 -> Nút XUỐNG
- GPIO27 -> Nút CHỌN
- GND    -> Đất chung cho các nút
```

## Yêu Cầu Phần Mềm
- Arduino IDE hoặc Platform IO
- Các thư viện cần thiết:
  - WiFi.h
  - ESPmDNS.h
  - WiFiClient.h
  - WebServer.h
  - Update.h
  - Adafruit_GFX.h
  - Adafruit_SSD1306.h
  - Wire.h

## Tính Năng
1. Menu Quét WiFi:
   - Quét các mạng WiFi có sẵn
   - Hiển thị tên mạng (SSID)
   - Hiển thị cường độ tín hiệu
   - Chỉ báo mạng có bảo mật/mở

2. Hệ Thống Điều Hướng:
   - Nút LÊN: Di chuyển lên trong menu
   - Nút XUỐNG: Di chuyển xuống trong menu
   - Nút CHỌN: Chọn mục đã chọn

3. Cấu Trúc Menu:
   - Quét WiFi
   - Kết Nối Mạng
   - Cập Nhật OTA
   - Thông Tin Hệ Thống
   - Cài Đặt

4. Tính Năng Cập Nhật OTA:
   - Cập nhật firmware qua WiFi
   - Hiển thị tiến trình trên màn hình OLED
   - Tự động khởi động lại sau khi cập nhật
   - Xử lý và hiển thị lỗi

## Hướng Dẫn Cài Đặt

### 1. Lắp Ráp Phần Cứng
1. Kết nối màn hình OLED với ESP32 qua chân I2C
2. Đấu nối các nút bấm với điện trở kéo lên
3. Kiểm tra tất cả kết nối trước khi cấp nguồn

### 2. Cài Đặt Môi Trường Phát Triển
1. Cài đặt Arduino IDE hoặc Platform IO
2. Cài đặt gói hỗ trợ bo mạch ESP32
3. Cài đặt các thư viện cần thiết qua trình quản lý thư viện
4. Cấu hình cài đặt cho ESP32

### 3. Cấu Hình Ban Đầu
1. Nạp code lần đầu qua USB
2. Sử dụng hệ thống menu để quét và kết nối WiFi
3. Ghi nhớ địa chỉ IP hiển thị trên OLED để cập nhật OTA

## Hướng Dẫn Sử Dụng

### Điều Hướng Menu
1. Sử dụng nút LÊN/XUỐNG để di chuyển qua các mục menu
2. Nhấn CHỌN để chọn một tùy chọn
3. Nhấn giữ CHỌN để quay lại menu trước

### Quét WiFi
1. Chọn "Quét WiFi" từ menu chính
2. Đợi quá trình quét hoàn tất
3. Dùng nút LÊN/XUỐNG để duyệt các mạng có sẵn
4. Nhấn CHỌN để chọn mạng cần kết nối

### Thực Hiện Cập Nhật OTA
1. Chọn "Cập Nhật OTA" từ menu chính
2. Hệ thống sẽ hiển thị địa chỉ IP
3. Kết nối máy tính vào cùng mạng WiFi
4. Truy cập giao diện web bằng IP đã hiển thị
5. Tải lên file firmware mới (.bin)
6. Theo dõi tiến trình trên màn hình OLED

## Cấu Trúc Dự Án
```
OTA-esp32/
├── src/
│   ├── main.cpp          # Mã nguồn chính
│   ├── menu.h           # Định nghĩa hệ thống menu
│   ├── wifi_scanner.h   # Chức năng quét WiFi
│   ├── display.h        # Chức năng hiển thị OLED
│   └── config.h         # Cấu hình header
├── include/
├── lib/
├── platformio.ini
└── README.md
```

## Cấu Hình
Các thông số chính trong `config.h`:
```cpp
// Cài Đặt Màn Hình
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

// Chân Nút Bấm
#define BUTTON_UP 12
#define BUTTON_DOWN 14
#define BUTTON_SELECT 27

// Cài Đặt WiFi
#define WIFI_SCAN_INTERVAL 10000  // ms
#define MAX_NETWORKS 20

// Cài Đặt OTA
#define OTA_PORT 8080
#define OTA_HOSTNAME "esp32-ota"
#define OTA_PASSWORD "admin"
```

## Xử Lý Sự Cố
- Màn hình không hiển thị: Kiểm tra kết nối I2C và địa chỉ
- Nút bấm không phản hồi: Kiểm tra kết nối chân và điện trở kéo lên
- Quét WiFi không hoạt động: Kiểm tra kết nối ăng-ten
- Cập nhật OTA thất bại: Đảm bảo kết nối WiFi ổn định và nguồn điện đầy đủ

## Đóng Góp
Chúng tôi hoan nghênh mọi đóng góp! Vui lòng gửi pull request.

## Giấy Phép
Dự án này được cấp phép theo Giấy phép MIT.
