#include <EEPROM.h>
#include <LiquidCrystal.h>

// Inisialisasi LCD
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

long duration, inches;
int set_val, percentage;
bool state, pump;

void setup() {
  // Inisialisasi LCD
  lcd.begin(16, 2);
  lcd.print("WATER LEVEL:");
  lcd.setCursor(0, 1); 
  lcd.print("PUMP:OFF MANUAL");
  
  // Inisialisasi pin
  pinMode(8, OUTPUT);         // Pin trig sensor ultrasonik
  pinMode(9, INPUT);          // Pin echo sensor ultrasonik
  pinMode(10, INPUT_PULLUP);  // Tombol pengaturan
  pinMode(11, INPUT_PULLUP);  // Tombol mode (manual/otomatis)
  pinMode(12, OUTPUT);        // Kontrol relay
  
  // Membaca nilai set point dari EEPROM
  set_val = EEPROM.read(0);
  if (set_val > 150) set_val = 150;
}

void loop() {
  // Mengirim sinyal trigger ke sensor ultrasonik
  digitalWrite(3, LOW);
  delayMicroseconds(2);
  digitalWrite(8, HIGH);
  delayMicroseconds(10);
  digitalWrite(8, LOW);
  
  // Menghitung durasi pantulan sinyal
  duration = pulseIn(9, HIGH);
  inches = microsecondsToInches(duration);
  
  // Menghitung persentase ketinggian air
  percentage = (set_val - inches) * 100 / set_val;
  
  // Menampilkan persentase di LCD
  lcd.setCursor(12, 0); 
  if (percentage < 0) percentage = 0;
  lcd.print(percentage);
  lcd.print("%   ");
  
  // Logika kontrol pompa berdasarkan ketinggian air dan mode
  if (percentage < 30 & digitalRead(11)) pump = 1;
  if (percentage > 99) pump = 0;
  digitalWrite(12, !pump);
  
  // Menampilkan status pompa di LCD
  lcd.setCursor(5, 1);
  if (pump == 1) lcd.print("ON ");
  else if (pump == 0) lcd.print("OFF");
  
  // Menampilkan mode (manual/otomatis) di LCD
  lcd.setCursor(9, 1);
  if (!digitalRead(11)) lcd.print("MANUAL");
  else lcd.print("AUTO   ");
  
  // Mengatur nilai set point atau mengubah status pompa
  if (!digitalRead(10) & !state & digitalRead(11)) {
    state = 1;
    set_val = inches;
    EEPROM.write(0, set_val);
  }
  
  if (!digitalRead(10) & !state & !digitalRead(11)) {
    state = 1;
    pump = !pump;
  }
  
  if (digitalRead(10)) state = 0;
  
  delay(500);
}

// Fungsi untuk mengonversi waktu microsecond menjadi inches
long microsecondsToInches(long microseconds) {
  return microseconds / 74 / 2;
}
