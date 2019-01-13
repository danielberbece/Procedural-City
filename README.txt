                        OpenGL - Procedural City
                        ~~~~~~~~~~~~~~~~~~~~~~~~

        1) Credentiale
    Proiectul a fost realizat de Daniel Berbece, student al Facultatii de
Automatica si Calculatoare, Universitatea Politehnica Bucuresti, 
din grupa 335CA, ca tema in cadrul cursului de Elemente de Grafica pe
Calculator.

        2) Cerinta
    Cerinta proiectului a presupus in crearea unui program care sa creeze,
folosind OpenGL, un oras, in mod procedural. Asta inseamna ca, in urma
desemnarii unor reguli de constructie, orasul sa arate diferit (dar tot a oras)
la fiecare rulare a programului.

        3) Implementare
    Implementarea programului a fost realizata in OpenGL si C++. Un oras consta,
in principiu, din 3 mari elemente:
    a) Cladiri - acestea sunt de trei tipuri: Simple (un bloc de locuit de ex),
Blocky (o cladire mai importanta sau mai veche, care are mai multe blocuri) si
Tower (o cladire de birouri, rotunda si cu geamuri mari). Alegerea fiecarui tip
de cladire se face intamplator, iar generarea este procedurala, in special in
cazul blocurilor Blocky. Fiecare cladire este generata impreuna cu un set de
lumini de noapte, care vor lumina numai in acest caz.
    b) Strazi - acestea formeaza topologia orasului. Generarea lor, procedurala,
consta in urmatoarele reguli:
        * Initial se alege un punct random pe harta, care va fi o intersectie
        * Din fiecare intersectie se pleaca in 3 din 4 directii cate o strada,
mai putin in directia din care a venit aceasta intersectie
        * Fiecare strada merge in acea directie intre 20 si 40 unitati stradale.
        * Daca strada curenta se intersecteaza in crearea ei cu o alta strada,
atunci procesul se intrerupe pentru aceasta. Daca nu, atunci la capat formam o
intersectie si ne intoarcem la punctul 2.
        In urma acestui proces, rezulta o topologie de strazi si spatii goale.
Pentru fiecare spatiu gol, verificam dimensiunile sale. Daca este mica, atunci
cream in acel loc un element decorativ. Daca este mai mare, atunci cream cladiri
in acest spatiu. Cladirile sunt de latime minima 1 unitate stradala si lungime
maxima 3 unitati stradale. Cladirile se adauga la o pozitie random intre aceste
valori, dupa procesul decris mai sus.

	c) Elemente decorative - acestea apar in zonele mai mici, si nu pe strazi.
Elementele decorative constau in: parcuri cu copaci si zone de parcare pentru
masini. Din cauza unor dificultati in importarea obiectelor in OpenGL, obiectele
decorative sunt in minoritate, dar reprezinta unul din viitoarele adaugiri ale
acestui proiect pentru a-l face mai realist.

    Pentru a vedea orasul este nevoie de lumina. Am implementat in acest proiect
doua moduri de vizualizare ale orasului: Zi si Noapte. Pe perioada de zi exista
o singura sursa de lumina (soare), iar pe perioada de noapte fiecare cladire
este luminata de un set de 4 lumini de tip spot, generate procedural o data cu 
cladirea.

        4) Utilizare
    Pentru a face switch intre cele doua tipuri de iluminare apasati tasta SPACE

        5) Future work
    Pe viitor, pentru a aduce aspectul orasului mai aproape de cel al unuia 
real, putem:
        a) adauga masini (element decorativ)
        b) adauga lumini stradale pentru modul de noapte (element decorativ)
        c) adauga regula de creare a unui centru pentru oras, unde cladirile
sa fie mai inalte, iar cele dinspre periferie sa fie mai scunde (Cladiri & 
topologie)
        d) sa facem strazile sa-si schimbe directie nu doar la 90 grade
(topologie)
        e) sa adaugam oameni care sa se plimbe in jurul cladirilor (element
decorativ si dinamic)
