#include "seal/seal.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;
using namespace seal;

// Função para salvar dados em um arquivo de texto
void save_to_text_file(const string &filename, const string &data)
{
    ofstream file(filename, ios::app); // Abre o arquivo em modo de adição
    if (file.is_open())
    {
        file << data << "\n";
        file.close();
        cout << "Dados salvos no arquivo: " << filename << endl;
    }
    else
    {
        cerr << "Erro ao abrir o arquivo: " << filename << endl;
    }
}

// Função para converter dados binários para hexadecimal
string to_hex(const string &s)
{
    ostringstream ret;
    for (string::size_type i = 0; i < s.length(); ++i)
    {
        int z = s[i] & 0xff;
        ret << hex << setfill('0') << setw(2) << z;
    }
    return ret.str();
}

int main()
{
    // Configurar parâmetros de criptografia
    EncryptionParameters parms(scheme_type::bgv);
    size_t poly_modulus_degree = 4096;
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

    // Números para serem criptografados (exemplo simples)
    vector<uint64_t> numeros = {25, 45}; // Dois números a serem criptografados

    string original_data = "Números originais -> A=" + to_string(numeros[0]) +
                           ", B=" + to_string(numeros[1]) + "\n";
    save_to_text_file("resultado_criptografia.txt", original_data);

    // Criptografar os números
    Plaintext plain_num1, plain_num2;
    Ciphertext encrypted_num1, encrypted_num2;

    encoder.encode(vector<uint64_t>{numeros[0]}, plain_num1);
    encryptor.encrypt(plain_num1, encrypted_num1);

    encoder.encode(vector<uint64_t>{numeros[1]}, plain_num2);
    encryptor.encrypt(plain_num2, encrypted_num2);

    // Salvar os números criptografados
    stringstream ss_num1, ss_num2;
    encrypted_num1.save(ss_num1);
    encrypted_num2.save(ss_num2);

    save_to_text_file("resultado_criptografia.txt",
                      "A criptografado -> " + to_hex(ss_num1.str()));
    save_to_text_file("resultado_criptografia.txt",
                      "B criptografado -> " + to_hex(ss_num2.str()));

    // Aplicar operação homomórfica (neste caso, multiplicação)
    Ciphertext encrypted_result;
    evaluator.multiply(encrypted_num1, encrypted_num2, encrypted_result);

    // Salvar resultado homomórfico
    stringstream ss_result;
    encrypted_result.save(ss_result);
    save_to_text_file("resultado_criptografia.txt", "Resultado homomórfico (A * B) criptografado -> " + to_hex(ss_result.str()));

    // Descriptografar o resultado
    Plaintext decrypted_result;
    decryptor.decrypt(encrypted_result, decrypted_result);

    // Decodificar o resultado
    vector<uint64_t> resultado_final;
    encoder.decode(decrypted_result, resultado_final);

    string final_data = "Resultado final após descriptografia -> A * B = " + to_string(resultado_final[0]) + "\n";
    save_to_text_file("resultado_criptografia.txt", final_data);

    return 0;
}
