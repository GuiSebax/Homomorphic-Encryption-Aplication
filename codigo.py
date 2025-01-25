import matplotlib.pyplot as plt

# Dados para o gráfico
labels = ['Criptografia', 'Soma', 'Multiplicação', 'Descriptografia']
bgv_times = [200, 300, 450, 180]  # Tempos hipotéticos em ms para BGV
ckks_times = [220, 350, 500, 200]  # Tempos hipotéticos em ms para CKKS
openssl_times = [2, 0, 0, 3]       # Tempos hipotéticos em ms para OpenSSL (simétrica)

x = range(len(labels))

# Criar o gráfico
plt.figure(figsize=(10, 6))
plt.bar(x, bgv_times, width=0.2, label='BGV', align='center')
plt.bar([i + 0.2 for i in x], ckks_times, width=0.2, label='CKKS', align='center')
plt.bar([i + 0.4 for i in x], openssl_times, width=0.2, label='OpenSSL', align='center')

# Adicionar rótulos e título
plt.xlabel('Processo')
plt.ylabel('Tempo (ms)')
plt.title('Comparação de Desempenho entre BGV, CKKS e OpenSSL')
plt.xticks([i + 0.2 for i in x], labels)
plt.legend()

# Mostrar gráfico
plt.show()
