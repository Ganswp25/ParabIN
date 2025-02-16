const int sensorPin = A0;

// Batasan untuk menentukan status kekeruhan air (dalam milivolt)
const float clearThreshold = 300.0; // Jernih jika >= 300
const float murkyThreshold = 150.0; // Kusam jika >= 150 dan < 300
// Di bawah 150 dianggap kotor

// Fungsi untuk menghitung keanggotaan fuzzy untuk kekeruhan rendah
float fuzzyMembershipLow(float x) {
  if (x <= murkyThreshold) {
    return 1.0;
  } else if (x >= clearThreshold) {
    return 0.0;
  } else {
    return (clearThreshold - x) / (clearThreshold - murkyThreshold);
  }
}

// Fungsi untuk menghitung keanggotaan fuzzy untuk kekeruhan sedang
float fuzzyMembershipMedium(float x) {
  if (x <= murkyThreshold || x >= clearThreshold) {
    return 0.0;
  } else if (x > murkyThreshold && x < clearThreshold) {
    return (x - murkyThreshold) / (clearThreshold - murkyThreshold);
  } else {
    return (clearThreshold - x) / (clearThreshold - murkyThreshold);
  }
}

// Fungsi untuk menghitung keanggotaan fuzzy untuk kekeruhan tinggi
float fuzzyMembershipHigh(float x) {
  if (x <= clearThreshold) {
    return 0.0;
  } else if (x >= 3000.0) {
    return 1.0;
  } else {
    return (x - clearThreshold) / (3000.0 - clearThreshold);
  }
}

// Fungsi untuk menentukan status kekeruhan berdasarkan keanggotaan fuzzy
String fuzzyStatus(float voltage, int sensorValue, float low, float medium, float high) {
  // Kondisi khusus untuk tegangan di antara 500 sampai 850 mV dan nilai sensor di antara 600 sampai 850
  if (voltage >= 500.0 && voltage <= 850.0 && sensorValue >= 600 && sensorValue <= 850) {
    return "Kusam";
  }

  if (low >= medium && low >= high) {
    return "Kotor";
  } else if (medium >= low && medium >= high) {
    return "Kusam";
  } else {
    return "Jernih";
  }
}

void setup() {
  // Memulai komunikasi serial pada baud rate 9600
  Serial.begin(9600);
}

void loop() {
  int sensorValue = analogRead(sensorPin);
  float voltage = sensorValue * (3300.0 / 4096.0);

  // Hitung keanggotaan fuzzy
  float lowMembership = fuzzyMembershipLow(voltage);
  float mediumMembership = fuzzyMembershipMedium(voltage);
  float highMembership = fuzzyMembershipHigh(voltage);

  // Menentukan status kekeruhan berdasarkan keanggotaan fuzzy dan kondisi tambahan
  String status = fuzzyStatus(voltage, sensorValue, lowMembership, mediumMembership, highMembership);

  // Print nilai sensor, tegangan (dalam milivolt), dan status kekeruhan ke serial monitor
  Serial.print("Nilai sensor: ");
  Serial.print(sensorValue);
  Serial.print("\t Tegangan (mV): ");
  Serial.print(voltage);
  Serial.print("\t Status: ");
  Serial.println(status);

  // Tunggu 1 detik sebelum membaca lagi
  delay(1000);
}
