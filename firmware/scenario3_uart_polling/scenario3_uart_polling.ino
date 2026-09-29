// Cenário 3: Transmissão por hardware dedicado (UART) + atendimento por polling
//
// Estratégia de medição: usa o Timer1 em modo contador livre (sem prescaler)
// para contar ciclos de clock entre o início da tentativa de transmissão e
// o momento em que o hardware sinaliza a conclusão da transmissão completa
// do frame (bit TXC0, Transmit Complete), incluindo start bit, dados e stop
// bit. Medir o frame completo (e não apenas o registrador de dados ficando
// livre, bit UDRE0) mantém a métrica comparável com os demais cenários, que
// também medem a transmissão do frame inteiro.

#define NUM_TRANSMISSIONS 50

void startCycleCounter() {
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= (1 << CS10);  // sem prescaler: 1 tick do timer = 1 ciclo de clock da CPU
  TCNT1 = 0;
}

uint16_t readCycleCounter() {
  return TCNT1;
}

void sendBytePolling(uint8_t data) {
  // Espera ativa (polling): fica verificando o bit UDRE0 do registrador de
  // status até que o hardware sinalize que o registrador de dados está
  // livre para receber um novo byte
  while (!(UCSR0A & (1 << UDRE0))) {
    // laço de polling: CPU não faz mais nada além de checar o status
  }

  UCSR0A |= (1 << TXC0);  // limpa a flag TXC0 antes de transmitir (escrever 1 zera o bit)
  UDR0 = data;  // escreve o byte; hardware cuida da serialização sozinho

  // Espera ativa (polling) pela conclusão do frame inteiro, sinalizada pelo
  // bit TXC0 (Transmit Complete), para manter a medição comparável aos
  // demais cenários
  while (!(UCSR0A & (1 << TXC0))) {
    // laço de polling: CPU aguarda o frame completo ser transmitido
  }
}

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println("Cenario 3: UART + Polling");
  Serial.println("byte_enviado,ciclos_overhead");
}

void loop() {
  for (uint8_t i = 0; i < NUM_TRANSMISSIONS; i++) {
    uint8_t testByte = 'A' + (i % 26);

    startCycleCounter();
    sendBytePolling(testByte);
    uint16_t cycles = readCycleCounter();

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