
#include "chacha.h"

int main()
{
    u32 x[16], k[8];

    INITIALISE_IV IV;
    INIT_KEY KEY;
    INSERT_KEY InsetKEY;

    IV.chacha();
    KEY.Key256();
    InsetKEY.chacha(x, k);
    PRINT_STATE(x, 16);
}