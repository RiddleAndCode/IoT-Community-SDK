#include <sec_elem_abstr.h>
#include <types.h>


#include <blake2b.h>
#include <blake2s.h>
#include <brg_types.h>
#include <check_mem.h>
#include <coins.h>
#include <crypto.h>
#include <curves.h>
#include <debug.h>
#include <ecdsa.h>
#include <gettext.h>
#include <hasher.h>
#include <hmac.h>
#include <macros.h>
#include <memzero.h>
#include <messages.h>
#include <options.h>
#include <pb.h>
#include <pbkdf2.h>
#include <pb_common.h>
#include <pb_decode.h>
#include <pb_encode.h>
#include <rand.h>
#include <rc4.h>
#include <rfc6979.h>
#include <ripemd160.h>
#include <script.h>
#include <secp256k1.h>
#include <segwit_addr.h>
#include <sha2.h>
#include <sha3.h>
#include <shamir.h>
#include <bip32.h>
#include <signing.h>
#include <transaction.h>
#include <types.pb.h>
#include <util.h>
#include <Arduino.h>

uint8_t testText[] = "Smart cities combine the Internet of Things with almost every aspect of life,"
                     "and with recent developments in mind,smart cities will become a reality in the near future."
                     "Investments by Bill Gates near Phoenix, Arizona, NEOM in the Middle East or development plans for Hangzhou in China,"
                     "show a universal interest all over the world.While numerous examples from every aspect of life, including education,"
                     "health and safety can be cited, particularly the concept of smart mobility and energy as well as smart retail & logistics"
                     "come to mind when thinking of the combination of IoT and digital twins. With an ever-increasing urban population, congestion"
                     "and pollution become a major concern for both residents and governments and therefore demand innovative technical solutions."
                     "The City of Los Angeles is working on an automated traffic surveillance and control system, which is scheduled for 2018"
                     "and uses IT components in roads and vehicles that provide accurate data with which to anticipate anticipate and reduce traffic"
                     "congestion and pollution. Smart parking, on the other hand, focuses on real-time information provided by drivers in regards"
                     "to available parking spots, which are represented as digital twins.An example of a smart parking solution can be found in Barcelona,"
                     "where a fully operational system is also scheduled for 2018.For both scenarios, one can visualize how digital twins, perhaps first as"
                     "traffic lights, then as street segments and finally as cars and parking spots, sufficiently accurately replicate the real-world"
                     "environment. Digital twins emerge from digital objects and allow status updates e.g. on location, utilization or temperature in near"
                     "real-time. This kind of transparency is highly beneficial to emerging IoT ecosystems which typically focus on increasing efficiency and optimization.";




#if VALGRIND
/* Call after secret data is written, before first use */
#define MARK_SECRET_DATA(addr, len) VALGRIND_MAKE_MEM_UNDEFINED(addr, len)
/* Call before secret data is freed or to mark non-secret data (public keys or
   signatures) */
#define UNMARK_SECRET_DATA(addr, len) VALGRIND_MAKE_MEM_DEFINED(addr, len)
#else
#define MARK_SECRET_DATA(addr, len)
#define UNMARK_SECRET_DATA(addr, len)
#endif

#define FROMHEX_MAXLEN 512

#define VERSION_PUBLIC 0x0488b21e
#define VERSION_PRIVATE 0x0488ade4

#define DECRED_VERSION_PUBLIC 0x02fda926
#define DECRED_VERSION_PRIVATE 0x02fda4e8





void setup() {
  Serial.begin(9600);
  Serial.println("------------------------------------------------------------------------------------------------------------------\n");
  se_configure_hardware(0xC0,0,400000,18,19);
  SE_STATUS ret = SE_COM_FAIL;
  ret = se_init(1);
  if (ret == SE_SUCCESS)
    Serial.println("Se init success");
  else
    Serial.println("Se init failed");
  Serial.println("\n\n");

  HDNode node;
  uint32_t fingerprint;
  char str[112];

  // init m
  hdnode_from_seed(fromhex("000102030405060708090a0b0c0d0e0f"), 16, SECP256K1_NAME, &node);

  // [Chain m]
  fingerprint = 0;
  hdnode_fill_public_key(&node);
  hdnode_serialize_private(&node, fingerprint, VERSION_PRIVATE, str,
                           sizeof(str));

  for (int i = 0; i < 112; ++i)
  {
    Serial.write(str[i]);
  }
  Serial.println("\n\n");

}

