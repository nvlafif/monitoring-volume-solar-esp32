document.addEventListener('DOMContentLoaded', function() {
    // Load data saat halaman pertama dibuka
    loadHistory();

// Optional: reload otomatis setiap 10 detik
// setInterval(loadHistory, 10000);

});
const tbody = document.getElementById('histbody');

// Fungsi untuk fetch data dari PHP endpoint
function loadHistory() {
    fetch('http://10.248.168.13/monitoring/save.php')
        .then(function(response) {
            if (!response.ok) {
                throw new Error('Network response was not ok');
            }
            return response.json();
        })
        .then(function(data) {
            tbody.innerHTML = ''; // Kosongkan tabel dulu
            if (data.length === 0) {
                tbody.innerHTML = '<tr><td colspan="3">Tidak ada data</td></tr>';
                return;
            }

            data.forEach(function(item) {
                const tr = document.createElement('tr');

                const tdNo = document.createElement('td');
                tdNo.textContent = item.no;

                const tdDate = document.createElement('td');
                tdDate.textContent = item.date_time;

                const tdVolume = document.createElement('td');
                tdVolume.textContent = parseFloat(item.volume).toFixed(2);

                tr.appendChild(tdNo);
                tr.appendChild(tdDate);
                tr.appendChild(tdVolume);

                tbody.appendChild(tr);
            });
        })
        .catch(function(error) {
            console.error('Error fetching history:', error);
            tbody.innerHTML = '<tr><td colspan="3">Gagal memuat data</td></tr>';
        });
}

// HISTORY
let counter = 1;   // Mulai dari row nomor 1

function goHome() {
    window.location.href = "index.html";
}


