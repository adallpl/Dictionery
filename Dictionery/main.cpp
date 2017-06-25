#include <QCoreApplication>
#include <fstream>//plik
#include <iostream>
#include <string>
#include <map>
#include <utility>//pair<>
#include <cstdlib>//exit()
#include <ctime>
#include <iterator>
#include <list>
#include <conio.h>//getch()
#include <windows.h>//beep
#include<mmsystem.h>

// deklarujemy aliasy typow, ktorych uzywamy w calym programie
using Slownik = std::map<std::string, std::string>;
using ParaSlownikow = std::pair<Slownik, Slownik>;

// plik, z ktorego odczytujemy i do ktorego zapisujemy slowa
const std::string PLIK_SLOWNIK = "Words.txt";

void dodaj_slowo_do_slownika(std::string polskie, std::string angielskie, ParaSlownikow& para)
{
    /// zapisujemy slowo w obu slownikach:
    /// polsko-angielskim
    para.first[polskie] = angielskie;
    /// i angielsko-polskim
    para.second[angielskie] = polskie;
}

void zapisz_caly_slownik_do_pliku(Slownik& slownik, const std::string& nazwa_pliku)
{
    std::fstream od(nazwa_pliku, std::ios::out);
    // zapisujemy liczbe slow w slowniku do pliku

    // zapisujemy slowa, nizej sprawdzanie bledow
    for (const auto& slowo : slownik)
    {
        od << slowo.first << " " << slowo.second << '\n';
    }
    if (!od)
    {
        std::cout << "Nie udalo sie zapisac slow do pliku" << std::endl;
        exit(1);
    }
    else
    {
        std::cout << "Zapisano slownik do pliku" << std::endl;
    }
}

void dopisz_slowo_do_pliku(const std::string& PL,
                           const std::string& ENG,
                           const std::string& nazwa_pliku)
{
    /// app - dopisujemy do konca pliku
    std::fstream od(nazwa_pliku, std::ios::app);
    od << PL << " - " << ENG << std::endl;
}

std::pair<Slownik, Slownik> wczytaj_slowa_z_pliku(const std::string& nazwa_pliku)
{
    // para slownikow, ktora zwrocimy po wczytaniu wszystkich slow
    std::pair<Slownik,Slownik> para;
    std::ifstream od(nazwa_pliku);
    std::string polskie;
    std::string angielskie;
    // najpierw wczytujemy liczbe slow
    /*if (!od.good())
    {
        std::cerr << "Nie udalo sie wczytac slow z pliku\n";
       std::cerr << od.rdstate();
        exit(1);
    }*/
    if ( (od.rdstate() & std::ifstream::failbit ) != 0 )
    {
       std::cerr << "Error opening: "<< PLIK_SLOWNIK << std::endl;
       std::cout << od.eofbit <<" "<< od.badbit <<" "<< od.failbit << std::endl;
       exit(-1);
    }
    /// ta petla wczyta dowolna liczbe par slow - nie bierzemy pod uwage
    /// wartosci liczba_slow
    while (od >> polskie >> angielskie)//ten while przeszukuje plik txt
    {
        dodaj_slowo_do_slownika(polskie, angielskie, para);
    }
    return para;
}

void wyswietl_tlumaczenie(const std::string& szukany, ParaSlownikow& para)
{
    // sprawdzamy czy slowo wystepuje w slowniku polsko-angielskim jako klucz
    // tj. slowo polskie
    if (para.first.find(szukany) != std::end(para.first))
    {
        // ...i wyswietlamy
        std::cout << "Tlumaczenie:   " << para.first[szukany] << std::endl;
        // to samo dla ang.-pol.
    }
    else if (para.second.find(szukany) != std::end(para.second))
    {
        std::cout << "Tlumaczenie:   " << para.second[szukany] << std::endl;
    }
    else
    {
        std::cout << "Nie znaleziono tlumaczenia" << std::endl;
    }
}

