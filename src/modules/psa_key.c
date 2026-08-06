#include <psa/crypto.h>
#include <zephyr/logging/log.h>
#include <zephyr/psa/key_ids.h>
#include <string.h> // per memcmp
#include <zephyr/kernel.h> // per il k_msleep

LOG_MODULE_REGISTER(psa_key);

#define SAMPLE_KEY_ID   ZEPHYR_PSA_APPLICATION_KEY_ID_RANGE_BEGIN
#define SAMPLE_KEY_TYPE PSA_KEY_TYPE_AES
#define SAMPLE_ALG  PSA_ALG_CTR
#define SAMPLE_KEY_BITS 256

int key_generation (void){
    LOG_INF("Generando la chiave ...");
    psa_status_t ret;
    psa_key_id_t key_id;
    // PSA_KEY_ATTRIBUTES_INIT è obbligatorio: azzera la struttura a uno stato pulito e noto. 
    // Saltare questa inizializzazione è una fonte classica di bug.
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    // La chiave è persistente e resiste anche al riavvio (il default è PSA_KEY_LIFETIME_VOLATILE)
    psa_set_key_lifetime(&key_attributes, PSA_KEY_LIFETIME_PERSISTENT);
    // policy d'uso (cosa è possibile fare con questa chiave)
    psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    // assegna id stabile, tipo della chiave, algoritmo di cifratura e dimensione della chiave
    psa_set_key_id(&key_attributes, SAMPLE_KEY_ID);
    psa_set_key_type(&key_attributes, SAMPLE_KEY_TYPE);
    psa_set_key_algorithm(&key_attributes, SAMPLE_ALG);
    psa_set_key_bits(&key_attributes, SAMPLE_KEY_BITS);
    // crea la chiave e la inserice subito nello storage sicuro (perché persistente)
    // su `/ns` questo avviene automaticamente dentro TF-M
    ret = psa_generate_key(&key_attributes, &key_id);

    // controllo di sicurezza
    if(ret != PSA_SUCCESS){
        LOG_ERR("Creazione fallita. (%d).", ret);
        return -1;
    }
    // verifica in debug che l'id che viene restituito concide con quello chiesto
   	__ASSERT_NO_MSG(key_id == SAMPLE_KEY_ID);
    // elimina la chiave dalla cache, lasciandola solo in memoria persistente
    ret = psa_purge_key(SAMPLE_KEY_ID);
    // controllo di sicurezza
    if (ret != PSA_SUCCESS) {
		LOG_ERR("Cancellazione da memoria volatile fallita (%d).", ret);
		return -1;
	}
	LOG_INF("Persistent key generated.");
	return 0;

}

int destroy_key(void){
    LOG_INF("Distruzione chiave persistente...");
	psa_status_t ret;
    // Elimina la chiave indicata dall'ID in modo definitivo
	ret = psa_destroy_key(SAMPLE_KEY_ID);
    // controllo di sicurezza
	if (ret != PSA_SUCCESS) {
		LOG_ERR("Distruzione della chiave fallita (%d)", ret);
		return -1;
	}
	LOG_INF("Chiave persistente distrutta.");
	return 0;
}

void module_psa_key_run(void){

    // assicura che non esista già una chiave con quell'ID
    psa_destroy_key(SAMPLE_KEY_ID);
    LOG_INF("Modulo chiavi psa");
    // generazione della chiave
    key_generation();
    LOG_INF("Utilizzo della chiave persistente per cifrare un testo");
    psa_status_t ret;
	size_t ciphertext_len;
	size_t decrypted_text_len;
    // testo da cifrare (viene cifrato anche il carattere di escape \0)
    static uint8_t testo[] = "Testo da cifrare. Esempio di utilizzo di chiave persistente";
    // buffer per il testo cifrato
    // viene dimensionato tramite macro per evitare errori
    static uint8_t testo_cifrato[PSA_CIPHER_ENCRYPT_OUTPUT_SIZE(SAMPLE_KEY_TYPE, SAMPLE_ALG, sizeof(testo))];
    // buffer testo decifrato
    static uint8_t testo_decifrato[sizeof(testo)]; 
    // cifratura one-shot e controllo di sicurezza
    // PSA/TF-M legge l'ID della chiave persistente e la recupera dalla ITS i modo trasparente automaticamente.
    ret = psa_cipher_encrypt(SAMPLE_KEY_ID, SAMPLE_ALG, testo, sizeof(testo), testo_cifrato, sizeof(testo_cifrato), &ciphertext_len);
	if (ret != PSA_SUCCESS) {
		LOG_ERR("Testo non criptato. (%d)", ret);
		return;
	}
    LOG_HEXDUMP_INF(testo_cifrato, sizeof(testo_cifrato), "testo cifrato:");    
    k_msleep(1000);
    LOG_INF("Decrittazione del testo ...");
    // decifratura: legge l'IV dai primi byte del ciphertext, decifra il resto con la stessa chiave
    ret = psa_cipher_decrypt(SAMPLE_KEY_ID, SAMPLE_ALG, testo_cifrato, ciphertext_len, testo_decifrato, sizeof(testo_decifrato), &decrypted_text_len);
	if (ret != PSA_SUCCESS) {
		LOG_ERR("Failed to decrypt the ciphertext. (%d)", ret);
		return;
	}
    // Controlla che la lunghezza recuperata corrisponda all'originale.
	__ASSERT_NO_MSG(decrypted_text_len == sizeof(plaintext));

	// Controllo di sicurezza buffer
    // se i buffer testo e testo_decifrato sono identici memcmp ritorna 0
	if (memcmp(testo, testo_decifrato, sizeof(testo))) {
		LOG_HEXDUMP_INF(testo, sizeof(testo), "testo:");
		LOG_HEXDUMP_INF(testo_cifrato, ciphertext_len, "testo_cifrato:");
		LOG_HEXDUMP_INF(testo_decifrato, sizeof(testo_decifrato), "testo_decifrato:");
		LOG_ERR("The decrypted text doesn't match the plaintext.");
		return;
	}
    LOG_HEXDUMP_INF(testo_decifrato, sizeof(testo_decifrato), "testo decriptato:");

    // distruzione della chiave 
    destroy_key();

}

