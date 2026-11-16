# CLI Library Management System (Sistem Manajemen Perpustakaan Berbasis CLI)

Project Akhir (UAS) Praktikum Pemrograman Kelas A - Program Studi Informatika FMIPA Universitas Syiah Kuala (USK) Tahun 2026.

## 👥 Anggota Kelompok
| Nama Lengkap | NPM | Kontribusi Modul Utama |
| :--- | :--- | :--- |
| **[Nama Anggota 1]** | [NPM 1] | Core System, Struktur Data (`models.h`), & File I/O Engine |
| **[Nama Anggota 2]** | [NPM 2] | Modul Admin CRUD (`Create`, `Update`, `Delete`) & Laporan Global[cite: 1] |
| **[Nama Anggota 3]** | [NPM 3] | Modul Sirkulasi Anggota (`Borrow`, `Return`, Search, & Personal Loans)[cite: 1] |
| **[Nama Anggota 4]** | [NPM 4] | Mesin 3 Algoritma Sorting Manual (`Bubble`, `Selection`, `Insertion`) & CLI Utils[cite: 1] |

---

## 🏛️ Arsitektur Aplikasi & Struktur File
Aplikasi ini dibangun menggunakan bahasa C standar (ANSI/C) dengan prinsip *clean code* dan modularitas tinggi[cite: 1]. 
* **`models.h`**: Kontrak struktur data utama (`Book`, `User`, `Loan`) menggunakan `struct` dan `typedef`[cite: 1].
* **`file_io.c` / `.h`**: Mesin pembaca dataset master (`Books.txt`)[cite: 2] dan pengelola basis data operasional (`books_db.txt`, `user.txt`, `peminjaman.txt`) secara persisten[cite: 1].
* **`auth.c` / `.h`**: Gerbang autentikasi dengan pilihan role eksplisit (Admin / Mahasiswa)[cite: 1] serta dukungan *Command Line Arguments* (CLA)[cite: 1].
* **`sort.c` / `.h`**: Implementasi mandiri 3 algoritma pengurutan tanpa pustaka eksternal (`qsort`)[cite: 1] serta sistem navigasi halaman (*pagination*).
* **`admin.c` / `.h`**: Fungsionalitas penuh Admin untuk manajemen inventaris buku dan audit transaksi[cite: 1].
* **`user.c` / `.h`**: Fungsionalitas pencarian string fleksibel dan sirkulasi peminjaman buku berbasis tanggal sistem[cite: 1].

---

## ⚙️ Panduan Kompilasi & Menjalankan Program

Pastikan kompiler `gcc` telah terpasang di sistem Anda. Buka terminal/PowerShell pada direktori proyek, lalu jalankan perintah kompilasi berikut[cite: 1]:

```powershell
gcc -Wall -Wextra -std=c99 main.c file_io.c utils.c auth.c sort.c admin.c user.c -o perpustakaan