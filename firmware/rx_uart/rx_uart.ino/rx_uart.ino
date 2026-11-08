
#define RX_PIN 2

const unsigned long BIT_US = 1000000UL / 9600;
const unsigned long HALF_BIT_US = BIT_US / 2;

unsigned long bytesValidos = 0;
unsigned long errosParidade = 0;
unsigned long errosStop = 0;

void esperarAte(unsigned long inicio, unsigned long tempo) {
  while ((unsigned long)(micros() - inicio) < tempo) {
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(RX_PIN, INPUT);

  Serial.println("RX UART 9600 8E1 iniciado");
}

void loop() {
  // Aguarda o start bit (linha passa de HIGH para LOW)
  while (digitalRead(RX_PIN) == LOW) {
  }

  while (digitalRead(RX_PIN) == HIGH) {
  }

  unsigned long inicio = micros();

  // Confirma o start no meio do periodo
  esperarAte(inicio, HALF_BIT_US);

  if (digitalRead(RX_PIN) != LOW) {
    return;
  }

  byte recebido = 0;
  byte quantidadeUns = 0;

  // Recebe os oito bits de dados
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
  bool stopValido = digitalRead(RX_PIN) == HIGH;

  // Paridade par: total de bits 1 deve ser par
  bool paridadeValida = ((quantidadeUns + paridade) % 2 == 0);

  if (!paridadeValida) {
    errosParidade++;
  }

  if (!stopValido) {
    errosStop++;
  }

  if (paridadeValida && stopValido) {
    bytesValidos++;

    Serial.print("OK decimal=");
    Serial.print(recebido);

    Serial.print(" hex=0x");
    if (recebido < 16) {
      Serial.print('0');
    }
    Serial.print(recebido, HEX);

    Serial.print(" char=");
    if (recebido >= 32 && recebido <= 126) {
      Serial.write(recebido);
    } else {
      Serial.print('-');
    }

    Serial.println();
  } else {
    Serial.print("ERRO byte=");
    Serial.print(recebido);

    if (!paridadeValida) {
      Serial.print(" PARIDADE");
    }

    if (!stopValido) {
      Serial.print(" STOP");
    }

    Serial.println();
  }

  Serial.print("Total OK=");
  Serial.print(bytesValidos);

  Serial.print(" | Erros paridade=");
  Serial.print(errosParidade);

  Serial.print(" | Erros stop=");
  Serial.println(errosStop);
}
