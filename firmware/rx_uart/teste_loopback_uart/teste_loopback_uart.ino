
#include <avr/interrupt.h>

#define TX_PIN 8
#define RX_PIN 2

const unsigned long BIT_US = 1000000UL / 9600;
const unsigned long HALF_BIT_US = BIT_US / 2;

// Variaveis usadas pela interrupcao do TX
volatile byte txValor = 0;
volatile byte txParidade = 0;
volatile byte txEtapa = 0;
volatile bool txFinalizado = true;

// Timer1: 16 MHz / 8 = 2 MHz
// 208 contagens = 104 microssegundos
ISR(TIMER1_COMPA_vect) {
  if (txEtapa == 0) {
    // Start bit
    digitalWrite(TX_PIN, LOW);
  }
  else if (txEtapa >= 1 && txEtapa <= 8) {
    // Oito bits de dados, LSB primeiro
    byte bitAtual = (txValor >> (txEtapa - 1)) & 1;
    digitalWrite(TX_PIN, bitAtual ? HIGH : LOW);
  }
  else if (txEtapa == 9) {
    // Paridade par
    digitalWrite(TX_PIN, txParidade ? HIGH : LOW);
  }
  else if (txEtapa == 10) {
    // Stop bit
    digitalWrite(TX_PIN, HIGH);
  }
  else {
    // Fim da transmissao
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

// Prepara o Timer1 para transmitir um byte
void iniciarTX(byte valor) {
  byte quantidadeUns = 0;

  for (byte i = 0; i < 8; i++) {
    quantidadeUns += (valor >> i) & 1;
  }

  noInterrupts();

  txValor = valor;
  txParidade = quantidadeUns % 2;
  txEtapa = 0;
  txFinalizado = false;

  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  OCR1A = 207;
  TIFR1 = (1 << OCF1A);

  // Modo CTC, prescaler 8
  TCCR1B = (1 << WGM12) | (1 << CS11);
  TIMSK1 |= (1 << OCIE1A);

  interrupts();
}

// Recebe um byte UART 8E1 pelo pino 2
bool receberByte(byte &recebido, bool &paridadeOK, bool &stopOK) {
  unsigned long esperaInicio = micros();

  // Aguarda a borda de descida do start bit
  while (digitalRead(RX_PIN) == HIGH) {
    if ((unsigned long)(micros() - esperaInicio) > 20000UL) {
      return false;
    }
  }

  unsigned long inicio = micros();

  // Confirma o start no meio do bit
  esperarAte(inicio, HALF_BIT_US);

  if (digitalRead(RX_PIN) != LOW) {
    return false;
  }

  recebido = 0;
  byte quantidadeUns = 0;

  // Le os oito bits
  for (byte i = 0; i < 8; i++) {
    esperarAte(inicio, BIT_US * (i + 1) + HALF_BIT_US);

    byte bitLido = digitalRead(RX_PIN) == HIGH ? 1 : 0;

    recebido |= (bitLido << i);
    quantidadeUns += bitLido;
  }

  // Le o bit de paridade
  esperarAte(inicio, BIT_US * 9 + HALF_BIT_US);
  byte paridade = digitalRead(RX_PIN) == HIGH ? 1 : 0;

  // Le o stop bit
  esperarAte(inicio, BIT_US * 10 + HALF_BIT_US);
  stopOK = digitalRead(RX_PIN) == HIGH;

  paridadeOK = ((quantidadeUns + paridade) % 2 == 0);

  return true;
}

void setup() {
  Serial.begin(115200);

  digitalWrite(TX_PIN, HIGH);
  pinMode(TX_PIN, OUTPUT);
  pinMode(RX_PIN, INPUT);

  Serial.println("TESTE REAL UART 9600 8E1");
  Serial.println("TX: pino 8 | RX: pino 2");
}

void loop() {
  // Testa diferentes bytes
  const byte testes[] = {65, 85, 0, 255};

  for (byte i = 0; i < 4; i++) {
    byte enviado = testes[i];
    byte recebido = 0;

    bool paridadeOK = false;
    bool stopOK = false;

    // Inicia transmissao por interrupcao
    iniciarTX(enviado);

    // Recebe enquanto o Timer1 transmite
    bool recebeu = receberByte(recebido, paridadeOK, stopOK);

    // Aguarda o fim do TX antes de imprimir
    while (!txFinalizado) {
    }

    Serial.print("TX=");
    Serial.print(enviado);

    Serial.print(" RX=");

    if (recebeu) {
      Serial.print(recebido);

      Serial.print(" PARIDADE=");
      Serial.print(paridadeOK ? "OK" : "ERRO");

      Serial.print(" STOP=");
      Serial.print(stopOK ? "OK" : "ERRO");

      if (recebido == enviado && paridadeOK && stopOK) {
        Serial.println(" RESULTADO=SUCESSO");
      } else {
        Serial.println(" RESULTADO=FALHA");
      }
    } else {
      Serial.println("SEM START VALIDO");
    }

    delay(1000);
  }

  Serial.println("--- NOVA RODADA ---");
}
