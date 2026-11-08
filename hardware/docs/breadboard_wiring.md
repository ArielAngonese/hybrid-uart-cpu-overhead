# Ligações da protoboard: TX UART discreto

Lista de ligações pino a pino do transmissor (TX), tirada da simulação `logisim/tx_frame_controller.circ`. Os nomes dos sinais são os mesmos do Logisim.

Frame: 11 bits, 9600 baud, paridade par: `start (0) | D0..D7 | P | stop (1)`.

## Convenção de pinos

Todos os CIs são DIP, com o entalhe voltado para a esquerda.

- Fileira de baixo: pinos 1 a 7 (ou 1 a 8), da esquerda para a direita.
- Fileira de cima: pinos 14 a 8 (ou 16 a 9), da esquerda para a direita.

## Regras gerais

- **Alimentação:** 5 V e GND do Arduino nos trilhos da protoboard, com **GND em comum** entre Arduino e protoboard.
- **Desacoplamento:** capacitor de 100 nF entre Vcc e GND, perto de cada CI.
- **Entradas não usadas vão para o GND.** No Logisim isso não aparecia, mas em chip HC real uma entrada solta oscila e esquenta. As tabelas abaixo já indicam quais.
- **RESET:** resistor de 10 kΩ ligando ao Vcc e botão ligando ao GND (repouso = 1, apertado = 0). Apertar **uma vez ao ligar**, porque os flip-flops nascem em estado aleatório.
- **SEND:** botão ligando ao Vcc e resistor de 10 kΩ ligando ao GND (repouso = 0, apertado = 1). Se um clique gerar dois frames, é repique do botão: capacitor de 1 µF em paralelo com o resistor, ou debounce em software no Arduino.
- **D0 a D7:** cada chave do DIP switch liga ao Vcc, e cada saída tem um resistor de 10 kΩ para o GND.
- **BCLK:** testar sempre com clock lento (1 a 2 Hz) para enxergar os LEDs. Só depois subir para 9600 Hz.

## Etapas de montagem

1. SYNC e START
2. BUSY e contador
3. U1 e U2 (frame sem paridade)
4. Saída TX
5. Paridade

## 7474 (flip-flops SYNC e BUSY)

| Pino | Sinal | Etapa |
|---|---|---|
| 1 | RESET | 2 |
| 2 | BUSY_D (pino 8 do 7408) | 2 |
| 3 | BCLK | 2 |
| 4 | Vcc | 2 |
| 5 | **BUSY** | 2 |
| 6 | livre | |
| 8 | **NSYNC** (vai ao pino 2 do 7408) | 1 |
| 9 | livre | |
| 10 | Vcc | 1 |
| 11 | BCLK | 1 |
| 12 | SEND | 1 |
| 13 | Vcc | 1 |
| 7 / 14 | GND / Vcc | |

## 7408 (portas AND)

| Pino | Sinal | Etapa |
|---|---|---|
| 1 | SEND | 1 |
| 2 | NSYNC | 1 |
| 3 | **START** | 1 |
| 4 | QD (pino 11 do 74161) | 2 |
| 5 | QB (pino 13 do 74161) | 2 |
| 6 | **DONE** | 2 |
| 8 | **BUSY_D** | 2 |
| 9 | START_OR_BUSY (pino 3 do 7432) | 2 |
| 10 | NDONE (pino 2 do 7404) | 2 |
| 12, 13 | GND (porta sem uso) | |
| 11 | livre | |
| 7 / 14 | GND / Vcc | |

## 7432 (portas OR)

| Pino | Sinal | Etapa |
|---|---|---|
| 1 | START | 2 |
| 2 | BUSY | 2 |
| 3 | **START_OR_BUSY** | 2 |
| 4 | QH1 (pino 9 do U1) | 4 |
| 5 | NBUSY (pino 4 do 7404) | 4 |
| 6 | **TX** | 4 |
| 9, 10, 12, 13 | GND (portas sem uso) | |
| 8, 11 | livres | |
| 7 / 14 | GND / Vcc | |

## 7404 (inversores)

| Pino | Sinal | Etapa |
|---|---|---|
| 1 | DONE | 2 |
| 2 | **NDONE** | 2 |
| 3 | BUSY | 4 |
| 4 | **NBUSY** | 4 |
| 5, 9, 11, 13 | GND (portas sem uso) | |
| 6, 8, 10, 12 | livres | |
| 7 / 14 | GND / Vcc | |

## 74161 (contador de bits, 16 pinos)

