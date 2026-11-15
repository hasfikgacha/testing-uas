#include <stdio.h>
#include "models.h"
#include "file_io.h"

/* Deklarasi static agar memori dialokasikan di Data Segment (aman untuk >10.000 struct) */
static Book catalog[MAX_BOOKS];
static User users[MAX_USERS];
static Loan loans[MAX_LOANS];

int main(void) {
    int book_count = 0;
    int user_count = 0;
    int loan_count = 0;
    int i;

    printf("============================================================\n");
    printf("   UJI COBA TAHAP 1: INISIALISASI & PARSING DATASET BUKU    \n");
    printf("============================================================\n\n");

    /* 1. Muat dataset buku (otomatis menyalin Books.txt -> books_db.txt pada run pertama) */
    book_count = init_and_load_books(catalog, MAX_BOOKS);
    if (book_count < 0) {
        printf("Program dihentikan karena dataset tidak ditemukan.\n");
        return 1;
    }

    /* 2. Muat data user dan peminjaman */
    user_count = load_users(users, MAX_USERS);
    loan_count = load_loans(loans, MAX_LOANS);

    printf("\n[RINGKASAN DATA DI MEMORI]\n");
    printf("- Total Buku Terbaca      : %d buku\n", book_count);
    printf("- Total Akun Terdaftar    : %d akun\n", user_count);
    printf("- Total Rekaman Transaksi : %d transaksi\n\n", loan_count);

    /* 3. Tampilkan 5 buku pertama untuk memvalidasi ketepatan parser */
    printf("[ SAMPEL 5 BUKU PERTAMA ]\n");
    printf("%-28s | %-35s | %-6s | %-5s | %-4s\n", "OCAID", "JUDUL (Potongan)", "TAHUN", "HAL", "STOK");
    printf("---------------------------------------------------------------------------------------\n");
    for (i = 0; i < 5 && i < book_count; i++) {
        printf("%-28.28s | %-35.35s | %-6d | %-5d | %-4d\n",
               catalog[i].ocaid,
               catalog[i].title,
               catalog[i].year,
               catalog[i].page_count,
               catalog[i].stock);
    }

    /* 4. Tampilkan sampel baris ke-32 (indeks 31) yang judulnya 1 kata tanpa tanda petik ("Poems") */
    if (book_count > 31) {
        printf("\n[ UJI EDGE-CASE: JUDUL 1 KATA TANPA TANDA PETIK (Indeks 31) ]\n");
        printf("OCAID  : %s\n", catalog[31].ocaid);
        printf("Judul  : %s\n", catalog[31].title);
        printf("Penulis: %s\n", catalog[31].author);
        printf("Tahun  : %d | Halaman: %d | Stok: %d\n",
               catalog[31].year, catalog[31].page_count, catalog[31].stock);
    }

    return 0;
}