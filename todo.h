#ifndef TODO_H_INCLUDED
#define TODO_H_INCLUDED



/** TODO (Administrator#1#):    samart bar üzerinden '+' tuşu ile oluşturulan satır '-' tuşu ile ile silinmek istendiğinde hata invalid ROW ID hatası oluşuyor.
*/


/** TODO (Administrator#1#):    Insert işleminde oluşan hata sonrası aynı satırda düzeltme yapılmasına rağmen satır değişikliği DIRTY bilgilerini sildiği için satırı
                                insert edemiyor. Silmekte mümkün olmuyor
*/


/** TODO (Administrator#1#):    Image'ı silebilme butonu ekle
*/

/** TODO (Administrator#1#):    if then els if; bloğu içinde "else" ifadesini içine alan bir /* */ comment pars işleminin yanlış çalışmasına sebep oluyor
*/

/** TODO (Administrator#1#):    dizin işlemleri için gerekli fonksiyonları ekle.
                                Yeni klasör oluşturmak, silmek, listelemek
*/

/** TODO (Administrator#1#): Recursive fonksyion icinde "return" statement doğru calismiyor.
                             Core test'deki "function_testi_13"u kullanarak düzeltme yap.
                             Ayrıca: function_testi_13.h dosyasında varsa gerekli düzeltmeyi yap.
*/

/** TODO (Administrator#1#): do { } while () statement ekle. HT/SQL Manual'deki do_statement.h dosyasına uygun olsun.
*/


/** TODO (Administrator#1#): While döngüsünden "break" ile çıkabiliyor muyuz ?  HT/SQL Manual'deki break_statement.h örneği çalişsın
*/


/** TODO (Administrator#1#): 007. Control block item'larının HTSQL veya NATIVE her iki tur içinde stack'e eklenmesine ve request bitene kadar silinmemesini sağla.
                                 Şu anda sadece HTSQL procedure cagrilari için ekleniyor. Benzer islem native procedure icinde yapılmalı.

                                 HTSQL procedure'lerde sıkıntı yok gibi duruyor olsa da bir procedure diğer proceduru cagirdiginda sıkıntı çikacak. Cunku her HTSQL procedure
                                 kendi stack alanını sıfırdan başlatıyor. Control block parametreleri kopyalamak cozum değil. Procedureler değişkenin değerini değiştirirse
                                 bir onceki procedure değişen haline erişmeli. Bu yüzden control item'lerın deüerleri değil ama adresleri yeni stacke eklenebilir. Bu durumda da
                                 silme işlemi tekrarlı olabilir gibi duruyor. (her htsql fonksiyon kendi stack'ını siliyor çünki). Çözüm ne ?
                                 Memory tüketmeden ve aynı zamanda local değişkenler için tanımlanan stack yapısını bozmadan şu olabilir : Her cağrı bailangıcında
                                 (HTSQL veya native farketmez) control block item'larını adları ile birlikte bir linked liste eklemek ve çağrı bittiğinde silmek.
                                 Bu işlem sonrasında tabiki su anda control block item'larını stack'den arayarak bulan yerlere bir de bu listeden ara dememiz gerekiyor.
*/
/** TODO (Administrator#1#): 007. C kodu içinde yer alan hata mesajlarının farklı bir dosyadan hata mesajı numarasına göre okunarak client'a gönderilmesini sağla.
                                Bu dosya UTF-8 formatında olmalı. Hata mesajları standart olmalı ve LOCALIZATION'a gore farklı dosyalardan okunmalı
*/
/** TODO (Administrator#1#): 002.   If () then
                                    else
                                    end if;

                                    komut dizesine

                                    if () rhen
                                    elseif () then   <----- ozelligi ekle
                                    end if;

*/



/** NEXT VERSION     */

/** TODO (Administrator#1#): Tree bileseni ekle
*/


/** DONE */


/** TODO (Administrator#1#):    Data block'a
                                    pre_delete
                                    pre_update
                                    pre_insert
                                trigger'larını eklendi.
                                BU triggerlardan false donulduğunde değiştirilen satır eski değer/görünümüne dönsün
*/

/** TODO (Administrator#1#):    Data block'un "pre_..." ve "post_..." triggerlarını HT/SQL Manual'e ekle
*/
/* TODO (Administrator#3#): Image bilesenine upload özelliği ekle  */

/* TODO (Administrator#3#): Text item alanlarının veritabanı işlemlerinde binding özelliğini kullan */

/*  Insert, update ve delete statement'lerini ekle
    Otomatik  master - detay block relation ozelligi ekle
    Htsql Procedure cagirmayi ekle
    If komutu içinde "return" komutunun kullanımındaki hatayı düzekt.
    Local variable ozelligi ekle
    degisken tanımlama satırında ilk değer ataması yapılabilinsin
       char str = "deneme";
       gibi
    degisken tanımlama satırında virgül ile ayrılmış birden fazla değişken tanımlanabilsin.
        int a, b, c;
        char str, str1;
    Web sayfaları arasýnda gezinti ozelligi ekle

*/
/* sizeof() function'ı ekle
                                HT/SQL Manual'deki sizeof_operator.html dosyasına bakabilirsin.
*/
/*    Ondalıklı sayı ekle.
                                int a = 10.02 gibi
*/
/*  Arıtmatiksel veya karşılaştırma işlemlerinde fonksiyon cağrılarının öncelik derecesi nedir ?
                            int ret;
                            ret = a + b * function();
                            durumunda sonuc ne olacak ?
                            Dokumanda yer alan "Operator Precedence" sayfasına bak

    C bilem bir şey diyemiyor bu konuda. Optimizer gibi durumlara bağlı olarak değişebiliyormuş. Bize farklı bir öncelikli operator kullanılmadıysa soldan sağa
*/
/*  Moduler bölme işlevi ekle x = 10 % 2;
*/

/*  Eksi sayılar için - operantı ekle x = -2;  (Negation)
*/

/* ! (not) operatorünü ekle. true'yu false, false'u true olarak cevirmeli
*/

/*    string karşılaştırma operandlarına "like" ve "not like" operandlarını ekle
*/

/*  004. Control block itemlarının değerinin sunumcudaki bir triggerdan değiştirilebilmesi özelliği var mı ? kontrol et, ekle.
*/

/* 004. User Login/log out ozelligi ekle
*/

#endif //
