# TODO LIST
1. Zbadać dlaczego przerywa stream przy wykrywaniu konturów, i czasem przy wykrywaniu punktów.
    Przerwania w komunikacji wynikają z mechanizmu odbierz-wyślij w komunikacji miniryś i pc
    Trzeba dać sleep w wysyłaniu obrazu - jest lepiej.
    Może dać tcp zamiast udp?

2. Zrobić automatyczne dobieranie progu przy binaryzacji.
    https://docs.opencv.org/master/d7/d4d/tutorial_py_thresholding.html
    Otsu's Binarization
    
3. Dowiedzieć się jak działa wykrywanie konturu w open cv, móc opisać jego działanie.
4. Zrobić akwizycję sygnałów z regulatora przy testatch minirysia
5. Rozwiązać problem zakrętów
6. Regulacja, sprawdzic czy prędkość oddalania się centrum linii od środka obrazu nie jest lepszym wejściem dla regulatora niż odległosć od środka obrazu
  


