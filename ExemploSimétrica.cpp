#include <openssl/evp.h>
#include <openssl/rand.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>

using namespace std;

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

// Função para converter bytes para hexadecimal
string to_hex(const unsigned char *data, size_t length)
{
    ostringstream ret;
    for (size_t i = 0; i < length; ++i)
    {
        ret << hex << setw(2) << setfill('0') << static_cast<int>(data[i]);
    }
    return ret.str();
}

// Função para criptografar dados usando AES
vector<unsigned char> aes_encrypt(const vector<unsigned char> &plaintext, const unsigned char *key, const unsigned char *iv)
{
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    vector<unsigned char> ciphertext(plaintext.size() + EVP_MAX_BLOCK_LENGTH);
    int len;
    int ciphertext_len;

    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);
    EVP_EncryptUpdate(ctx, ciphertext.data(), &len, plaintext.data(), plaintext.size());
    ciphertext_len = len;
    EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len);
    ciphertext_len += len;

    ciphertext.resize(ciphertext_len);
    EVP_CIPHER_CTX_free(ctx);
    return ciphertext;
}

// Função para descriptografar dados usando AES
vector<unsigned char> aes_decrypt(const vector<unsigned char> &ciphertext, const unsigned char *key, const unsigned char *iv)
{
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    vector<unsigned char> plaintext(ciphertext.size());
    int len;
    int plaintext_len;

    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);
    EVP_DecryptUpdate(ctx, plaintext.data(), &len, ciphertext.data(), ciphertext.size());
    plaintext_len = len;
    EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len);
    plaintext_len += len;

    plaintext.resize(plaintext_len);
    EVP_CIPHER_CTX_free(ctx);
    return plaintext;
}

int main()
{
    // Dados para serem criptografados
    vector<unsigned char> num1 = {'2', '5'}; // Número 1
    vector<unsigned char> num2 = {'4', '5'}; // Número 2

    // Chave e IV (Initialization Vector) para AES
    unsigned char key[32]; // 256 bits
    unsigned char iv[16];  // 128 bits
    RAND_bytes(key, sizeof(key));
    RAND_bytes(iv, sizeof(iv));

    // Criptografar os números
    vector<unsigned char> encrypted_num1 = aes_encrypt(num1, key, iv);
    vector<unsigned char> encrypted_num2 = aes_encrypt(num2, key, iv);

    // Salvar os números criptografados
    save_to_text_file("resultado_simetrico.txt", "Num1 criptografado -> " + to_hex(encrypted_num1.data(), encrypted_num1.size()));
    save_to_text_file("resultado_simetrico.txt", "Num2 criptografado -> " + to_hex(encrypted_num2.data(), encrypted_num2.size()));

    // Descriptografar os números
    vector<unsigned char> decrypted_num1 = aes_decrypt(encrypted_num1, key, iv);
    vector<unsigned char> decrypted_num2 = aes_decrypt(encrypted_num2, key, iv);

    // Converter para inteiros e multiplicar
    int a = stoi(string(decrypted_num1.begin(), decrypted_num1.end()));
    int b = stoi(string(decrypted_num2.begin(), decrypted_num2.end()));
    int resultado = a * b;

    // Salvar o resultado final
    string final_data = "Resultado final após descriptografia e multiplicação -> A * B = " + to_string(resultado) + "\n";
    save_to_text_file("resultado_simetrico.txt", final_data);

    return 0;
}
