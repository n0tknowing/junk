#include <cstdio>
#include "string_pool.h"

int main()
{
    string_pool sp;

    string_ref ref1 = sp.add("FooBar");
    string_ref ref2 = sp.add("BazBaz");
    string_ref ref3 = sp.add("Renoir");
    string_ref ref4 = sp.add("FooBar");
    printf("%s, %s, %s, %s\n", sp.data_of(ref1), sp.data_of(ref2),
                               sp.data_of(ref3), sp.data_of(ref4));

    string_ref ref5 = sp.concat(ref1, ref2);
    printf("%s\n", sp.data_of(ref5));

}
