cls

if (Test-Path "lemon.html.c") {
    del "lemon.html.c"
    del "lemon.post.c"
    del "lemon.pre.c"
    del "lexer.html.c"
    del "lexer.post.c"
    del "lexer.pre.c"
}

echo "*.c uzant�l� dosya sat�r say�s�:"
echo "--------------------------------"
(Get-Content ".\*.c" | Measure-Object -line ).Lines
echo ""

echo "*.h uzant�l� dosya sat�r say�s�:"
echo "--------------------------------"
(Get-Content ".\*.h" | Measure-Object -line ).Lines
echo ""

echo "*Tokenizer dosyalar� i�erisindeki sat�r say�s�:"
echo "-----------------------------------------------"
(Get-Content ".\*.ll" | Measure-Object -line ).Lines
echo ""

echo "*Parser gramer dosyalar� i�erisindeki sat�r say�s�:"
echo "---------------------------------------------------"
(Get-Content ".\*.yy" | Measure-Object -line ).Lines
echo ""

echo "JavaScript k�t�phanesi i�erisindeki sat�r say�s�:"
echo "--------------------------------------------"
(Get-Content "..\site\htsql_*.js" | Measure-Object -line ).Lines
echo ""

echo "�rnek/Test ama�l� olu�turulan *.frmx dosyalar� i�erisindeki sat�r say�s�:"
echo "-------------------------------------------------------------------------"
(Get-Content "..\site\pages\*.frmx" | Measure-Object -line ).Lines
echo ""

echo "�rnek/Test ama�l� olu�turulan *.sqlx dosyalar� i�erisindeki sat�r say�s�:"
echo "-------------------------------------------------------------------------"
(Get-Content "..\site\pages\*.sqlx" | Measure-Object -line ).Lines
echo ""


pause