#include <blake2b.h>
#include <blake2s.h>
#include <blake2_common.h>
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


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  HDNode node;
  uint32_t fingerprint;
  char str[112];

  // init m
  hdnode_from_seed(fromhex("000102030405060708090a0b0c0d0e0f"), 16,
                   SECP256K1_NAME, &node);
  // [Chain m]
  fingerprint = 0;
  hdnode_fill_public_key(&node);
  hdnode_serialize_private(&node, fingerprint, VERSION_PRIVATE, str,
                           sizeof(str));

  for (int i = 0; i < 112; ++i)
  {
    Serial.write(str[i]);
  }
}

void loop() {


}
