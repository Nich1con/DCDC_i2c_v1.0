/*
   Скетч для тестирования управляемого
   DC-DC на основе ЦАП MCP4725

   Выходное напряжение:
   Vout = Vref * K
   K = (Rup / Rdwn) + 1
      > Vref  - опорное напряжение с выхода ЦАП
      > K     - коэф. деления обратной связи
      > Rup   - сопротивление верхнего плеча делителя
      > Rdwn  - сопротивление нижнего плеча делителя

   Получения 12-битного значения для ЦАП (напряжение в мВ):
   Data = (Vout * 4096UL) / (DAvcc * K)
      > Data  - значение для отправки в ЦАП
      > Vout  - требуемое выходное напряжение
      > DAvcc - напряжение питания ЦАП (опорное напряжение ЦАП)
      > K     - коэф. деления обратной связи

   Пример:
      > Vout = 15000 мВ (15 В)
      > DAvcc = 5000 мВ (5В)
      > K = 6.85
   Data = (15000 * 4096UL) / (5000 * 6.85f) = 1793
*/

#include <Wire.h>

#define MCP4725_ADDR 0x60 // Адрес MCP на шине I2c
float KDiv = 6.85f;       // Рассчитанный коэф. деления
uint16_t DAvcc = 5037;    // Напряжение питания ЦАП
uint16_t Vout = 24000;    // Выходное напряжение по умолчанию

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(50);
  Wire.begin();

  /* Устанавливаем напряжение по умолчанию */
  uint16_t data = ((uint32_t)Vout * 4096UL) / ((float)DAvcc * KDiv);
  dac_write(data);
}

void loop() {
  /*
      Команды для управления из Serial (примеры):
      1. Установка K = 6.84:          K6.84
      2. Установка Vout = 15000 мВ:   V15000
      3. Установка DAvcc = 4900 мВ:   R4900

      Установка монитора порта - 9600, нет конца строки
  */

  if (Serial.available() > 1) { // Если доступны данные
    switch (Serial.read()) {    // Парсинг переменных
      case 'K': KDiv = Serial.parseFloat(); break;
      case 'V': Vout = Serial.parseInt();   break;
      case 'R': DAvcc = Serial.parseInt();  break;
    }
    /* Рассчет и обновление значения ЦАП */
    uint16_t data = ((uint32_t)Vout * 4096UL) / ((float)DAvcc * KDiv);
    dac_write(data);
  }
}

void dac_write(uint16_t data) {
  Wire.beginTransmission(MCP4725_ADDR);
  Wire.write(highByte(data));
  Wire.write(lowByte(data));
  Wire.endTransmission();
}
