# 🐍 Sumbul OS v1.0 - Arduino Nano Console

Proyek konsol mini multifungsi berbasis **Arduino Nano** yang menggabungkan fitur **Countown Timer** (Manual & Preset) dengan game klasik **Snake** ala Nokia 3310. Perangkat ini menggunakan dual-display untuk visibilitas maksimal: OLED untuk menu/grafik game dan 7-Segment untuk angka waktu/skor.

## 🚀 Fitur Utama
- **Dual Display System**:
  - OLED 0.96" I2C: Menampilkan Menu Utama, Grafik Game, dan Status.
  - 7-Segment TM1637: Menampilkan angka Countdown (MM:SS) dan Skor Game.
- **3 Mode Operasi**:
  - **Manual Timer**: Atur waktu sesuai keinginan (Menit & Detik).
  - **Preset Timer**: Pilihan cepat 1 menit, 5 menit, dan 10 menit.
  - **Snake Game**: Game ular klasik dengan fitur simpan skor tertinggi (High Score).
- **Audio Feedback**: Startup Nokia Tune, suara makan (Snake), dan Alarm Timer menggunakan modul Buzzer Low Level Trigger.
- **High Score Persistence**: Skor tertinggi tetap tersimpan di EEPROM meskipun perangkat dimatikan.

## 🛠️ Daftar Komponen
- Arduino Nano
- OLED 0.96" I2C (SSD1306)
- 7-Segment Display (TM1637)
- 5x Push Button (Navigasi: Up, Down, Left, Right, Select)
- Buzzer (Low Level Trigger - MH-FMD)
- Baterai Li-ion 3.7V + Modul Step-Up 5V (TP5400)

## 🔌 Wiring Diagram
| Komponen | Pin Arduino Nano |
| :--- | :--- |
| **OLED SDA/SCL** | A4 / A5 |
| **TM1637 CLK/DIO** | D11 / D12 |
| **Buzzer I/O** | D3 |
| **Button UP/DOWN** | D4 / D5 |
| **Button LEFT/RIGHT** | D6 / D7 |
| **Button SELECT** | D2 |

## 📦 Library yang Dibutuhkan
1. `Adafruit_SSD1306`
2. `Adafruit_GFX`
3. `TM1637Display`
4. `EEPROM` (Bawaan Arduino)

## 🎮 Cara Penggunaan
1. Hubungkan perangkat ke sumber daya 5V.
2. Tunggu "Sumbul OS" loading dan melodi selesai.
3. Gunakan tombol **UP/DOWN** untuk memilih menu, dan **SELECT** untuk masuk.
4. Pada mode Timer, gunakan arah panah untuk mengatur waktu dan **SELECT** untuk memulai.
