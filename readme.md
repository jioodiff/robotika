# ESP32 Car Controller

Panduan penggunaan dan instalasi untuk mobil kontrol Wi-Fi berbasis ESP32.

## Cara Penggunaan

### 1. Upload Sketch ke ESP32

Upload sketch program yang telah disediakan ke board ESP32 menggunakan Arduino IDE atau platform lainnya.

### 2. Power Supply

Nyalakan driver dan motor dengan baterai/power pack terpisah. Pastikan untuk menyatukan GND dengan ESP32.

**Peringatan:** Jangan menghubungkan sumber daya motor langsung ke ESP32 karena dapat menyebabkan kerusakan akibat beda tegangan.

### 3. Koneksi Wi-Fi

Dari ponsel atau laptop, sambungkan ke jaringan Wi-Fi yang dipancarkan ESP32:

- SSID: **KelompokGila**
- Password: **Kelompok11**

### 4. Akses Kontrol Web

Buka browser dan akses alamat: **http://192.168.4.1**

Halaman kontrol akan muncul dengan antarmuka yang mudah digunakan.

### 5. Kontrol Mobil

Tekan dan tahan tombol arah untuk menggerakkan mobil. Lepaskan tombol untuk berhenti.

Gunakan slider untuk mengatur kecepatan mobil sesuai keinginan.


https://github.com/user-attachments/assets/e36cc69f-630b-48b4-92de-fc542b0d18e4


## Koneksi L298N

Jika menggunakan driver motor L9110S (dengan pin A-1A/A-1B dan B-1A/B-1B), hubungkan sebagai berikut:

### Diagram Koneksi
ESP32 → L9110S Motor Driver

| ESP32 GPIO | L298N Pin | Fungsi |
|------------|------------|--------|
| 26         | A-1A       | Motor kiri - kontrol arah 1 |
| 27         | A-1B       | Motor kiri - kontrol arah 2 |
| 32         | B-1A       | Motor kanan - kontrol arah 1 |
| 14         | B-1B       | Motor kanan - kontrol arah 2 |

**Catatan:** Abaikan pin ENA/ENB pada L9110S (tidak ada). Untuk kontrol kecepatan, gunakan PWM pada GPIO25 untuk motor kiri dan GPIO33 untuk motor kanan.

**Alternatif:** Jika ingin kontrol kecepatan yang lebih halus, pertimbangkan untuk menggunakan modul driver yang mendukung pin ENA/ENB seperti L298N atau L293D.

## Pemecahan Masalah

### Motor Berputar Terbalik

Jika motor berputar dengan arah yang terbalik:

- Ubah nilai `LEFT_REVERSED` atau `RIGHT_REVERSED` menjadi `true` dalam kode program, atau
- Tukar kabel + dan - motor yang terhubung ke driver

### Kontrol Kecepatan Tidak Berfungsi

Jika kontrol kecepatan tidak berfungsi dengan L9110S:

- Ganti nilai `ledcWrite(...)` menjadi 255 dan kendalikan motor hanya dengan HIGH/LOW pada pin IN
- Atau sambungkan GPIO25 ke A-1A melalui resistor 1kΩ (opsional)

## Fitur Tambahan yang Tersedia

Jika membutuhkan versi dengan fitur tambahan, berikut varian yang dapat disiapkan:

- **Station mode** - ESP32 terhubung ke router Wi-Fi existing
- **Joystick virtual** - Kontrol dengan joystick pada antarmuka web
- **WebSocket realtime** - Respons kontrol yang lebih cepat dan realtime

Hubungi developer untuk mendapatkan varian dengan fitur-fitur tersebut.

---

© 2023 ESP32 Car Controller Project | Dokumentasi ini diperbarui terakhir pada 15 September 2023
