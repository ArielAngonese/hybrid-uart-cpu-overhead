// Cenário 2: Transmissão por bit-banging (software) + atendimento por interrupção
//
// Estratégia de medição: usa o Timer1 em modo contador livre (sem prescaler)
// para contar ciclos de clock entre o início do armamento da transmissão e
// o momento em que a ISR do Timer2 processa o último bit (stop bit) do
// frame. O Timer2 é responsável por gerar uma interrupção a cada intervalo
// de bit (mesma taxa de baud rate dos demais cenários: 9600), e a própria
// ISR avança manualmente o estado de transmissão, bit a bit.

#define NUM_TRANSMISSIONS 50
#define TX_PIN 8

// Estados da máquina de transmissão bit a bit
enum TxState {
  TX_IDLE,
  TX_START_BIT,
  TX_DATA_BITS,
  TX_STOP_BIT
};

volatile TxState txState = TX_IDLE;
volatile uint8_t txData = 0;
volatile uint8_t txBitIndex = 0;
volatile uint16_t cyclesResult = 0;
volatile bool transmissionDone = false;

void startCycleCounter() {
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= (1 << CS10);  // sem prescaler: 1 tick do timer = 1 ciclo de clock da CPU
  TCNT1 = 0;
}

uint16_t readCycleCounter() {
  return TCNT1;
}

// Configura o Timer2 para disparar uma interrupção a cada intervalo de bit,
// na taxa correspondente ao baud rate definido (9600 bps).
// Prescaler 8 escolhido especificamente para dar resolução suficiente
// nessa faixa de baud rate (prescalers maiores, como 1024, resultariam
// em OCR2A menor que 1, tornando o timing impreciso).
void setupBitTimer() {
  TCCR2A = (1 << WGM21);  // modo CTC (Clear Timer on Compare Match)
  TCCR2B = 0;  // timer inicia parado; prescaler é setado apenas ao disparar a transmissao

  // OCR2A calculado para 9600 bps com prescaler 8:
  // (16000000 / 8) / 9600 - 1 = 207 (período real: 104,0 µs, alvo: 104,17 µs)
  OCR2A = 207;

  TIMSK2 |= (1 << OCIE2A);  // habilita interrupcao de comparacao do Timer2
}

// ISR disparada a cada intervalo de bit; avança manualmente o estado da transmissão
ISR(TIMER2_COMPA_vect) {
  switch (txState) {
    case TX_START_BIT:
      digitalWrite(TX_PIN, LOW);
      txState = TX_DATA_BITS;
      txBitIndex = 0;
      break;

    case TX_DATA_BITS:
      digitalWrite(TX_PIN, (txData >> txBitIndex) & 0x01);
      txBitIndex++;
      if (txBitIndex >= 8) {
        txState = TX_STOP_BIT;
      }
      break;

    case TX_STOP_BIT:
      digitalWrite(TX_PIN, HIGH);
      cyclesResult = readCycleCounter();
      transmissionDone = true;
      txState = TX_IDLE;
      TCCR2B &= ~(1 << CS21);  // para o timer
      break;

    case TX_IDLE:
    default:
      break;
  }
}

void sendByteInterrupt(uint8_t data) {
  txData = data;
  txBitIndex = 0;
  transmissionDone = false;
  txState = TX_START_BIT;

  startCycleCounter();
  TCCR2B |= (1 << CS21);  // liga o timer com prescaler 8, dispara a ISR

  // aguarda a ISR sinalizar conclusao (aguardo simples, apenas para fins de teste;
  // na pratica a CPU ficaria livre para outras tarefas nesse intervalo)
  while (!transmissionDone) {
    // espera passiva pela flag setada na ISR
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(TX_PIN, OUTPUT);
  digitalWrite(TX_PIN, HIGH);  // linha em repouso: nivel alto

  setupBitTimer();

  delay(1000);
  Serial.println("Cenario 2: Bit-banging + Interrupcao");
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

    delay(100);  // espacamento entre transmissoes, so pra facilitar leitura no serial monitor
  }

  while (true) {
    // trava aqui apos completar as transmissoes de teste
  }
}