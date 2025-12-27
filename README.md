# Monitoring Volume Solar

Proyek ini adalah sistem monitoring volume solar berbasis IoT dan web dashboard yang bekerja dalam jaringan Local Area Network (LAN). Sistem ini dirancang untuk memantau ketinggian, volume, dan persentase solar dalam tangki secara real-time tanpa server eksternal.

🔧 System Overview
Sistem terdiri dari dua bagian utama:
- Hardware (ESP32 + sensor ultrasonik JSN-SR04T + modul Ethernet LAN8720)
- Web Dashboard yang di-host langsung di memori flash ESP32 melalui LittleFS
ESP32 berfungsi sebagai HTTP Server, data processor, dan web server sekaligus.

🧠 System Architecture
Ultrasonic Sensor (JSN-SR04T)
            ↓
         ESP32
   (Data Processing)
            ↓
   HTTP JSON Endpoint
            ↓
   Web Dashboard (HTML, CSS, JS)
            ↓
        PC Client (LAN)

⚙️ Hardware Components
- ESP32
- Ultrasonic Sensor JSN-SR04T
- Ethernet Module LAN8720
- PC / Laptop (Client Monitoring)

💻 Software & Technologies
- Arduino IDE
- ESP32 WebServer Library
- HTTP Protocol
- JSON (Data Exchange)
- HTML, CSS, JavaScript
- Fetch API
- LittleFS (ESP32 Flash Storage)

📡 Network Configuration
- ESP32 menggunakan IP statis
- Akses dashboard melalui browser: http://<IP-ESP32>/
- Komunikasi data menggunakan HTTP Request (GET/POST) dalam jaringan LAN

📊 Web Features
- Monitoring volume solar
- Monitoring tinggi permukaan solar
- Monitoring persentase sisa solar
- Input dimensi tangki
- Mengubah konfigurasi IP statis ESP32
- Validasi input (data tidak ditampilkan sebelum dimensi tangki diatur)
- Web dashboard di-host langsung di ESP32 (tanpa server eksternal)

🔄 Data Flow
1. Sensor ultrasonik membaca jarak permukaan solar
2. ESP32 menghitung tinggi, volume, dan persentase solar
3. Data diekspor dan diimpor ke frontend web sebagai JSON endpoint
4. Frontend web memanggil dan mengirim data ke ESP32 menggunakan Fetch API
5. Data ditampilkan pada dashboard web


🚀 Installation & Usage
1. Upload file web ke ESP32 menggunakan LittleFS
2. Upload program ESP32 via Arduino IDE
3. Hubungkan ESP32 ke jaringan LAN menggunakan modul ethernet LAN8720
4. Akses dashboard melalui browser menggunakan IP statis ESP32
5. Input dimensi tangki untuk mulai monitoring

🎯 Use Case
- Monitoring tangki solar genset
- Sistem kontrol BBM fasilitas industri
- Proyek IoT industri berbasis LAN
- Sistem monitoring lokal tanpa cloud

📌 Notes
- Sistem berjalan sepenuhnya di jaringan lokal (offline)
- Tidak memerlukan koneksi internet
- Cocok untuk aplikasi industri yang membutuhkan stabilitas LAN
- Bisa dikembangkan lagi menjadi SCADA dengan menambahkan fitur penyimpanan data ke database dan alarm (tangki penuh, suhu tangki, dan lain lain)

