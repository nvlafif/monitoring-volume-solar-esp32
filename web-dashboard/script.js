console.log("script.js aktif");

// FUNGSI TANGGAL DAN JAM
function updateDateTime() {
    const now = new Date();

    // Format tanggal
    const options = { year: 'numeric', month: 'long', day: 'numeric' };
    const tanggal = now.toLocaleDateString('id-ID', options);

    // Format waktu (jam:menit:detik)
    const jam = String(now.getHours()).padStart(2, '0');
    const menit = String(now.getMinutes()).padStart(2, '0');
    const detik = String(now.getSeconds()).padStart(2, '0');

    // Masukkan ke dalam div
    document.getElementById("tanggal").textContent =
        `${tanggal} - ${jam}:${menit}:${detik}`;
}

// Update setiap 1 detik
setInterval(updateDateTime, 1000);

// Panggil pertama kali
updateDateTime();

function goHistory() {
    window.location.href = "history.html";
}

// ========================
// UTILITIES
// ========================

// Validasi angka
function inputValid(...fields) {
    return fields.every(v => v !== "" && !isNaN(v));
}

// Safe number untuk toFixed
function safeNumber(val, decimals = 2) {
    if (typeof val !== "number" || isNaN(val)) return "-";
    return val.toFixed(decimals);
}

// ========================
// DOM READY
// ========================
document.addEventListener("DOMContentLoaded", () => {
    getDimensiTanki();
    getIPConfig();
    tampilDataSolar();
    setInterval(tampilDataSolar, 3000);
});

// ========================
// DIMENSI TANKI (POST)
// ========================
async function kirimDimensi() {
    const p = document.getElementById("inputP").value;
    const l = document.getElementById("inputL").value;
    const t = document.getElementById("inputT").value;

    if (!inputValid(p, l, t)) {
        alert("Semua input P, L, T wajib diisi angka!");
        return;
    }

    try {
        const res = await fetch("/setdim", {
            method: "POST",
            headers: { "Content-Type": "application/x-www-form-urlencoded" },
            body: `p=${p}&l=${l}&t=${t}`
        });

        if (!res.ok) throw new Error("HTTP " + res.status);

        alert("Dimensi tanki berhasil disimpan!");
        getDimensiTanki();
    } 
    catch (err) {
        console.error("ERROR kirimDimensi():", err);
        alert("Gagal mengirim data ke ESP32!");
    }
}

// GET dimensi tanki
async function getDimensiTanki() {
    try {
        const res = await fetch("/getdim");
        if (!res.ok) throw new Error("HTTP " + res.status);

        const data = await res.json();

        if (data.status === "empty") {
            updateDimUI("-", "-", "-");
            return;
        }

        updateDimUI(
            "Panjang Tanki = " + safeNumber(data.p) + " cm",
            "Lebar Tanki = " + safeNumber(data.l) + " cm",
            "Tinggi Tanki = " + safeNumber(data.t) + " cm"
        );
    }
    catch (err) {
        console.error("Gagal ambil data dimensi:", err);
        updateDimUI("-", "-", "-");
    }
}

// UI untuk P, L, T
function updateDimUI(p, l, t) {
    document.getElementById("showP").textContent = p;
    document.getElementById("showL").textContent = l;
    document.getElementById("showT").textContent = t;
}

// ========================
// DATA SENSOR (GET)
// ========================
async function tampilDataSolar() {
    try {
        const res = await fetch("/data");
        if (!res.ok) throw new Error("HTTP " + res.status);

        const data = await res.json();

        if (data.status === "empty") {
            updateSolarUI("-", "-", "-", "-");
            return;
        }

        updateSolarUI(
            safeNumber(data.volume, 2) + " L",
            safeNumber(data.tinggi, 2) + " cm",
            safeNumber(data.persen, 1) + " %",
            safeNumber(data.jarak, 2) + " cm"
        );
    }
    catch (err) {
        console.error("Gagal ambil data sensor:", err);
        updateSolarUI("-", "-", "-", "-");
    }
}

