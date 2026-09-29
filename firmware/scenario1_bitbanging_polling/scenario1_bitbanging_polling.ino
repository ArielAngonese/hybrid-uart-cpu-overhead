// Cenário 1: Transmissão por bit-banging (software) + atendimento por polling
//
// Estratégia de medição: usa o Timer1 em modo contador livre
// para contar ciclos de clock do início ao fim da transmissão completa do
// byte. Diferente do cenário 3, aqui não existe hardware fazendo a
// serialização: a própria CPU controla cada bit manualmente, então a medição
// cobre a transmissão inteira, não apenas o disparo.
//
// Baud rate alvo: 9600 (mesma taxa do cenário 3, para comparação justa)

#define NUM_TRANSMISSIONS 50
#define BAUD_RATE 9600
#define TX_PIN 8

// Tempo de 1 bit em microssegundos, calculado a partir do baud rate
const unsigned long BIT_DELAY_US = 1000000UL / BAUD_RATE;

void startCycleCounter() {
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= (1 << CS10);  // 1 tick do timer = 1 ciclo de clock da CPU
  TCNT1 = 0;
}

uint16_t readCycleCounter() {
  return TCNT1;
}

void sendByteBitBanging(uint8_t data) {
  // Bit de start: nível baixo
  digitalWrite(TX_PIN, LOW);
  delayMicroseconds(BIT_DELAY_US);

  // 8 bits de dado, começando pelo menos significativo (LSB first, igual UART)
  for (uint8_t i = 0; i < 8; i++) {
    digitalWrite(TX_PIN, (data >> i) & 0x01);
    delayMicroseconds(BIT_DELAY_US);
  }

  // Bit de stop: nível alto
  digitalWrite(TX_PIN, HIGH);
  delayMicroseconds(BIT_DELAY_US);
}

void setup() {
  Serial.begin(9600);
  pinMode(TX_PIN, OUTPUT);
  digitalWrite(TX_PIN, HIGH);  // linha em repouso: nível alto

  delay(1000);
  Serial.println("Cenario 1: Bit-banging + Polling");
  Serial.println("byte_enviado,ciclos_overhead");
}

void loop() {
  for (uint8_t i = 0; i < NUM_TRANSMISSIONS; i++) {
    uint8_t testByte = 'A' + (i % 26);

    startCycleCounter();
    sendByteBitBanging(testByte);
    uint16_t cycles = readCycleCounter();

    Serial.print((char)testByte);
    Serial.print(",");
    Serial.println(cycles);

    delay(100);  // espaçamento entre transmissões, só pra facilitar leitura no serial monitor
  }

  while (true) {
    // trava aqui após completar as transmissões de teste
  }
}