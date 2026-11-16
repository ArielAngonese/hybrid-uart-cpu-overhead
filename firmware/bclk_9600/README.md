# bclk_9600

Gerador do BCLK do TX discreto (Arduino Uno, Timer1 em CTC, saida no pino 9).

- Modo rapido: 9603,8 Hz (OCR1A=832, prescaler 1, erro de +0,04 %)
- Modo lento: 2 Hz (OCR1A=3905, prescaler 1024), para ver os LEDs
- Comandos no Serial Monitor (9600): l, r, s, x

## Status

- Compila na Arduino IDE 2.3.10 para Arduino Uno (2450 bytes, 7 % da flash).
- Roda no Arduino Uno real: o Serial Monitor mostra as mensagens de
  boas-vindas a 9600 baud.
- **Frequencia do modo rapido validada em hardware real**: o sketch
  `../bclk_freq_test/` conta as bordas de subida do pino 9 num jumper
  ligado ao pino 2 (INT0). Medido: **9603,0 a 9603,5 Hz** na maioria das
  janelas de 2 s (esperado 9603,8 Hz; periodo de ~104,1 us). Algumas
  janelas isoladas deram ate 9608,5 Hz, ruido da propria medicao
  (millis() com granularidade de 1 ms).
- A medida usa o mesmo cristal que gera o clock: confirma a configuracao
  do timer, nao a precisao absoluta do cristal.
- **Ainda nao testado em hardware**: modo lento (2 Hz) com LED, duty
  cycle no osciloscopio, e os comandos `s` e `x` (SEND e RESET) ligados
  na protoboard.

Evidencias em `hardware/images/bclk/`:
- `bclk_medicao_arduino_real.png` (Serial Monitor com as medidas)
- `bclk_arduino_jumper_9_para_2.jpg` (montagem do teste)
- Os prints do Tinkercad ficam como historico: a simulacao foi inconclusiva.
