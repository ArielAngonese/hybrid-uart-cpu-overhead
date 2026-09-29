// Cenário 2: Transmissão por bit-banging (software) + atendimento por interrupção
// TODO: configurar timer para gerar interrupção a cada intervalo de bit
// TODO: implementar ISR que envia o próximo bit manualmente
// TODO: implementar medição de overhead (ciclos de clock / tempo de execução)

void setup() {
  Serial.begin(9600);
  // TODO: configurar timer/interrupção
}

void loop() {
  // CPU livre; transmissão ocorre via ISR
}