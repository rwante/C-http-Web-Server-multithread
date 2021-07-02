#include <stdio.h>
#include <stdlib.h>

#include "kuyruk.h"

//Fonksiyonlar

void kuyruk_bas (kuyruk_t * q) //Kuyruk yapısını başlatır.
void kuyruk_yik kuyruk_t* q) //Tüm ilişkili hafızayı boşaltır.
void * kuyruk_cik (kuyruk_t * q) //Kuyruğun önünden elemanı kaldırır.
void * kuyruk_kaldir (kuyruk_t * q, int konum) //Konumdaki elemanı kaldırır.
void * kuyruk_var (kuyruk_t * q, int konum) //Konumdaki elemanı döndürür.
void kuyruk_tekrar (kuyruk_t * q, void * item) /Elemanı sıranın arkasında saklar.
unsigned int kuyruk_sayi (kuyruk_t *q) // Kuyruktaki eleman sayısını döndürür.
void kuyruk_yine (kuyruk_t *q, void(*yine_fonk)(void *, void *), void *arg) //Her elemana işlem uygulamak için yardımcı fonksiyon.