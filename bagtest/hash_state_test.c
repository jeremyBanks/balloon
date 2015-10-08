
#include <stdio.h>
#include "mutest.h"

#include <baghash.h>
#include "libbaghash/constants.h"
#include "libbaghash/hash_state.h"

static void
init_options (struct baghash_options *opts)
{
  opts->m_cost = 1024 * 128;
  opts->t_cost = 3;
  opts->n_neighbors = 20;
  opts->comp = COMP__KECCAK_1600;
  opts->mix = MIX__BAGHASH_ONE_BUFFER;
  opts->xor_then_hash = 0;
}

void 
mu_test_hash_state_init_free (void) 
{
  struct hash_state s;
  struct baghash_options opts;
  init_options (&opts);

  const unsigned char salt[] = "abcdefghijkl";
  mu_check ( !hash_state_init (&s, &opts, salt, sizeof (salt)) );
  hash_state_free (&s);
}

void 
mu_test_hash_state_fill (void) 
{
  struct hash_state s;
  struct baghash_options opts;
  init_options (&opts);
  const unsigned char salt[] = "abcdefghijkl";
  const unsigned char in[] = "ZZZZZZZZZZZZ";
  mu_check ( !hash_state_init (&s, &opts, salt, sizeof (salt)) );

  mu_check ( !hash_state_fill (&s, in, sizeof (in), salt, sizeof (salt)) ); 

  size_t n_zero = 0;
  for (size_t i = 0; i < s.n_blocks; i++) {
    if (!s.buffer[i]) n_zero++;
  }

  mu_check (n_zero < s.n_blocks/200);

  hash_state_free (&s);
}

void 
mu_test_hash_state_fill2 (void) 
{
  struct baghash_options opts;
  init_options (&opts);

  struct hash_state s1;
  struct hash_state s2;
  struct hash_state s3;
  unsigned char salt[] = "abcdefghijkl";
  unsigned char in[] = "ZZZZZZZZZZZZ";
  mu_check ( !hash_state_init (&s1, &opts, salt, sizeof (salt)) );
  mu_check ( !hash_state_fill (&s1, in, sizeof (in), salt, sizeof (salt)) ); 

  salt[0] = '5';
  mu_check ( !hash_state_init (&s2, &opts, salt, sizeof (salt)) );
  mu_check ( !hash_state_fill (&s2, in, sizeof (in), salt, sizeof (salt)) ); 


  in[3] = '7';
  mu_check ( !hash_state_init (&s3, &opts, salt, sizeof (salt)) );
  mu_check ( !hash_state_fill (&s3, in, sizeof (in), salt, sizeof (salt)) ); 

  // These checks should all pass w.h.p.
  size_t collisions = 0;
  for (size_t i = 0; i < s1.n_blocks; i++) {
    if ( s1.buffer[i] == s2.buffer[i] ||
            s2.buffer[i] == s3.buffer[i] )
      collisions++;
  }

  mu_check (collisions < 50);

  hash_state_free (&s1);
  hash_state_free (&s2);
  hash_state_free (&s3);
}

void 
mu_test_hash_state_mix (void) 
{
  struct baghash_options opts;
  init_options (&opts);
  struct hash_state s;
  const unsigned char salt[] = "abcdefghijkl";
  const unsigned char in[] = "ZZZZZZZZZZZZ";
  mu_check ( !hash_state_init (&s, &opts, salt, sizeof (salt)) );
  mu_check ( !hash_state_fill (&s, in, sizeof (in), salt, sizeof (salt)) ); 

  for (int i = 0; i < 10; i++)
    mu_check ( !hash_state_mix (&s) );

  unsigned char out[1024];
  mu_check ( !hash_state_extract (&s, out, 1024) );
  mu_check ( out[0] || out[1] || out[2] || out[3] );
  hash_state_free (&s);
}

