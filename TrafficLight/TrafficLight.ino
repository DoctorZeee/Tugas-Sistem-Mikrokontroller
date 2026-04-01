/*
 * ============================================================
 *  SIMULASI LAMPU LALU LINTAS 4 ARAH (PERSIMPANGAN)
 *  Arah: A (Utara), B (Timur), C (Selatan), D (Barat)
 *  Platform: Arduino UNO
 * ============================================================
 */

// Struktur data untuk setiap arah
struct TrafficLight {
  const char* nama;
  uint8_t pinMerah;
  uint8_t pinKuning;
  uint8_t pinHijau;
};

// Definisi 4 arah persimpangan
const TrafficLight arah[] = {
  { "Utara",  5,  6,  7  }, //Nama, pinMerah, pinKuning, pinHijau
  { "Timur",   2,  3,  4  }, //Nama, pinMerah, pinKuning, pinHijau
  { "Selatan",   11,  12,  13 }, //Nama, pinMerah, pinKuning, pinHijau
  { "Barat",  8, 9, 10 }  //Nama, pinMerah, pinKuning, pinHijau
};

const uint8_t JUMLAH_ARAH = sizeof(arah) / sizeof(arah[0]);

// Waktu (ms)
const uint16_t DURASI_HIJAU        = 5000;  // Hijau menyala 5 detik
const uint16_t DURASI_KEDIP_KUNING = 333;   // Periode kedip kuning (on/off masing-masing)
const uint8_t  JUMLAH_KEDIP        = 3;     // Berapa kali kuning berkedip
const uint16_t JEDA_ANTAR_ARAH     = 300;   // Jeda singkat sebelum arah berikutnya
const uint16_t JEDA_MERAH_AWAL     = 2000;  // Jeda merah semua sebelum mulai kedip (ms)


// ============================================================
//  FUNGSI UTILITAS
// ============================================================

// Matikan semua LED pada satu arah
void matikanArah(uint8_t i) {
  digitalWrite(arah[i].pinMerah,  LOW);
  digitalWrite(arah[i].pinKuning, LOW);
  digitalWrite(arah[i].pinHijau,  LOW);
}

// Nyalakan merah, matikan lainnya pada satu arah
void nyalakanMerah(uint8_t i) {
  digitalWrite(arah[i].pinMerah,  HIGH);
  digitalWrite(arah[i].pinKuning, LOW);
  digitalWrite(arah[i].pinHijau,  LOW);
}

// Semua arah merah (kondisi default / reset)
void semuaMerah() {
  for (uint8_t i = 0; i < JUMLAH_ARAH; i++) {
    nyalakanMerah(i);
  }
}

// Kuning berkedip N kali pada satu arah
void kuningKedip(uint8_t i) {
  digitalWrite(arah[i].pinMerah, LOW);  // Matikan merah saat transisi
  for (uint8_t k = 0; k < JUMLAH_KEDIP; k++) {
    digitalWrite(arah[i].pinKuning, HIGH);
    delay(DURASI_KEDIP_KUNING);
    digitalWrite(arah[i].pinKuning, LOW);
    delay(DURASI_KEDIP_KUNING);
  }
}

// Nyalakan hijau, matikan lainnya pada satu arah
void nyalakanHijau(uint8_t i) {
  digitalWrite(arah[i].pinMerah,  LOW);
  digitalWrite(arah[i].pinKuning, LOW);
  digitalWrite(arah[i].pinHijau,  HIGH);
  delay(DURASI_HIJAU);
  digitalWrite(arah[i].pinHijau,  LOW);
}

// Urutan lengkap satu siklus untuk satu arah:
void siklus(uint8_t i) {
  delay(JEDA_MERAH_AWAL);  // Tahan merah semua agar terlihat jelas
  kuningKedip(i);           // Kuning berkedip sebelum hijau
  nyalakanHijau(i);         // Hijau menyala selama DURASI_HIJAU
  nyalakanMerah(i);         // Kembali ke merah
  delay(JEDA_ANTAR_ARAH);
}

void setup() {
  // Inisialisasi semua pin sebagai OUTPUT
  for (uint8_t i = 0; i < JUMLAH_ARAH; i++) {
    pinMode(arah[i].pinMerah,  OUTPUT);
    pinMode(arah[i].pinKuning, OUTPUT);
    pinMode(arah[i].pinHijau,  OUTPUT);
  }

  // Kondisi awal semua merah
  semuaMerah();
  delay(3000);
}

void loop() {
  for (uint8_t i = 0; i < JUMLAH_ARAH; i++) {
    siklus(i);    // Jalankan satu siklus untuk arah ke-i
    semuaMerah(); // Reset semua ke merah setelah satu arah selesai
  }
}
