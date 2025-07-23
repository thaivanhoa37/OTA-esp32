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

## Chi Tiết Menu và Hiển Thị

### 1. Menu Chính
- **Quét WiFi**
  - Danh sách mạng WiFi được tìm thấy
  - Hiển thị cường độ tín hiệu bằng icon (1-4 vạch)
  - Chỉ báo mạng đã kết nối (✓)
  - Chỉ báo mạng có mật khẩu (🔒)

- **Kết Nối Mạng**
  - Hiển thị trạng thái kết nối hiện tại
  - Tên mạng đang kết nối
  - Địa chỉ IP
  - Cường độ tín hiệu
  - Nút ngắt kết nối

- **Cập Nhật OTA**
  - Địa chỉ IP và cổng web server
  - Trạng thái cập nhật
  - Tiến trình tải lên (thanh tiến trình)
  - Phiên bản firmware hiện tại
  - Dung lượng bộ nhớ còn trống

- **Thông Tin Hệ Thống**
  - Phiên bản phần cứng
  - Phiên bản firmware
  - Thời gian hoạt động
  - Nhiệt độ CPU
  - RAM còn trống
  - Dung lượng flash còn trống
  - MAC Address

- **Cài Đặt**
  - Độ sáng màn hình
  - Thời gian chờ màn hình
  - Tự động kết nối WiFi
  - Đổi tên thiết bị
  - Cài đặt mật khẩu OTA
  - Khôi phục cài đặt gốc

### 2. Hiển Thị Trạng Thái
- **Thanh Trạng Thái**
  - Icon WiFi và cường độ tín hiệu
  - Icon pin và mức pin
  - Đồng hồ thời gian thực
  - Nhiệt độ CPU

- **Thông Báo**
  - Hiển thị popup thông báo
  - Tự động ẩn sau 3 giây
  - Các loại thông báo:
    - Kết nối thành công
    - Lỗi kết nối
    - Bắt đầu cập nhật
    - Cập nhật thành công
    - Lỗi cập nhật

### 3. Hiệu Ứng Hiển Thị
- Animation chuyển menu
- Icon động cho quá trình quét WiFi
- Thanh tiến trình động
- Hiệu ứng làm mờ khi popup
- Animation khi nhấn nút

### 4. Tính Năng Bảo Mật
- **Bảo Vệ OTA**
  - Mật khẩu cho web server
  - Xác thực firmware
  - Mã hóa quá trình truyền
  
- **Bảo Vệ WiFi**
  - Lưu trữ mật khẩu an toàn
  - Tự động kết nối lại
  - Giới hạn số lần thử kết nối

### 5. Tính Năng Nâng Cao
- **Lưu Trữ**
  - Lưu danh sách mạng wifi đã kết nối
  - Tự động kết nối với mạng ưu tiên
  - Lưu cài đặt người dùng

- **Quản Lý Năng Lượng**
  - Chế độ tiết kiệm pin
  - Tự động giảm độ sáng
  - Tắt màn hình sau thời gian chờ

- **Gỡ Lỗi**
  - Log hệ thống
  - Thông tin debug
  - Báo cáo lỗi
  - Trạng thái phần cứng

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
