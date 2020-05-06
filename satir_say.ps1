cls

if (Test-Path "lemon.html.c") {
    del "lemon.html.c"
    del "lemon.post.c"
    del "lemon.pre.c"
    del "lexer.html.c"
    del "lexer.post.c"
    del "lexer.pre.c"
}

echo "*.c uzantýlý dosya satýr sayýsý:"
echo "--------------------------------"
(Get-Content ".\*.c" | Measure-Object -line ).Lines
echo ""

echo "*.h uzantýlý dosya satýr sayýsý:"
echo "--------------------------------"
(Get-Content ".\*.h" | Measure-Object -line ).Lines
echo ""

echo "*Tokenizer dosyalarý içerisindeki satýr sayýsý:"
echo "-----------------------------------------------"
(Get-Content ".\*.ll" | Measure-Object -line ).Lines
echo ""

echo "*Parser gramer dosyalarý içerisindeki satýr sayýsý:"
echo "---------------------------------------------------"
(Get-Content ".\*.yy" | Measure-Object -line ).Lines
echo ""

echo "JavaScript kütüphanesi içerisindeki satýr sayýsý:"
echo "--------------------------------------------"
(Get-Content "..\site\htsql_*.js" | Measure-Object -line ).Lines
echo ""

echo "Örnek/Test amaçlý oluþturulan *.frmx dosyalarý içerisindeki satýr sayýsý:"
echo "-------------------------------------------------------------------------"
(Get-Content "..\site\pages\*.frmx" | Measure-Object -line ).Lines
echo ""

echo "Örnek/Test amaçlý oluþturulan *.sqlx dosyalarý içerisindeki satýr sayýsý:"
echo "-------------------------------------------------------------------------"
(Get-Content "..\site\pages\*.sqlx" | Measure-Object -line ).Lines
echo ""


pause