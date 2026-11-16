📖 CLI Library Management System
> **Proyek Akhir (UAS) Praktikum Pemrograman - Informatika FMIPA Universitas Syiah Kuala (2026)**  
> Sistem manajemen perpustakaan interaktif berbasis *Command Line Interface* (CLI) yang diimplementasikan murni dalam bahasa C standar (C99).

✨ Fitur Utama
Sistem dirancang dengan arsitektur memori Static Data Segment untuk memproses 10.000+ data buku secara instan dan aman dari risiko Stack Overflow.

🔐 Role-Based Authentication: Gerbang masuk terpisah untuk Admin dan Mahasiswa, dilengkapi bypass login super cepat menggunakan Command Line Arguments (CLA).

⚙️ Algoritma Sorting Native: Menyediakan 3 mesin pengurutan manual (Bubble Sort, Selection Sort, Insertion Sort) yang dilengkapi metrik pengukuran waktu eksekusi aktual (dalam milidetik).

🛡️ Smart File I/O Parser: Mesin pembaca (custom parser) tangguh yang mampu menangani anomali format dataset `Books.txt` (seperti escaped quotes `""` atau judul tanpa tanda petik). Data master aman (Read-Only) karena sistem otomatis membuat basis data operasional `books_db.txt`.

📄 Sistem Paginasi Terminal: Menampilkan katalog data dalam bentuk tabel rapi berhalaman (15 buku/layar) untuk kenyamanan navigasi (Next, Prev, Go to Page).

🔄 Sirkulasi Relasional: Validasi real-time untuk peminjaman (stok otomatis berkurang, pengambilan tanggal sistem otomatis) dan proteksi ketat (mencegah Admin menghapus buku yang sedang berstatus `DIPINJAM`).

📂 Arsitektur & Struktur Modul
Setiap fitur dipisahkan secara modular untuk mempermudah perawatan kode (clean code):
Berkas	Modul Utama	Tanggung Jawab Teknis
`models.h`	Data Contract	Definisi struktur `Book`, `User`, `Loan` & limitasi kapasitas konstan.
`file_io.c/h`	Storage Engine	Sinkronisasi I/O persisten (`books_db.txt`, `user.txt`, `peminjaman.txt`).
`auth.c/h`	Security	Otorisasi Role-based, CLA Login parsers, dan registrasi akun baru.
`sort.c/h`	Algorithm	Mesin 3 algoritma pengurutan & antarmuka pagination katalog.
`admin.c/h`	Admin Panel	CRUD katalog, sinkronisasi stok, dan audit rekapitulasi transaksi.
`user.c/h`	User Panel	Mesin pencari case-insensitive & sirkulasi pinjam-kembali mandiri.
`utils.c/h`	Core Utils	Pembersih buffer & validasi input ketat (mencegah infinite loop).

🚀 Panduan Kompilasi & Eksekusi
Proyek ini telah dilengkapi dengan `Makefile` untuk mempercepat proses kompilasi secara inkremental (hanya mengompilasi berkas yang berubah).
1. Kompilasi (Build)
Buka terminal pada direktori proyek dan jalankan perintah:
```bash
make
```
2. Menjalankan Aplikasi (Run)
Mode Interaktif (Normal):
```bash
./perpustakaan
```
Mode Fast-Login (Via CLA):
```bash
# Format langsung: ./perpustakaan <username> <password>
./perpustakaan admin admin123

# Format flags: ./perpustakaan -u <username> -p <password>
./perpustakaan -u mahasiswa -p mhs123
```
3. Pembersihan (Clean)
Menghapus seluruh berkas objek `.o` dan executable hasil kompilasi:
```bash
make clean
```
👥 Tim Pengembang
Nama Lengkap	NPM	Fokus Pengembangan Modul
[Nama Anggota 1]	[NPM 1]	Core System, Data Structures, & File I/O Engine
[Nama Anggota 2]	[NPM 2]	Admin Panel (CRUD, Relational Protection), Global Reports
[Nama Anggota 3]	[NPM 3]	User Sirkulasi (Borrow/Return), Search Engine
[Nama Anggota 4]	[NPM 4]	Sorting Engine (Bubble/Selection/Insertion), CLI Utilities
---
Repositori ini dinilai berdasarkan transparansi kontribusi melalui riwayat Commit dan Pull Request.