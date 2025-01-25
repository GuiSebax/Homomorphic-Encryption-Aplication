#include <seal/seal.h> // Biblioteca SEAL para criptografia homomórfica
#include <vector>      // Para o uso de vetores
#include <string>      // Para manipulação de strings
#include <sstream>     // Para fluxos de strings (stringstream)
#include <iostream>    // Para entrada e saída padrão (opcional)
#include <fstream>     // Para manipulação de arquivos

using namespace std;
using namespace seal;

int main()
{

    // Configurar parâmetros de criptografia
    EncryptionParameters parms(scheme_type::BGV);
    size_t poly_modulus_degree = 4096;
    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(CoeffModulus::BGVDefault(poly_modulus_degree));
    parms.set_plain_modulus(256);

    SEALContext context(parms);
    KeyGenerator keygen(context);
    PublicKey public_key;
    keygen.create_public_key(public_key);
    SecretKey secret_key = keygen.secret_key();
    Encryptor encryptor(context, public_key);
    Decryptor decryptor(context, secret_key);
    Evaluator evaluator(context);
    BatchEncoder encoder(context);

    // Números Utilizados para a criptografia
    vector<uint64_t> numeros = {25, 45};

    // Salvar os números originais no arquivo
    string original_data = "Números originais -> A=" + to_string(numeros[0]) +
                           ", B=" + to_string(numeros[1]) + "\n";
    ofstream output_file("resultado_criptografia.txt", ios::app);
    output_file << original_data;

    // Criptografar os números
    Plaintext plain_num1, plain_num2;
    Ciphertext encrypted_num1, encrypted_num2;

    encoder.encode(vector<uint64_t>{numeros[0]}, plain_num1);
    encryptor.encrypt(plain_num1, encrypted_num1);

    encoder.encode(vector<uint64_t>{numeros[1]}, plain_num2);
    encryptor.encrypt(plain_num2, encrypted_num2);

    stringstream ss_num1, ss_num2;
    encrypted_num1.save(ss_num1);
    encrypted_num2.save(ss_num2);

    // Aplicar operação Homomórfica
    Ciphertext encrypted_result;
    evaluator.multiply(encrypted_num1, encrypted_num2, encrypted_result);

    stringstreamm ss_result;
    encrypted_result.save(ss_result);
    save_to_text_file(ss_result.str(), "resultado_criptografia.txt" + to_hex(ss_result.str()));

    // Descriptografar o resultado
    Plaintext plain_result;
    decryptor.decrypt(encrypted_result, plain_result);

    // Decodificar o resultado
    vector<uint64_t> result;
    encoder.decode(plain_result, result);

    // Salvar o resultado no arquivo
    string result_data = "Resultado -> A*B=" + to_string(result[0]) + "\n";
    output_file << result_data;

    // Fechar o arquivo
    output_file.close();

    return 0;
}