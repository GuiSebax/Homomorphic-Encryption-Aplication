#include "seal/seal.h"
#include <iostream>
#include <vector>
#include <fstream>

using namespace std;
using namespace seal;

int main()
{
    // 1. Configuração dos parâmetros de criptografia
    EncryptionParameters parms(scheme_type::bgv);
    size_t poly_modulus_degree = 8192;
    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(CoeffModulus::BFVDefault(poly_modulus_degree));
    parms.set_plain_modulus(PlainModulus::Batching(poly_modulus_degree, 20));

    SEALContext context(parms);
    KeyGenerator keygen(context);
    PublicKey public_key;
    keygen.create_public_key(public_key);
    SecretKey secret_key = keygen.secret_key();
    Encryptor encryptor(context, public_key);
    Evaluator evaluator(context);
    Decryptor decryptor(context, secret_key);
    BatchEncoder encoder(context);

    // 2. Dados de exemplo (saldos e transações)
    vector<uint64_t> saldos = {1000, 2500, 5000};            // Conta Corrente, Poupança, Investimentos
    vector<uint64_t> transacoes = {200, 150, 300, 100, 250}; // Valores das transações

    // 3. Criptografar os saldos e transações
    Plaintext plain_saldo_corrente, plain_saldo_poupanca, plain_saldo_investimentos;
    Ciphertext encrypted_saldo_corrente, encrypted_saldo_poupanca, encrypted_saldo_investimentos;
    encoder.encode(vector<uint64_t>{saldos[0]}, plain_saldo_corrente);
    encoder.encode(vector<uint64_t>{saldos[1]}, plain_saldo_poupanca);
    encoder.encode(vector<uint64_t>{saldos[2]}, plain_saldo_investimentos);

    encryptor.encrypt(plain_saldo_corrente, encrypted_saldo_corrente);
    encryptor.encrypt(plain_saldo_poupanca, encrypted_saldo_poupanca);
    encryptor.encrypt(plain_saldo_investimentos, encrypted_saldo_investimentos);

    // 4. Cálculo do saldo total
    Ciphertext encrypted_saldo_total;
    evaluator.add(encrypted_saldo_corrente, encrypted_saldo_poupanca, encrypted_saldo_total);
    evaluator.add_inplace(encrypted_saldo_total, encrypted_saldo_investimentos);

    // 5. Criptografar as transações e calcular a média
    Ciphertext encrypted_soma_transacoes;
    Plaintext plain_transacao;
    Ciphertext encrypted_transacao;

    // Inicializar a soma das transações como zero
    encoder.encode(vector<uint64_t>{0}, plain_transacao);
    encryptor.encrypt(plain_transacao, encrypted_soma_transacoes);

    // Somar todas as transações criptografadas
    for (uint64_t transacao : transacoes)
    {
        encoder.encode(vector<uint64_t>{transacao}, plain_transacao);
        encryptor.encrypt(plain_transacao, encrypted_transacao);
        evaluator.add_inplace(encrypted_soma_transacoes, encrypted_transacao);
    }

    // Calcular a média das transações (divisão inteira)
    Plaintext plain_num_transacoes;
    encoder.encode(vector<uint64_t>{transacoes.size()}, plain_num_transacoes);
    Ciphertext encrypted_media_transacoes;
    evaluator.divide_plain_inplace(encrypted_soma_transacoes, plain_num_transacoes);

    // 6. Descriptografar e mostrar os resultados
    Plaintext decrypted_saldo_total, decrypted_media_transacoes;
    decryptor.decrypt(encrypted_saldo_total, decrypted_saldo_total);
    decryptor.decrypt(encrypted_media_transacoes, decrypted_media_transacoes);

    vector<uint64_t> resultado_saldo_total, resultado_media_transacoes;
    encoder.decode(decrypted_saldo_total, resultado_saldo_total);
    encoder.decode(decrypted_media_transacoes, resultado_media_transacoes);

    cout << "Saldo Total do Cliente (Descriptografado): R$ " << resultado_saldo_total[0] << endl;
    cout << "Média das Transações (Descriptografada): R$ " << resultado_media_transacoes[0] << endl;

    return 0;
}
