<?php
header("Content-Type: application/json");

// ========================
// CONFIG DATABASE
// ========================
$host = "localhost";
$user = "root";
$pass = "";
$db   = "monitoring";

$conn = new mysqli($host, $user, $pass, $db);

if ($conn->connect_error) {
    http_response_code(500);
    echo json_encode(["status" => "error", "msg" => "DB connection failed"]);
    exit;
}

// ========================
// BACA RAW JSON
// ========================
$raw = file_get_contents("php://input");

if(!$raw){
    echo json_encode(["status" => "error", "msg" => "RAW EMPTY"]);
    exit;
}

$data = json_decode($raw, true);

// Jika JSON invalid atau kosong
if (!$data || !isset($data["volume"])) {
    echo json_encode(["status" => "error", "msg" => "Invalid JSON or missing 'volume'"]);
    exit;
}

// Ambil nilai
$volume = $conn->real_escape_string($data["volume"]);

// ========================
// UPDATE DATABASE
// ========================
$sql = "UPDATE history SET volume='$volume', date_time =NOW() WHERE no =1";

if ($conn->query($sql)) {
    echo json_encode(["status"=>"success","msg"=>"Volume updated"]);
} else {
    echo json_encode(["status"=>"error","msg"=>$conn->error]);
}

$conn->close();
?>
