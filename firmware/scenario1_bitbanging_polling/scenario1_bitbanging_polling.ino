// Cenário 1: Transmissão por bit-banging (software) + atendimento por polling
// TODO: implementar transmissão manual bit a bit via pino digital
// TODO: implementar medição de overhead (ciclos de clock / tempo de execução)

void setup() {
  Serial.begin(9600);
}

void loop() {
  // TODO: espera ativa (polling) até dado pronto para envio
  // TODO: transmissão manual do byte, bit a bit, com timing por delay
}