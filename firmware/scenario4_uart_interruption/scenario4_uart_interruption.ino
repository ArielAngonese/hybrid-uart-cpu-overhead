// Cenário 4: Transmissão por hardware dedicado (UART) + atendimento por interrupção
// TODO: habilitar interrupção de "transmissão completa" da UART
// TODO: implementar ISR de conclusão de transmissão
// TODO: implementar medição de overhead (ciclos de clock / tempo de execução)

void setup() {
  Serial.begin(9600);
  // TODO: habilitar interrupção da UART
}

void loop() {
  // TODO: escrever byte no registrador de dados da UART
  // CPU livre; conclusão sinalizada via ISR
}