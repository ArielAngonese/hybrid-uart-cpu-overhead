/*
  bclk_9600.ino - gerador do BCLK (clock de bit) do TX discreto
  Arduino Uno (ATmega328P, 16 MHz)

  O BCLK sai no pino 9 (OC1A), gerado em hardware pelo Timer1 (modo CTC,
  toggle no compare match). Depois de iniciado, a CPU nao gasta nenhum ciclo
  para manter o clock rodando.

  Comandos pelo Serial Monitor (9600 baud):
    l  clock LENTO (2 Hz), para enxergar os LEDs
    r  clock RAPIDO (9603,8 Hz, erro de 0,04 %)
    s  pulso no SEND  (manda um frame)
    x  pulso no RESET

  Ligacoes:
    pino 9  -> BCLK  (fixo por hardware, nao da pra trocar)
    pino 6  -> SEND  (so assume saida durante o pulso; fora dele fica solto)
    pino 7  -> RESET (idem)
    GND do Arduino -> GND da protoboard (obrigatorio)

  Cuidado: nao use analogWrite() nos pinos 9 e 10 neste sketch (usam o Timer1).
*/

const uint8_t SEND_PIN  = 6;
const uint8_t RESET_PIN = 7;

bool modoRapido = false;

void bclkIniciar(bool rapido) {
  TCCR1B = 0;                      // para o timer
  TCCR1A = 0;                      // desconecta o OC1A do pino
  PORTB &= ~(1 << PB1);            // pino 9 comeca em 0
  DDRB  |=  (1 << PB1);            // pino 9 como saida
  TCNT1 = 0;
  if (rapido) {
    OCR1A = 832;                   // 16 MHz / (2 * 833) = 9603,8 Hz
    TCCR1A = (1 << COM1A0);        // toggle OC1A no compare match
    TCCR1B = (1 << WGM12) | (1 << CS10);                  // CTC, sem prescaler
  } else {
    OCR1A = 3905;                  // 16 MHz / (2 * 1024 * 3906) = 2,0 Hz
    TCCR1A = (1 << COM1A0);
    TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);    // CTC, prescaler 1024
  }
  modoRapido = rapido;
}

// SEND: repouso em 0 (resistor de 10k para o GND). Segura em 1 por ~3 ticks do BCLK.
void pulsoSend() {
  pinMode(SEND_PIN, OUTPUT);
  digitalWrite(SEND_PIN, HIGH);
  if (modoRapido) delayMicroseconds(320); else delay(1500);
  pinMode(SEND_PIN, INPUT);        // solta o pino: o resistor volta a linha para 0
}

// RESET: repouso em 1 (resistor de 10k para o Vcc). Segura em 0 por 50 ms.
void pulsoReset() {
  pinMode(RESET_PIN, OUTPUT);
  digitalWrite(RESET_PIN, LOW);
  delay(50);
  pinMode(RESET_PIN, INPUT);       // solta o pino: o resistor volta a linha para 1
}

void setup() {
  pinMode(SEND_PIN, INPUT);        // solto, para o botao fisico continuar funcionando
  pinMode(RESET_PIN, INPUT);
  Serial.begin(9600);
  bclkIniciar(false);              // comeca LENTO, de proposito
  Serial.println(F("BCLK no pino 9 - LENTO (2 Hz)"));
  Serial.println(F("Comandos: l = lento, r = rapido, s = SEND, x = RESET"));
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'l') { bclkIniciar(false); Serial.println(F("BCLK LENTO (2 Hz)")); }
    if (c == 'r') { bclkIniciar(true);  Serial.println(F("BCLK RAPIDO (9603,8 Hz)")); }
    if (c == 's') { pulsoSend();        Serial.println(F("SEND enviado")); }
    if (c == 'x') { pulsoReset();       Serial.println(F("RESET enviado")); }
  }
}
