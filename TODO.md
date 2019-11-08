# TODO LIST
1. Zbadać dlaczego przerywa stream przy wykrywaniu konturów, i czasem przy wykrywaniu punktów.
    Przerwania w komunikacji wynikają z mechanizmu odbierz-wyślij w komunikacji miniryś i pc
    Trzeba dać sleep w wysyłaniu obrazu - jest lepiej.
    Może dać tcp zamiast udp?

	Wysyłanie obrazu daje duży narzut na pracę procesora, i wykrywanie linii działa przez to wolniej mimo że wysyłanie obrazu jest w  oddzielnym wątku. Spowolnienie wysyłania obrazu (sleep 100 ms) nie daje dużej różnicy.

2. Zrobić automatyczne dobieranie progu przy binaryzacji.
    https://docs.opencv.org/master/d7/d4d/tutorial_py_thresholding.html
    Otsu's Binarization
	Problem jak robot nie widzi linii, wtedy i tak myśli ze widzii.
	Wystarczy w takim razie dobrać próg ręcznie, musi być odpowiednio mały aby cień nie był wykrywany.
    
3. Usuwanie odblasków
    http://www.amphident.de/en/blog/preprocessing-for-automatic-pattern-identification-in-wildlife-removing-glare.html

    NIE DZIAŁA I NIE ROBIĆ

4. Dowiedzieć się jak działa wykrywanie konturu w open cv, móc opisać jego działanie.
	Można spytać Słonia

5. Zrobić akwizycję sygnałów z regulatora przy testatch minirysia.
	Zrobione. Nagrywanie pliku filmowego, powinno chyba odbywać się po stronie klienta by nie obciążać robota. Nagrywanie pliku filmowego wraz z txt zajmuje na rpi3 do 30 ms czasem (chyba bez streama nawet). Zapisywanie samego pliku tekstowego około 180 microsekund (raczej do 200 microsekund). 

6. Rozwiązać problem zakrętów
    Można analizować współrzędną y środka ciężkości, jeśli jest na dole to zatrzymaj robota i obróć tylko.
    Można obramować kontur najmniejszym prostokatem (minAreaRect), a potem zbadać stosunek boków. Jeśli prostokątm jest œęższy niż wyższy to znaczy że zakręt.
    https://docs.opencv.org/2.4/doc/tutorials/imgproc/shapedescriptors/bounding_rotated_ellipses/bounding_rotated_ellipses.html

	Można spytać Słonia o monenty w konturze

7. Regulacja, sprawdzic czy prędkość oddalania się centrum linii od środka obrazu nie jest lepszym wejściem dla regulatora niż odległosć od środka obrazu
  
8. Strojenie.

- Parametry obrazu 
	- scaleFactor = 0.5, 
	- roi = 1/6 - 5/6, 
	- threshold = 50 

- PID 
	- regulation_period = 0.06 s 

- PID dobry 
	- K = 0.3, 
	- Vconst = 40, 
	- V- = -70, 
	- V+ = 70, 
	- T = 0.06s, 
	- Ti = 50, dla Ti = 2 nie pokonuje trasy, problemem jest ostatni ostry zakręt, Ti = 1 - problemem jest okrągły zakręt, 
	- Td = 0.1 - jest ok ale 0.2 też, dla 0.25 jest słabo wywala sie na przedostatnim zakręcie (oscyluje), im mniej niż 0.1   tym bardziej się buja po zakręcie chyba 

- PID zły
	- K = 0.5, 
	- Ti = 10000, 
	- Td = 0, 
	- Vconst = 30, 
	- V- = -25, 
	- V+ = 25, 

10. Regulator nie trzyma pętli czasowej, poslizg +-3ms.
    Naprawione, timestamp przeskakuje o 1ms bo gromadzi się czas związany z zapisywaniem do pliku. Ale generalnie pętla trzyma około 60 ms.

11. Wyeliminować segmentation fault jak nie znajduje konturu.
	Naprawione.

12. Wyliczyć jakiś wskaźnik jakości otrzymanych przebiegów (da sie bo można zmierzyć współczynnik od momentu skrętu).
13. Porobić statystyki: odchylenie standartowe, min i max w tabelkach czasów.
14. Porobić statystyki: ile razy rys przejechał poprawnie skrzyżowanie/zakręt (moze tylko porobić dla dostrojonego regulatora)?
15. Sprawdzić czy da sie postawic ubuntu na rpi.
16. Przerobic drugi algorytm wykrywania linii.
17. Poprawić odniesienie do wzorów na \eqref
18. Przerobić diagram modułów oprogramowania, zrobić diagramy sekwencji, używać enterprise architect.
