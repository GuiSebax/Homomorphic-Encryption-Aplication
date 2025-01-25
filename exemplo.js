const { MongoClient } = require('mongodb');

async function run() {
    const uri = "mongodb://localhost:27017"; // URL de conexão com MongoDB
    const client = new MongoClient(uri);

    try {
        await client.connect();
        const database = client.db('nome_do_banco');
        const collection = database.collection('nome_da_colecao');

        // Documento a ser inserido
        const documento = {
            "dado_normal": {
                "m1": 5,
                "m2": 3
            },
            "dado_criptografado": {
                "c1": "ENCRYPTED_VALUE_1",
                "c2": "ENCRYPTED_VALUE_2"
            },
            "operacao_homomorfica": {
                "tipo": "soma",
                "resultado_criptografado": "ENCRYPTED_SUM_RESULT"
            },
            "resultado_final_descriptografado": {
                "resultado_soma": 8
            }
        };

        const resultado = await collection.insertOne(documento);
        console.log(`Documento inserido com o id: ${resultado.insertedId}`);
    } finally {
        await client.close();
    }
}

run().catch(console.dir);
