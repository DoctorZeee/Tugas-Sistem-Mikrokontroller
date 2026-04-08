// ============================================================
//  TRAFFIC LIGHT - Logika yang Benar
//  Default: Kendaraan MERAH, Pedestrian MERAH
//  Tekan tombol (dari MERAH):
//    1. Pedestrian HIJAU (5 detik)
//    2. Pedestrian MERAH
//    3. Kuning kedip (transisi ke kendaraan)
//    4. Kendaraan HIJAU (minimal 5 detik)
//  Tekan tombol (dari HIJAU setelah ≥5 detik):
//    1. Kuning kedip (peringatan)
//    2. Kembali ke MERAH (default)
// ============================================================

// Pin Kendaraan
const int KEND_HIJAU  = 4;
const int KEND_KUNING = 5;
const int KEND_MERAH  = 6;

// Pin Pedestrian
const int PED_KIRI_HIJAU = 7;
const int PED_KIRI_MERAH = 8;
const int PED_KANAN_HIJAU = 2;
const int PED_KANAN_MERAH = 3;

// Pin Tombol
const int TOMBOL_KIRI  = 12;
const int TOMBOL_KANAN = 11;

// Durasi
const unsigned long DURASI_PEDESTRIAN = 5000;
const unsigned long DURASI_HIJAU_MIN  = 5000;
const unsigned long KUNING_ON  = 300;
const unsigned long KUNING_OFF = 300;
const int JUMLAH_KEDIP = 4;

// Mode
// 0 = MERAH (default)
// 1 = Pedestrian HIJAU
// 2 = Pedestrian MERAH (sebelum transisi ke kendaraan)
// 3 = Kuning transisi ke HIJAU
// 4 = Kendaraan HIJAU
// 5 = Kuning transisi ke MERAH
int mode = 0;

unsigned long waktuMode = 0;
unsigned long waktuKedip = 0;
int hitungKedip = 0;
bool kuningNyala = false;
bool tombolDitunda = false;

void setup() {
  pinMode(KEND_MERAH,  OUTPUT);
  pinMode(KEND_KUNING, OUTPUT);
  pinMode(KEND_HIJAU,  OUTPUT);
  pinMode(PED_KIRI_MERAH,  OUTPUT);
  pinMode(PED_KIRI_HIJAU,  OUTPUT);
  pinMode(PED_KANAN_MERAH, OUTPUT);
  pinMode(PED_KANAN_HIJAU, OUTPUT);
  pinMode(TOMBOL_KIRI,  INPUT_PULLUP);
  pinMode(TOMBOL_KANAN, INPUT_PULLUP);

  kondisiMerah(); // Mulai dengan MERAH
}

void loop() {
  // Baca tombol (hanya transisi dari tidak ditekan ke ditekan)
  static bool lastTombol = false;
  bool tombolSekarang = tombolDitekan();
  bool tombolBaru = !lastTombol && tombolSekarang;
  lastTombol = tombolSekarang;

  if (tombolBaru) {
    if (mode == 0) {  // Dari MERAH -> langsung pedestrian hijau
      mode = 1;
      waktuMode = millis();
      kondisiPedestrianHijau();
    }
    else if (mode == 4) { // Dari HIJAU
      unsigned long lamaHijau = millis() - waktuMode;
      if (lamaHijau >= DURASI_HIJAU_MIN) {
        mode = 5;
        waktuMode = millis();
        mulaiKuningKeMerah();
      } else {
        tombolDitunda = true; // Tunda sampai cukup 5 detik
      }
    }
    // Mode lain diabaikan
  }

  unsigned long sekarang = millis();

  switch (mode) {
    case 1: // Pedestrian HIJAU 5 detik
      if (sekarang - waktuMode >= DURASI_PEDESTRIAN) {
        mode = 2;
        waktuMode = sekarang;
        kondisiPedestrianMerah();
      }
      break;

    case 2: // Pedestrian MERAH -> langsung ke kuning transisi
      mode = 3;
      waktuMode = sekarang;
      mulaiKuningTransisiKeHijau();
      break;

    case 3: // Kuning transisi ke HIJAU
      if (prosesKedip(sekarang)) {
        mode = 4;
        waktuMode = sekarang;
        tombolDitunda = false;
        kondisiHijau();
      }
      break;

    case 4: // HIJAU
      if (tombolDitunda && (sekarang - waktuMode >= DURASI_HIJAU_MIN)) {
        tombolDitunda = false;
        mode = 5;
        waktuMode = sekarang;
        mulaiKuningKeMerah();
      }
      break;

    case 5: // Kuning transisi ke MERAH
      if (prosesKedip(sekarang)) {
        mode = 0;
        kondisiMerah();
      }
      break;
  }
}

