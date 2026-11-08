
#include <avr/interrupt.h>

#define TX_PIN 8
#define RX_PIN 2

const unsigned long BIT_US = 1000000UL / 9600;
const unsigned long HALF_BIT_US = BIT_US / 2;

volatile byte txValor = 0;
volatile byte txParidade = 0;
volatile byte txEtapa = 0;
volatile bool txFinalizado = true;
volatile bool txStopErrado = false;

// Interrupcao do Timer1: um passo a cada 104 us
ISR(TIMER1_COMPA_vect) {
  if (txEtapa == 0) {
    digitalWrite(TX_PIN, LOW); // Start
  }
  else if (txEtapa >= 1 && txEtapa <= 8) {
    byte bitAtual = (txValor >> (txEtapa - 1)) & 1;
    digitalWrite(TX_PIN, bitAtual ? HIGH : LOW);
  }
  else if (txEtapa == 9) {
    digitalWrite(TX_PIN, txParidade ? HIGH : LOW);
  }
  else if (txEtapa == 10) {
    digitalWrite(TX_PIN, txStopErrado ? LOW : HIGH);
  }
  else {
    // Retorna a linha ao repouso
    digitalWrite(TX_PIN, HIGH);
    TIMSK1 &= ~(1 << OCIE1A);
    TCCR1B = 0;
    txFinalizado = true;
  }

  txEtapa++;
}

void esperarAte(unsigned long inicio, unsigned long tempo) {
  while ((unsigned long)(micros() - inicio) < tempo) {
  }
}

void iniciarTX(byte valor, bool erroParidade, bool erroStop) {
  byte quantidadeUns = 0;

  for (byte i = 0; i < 8; i++) {
    quantidadeUns += (valor >> i) & 1;
  }

  noInterrupts();

  txValor = valor;
  txParidade = quantidadeUns % 2;

  if (erroParidade) {
    txParidade ^= 1;
  }

  txStopErrado = erroStop;
  txEtapa = 0;
  txFinalizado = false;

  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = 207;
  TIFR1 = (1 << OCF1A);

  TCCR1B = (1 << WGM12) | (1 << CS11);
  TIMSK1 |= (1 << OCIE1A);

  interrupts();
}

bool receberByte(byte &recebido, bool &paridadeOK, bool &stopOK) {
  unsigned long esperaInicio = micros();

  while (digitalRead(RX_PIN) == HIGH) {
    if ((unsigned long)(micros() - esperaInicio) > 20000UL) {
      return false;
    }
  }

  unsigned long inicio = micros();

  esperarAte(inicio, HALF_BIT_US);

  if (digitalRead(RX_PIN) != LOW) {
    return false;
  }

  recebido = 0;
  byte quantidadeUns = 0;

  for (byte i = 0; i < 8; i++) {
    esperarAte(inicio, BIT_US * (i + 1) + HALF_BIT_US);

    byte bitLido = digitalRead(RX_PIN) == HIGH ? 1 : 0;

    recebido |= (bitLido << i);
    quantidadeUns += bitLido;
  }

  esperarAte(inicio, BIT_US * 9 + HALF_BIT_US);
  byte paridade = digitalRead(RX_PIN) == HIGH ? 1 : 0;

  esperarAte(inicio, BIT_US * 10 + HALF_BIT_US);
  stopOK = digitalRead(RX_PIN) == HIGH;

  paridadeOK = ((quantidadeUns + paridade) % 2 == 0);

  return true;
}

void testar(byte valor, bool erroParidade, bool erroStop) {
  byte recebido = 0;
  bool paridadeOK = false;
  bool stopOK = false;

  iniciarTX(valor, erroParidade, erroStop);

  bool recebeu = receberByte(recebido, paridadeOK, stopOK);

  while (!txFinalizado) {
  }

  Serial.print("TX=");
  Serial.print(valor);
  Serial.print(" RX=");

  if (!recebeu) {
    Serial.println("SEM START VALIDO");
    return;
  }

  Serial.print(recebido);
  Serial.print(" PARIDADE=");
  Serial.print(paridadeOK ? "OK" : "ERRO");
  Serial.print(" STOP=");
  Serial.print(stopOK ? "OK" : "ERRO");

  bool resultadoEsperado =
    recebido == valor &&
    paridadeOK == !erroParidade &&
    stopOK == !erroStop;

  Serial.print(" TESTE=");
  Serial.println(resultadoEsperado ? "PASSOU" : "FALHOU");
}

void setup() {
  Serial.begin(115200);

  digitalWrite(TX_PIN, HIGH);
  pinMode(TX_PIN, OUTPUT);
  pinMode(RX_PIN, INPUT);

  Serial.println("TESTE DE ERROS UART 9600 8E1");
  Serial.println("TX=D8 RX=D2");
}

void loop() {
  Serial.println("--- INICIO DOS TESTES ---");

  testar(65, false, false);
  delay(1000);

  testar(65, true, false);
  delay(1000);

  testar(65, false, true);
  delay(1000);

  testar(85, false, false);
  delay(1000);

  Serial.println("--- FIM DOS TESTES ---");
  delay(2000);
}