| Pino | Sinal | Etapa |
|---|---|---|
| 1 | BUSY | 2 |
| 2 | BCLK | 2 |
| 3, 4, 5, 6 | GND | 2 |
| 7, 9, 10 | Vcc | 2 |
| 11 | **QD** | 2 |
| 12 | QC (livre, LED opcional) | |
| 13 | **QB** | 2 |
| 14 | QA (livre, LED opcional) | |
| 15 | livre | |
| 8 / 16 | GND / Vcc | |

## U1 (74165, 16 pinos)

| Pino | Sinal | Etapa |
|---|---|---|
| 1 | BUSY | 3 |
| 2 | BCLK | 3 |
| 3 | D2 | 3 |
| 4 | D1 | 3 |
| 5 | D0 | 3 |
| 6 | GND (start bit) | 3 |
| 9 | **QH1** (LED de teste na etapa 3) | 3 |
| 10 | SER (pino 9 do U2) | 3 |
| 11 | D6 | 3 |
| 12 | D5 | 3 |
| 13 | D4 | 3 |
| 14 | D3 | 3 |
| 15 | GND | 3 |
| 7 | livre | |
| 8 / 16 | GND / Vcc | |

## U2 (74165, 16 pinos)

| Pino | Sinal | Etapa |
|---|---|---|
| 1 | BUSY | 3 |
| 2 | BCLK | 3 |
| 3, 4 | Vcc | 3 |
| 5 | Vcc nas etapas 3 e 4; **P** na etapa 5 | 3 / 5 |
| 6 | D7 | 3 |
| 9 | QH (vai ao pino 10 do U1) | 3 |
| 10 | Vcc | 3 |
| 11, 12, 13, 14 | Vcc | 3 |
| 15 | GND | 3 |
| 7 | livre | |
| 8 / 16 | GND / Vcc | |

## X1 (7486, XORs do nível 1)

| Pino | Sinal | Etapa |
|---|---|---|
| 1 / 2 | D0 / D1 | 5 |
| 3 | **X0_1** | 5 |
| 4 / 5 | D2 / D3 | 5 |
| 6 | **X2_3** | 5 |
| 9 / 10 | D4 / D5 | 5 |
| 8 | **X4_5** | 5 |
| 12 / 13 | D6 / D7 | 5 |
| 11 | **X6_7** | 5 |
| 7 / 14 | GND / Vcc | |

## X2 (7486, XORs dos níveis 2 e 3)

| Pino | Sinal | Etapa |
|---|---|---|
| 1 / 2 | X0_1 / X2_3 (pinos 3 e 6 do X1) | 5 |
| 3 | **X0a3** | 5 |
| 4 / 5 | X4_5 / X6_7 (pinos 8 e 11 do X1) | 5 |
| 6 | **X4a7** | 5 |
| 9 / 10 | X0a3 / X4a7 (pinos 3 e 6 do X2) | 5 |
| 8 | **P** (vai ao pino 5 do U2) | 5 |
| 12, 13 | GND (porta sem uso) | |
| 11 | livre | |
| 7 / 14 | GND / Vcc | |

## Testes ao fim de cada etapa (BCLK lento)

1. **START:** LED no `START`. Com o `SEND` mantido apertado, ele pulsa por **um tick só**.
2. **BUSY:** LED no `BUSY` e nos `Q`. O `BUSY` fica aceso por **11 ticks**, o contador sobe até 10 e o `BUSY` cai.
3. **Frame:** LED no `QH1` (pino 9 do U1). Com `0x41`, sai `0 1 0 0 0 0 0 1 0 1 1`, com o 10º bit fixo em 1.
4. **TX:** LED no `TX`. Em repouso fica aceso, e o frame sai igual ao da etapa 3.
5. **Paridade:** ligar o `P` ao pino 5 do U2. Repetir os testes:
   - `0x41` (`D0` e `D6` em 1): `0 1 0 0 0 0 0 1 0 0 1`
   - `0x01` (só `D0` em 1): `0 1 0 0 0 0 0 0 0 1 1`

## Lista de materiais para esta montagem

- CIs: 7474 x1, 7408 x1, 7432 x1, 7404 x1, 74161 x1, 74165 x2, 7486 x2 (mais os reservas)
- DIP switch de 8 posições, 2 botões
- LEDs com resistor de 330 Ω: no mínimo em `START`, `BUSY`, `DONE`, `QH1`, `TX` e `P`
- Resistores de 10 kΩ: cerca de 10 (8 do DIP switch, 1 do `RESET`, 1 do `SEND`)
- Capacitores de 100 nF: 9 (um por CI)
- Protoboard de 830 pontos, jumpers em cores diferentes (vermelho no Vcc, preto no GND, amarelo no BCLK)