void wyswietl_slowa_na_litere(ParaSlownikow& para, char litera='\0', bool polskie=true)
{
    /// pomocnicza funkcja wyswietlajaca slowo, sprawdzajac wczesniej, czy jest
    /// na litere, po ktorej filtrujemy (jesli chcemy filtrowac - w przeciwnym
    /// razie zmienna  litera ma wartosc domyslna - '\0'
    auto wyswietl_slowa = [&para, &litera](Slownik slownik)
    {
        for (const auto& slowo : slownik)
        {
            if (litera != '\0')
            {
                // sprawdzamy pierwszy znak slowa
                if (slowo.first.front() == litera)
                {
                    std::cout << slowo.first << "\t-\t" << slowo.second << std::endl;
                }
            }
            else
            {
                std::cout << slowo.first << "\t-\t" << slowo.second << std::endl;
            }
        }
    };
    // jesli szukamy polskich, to wywolujemy powyzsza funkcje
    if (polskie)
    {
        // dla slownika pol.-ang.
        wyswietl_slowa(para.first);
    }
    else
    {
        // w przeciwnym wypadku ang.-pol.
        wyswietl_slowa(para.second);
    }
}

void poprawa_literowki(ParaSlownikow& para,
                       const std::string& poprzednia_wersja,
                       const std::string& nowa_wersja)
{
    // pomocnicza funkcja poprawiajaca literowke
    auto popraw_slowo = [&poprzednia_wersja,&nowa_wersja](Slownik& s1, Slownik s2)
    {
        // najpierw zapisujemy sobie tlumaczenie danego slowa
        std::string tlumaczenie = s1[poprzednia_wersja];
        // usuwamy stara wersje slowa ze slownika s1
        s1.erase(poprzednia_wersja);
        // dodajemy nowa wersje
        s1[nowa_wersja] = tlumaczenie;
        // do drugiego slownika zapisujemy nowa wersje (slowo w przeciwnym
        // jezyku, czyli tlumaczenie - zostaje bez zmian)
        s2[tlumaczenie] = nowa_wersja;
    };
    if (para.first.find(poprzednia_wersja) != std::end(para.first))
    {
        // jesli to slowo polskie, to w powyzszej funkcji przyjmujemy:
        // s1 - pol.-ang.
        // s2 - ang.-pol.
        popraw_slowo(para.first, para.second);
        // no i zapisujemy poprawke do pliku
        zapisz_caly_slownik_do_pliku(para.first, PLIK_SLOWNIK);
        std::cout << "Poprawiono :-)!" << std::endl;
    }
    else if (para.second.find(poprzednia_wersja) != std::end(para.second))
    {
        // analogicznie jesli to slowo angielskie
        popraw_slowo(para.second, para.first);
        zapisz_caly_slownik_do_pliku(para.first, PLIK_SLOWNIK);
        std::cout << "Poprawiono :-)!" << std::endl;
    }
    else
    {
        std::cout << "Nie znaleziono slowa: " << poprzednia_wersja << std::endl;
    }
}
Slownik slownik;
int losuj(int roz);

