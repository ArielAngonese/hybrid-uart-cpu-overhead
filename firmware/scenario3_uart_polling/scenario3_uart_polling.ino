// Cenário 3: Transmissão por hardware dedicado (UART) + atendimento por polling
// TODO: implementar medição de overhead (ciclos de clock / tempo de execução)

void setup() {
  Serial.begin(9600);
}

void loop() {
  // TODO: espera ativa (polling) até UART indicar pronto para envio
  // TODO: escrever byte no registrador de dados da UART
}