// //WEBSOCKET
// const ws = new WebSocket("ws://" + location.hostname + ":81/");

// ws.onmessage = (event) => {
//     const data = JSON.parse(event.data);

//     updateSolarUI(
//         safeNumber(data.volume, 2) + " L",
//         safeNumber(data.tinggi, 2) + " cm",
//         safeNumber(data.persen, 1) + " %",
//         safeNumber(data.jarak, 2) + " cm"
//     );
// };

// UI data sensor
function updateSolarUI(volume, tinggi, persen, jarak) {
    document.getElementById("volume").textContent  = volume;
    document.getElementById("tinggi").textContent  = tinggi;
    document.getElementById("percent").textContent = persen;
    document.getElementById("jarak").textContent   = jarak;
}

// ========================
// IP CONFIG (POST)
// ========================
async function applyIPConfig() {
    // Ambil nilai IP, Gateway, Subnet (4 segmen)
    const fields = {
        ip1: document.getElementById("ip1").value,
        ip2: document.getElementById("ip2").value,
        ip3: document.getElementById("ip3").value,
        ip4: document.getElementById("ip4").value,

        gw1: document.getElementById("gw1").value,
        gw2: document.getElementById("gw2").value,
        gw3: document.getElementById("gw3").value,
        gw4: document.getElementById("gw4").value,

        sn1: document.getElementById("sn1").value,
        sn2: document.getElementById("sn2").value,
        sn3: document.getElementById("sn3").value,
        sn4: document.getElementById("sn4").value,

        stdns1: document.getElementById("stdns1").value,
        stdns2: document.getElementById("stdns2").value,
        stdns3: document.getElementById("stdns3").value,
        stdns4: document.getElementById("stdns4").value,

        nddns1: document.getElementById("nddns1").value,
        nddns2: document.getElementById("nddns2").value,
        nddns3: document.getElementById("nddns3").value,
        nddns4: document.getElementById("nddns4").value
    };

    // Validasi angka
    for (const key in fields) {
        if (fields[key] === "" || isNaN(fields[key])) {
            alert("Semua field harus berisi angka!");
            return;
        }
    }

    // Kirim data ke ESP32 (POST)
    try {
        const res = await fetch("/setIP", {
            method: "POST",
            headers: { "Content-Type": "application/x-www-form-urlencoded" },
            body: new URLSearchParams(fields).toString()
        });

        if (!res.ok) throw new Error("HTTP " + res.status);

        const text = await res.text();
        alert(text); // ESP32 akan mengirim pesan sukses

    } catch (err) {
        console.error("ERROR applyIPConfig():", err);
        alert("Gagal mengirim data ke ESP32!");
    }
}


// GET IP
async function getIPConfig() {
    try {
        const res = await fetch("/getip");
        if (!res.ok) throw new Error("HTTP " + res.status);

        const data = await res.json();

        if (data.status === "empty") {
            updateIpUI("-", "-", "-","-","-");
            return;
        }

        updateIpUI(
            "IP Address = " + data.ip,
            "Gateway = " + data.gateway,
            "Subnet Mask = " + data.subnet,
            "Primary DNS = " + data.dns1,
            "Secondary DNS = " + data.dns2
        );
    }
    catch (err) {
        console.error("Gagal ambil data IP Static:", err);
        updateIpUI("-", "-", "-", "-", "-");
    }
}

// UI untuk IP Address
function updateIpUI(ip, gateway , subnet, dns1, dns2 ) {
    document.getElementById("showIp").textContent = ip;
    document.getElementById("showSubnet").textContent = subnet;
    document.getElementById("showGw").textContent = gateway;
    document.getElementById("showStDns").textContent = dns1;
    document.getElementById("showNdDns").textContent = dns2;
}