void loop() {




  uint8_t rnd_num[32] = {0};

  SE_STATUS rnd_status = SE_COM_FAIL;
  rnd_status = se_get_random(rnd_num, 32);
  if (rnd_status == SE_SUCCESS) {
    Serial.println("Random number is :");
    hexify(rnd_num, sizeof rnd_num);

  } else {
    Serial.print(F("Random number generation failed:"));
    Serial.println(rnd_status, HEX);
  }
  Serial.println("\n\n");

  SE_STATUS rv;
  uint8_t public_key_0[64] = {0};
  uint8_t public_key_1[64] = {0};
  uint8_t public_key_2[64] = {0};

  se_generate_keypair(0);
  se_generate_keypair(2);
  se_generate_keypair(7);

  se_get_pubkey(0, public_key_0, NULL);
  se_get_pubkey(2, public_key_1, NULL);
  se_get_pubkey(7, public_key_2, NULL);

  Serial.println("Public Key in slot 0 is :");
  hexify(public_key_0, sizeof public_key_0);
  Serial.println("Public Key in slot 2 is :");
  hexify(public_key_1, sizeof public_key_1);
  Serial.println("Public Key in slot 7 is :");
  hexify(public_key_2, sizeof public_key_2);


  Serial.println("\n\n");
  uint8_t sha[32] = {0};
  uint16_t shaLen = 32;
  Serial.println("sha256 of the input text is :");
  se_get_sha256(testText, sizeof(testText), sha, &shaLen);
  hexify(sha, sizeof sha);
  Serial.println("\n\n");

  uint8_t signature[100] = {0};
  Serial.println("Signing the digest of the text file with secret key at slot 0...");
  se_sign(0, sha, 0, signature, 0);
  Serial.println("Signature is :");
  hexify(signature, sizeof signature);

  Serial.println("\n\n");
  Serial.println("Verifying the signature with the public key at slot 0 ...");

  if ((rv = se_verify_external(0, public_key_0, 0, sha, 0, signature, 0)) == SE_SUCCESS)
    Serial.println("Verify Success");
  else
  {
    Serial.println("Verify Failed");
    Serial.println(rv, HEX);

  }
  Serial.println("Verifying the signature with the public key at slot 1 ...");

  if (se_verify_external(0, public_key_1, 0, sha, 0, signature, 0) == SE_SUCCESS) // This should fail as we you a different pubkey to verify
    Serial.println("Verify Success");
  else
    Serial.println("Verify Failed as expected");
  Serial.println("\n\n");

  Serial.println("------------------------------------------------------------------------------------------------------------------");
  Serial.println("------------------------------------------------------------------------------------------------------------------");

  Serial.println("\n\n");

}




void hexify(const uint8_t *hex, unsigned int len) {
  int i;

  for (i = 0; i < len; i++)  {
    static char tmp[4] = {};
    sprintf(tmp, "%02X", hex[i]);
    Serial.write(tmp);
  }

  Serial.println();

}

const uint8_t *fromhex(const char *str) {
  static uint8_t buf[FROMHEX_MAXLEN];
  size_t len = strlen(str) / 2;
  if (len > FROMHEX_MAXLEN) len = FROMHEX_MAXLEN;
  for (size_t i = 0; i < len; i++) {
    uint8_t c = 0;
    if (str[i * 2] >= '0' && str[i * 2] <= '9') c += (str[i * 2] - '0') << 4;
    if ((str[i * 2] & ~0x20) >= 'A' && (str[i * 2] & ~0x20) <= 'F')
      c += (10 + (str[i * 2] & ~0x20) - 'A') << 4;
    if (str[i * 2 + 1] >= '0' && str[i * 2 + 1] <= '9')
      c += (str[i * 2 + 1] - '0');
    if ((str[i * 2 + 1] & ~0x20) >= 'A' && (str[i * 2 + 1] & ~0x20) <= 'F')
      c += (10 + (str[i * 2 + 1] & ~0x20) - 'A');
    buf[i] = c;
  }
  return buf;
}
