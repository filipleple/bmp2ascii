#include <iostream>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <cstring>

using namespace std;

struct BMPinfo {
    char sygnatura[2];
    short int reserved1;
    short int reserved2;
    int rozmiar;
    short int offset;

    short int infHeadSize;
    int szer;
    int wys;
    short int planes;
    short int bitcount;
    short int comp;
    int rozmiarRysunku;
    short int XPelsPM;
    short int YPelsPM;
    short int clrUses;
    short int clrImportant;
    int l_bajt_0;

};

struct pixel {
    int red, green, blue;
};

void piksele_init(pixel** piksele, BMPinfo info) {
    for (int i = 0; i < info.wys; i++) {
        piksele[i] = new pixel[info.szer];
    }

}

void otworz_plik(string& nazwa, ifstream& plik) {
    while (1) {
        cout << "Podaj nazwe pliku: ";
        cin >> nazwa;
        

        plik.open(nazwa, ios::in | ios::binary);

        if (!plik) {
            cout << "Nie znaleziono pliku, wprowadz nazwe ponownie." << endl;
            cin.clear();
            
            continue;
        }

        char sygnatura[3];
        plik.read(sygnatura, 2);
        sygnatura[2] = NULL;
        

        char sygn[3] = "BM";

        if (strcmp(sygnatura, sygn))
        {
            cout << "To nie jest plik BMP. ";
            plik.close();
            continue;
        }
        else {
            plik.seekg(28, ios::beg);
            short int bitcount;
            plik.read(reinterpret_cast<char*>(&(bitcount)), 2);
            
            if (bitcount==24)
                break;
            else {
                cout << "Oczekiwano pliku 24bit" << endl;
                continue;
            }
        }
    }

}

void wczytaj_info(BMPinfo& info, ifstream& plik) {
    plik.seekg(ios::beg);
    plik.read(reinterpret_cast<char*>(&(info.sygnatura)), 2);
    plik.read(reinterpret_cast<char*>(&(info.rozmiar)), 4);
    plik.read(reinterpret_cast<char*>(&(info.reserved1)), 2);
    plik.read(reinterpret_cast<char*>(&(info.reserved2)), 2);
    plik.read(reinterpret_cast<char*>(&(info.offset)), 4);
    plik.read(reinterpret_cast<char*>(&(info.infHeadSize)), 4);
    plik.read(reinterpret_cast<char*>(&(info.szer)), 4);
    plik.read(reinterpret_cast<char*>(&(info.wys)), 4);
    plik.read(reinterpret_cast<char*>(&(info.planes)), 2);
    plik.read(reinterpret_cast<char*>(&(info.bitcount)), 2);
    plik.read(reinterpret_cast<char*>(&(info.comp)), 4);
    plik.read(reinterpret_cast<char*>(&(info.rozmiarRysunku)), 4);
    plik.read(reinterpret_cast<char*>(&(info.XPelsPM)), 4);
    plik.read(reinterpret_cast<char*>(&(info.YPelsPM)), 4);

    info.l_bajt_0 = info.szer % 4;

}

void wypisz_info(BMPinfo& info, string nazwa) {
    cout << endl << endl << "---" << nazwa << "---" << endl << endl;
    cout << "Typ: " << info.sygnatura << endl;
    cout << "Rozmiar pliku: " << info.rozmiar << endl;
    cout << "Pozycja danych obrazowych w pliku: " << info.offset << endl;
    cout << "Rozmiar naglowka informacyjnego: " << info.infHeadSize << endl;
    cout << "Szerokosc w pikselach: " << info.szer << endl;
    cout << "Wysokosc w pikselach: " << info.wys << endl;
    cout << "Liczba platow: " << info.planes << endl;
    cout << "Liczba bitow na piksel: " << info.bitcount << endl;
    cout << "Algorytm kompresji: " << info.comp << endl;
    cout << "Rozmiar rysunku: " << info.rozmiarRysunku << endl;
    cout << "Rozdzielczosc pozioma: " << info.XPelsPM << endl;
    cout << "Rozdzielczosc pionowa: " << info.YPelsPM << endl;
    cout << endl << "-----------------------" << endl;

}

void wczytaj_piksele(ifstream& plik, BMPinfo info, pixel** piksele) {
    plik.seekg(info.offset, ios::beg);

    unsigned char R, G, B;

    for (int i = 0; i < (info.wys); i++)
    {
        for (int j = 0; j < (info.szer); j++) {
            plik.read((char*)&B, sizeof(unsigned char));
            plik.read((char*)&G, sizeof(unsigned char));
            plik.read((char*)&R, sizeof(unsigned char));

            piksele[i][j].red = int(R);
            piksele[i][j].green = int(G);
            piksele[i][j].blue = int(B);

        }
        if (info.l_bajt_0) {
            for (int i = 0; i < info.l_bajt_0; i++) {
                int czarnek = 0;
                plik.read((char*)&czarnek, sizeof(unsigned char));
            }
        }


    }


}

void pixel_na_znak(BMPinfo info, pixel** piksele, char** znaki){

    for (int i=0; i<info.wys; i++){
        for(int j=0; j<info.szer; j++){
            if (piksele[i][j].red<=100){
                znaki[i][j] = 'a';
            }
            else{
                znaki[i][j] = '.';
            }
        }
    }

}

void wypisz_znaki(BMPinfo info, char** znaki){
    for (int i=info.wys-1; i>=0; i--){
        for (int j=info.szer-1; j>=0; j--){
            cout << znaki[i][j];
        }
        cout << endl;
    }
}


int main()
{
    string nazwa;
    char nazwa_out[50];
    ifstream plik;
    
    BMPinfo info;
    
    otworz_plik(nazwa, plik);
    wczytaj_info(info, plik);
    wypisz_info(info, nazwa);

    pixel** piksele = new pixel * [info.wys];
    piksele_init(piksele, info);

    char** znaki = new char* [info.wys]; 
    for (int i = 0; i < info.wys; i++) {
        znaki[i] = new char[info.szer];
    }
        
    wczytaj_piksele(plik, info, piksele);

    pixel_na_znak(info, piksele, znaki);
    wypisz_znaki(info, znaki);

    plik.close();
  

}
