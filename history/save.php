<?php
header('Content-Type: application/json'); // Supaya output JSON

// Konfigurasi database
$host = "localhost";
$user = "root";           // ganti sesuai username MySQL
$pass = "";               // ganti sesuai password MySQL
$dbname = "monitoring"; // ganti sesuai nama database

// Buat koneksi
$conn = new mysqli($host, $user, $pass, $dbname);

// Cek koneksi
if ($conn->connect_error) {
    die(json_encode(["error" => "Koneksi gagal: " . $conn->connect_error]));
}

// Ambil data history dari tabel sesuai kolom: no, date_time, volume
$sql = "SELECT no, date_time, volume FROM history ORDER BY no ASC";
$result = $conn->query($sql);

$data = [];

if ($result->num_rows > 0) {
    while ($row = $result->fetch_assoc()) {
        // Pastikan volume dikonversi ke float agar JS bisa langsung pakai
        $row['volume'] = floatval($row['volume']);
        $data[] = $row;
    }
}

// Kirim data dalam format JSON
echo json_encode($data);

// Tutup koneksi
$conn->close();
?>
