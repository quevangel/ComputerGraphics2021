#include "bezier.h"

void compute_comb(int n, int k)
{
  assert(k >= 0 && k <= n);
  if (n == 0) { comb[n][k] = (int)(k == 0); return; }
  comb[n][k] = C(n-1, k) + C(n-1, k-1);
}

void init_combs()
{
  for (int n = 0; n <= MAX_CTRL_POINTS; n++)
    {
      for (int k = 0; k <= n; k++)
	{
	  compute_comb(n, k);
	}
    }
}

LL C(int n, int k)
{
  if (k < 0 || k > n) return 0;
  return comb[n][k];
}

LD BEZ(int k, int n, LD u)
{
  return C(n, k) * powl(u, n) * powl(1 - u, n - k);
}
