import pandas as pd
import matplotlib.pyplot as plt

# 1. Leitura dos CSVs
df_cenario1 = pd.read_csv('scenario1_bitbanging_polling.csv')
df_cenario2 = pd.read_csv('scenario2_bitbanging_interruption.csv')
df_cenario3 = pd.read_csv('scenario3_uart_polling.csv')
df_cenario4 = pd.read_csv('scenario4_uart_interruption.csv')

# 2. Gerar Estatísticas usando o 'ciclos_overhead'
medias = [
    df_cenario1['ciclos_overhead'].mean(),
    df_cenario2['ciclos_overhead'].mean(),
    df_cenario3['ciclos_overhead'].mean(),
    df_cenario4['ciclos_overhead'].mean()
]

cenarios = [
    'Bitbanging\nPolling', 
    'Bitbanging\nInterrupt', 
    'UART\nPolling', 
    'UART\nInterrupt'
]

print("--- Estatísticas (Média de Ciclos de Overhead) ---")
for i, cenario in enumerate(cenarios):
    print(f"{cenario.replace('\n', ' ')}: {medias[i]:.2f}")

# 3. Gráfico Comparativo
plt.figure(figsize=(10, 6))
barras = plt.bar(cenarios, medias, color=['red', 'orange', 'blue', 'green'])

plt.title('Comparativo de CPU Overhead por Cenário')
plt.ylabel('Overhead (Ciclos da CPU)')
plt.xlabel('Cenários')

for barra in barras:
    yval = barra.get_height()
    plt.text(barra.get_x() + barra.get_width()/2, yval + (max(medias)*0.01), 
             f'{yval:.0f}', ha='center', va='bottom')

plt.savefig('grafico_comparativo.png')
plt.show()
