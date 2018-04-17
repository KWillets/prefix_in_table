# prefix_in_table
This is an approach to a problem posed by Trent Nelson of finding strings in a table which are prefixes of a given query string (http://trent.me/is-prefix-of-string-in-table/).

## Prefix Hashing, or Multiplication as right-accumulation

The main idea is to match bytes from the input string against one "signature" byte from each string in the table, using SIMD to place the bytes in a vector for comparison.  However finding single distinctive signature bytes in the original strings is not guaranteed, so a hash which summarizes multiple bytes at a time seems preferable.  Ideally, each entry in the table would be represented by a small hash, and the query could be processed into a set of hashes, one for each prefix. So for each byte in the query, we would like to construct a hash from just that byte and the bytes preceding it.

One way to construct this prefix hash is multiplication by a constant, since it consists of left-shifts and adds.  Each bit k in the result is a function only of bits k, k-1,...0.  So a quick way to hash a byte sequence into a sequence of prefix "hashes" is to load it into an arithmetic register and multiply by some constant, hopefully something that mixes the bits a fair amount.  

Once we have these bytes we want to match against prefixes of varying lengths, so for a prefix of length 3, say, we compare the third byte, for length 4 the fourth byte, and so on.  A shuffle maps these bytes to a vector for comparison.

Currently this code uses one 64-bit multiplication, so it can match prefixes up to 8 bytes.  Extending further seems possible.
