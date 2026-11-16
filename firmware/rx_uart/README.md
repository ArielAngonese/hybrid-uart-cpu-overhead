# Receptor UART 9600 baud — 8E1

## 1. Objetivo

Implementar um receptor UART por software utilizando o Arduino Uno, permitindo receber dados de um transmissor externo construído com circuitos integrados da família 74xx.

O receptor foi desenvolvido para interpretar quadros UART, reconstruir os bytes recebidos e verificar possíveis erros de comunicação.

## 2. Configuração

- **Placa:** Arduino Uno
- **Pino de recepção:** D2
- **Velocidade:** 9600 baud
- **Formato:** 8E1 (8 bits de dados, paridade par e 1 bit de parada)
- **Monitor Serial:** 115200 baud

O formato 8E1 utiliza:

1. Um bit de início (START = 0).
2. Oito bits de dados, transmitidos do menos significativo para o mais significativo (LSB primeiro).
3. Um bit de paridade par (EVEN).
4. Um bit de parada (STOP = 1).

## 3. Arquivos

| Arquivo | Descrição |
|---|---|
| `rx_uart.ino/rx_uart.ino` | Código principal do receptor UART por software. |
| `teste_loopback_uart/teste_loopback_uart.ino` | Teste de transmissão e recepção no mesmo Arduino. |
| `teste_erros_uart/teste_erros_uart.ino` | Teste de detecção de erros de paridade e parada. |

## 4. Funcionamento

O receptor monitora o pino D2 para identificar o início de um quadro UART.

Após detectar o bit START, utiliza a função `micros()` para controlar os instantes de amostragem dos bits.

Em seguida:

1. Lê os oito bits de dados.
2. Reconstrói o byte recebido.
3. Verifica a paridade par.
4. Verifica o bit de parada.
5. Apresenta o resultado no Monitor Serial.

O receptor utiliza leitura por software, sem depender do periférico UART de hardware para receber os dados no pino D2.

## 5. Testes realizados

### 5.1. Teste de loopback

Foi utilizado um Arduino Uno com um jumper conectando o pino D8 (transmissão) ao pino D2 (recepção).

Foram transmitidos e recebidos os seguintes valores:

| Byte enviado | Byte recebido | Paridade | Stop | Resultado |
|---|---|---|---|---|
| 65 | 65 | OK | OK | Sucesso |
| 85 | 85 | OK | OK | Sucesso |
| 0 | 0 | OK | OK | Sucesso |
| 255 | 255 | OK | OK | Sucesso |

Todos os quatro casos foram recebidos corretamente.

### 5.2. Testes de erros

Foram executados testes com quadros válidos e com erros introduzidos intencionalmente.

| Teste | Resultado |
|---|---|
| Byte 65 com paridade e stop corretos | Passou |
| Byte 65 com paridade incorreta | Passou |
| Byte 65 com stop incorreto | Passou |
| Byte 85 com quadro válido | Passou |

O receptor identificou os erros de paridade e parada nos casos testados.

## 6. Como executar

1. Abrir o Arduino IDE.
2. Selecionar a placa Arduino Uno e a porta correspondente.
3. Abrir o sketch desejado.
4. Compilar e carregar o programa.
5. Abrir o Monitor Serial configurado para 115200 baud.

Para os testes de loopback e erros, conectar D8 ao D2 com um jumper.

Os programas de teste devem ser carregados separadamente. O sketch principal `rx_uart.ino` é destinado à recepção de um transmissor externo.

## 7. Integração com o transmissor discreto

A integração prevista utiliza um transmissor implementado com circuitos lógicos da família 74xx.

As conexões necessárias são:

- Saída serial do TX discreto → D2 do Arduino Uno.
- GND do TX discreto → GND do Arduino Uno.
- Níveis lógicos compatíveis com a entrada do Arduino.

A linha de transmissão deve permanecer em nível lógico alto durante o estado ocioso.

**Situação:** a recepção foi validada nos testes de loopback e detecção de erros. A comunicação com o transmissor discreto externo ainda precisa ser validada na montagem completa.

## 8. Conclusão

O receptor UART por software demonstrou funcionamento nos testes realizados com o Arduino Uno, recebendo corretamente diferentes valores e identificando erros introduzidos intencionalmente.

A próxima etapa é verificar a comunicação com o transmissor discreto do grupo, comprovando a integração entre os circuitos lógicos e o Arduino.