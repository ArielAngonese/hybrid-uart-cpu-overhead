# bclk_9600

Gerador do BCLK do TX discreto (Arduino Uno, Timer1 em CTC, saida no pino 9).

- Modo rapido: 9603,8 Hz (OCR1A=832, prescaler 1, erro de +0,04 %)
- Modo lento: 2 Hz (OCR1A=3905, prescaler 1024), para ver os LEDs
- Comandos no Serial Monitor (9600): l, r, s, x

## Status
- Calculo conferido (frequencias acima).
- Compilou e rodou no Tinkercad, mas a simulacao foi inconclusiva:
  o monitor serial nao respondeu de forma confiavel e o osciloscopio
  nao mostrou a onda. Prints em hardware/images/bclk/.
- **Nao validado em hardware real.** Falta medir o periodo (~104 us) com
  osciloscopio/analisador logico num Arduino Uno.
