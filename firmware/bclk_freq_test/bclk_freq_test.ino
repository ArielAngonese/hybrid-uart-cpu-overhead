/*
  bclk_freq_test.ino - mede a frequencia do BCLK usando o proprio Arduino
  Arduino Uno (ATmega328P, 16 MHz)

  O BCLK sai no pino 9 (OC1A, Timer1, modo CTC, 9603,8 Hz esperado).
  O pino 2 (INT0) conta as bordas de subida e o sketch imprime a
  frequencia medida a cada 2 segundos no Serial Monitor (9600 baud).

  LIGACAO DO TESTE:
    jumper do pino 9 ao pino 2
    (o LED com resistor no pino 9 pode ficar ligado, nao atrapalha)

  Esperado: perto de 9604 Hz (aceitavel: 9580 a 9630 Hz).
  Obs.: a medicao usa millis() (Timer0) e o clock vem do Timer1; os dois
  usam o mesmo cristal, entao o erro da medida e pequeno (~0,1 %).
*/

volatile uint32_t bordas = 0;

void contar() {
  bordas++;
}

void bclkRapido() {
  TCCR1B = 0;
  TCCR1A = 0;
  PORTB &= ~(1 << PB1);
  DDRB  |=  (1 << PB1);          // pino 9 como saida
  TCNT1  = 0;
  OCR1A  = 832;                  // 16 MHz / (2 * 833) = 9603,8 Hz
  TCCR1A = (1 << COM1A0);        // toggle OC1A no compare match
  TCCR1B = (1 << WGM12) | (1 << CS10);   // CTC, sem prescaler
}

uint32_t ultimoT = 0;
uint32_t ultimoB = 0;

void setup() {
  Serial.begin(9600);
  pinMode(2, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), contar, RISING);
  bclkRapido();
  Serial.println(F("Medindo BCLK. Jumper do pino 9 ao pino 2."));
  Serial.println(F("Esperado: ~9604 Hz"));
  ultimoT = millis();
  ultimoB = 0;
}

void loop() {
  uint32_t agoraT = millis();
  if (agoraT - ultimoT >= 2000) {
    noInterrupts();
    uint32_t b = bordas;
    interrupts();
    float hz = (b - ultimoB) * 1000.0f / (agoraT - ultimoT);
    Serial.print(F("Frequencia medida: "));
    Serial.print(hz, 1);
    Serial.println(F(" Hz"));
    ultimoB = b;
    ultimoT = agoraT;
  }
}
