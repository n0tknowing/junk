/*
   bagi yang sedang belajar C, dan belum pernah mendengar yang namanya *bit fields*,
   dia itu fungsinya untuk mengatur berapa banyak bit yang bisa ditampung.
   terus juga *bit fields* cuma bekerja di `struct` ama `union`, dan data tipe
   yang bisa digunakan cuma:
     - `signed int`,
     - `unsigned int`,
     - `bool`, dan
     - `int`.
   buat tipe data yang `int`, ini lebih ke *implementation-defined*, di mana
   `signed` atau `unsigned`-nya tergantung banyak hal, jadi besar kemungkinan
   kode nanti gak portabel.

   contohnya:

   ```c
   struct bit_fields {
     unsigned int a : 2;
   };
   ```

   kita atur `bit fields` untuk `a` sebanyak 2, jadi `binary(11) = 3`.
   yang artinya kita cuma punya nilai minimum 0 (karena unsigned) dan
   maksimum 3 untuk `bit_fields.a`.

   contoh lain:
   ```c
   struct bit_fields {
     unsigned int a : 10;
   };
   ```

   jadi, `binary(11 1111 1111) = 1023`, batas maksimum 1023.
*/

#include <stdio.h>

int main(void)
{
    struct bit_fields {
        unsigned int a : 2;
    };

    struct bit_fields test;

    /*
     * ????
     * test.a = -2u;
     * printf("%u\n", test.a);
     *
     * test.a = 4u;
     * printf("%u\n", test.a);
     */

    test.a = 0u;
    printf("%u\n", test.a);

    test.a = 1u;
    printf("%u\n", test.a);

    test.a = 2u;
    printf("%u\n", test.a);

    test.a = 3u;
    printf("%u\n", test.a);

    return 0;
}
