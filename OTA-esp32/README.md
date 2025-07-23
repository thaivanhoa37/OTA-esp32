# ESP32 WiFi Scanner & OTA Update System

## Tổng Quan
Dự án này triển khai hệ thống quét WiFi và cập nhật firmware qua mạng (OTA) sử dụng ESP32 với màn hình OLED và các nút điều khiển. Hệ thống cho phép người dùng quét các mạng WiFi có sẵn, hiển thị chúng trên màn hình OLED, và thực hiện cập nhật firmware không dây thông qua giao diện web.

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
- GPIO4  -> Nút LÊN
- GPIO0  -> Nút XUỐNG
- GPIO2  -> Nút CHỌN
- GND    -> Đất chung cho các nút
```

## Tính Năng

### 1. Quản Lý WiFi
- Quét và hiển thị mạng WiFi
  * Hiển thị cường độ tín hiệu
  * Chỉ báo mạng đã kết nối (✓)
  * Chỉ báo mạng có mật khẩu (🔒)
- Chế độ AP (Access Point)
  * SSID: ESP32-Config
  * Mật khẩu: 12345678
  * IP: 192.168.1.1
  * Giao diện web cấu hình WiFi
- Trạng thái kết nối chi tiết
  * SSID và cường độ tín hiệu
  * Địa chỉ IP và MAC
  * Kênh WiFi đang sử dụng

### 2. Cập Nhật OTA
- Giao diện web tải firmware
- Hỗ trợ truy cập qua IP hoặc mDNS
- Hiển thị tiến trình cập nhật
- Khởi động lại tự động sau khi cập nhật
- Tùy chọn bảo vệ bằng mật khẩu

### 3. Thông Tin Hệ Thống
- Thời gian hoạt động (ngày, giờ, phút)
- Nhiệt độ CPU
- Bộ nhớ RAM còn trống
- Dung lượng Flash
- Tần số CPU
- Phiên bản SDK

### 4. Cài Đặt
- Độ sáng màn hình (4 mức)
- Thời gian chờ màn hình (30s, 60s, 120s, 300s)
- Tự động kết nối WiFi
- Tùy chỉnh tên thiết bị
- Bảo vệ cập nhật OTA
- Khôi phục cài đặt gốc

## Giao Diện Menu

### 1. Menu Chính
- Scan WiFi: Quét và hiển thị mạng
- WiFi Status: Thông tin kết nối
- AP Mode: Chế độ điểm truy cập
- OTA Update: Cập nhật firmware
- System Info: Thông tin hệ thống
- Settings: Cài đặt thiết bị

### 2. Hiển Thị Trạng Thái
- Icon WiFi và cường độ tín hiệu
- Thời gian hoạt động
- Nhiệt độ CPU
- Thông báo tự động ẩn

## Cách Sử Dụng

### 1. Kết Nối WiFi
**Phương pháp 1 - Mạng Mở:**
1. Sử dụng nút điều hướng chọn "Scan WiFi"
2. Chọn mạng WiFi từ danh sách
3. Kết nối trực tiếp với mạng không mật khẩu

**Phương pháp 2 - Mạng Bảo Mật:**
1. Chọn "AP Mode" từ menu chính
2. Kết nối với mạng "ESP32-Config"
3. Truy cập 192.168.1.1 trên trình duyệt
4. Chọn mạng và nhập mật khẩu
5. Thiết bị sẽ tự động khởi động lại và kết nối

### 2. Cập Nhật Firmware
1. Kết nối thiết bị với WiFi
2. Chọn "OTA Update" từ menu
3. Truy cập địa chỉ IP hoặc hostname hiển thị
4. Tải lên file firmware mới
5. Chờ quá trình cập nhật hoàn tất

### 3. Cấu Hình Thiết Bị
1. Vào menu "Settings"
2. Điều chỉnh các tùy chọn:
   - Độ sáng màn hình
   - Thời gian chờ
   - Kết nối tự động
   - Tên thiết bị
   - Bảo vệ OTA
   - Khôi phục cài đặt

## Cấu Trúc Mã Nguồn
```
src/
├── main.cpp          # Chương trình chính
include/
├── config.h         # Cấu hình hệ thống
├── display.h        # Xử lý màn hình OLED
├── wifi_scanner.h   # Quét và quản lý WiFi
└── menu.h          # Hệ thống menu
```

## Môi Trường Phát Triển
- Platform: PlatformIO
- Framework: Arduino
- Board: ESP32 Dev Module
- Libraries:
  * Adafruit_GFX
  * Adafruit_SSD1306
  * WiFi
  * WebServer
  * Update
  * ESPmDNS

## Cấu Hình
Các thông số có thể điều chỉnh trong `config.h`:
```cpp
// Màn Hình
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C

// Nút Bấm
#define BUTTON_UP 4
#define BUTTON_DOWN 0
#define BUTTON_SELECT 2

// Access Point
#define AP_SSID "ESP32-Config"
#define AP_PASSWORD "12345678"

// OTA
#define OTA_PORT 8080
#define OTA_HOSTNAME "esp32-ota"
#define OTA_PASSWORD "admin"
```

## Biên Dịch và Nạp Code
1. Cài đặt PlatformIO
2. Clone repository
3. Mở dự án trong PlatformIO
4. Biên dịch và nạp code:
   ```
   pio run -t upload
   ```
5. Nạp qua OTA (sau khi đã kết nối WiFi):
   ```
   pio run -t upload --upload-port <IP_ADDRESS>
   ```

## Xử Lý Sự Cố
- **Màn hình không hiển thị**: Kiểm tra kết nối I2C và địa chỉ
- **Nút bấm không phản hồi**: Kiểm tra kết nối và điện trở kéo lên
- **Không quét được WiFi**: Kiểm tra ăng-ten ESP32
- **Lỗi cập nhật OTA**: Đảm bảo kết nối WiFi ổn định
- **Không vào được AP**: Thử khởi động lại thiết bị

## Đóng Góp
Mọi đóng góp đều được hoan nghênh. Vui lòng tạo pull request hoặc báo cáo lỗi qua mục Issues.

## Giấy Phép
Dự án này được phân phối dưới giấy phép MIT.