void  wyswietl_jedno_losowe(ParaSlownikow& para, bool polskie = true)
{
    auto wyswietl = [&para](Slownik slownik)
    {
        auto item = slownik.cbegin();
        std::advance(item, losuj(slownik.size()));//advance zwraca wskanzik do elementu ze zbioru, tutaj mapa
        std::cout << item -> first << std::endl;
    };
    if(polskie)
    {
        wyswietl(para.first);
    }
    else
    {
        wyswietl(para.second);
    }
}
void wyswietl_slowo(ParaSlownikow& para,  int& numer, bool polskie = true)
{
    auto wyswietl = [&para, &numer](Slownik slownik)
    {
        auto item = slownik.cbegin();
        std::advance(item, numer);//advance zwraca wskanzik do elementu ze zbioru, tutaj mapa
        std::cout << item -> first << std::endl;
    };
    if(polskie)
    {
        wyswietl(para.first);
    }
    else
    {
        wyswietl(para.second);
    }
}
void test_PL_ANG(float& dobre, ParaSlownikow& para, const std::string& slowo_uzytk, bool polskie = true)
{
    if(polskie)
    {
        if(para.second.find(slowo_uzytk) != std::end(para.second)) // end() - iterator
        {
            if(para.second[slowo_uzytk] == para.second[slowo_uzytk])
            {
               // sndPlaySound("C:\\Users\\adalb\\Documents\\QT_C++\\slownik\\audio\\dobra_odp.wav", SND_ASYNC);
                dobre+=1.0;
                std::cout<<"GOOD"<<std::endl;
            }
            else
            { ///C:\Users\adalb\Documents\QT_C++\slownik\audio
             //   sndPlaySound("C:\\Users\\adalb\\Documents\\QT_C++\\slownik\\audio\\zla_odp.wav", SND_ASYNC);
            }
        }
        else
        {
          //  sndPlaySound("C:\\Users\\adalb\\Documents\\QT_C++\\slownik\\audio\\zla_odp.wav", SND_ASYNC);
        }
    }
    else
    {
        if(para.first.find(slowo_uzytk) != std::end(para.first)) // end() - iterator
        {
            if(para.second[slowo_uzytk] == para.second[slowo_uzytk])
            {
               // sndPlaySound("C:\\Users\\adalb\\Documents\\QT_C++\\slownik\\audio\\dobra_odp.wav", SND_ASYNC);
                std::cout<<"GOOD"<<std::endl;
                dobre += 1.0;
            }
            else
            {
              //  sndPlaySound("C:\\Users\\adalb\\Documents\\QT_C++\\slownik\\audio\\zla_odp.wav", SND_ASYNC);
            }
        }
        else
        {
        //    sndPlaySound("C:\\Users\\adalb\\Documents\\QT_C++\\slownik\\audio\\zla_odp.wav", SND_ASYNC);
        }
    }
}
int wiel_kon(const std::string PLIK_SLOWNIK)
{
    int liczba_lini = 0;
    std::string linia;
    std::ifstream moj_plik(PLIK_SLOWNIK);

    while (std::getline(moj_plik, linia))
        ++liczba_lini;
    return liczba_lini;
}
int *te_byly = new int[wiel_kon(PLIK_SLOWNIK)];//globalna zmienna potrzebna do funkcji losuj()
int losuj(int roz)
{
    srand(time(NULL));
    bool juz_byla;
    static int wylosowana;
    int liczba;
    do
    {
        liczba = ((rand() % (roz)) + 0);
        juz_byla = false;
        for(int i = 0 ; i <= wylosowana ; i++)
        {
            if(te_byly[i]==liczba) juz_byla = true;
        }
        if(juz_byla == false)
        {
            te_byly[wylosowana] = liczba;
            wylosowana++;
        }
    }
    while(juz_byla==true);
    return liczba;
}
void test_slow(float& dobre, ParaSlownikow& para, std::string& slowo_uzytk, float& wielkosc_testu, bool polskie = true)
{
    int  i = 0;
    do
    {
        int numer = losuj(wiel_kon(PLIK_SLOWNIK));
        std::cout<<i+1<<". ";
        wyswietl_slowo(para, numer, polskie);
        std::cin>>slowo_uzytk;
        test_PL_ANG(dobre, para, slowo_uzytk, polskie);
        i++;
    }
    while(i < wielkosc_testu);
}
void wynik_testu(float& wielkosc_testu, float& dobre)
{
    float wynik = dobre/wielkosc_testu;
    std::cout<<"Twoj wynik to:  "<<dobre<<"/"<<wielkosc_testu<<" ";
    std::cout<<(wynik*100.0)<<" %"<<std::endl;
}
int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);

    std::cout << "Nazwa programu to: " << argv[0] << std::endl;
    std::cout << "Pozostale jego parametry:\n";
    for(int i = 1; i < argc; i++)
    {
        std::cout << "Parametr nr. " << i << " to: " << argv[i] << std::endl;
    }
    bool koniec = 0;
    auto para_slownikow = wczytaj_slowa_z_pliku(PLIK_SLOWNIK);
    // void (*wsk_funk)(std::string);
    while (!koniec)
    {
        std::cout<<"Baza slowek: "<<wiel_kon(PLIK_SLOWNIK)<<std::endl;
        std::cout << "1. Slownik" << std::endl;
        std::cout << "2. Dodaj do slownika" << std::endl;
        std::cout << "3. Pokaz slownik" << std::endl;
        std::cout << "4. Wygenerowanie strony" << std::endl;
        std::cout << "5. Exit" << std::endl;
        std::cout << "6. Wyswietl slowa na dana litere" << std::endl;
        std::cout << "7. Popraw literowke" << std::endl;
        std::cout << "8. Test" << std::endl;
        char wybor = '\0';
        std::cin >> wybor;
        // wewnatrz instrukcji switch po prostu wywolujemy powyzsze funkcje
        // zadnej magii ;-)
        switch (wybor)
        {
        case '1':
        {
            std::string szukany;
            std::cout << "Szukany wyraz: ";
            std::cin >> szukany;
            wyswietl_tlumaczenie(szukany, para_slownikow);
            break;
        }
        case '2':
        {
            std::string PL;
            std::string ENG;
            std::cout << "Podaj PL" << std::endl;
            getline(std::cin, PL);
            getline(std::cin, PL);
            std::cout << "Podaj ENG" << std::endl;
            getline(std::cin, ENG, '\n');
            dodaj_slowo_do_slownika(PL, ENG, para_slownikow);
            dopisz_slowo_do_pliku(PL, ENG, PLIK_SLOWNIK);
            break;
        }
        case '3':
        {
            std::cout << "\n-----SLOWNIK-----\n" << std::endl;
            wyswietl_slowa_na_litere(para_slownikow);
            std::cout << std::endl;
            break;
        }

        case '4':
        {
            std::ofstream od;
            od.open("strona.html");

            if (od.good()==false)
            {
                std::cout << "Plik nie istnieje!";
                exit(1);
            }
            //html(od,tab,roz);
            od.close();
            std::cout << std::endl;
            break;
        }

        case '5':
        {
            koniec = 1;
            break;
        }

        case '6':
        {
            std::cout<<"Podaj slownik:"<<std::endl;
            std::cout<<"1 - polski"<<std::endl;
            std::cout<<"2 - angielski"<<std::endl;
            int wyb;
            std::cin>>wyb;
            if(wyb==1)
            {
                char litera = '\0';
                std::cout << "Podaj litere, od ktorej maja zaczynac sie wyswietlone slowa: ";
                std::cin >> litera;
                wyswietl_slowa_na_litere(para_slownikow, litera);

            }
            else if(wyb==2)
            {
                char litera = '\0';
                std::cout << "Podaj litere, od ktorej maja zaczynac sie wyswietlone slowa: ";
                std::cin >> litera;
                wyswietl_slowa_na_litere(para_slownikow, litera, false);
            }
            else
            {
                std::cout<<"Zly wybor!!!"<<std::endl;
            }
            break;
        }
        case '7':
        {
            std::string stara;
            std::string nowa;
            std::cout << "Podaj slowo, w ktorym chcesz poprawic literowke: ";
            std::cin >> stara;
            std::cout << "Podaj poprawiona wersje: ";
            std::cin >> nowa;
            poprawa_literowki(para_slownikow, stara, nowa);
            break;
        }
        case '8':
        {
            float dobre = 0.0;
            std::cout<<"1 - angielsko-polski"<<std::endl;
            std::cout<<"2 - polsko-angielski"<<std::endl;
            int wyb;
            std::cin >> wyb;
            if(wyb==1)
            {
                std::cout<<"Podaj ilosc slowek w tescie"<<std::endl;
                float wielkosc_testu;
                std::cin>>wielkosc_testu;
                std::string slowo_uzytk;
                test_slow(dobre, para_slownikow, slowo_uzytk, wielkosc_testu, false);
                wynik_testu(wielkosc_testu, dobre);
            }
            else if(wyb==2)
            {
                std::cout<<"Podaj ilosc slowek w tescie"<<std::endl;
                float wielkosc_testu;
                std::cin>>wielkosc_testu;
                std::string slowo_uzytk;
                test_slow(dobre, para_slownikow, slowo_uzytk, wielkosc_testu);
                wynik_testu(wielkosc_testu, dobre);
            }
            break;
        }
        default:
            std::cout << "Wcisnieto niewlasciwy klawisz" << std::endl;
        }
    }
    return a.exec();
}

/// do poprawienia:
/// - wypisywanie poprawnej odpowiedzi w tescie w przypadku blednej odpowiedzi w funkcji test
/// - komunikat w przypadku dodawania slowa juz dodanego w funkcji dodaj slowo, pokazujacy ze juz takie slowo jest i wpisanie jeszcze raz