// ------------------------------------------------------------------
// Fungsi Bantu
// ------------------------------------------------------------------
bool tombolDitekan() {
  bool kiri  = (digitalRead(TOMBOL_KIRI)  == LOW);
  bool kanan = (digitalRead(TOMBOL_KANAN) == LOW);
  if (kiri || kanan) {
    delay(50); // debounce
    kiri  = (digitalRead(TOMBOL_KIRI)  == LOW);
    kanan = (digitalRead(TOMBOL_KANAN) == LOW);
    return (kiri || kanan);
  }
  return false;
}

void kondisiMerah() {
  matikanSemua();
  digitalWrite(KEND_MERAH, HIGH);
  digitalWrite(PED_KIRI_MERAH, HIGH);
  digitalWrite(PED_KANAN_MERAH, HIGH);
}

void kondisiPedestrianHijau() {
  matikanSemua();
  digitalWrite(KEND_MERAH, HIGH);
  digitalWrite(PED_KIRI_HIJAU, HIGH);
  digitalWrite(PED_KANAN_HIJAU, HIGH);
}

void kondisiPedestrianMerah() {
  digitalWrite(PED_KIRI_HIJAU, LOW);
  digitalWrite(PED_KANAN_HIJAU, LOW);
  digitalWrite(PED_KIRI_MERAH, HIGH);
  digitalWrite(PED_KANAN_MERAH, HIGH);
  // Kendaraan merah tetap menyala
}

void kondisiHijau() {
  matikanSemua();
  digitalWrite(KEND_HIJAU, HIGH);
  digitalWrite(PED_KIRI_MERAH, HIGH);
  digitalWrite(PED_KANAN_MERAH, HIGH);
}

void mulaiKuningTransisiKeHijau() {
  // Pedestrian merah sudah nyala, kendaraan merah juga nyala
  hitungKedip = 0;
  kuningNyala = false;
  waktuKedip = millis();
}

void mulaiKuningKeMerah() {
  // Matikan hijau, nyalakan pedestrian merah
  digitalWrite(KEND_HIJAU, LOW);
  digitalWrite(PED_KIRI_MERAH, HIGH);
  digitalWrite(PED_KANAN_MERAH, HIGH);
  hitungKedip = 0;
  kuningNyala = false;
  waktuKedip = millis();
}

bool prosesKedip(unsigned long sekarang) {
  if (kuningNyala) {
    if (sekarang - waktuKedip >= KUNING_ON) {
      digitalWrite(KEND_KUNING, LOW);
      kuningNyala = false;
      waktuKedip = sekarang;
      hitungKedip++;
      if (hitungKedip >= JUMLAH_KEDIP) {
        digitalWrite(KEND_KUNING, LOW);
        return true;
      }
    }
  } else {
    if (sekarang - waktuKedip >= KUNING_OFF) {
      digitalWrite(KEND_KUNING, HIGH);
      kuningNyala = true;
      waktuKedip = sekarang;
    }
  }
  return false;
}

void matikanSemua() {
  digitalWrite(KEND_MERAH,  LOW);
  digitalWrite(KEND_KUNING, LOW);
  digitalWrite(KEND_HIJAU,  LOW);
  digitalWrite(PED_KIRI_MERAH,  LOW);
  digitalWrite(PED_KIRI_HIJAU,  LOW);
  digitalWrite(PED_KANAN_MERAH, LOW);
  digitalWrite(PED_KANAN_HIJAU, LOW);
}