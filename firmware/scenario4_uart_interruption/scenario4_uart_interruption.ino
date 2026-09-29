// Cenário 4: Transmissão por hardware dedicado (UART) + atendimento por interrupção
//
// Estratégia de medição: usa o Timer1 em modo contador livre para
// contar ciclos de clock entre o início da escrita no registrador de
// dados da UART (UDR0) e o momento em que a interrupção de "transmissão
// completa" (TXCIE0) é disparada, sinalizando que o hardware terminou de
// enviar o frame inteiro (incluindo stop bit).

#define NUM_TRANSMISSIONS 50

volatile uint16_t cyclesResult = 0;
volatile bool transmissionDone = false;

void startCycleCounter() {
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= (1 << CS10);  // 1 tick do timer = 1 ciclo de clock da CPU
  TCNT1 = 0;
}

uint16_t readCycleCounter() {
  return TCNT1;
}

// ISR disparada quando o hardware da UART termina de transmitir o frame completo
ISR(USART_TX_vect) {
  cyclesResult = readCycleCounter();
  transmissionDone = true;
}

void sendByteInterrupt(uint8_t data) {
  transmissionDone = false;
  startCycleCounter();
  UDR0 = data;  // dispara a transmissão; hardware assume a partir daqui

  // aguarda a ISR sinalizar conclusão (aguardo simples, não é polling de status:
  // apenas espera a flag ser setada pela interrupção, sem checar registrador de hardware)
  while (!transmissionDone) {
    // CPU poderia estar fazendo outra coisa aqui; loop de espera é só para fins de teste
  }
}

void setup() {
  Serial.begin(9600);
  UCSR0B |= (1 << TXCIE0);  // habilita interrupção de transmissão completa

  delay(1000);
  Serial.println("Cenario 4: UART + Interrupcao");
  Serial.println("byte_enviado,ciclos_overhead");
}

void loop() {
  for (uint8_t i = 0; i < NUM_TRANSMISSIONS; i++) {
    uint8_t testByte = 'A' + (i % 26);

    sendByteInterrupt(testByte);
    uint16_t cycles = cyclesResult;

    while (!(UCSR0A & (1 << UDRE0)));
    Serial.print((char)testByte);
    Serial.print(",");
    Serial.println(cycles);

    delay(100);  // espaçamento entre transmissões, só pra facilitar leitura no serial monitor
  }

  while (true) {
    // trava aqui após completar as transmissões de teste
  }
}