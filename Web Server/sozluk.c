#include <search.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "sozluk.h"

//Fonksiyonlar

void sozluk_bas (sozluk_t* d) //İlk olarak çağrılıp sözlük veri yapısını başlatır.
int sozluk_ekle (sozluk_t * d, const char * key, const char * value) //Sözlüğe anahtar, değer çifti ekler.
const char * sozluk_al (sozluk_t * d, const char * key) //Belirli bir anahtar için sözlükten değeri alır.
int sozluk_ayir (sozluk_t* d, char * key_value) //key_value dizesini ayrıştırır ve ayrıştırılmış anahtarı ve değeri sözlüğe ekler.
int sozluk_kaldir (sozluk_t * d, const char * key) //Sözlükten (anahtar, değer) girişini kaldırır.
void sozluk_yik sozluk_t* d)//Sözlükle ilişkili tüm dahili belleği boşaltır.