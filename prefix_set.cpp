#include <string.h>
#include <inttypes.h>
#include <immintrin.h>
#include <stdio.h>

class PrefixSet {

private:

  static const uint64_t  magic = 0x0102030405060708; // ?? maybe random 

  static uint64_t prefix_hash8( char *s ) {  // basically a MUL
      uint64_t first8 = *(uint64_t *) s;
      return  magic * first8;
  }

  __m128i lasts, hashes;

public:

  PrefixSet(char **pfxs, int n) {
    unsigned char tlasts[16], thashes[16];
    for(int i = 0; i < n && i < 16; i++ ) {
      size_t length = strnlen(pfxs[i],8);
      tlasts[i] = length-1;
      uint64_t hash = prefix_hash8(pfxs[i]);
      thashes[i] = (hash >> ((length-1)*8)) & 0xFF;
    }
    lasts  = _mm_loadu_si128((__m128i *) tlasts);
    hashes = _mm_loadu_si128((__m128i *) thashes);
  }

  int match(char * s) {
    uint64_t h = prefix_hash8(s);
    __m128i src = _mm_set1_epi64x(h);
    __m128i s_hashes  = _mm_shuffle_epi8(src, lasts);
    __m128i matches = _mm_cmpeq_epi8(s_hashes, hashes);

    return !_mm_testz_si128(matches,matches);
  }

};

  
int main() {

  char *ps[] ={"one","two","three","four","five","six","seven","eight",
	       "nine","ten","eleven","twelve","thirteen","fourteen","fifteen","sixteen"};

  PrefixSet p(ps,16);

  int m = p.match("threefour");

  printf("%d\n", m);

  m = p.match("xyz");
  printf("%d\n", m);

  m = p.match("one");
  printf("%d\n", m);

  m = p.match("fifteenABC");
  printf("%d\n", m);

  m = p.match("negative");
  printf("%d\n", m);

  m = p.match("ABCDEFGHIJKL");
  printf("%d\n", m);
  
}
