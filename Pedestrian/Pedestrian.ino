// ============================================================
// TRAFFIC LIGHT sederhana dengan INTERRUPT
// State awal: Kendaraan HIJAU, Pedestrian MERAH
// Tombol Kiri  -> Pin 2 (INT0)
// Tombol Kanan -> Pin 3 (INT1)
// ============================================================

// Pin Kendaraan
const int KEND_HIJAU  = 6;
const int KEND_KUNING = 7;
const int KEND_MERAH  = 8;

// Pin Pedestrian Kiri
const int PED_KIRI_HIJAU  = 4;
const int PED_KIRI_MERAH  = 5;

// Pin Pedestrian Kanan
const int PED_KANAN_HIJAU  = 9;
const int PED_KANAN_MERAH  = 10;

// Pin Tombol (harus pin 2 & 3 untuk external interrupt Arduino UNO)
const int TOMBOL_KIRI  = 2;   // INT0
const int TOMBOL_KANAN = 3;   // INT1

// Durasi (milidetik)
const int DURASI_PEDESTRIAN = 5000;
const int DURASI_DEBOUNCE   = 50;
const int DURASI_KUNING_ON  = 300;
const int DURASI_KUNING_OFF = 300;
const int JUMLAH_KEDIP      = 4;

// Flag global (volatile karena diubah dalam ISR)
volatile bool tombolDitekan = false;
bool sedangSibuk = false;

void setup() {
  // Output LED
  pinMode(KEND_HIJAU,      OUTPUT);
  pinMode(KEND_KUNING,     OUTPUT);
  pinMode(KEND_MERAH,      OUTPUT);
  pinMode(PED_KIRI_HIJAU,  OUTPUT);
  pinMode(PED_KIRI_MERAH,  OUTPUT);
  pinMode(PED_KANAN_HIJAU, OUTPUT);
  pinMode(PED_KANAN_MERAH, OUTPUT);

  // Input tombol dengan pull-up internal
  pinMode(TOMBOL_KIRI,  INPUT_PULLUP);
  pinMode(TOMBOL_KANAN, INPUT_PULLUP);

  // Pasang interrupt pada pin 2 dan 3 (FALLING: tombol ditekan = LOW)
  attachInterrupt(digitalPinToInterrupt(TOMBOL_KIRI), isrTombol, FALLING);
  attachInterrupt(digitalPinToInterrupt(TOMBOL_KANAN), isrTombol, FALLING);

  kondisiAwal();
}

void loop() {
  // Periksa flag dari interrupt
  if (tombolDitekan && !sedangSibuk) {
    delay(DURASI_DEBOUNCE);   // debounce singkat

    // Konfirmasi ulang tombol benar-benar ditekan
    bool kiri  = (digitalRead(TOMBOL_KIRI)  == LOW);
    bool kanan = (digitalRead(TOMBOL_KANAN) == LOW);

    if (kiri || kanan) {
      sedangSibuk = true;     // kunci agar tidak dipicu ulang
      tombolDitekan = false;  // reset flag
      modePenyebrang();       // jalankan siklus penyebrangan
      sedangSibuk = false;    // buka kunci
    } else {
      // False trigger (bouncing), abaikan
      tombolDitekan = false;
    }
  }
}

// ------------------------------------------------------------------
// Interrupt Service Routine (dipanggil saat tombol ditekan)
// ------------------------------------------------------------------
void isrTombol() {
  tombolDitekan = true;   // hanya set flag, jangan lakukan delay di sini!
}

// ------------------------------------------------------------------
// Fungsi-fungsi kontrol lampu
// ------------------------------------------------------------------
void kondisiAwal() {
  matikanSemua();
  digitalWrite(KEND_HIJAU,      HIGH);
  digitalWrite(PED_KIRI_MERAH,  HIGH);
  digitalWrite(PED_KANAN_MERAH, HIGH);
}

void modePenyebrang() {
  // Transisi ke pedestrian hijau
  matikanSemua();
  digitalWrite(KEND_MERAH,      HIGH);
  digitalWrite(PED_KIRI_HIJAU,  HIGH);
  digitalWrite(PED_KANAN_HIJAU, HIGH);

  delay(DURASI_PEDESTRIAN);

  // Pedestrian merah, kendaraan masih merah
  digitalWrite(PED_KIRI_HIJAU,  LOW);
  digitalWrite(PED_KANAN_HIJAU, LOW);
  digitalWrite(PED_KIRI_MERAH,  HIGH);
  digitalWrite(PED_KANAN_MERAH, HIGH);

  // Transisi kuning kedip lalu kembali ke kondisi awal
  digitalWrite(KEND_MERAH, LOW);
  kedipKuning();
  kondisiAwal();
}

void matikanSemua() {
  digitalWrite(KEND_HIJAU,      LOW);
  digitalWrite(KEND_KUNING,     LOW);
  digitalWrite(KEND_MERAH,      LOW);
  digitalWrite(PED_KIRI_HIJAU,  LOW);
  digitalWrite(PED_KIRI_MERAH,  LOW);
  digitalWrite(PED_KANAN_HIJAU, LOW);
  digitalWrite(PED_KANAN_MERAH, LOW);
}

void kedipKuning() {
  for (int i = 0; i < JUMLAH_KEDIP; i++) {
    digitalWrite(KEND_KUNING, HIGH);
    delay(DURASI_KUNING_ON);
    digitalWrite(KEND_KUNING, LOW);
    delay(DURASI_KUNING_OFF);
  }
  digitalWrite(KEND_KUNING, LOW);
}