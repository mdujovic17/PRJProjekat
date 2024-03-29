﻿//Marko Dujović, NRT-85/19
#define _CRT_SECURE_NO_WARNINGS //Noviji VS kompajler zahteva ovaj fleg za funkciju strcpy(). U suprotnom se koristi strcpy_s()

#include <iostream>
#include <fstream>
#include<string>

#include <stdio.h>
#include <ctype.h>

#define MAX 80
#define BROJ 20

using namespace std;

//Strukture koje sadrže sve informacije o zaposlenom.
struct adresaStanovanja { char imeUlice[MAX + 1], broj[5], mesto[BROJ + 1], grad[BROJ + 1]; };
struct kontakt			{ char tel[14], mail[80 + 1]; };
struct datum			{ char godina[4 + 1], mesec[2 + 1], dan[2 + 1]; };
struct zaposleni		{ int idZaposlenog; char ime[BROJ + 1], prezime[BROJ + 1]; datum datRodjenja; kontakt kt; adresaStanovanja adresa; };

//Globalne promenljive koje se pozivaju iz većine funkcija
fstream f;
char imeFajla[MAX + 1];

//Prototipi funkcija
void info();
char* generisiKljuc(char* sifra, char* kljuc);
void upisPodataka(zaposleni* osoba, int j, int identifikator, char* imeF = imeFajla);
int citanjePodataka(zaposleni* osoba, int j, char* imeF = imeFajla);

void proveraFajla(char* imeF = imeFajla);

int sifrovanje(char* imeF = imeFajla);
int E_cezar(int odstup, char* imeF = imeFajla);
int E_vizner(char* sifra, char* imeF = imeFajla);

int desifrovanje(char* imeF = imeFajla);
int D_cezar(int odstup, char* imeF = imeFajla);
int D_vizner(char* sifra, char* imeF = imeFajla);

char* cezarSifrovanje(char* tekst, int odstup);
char* viznerSifrovanje(char * tekst, char* sifra);
char* cezarDesifrovanje(char* tekst, int odstup);
char* viznerDesifrovanje(char* tekst, char * kljuc);


int unosPodataka(zaposleni *osoba, int n, char* imeF = imeFajla);
void prikazPodataka(zaposleni* osoba, int n, char* imeF = imeFajla);
int izmenaPodataka(char* imeF = imeFajla);


int main()
{
	int i, n;
	char c;
	zaposleni* bazaZaposlenih;

	//Unos imena fajla i provera da li fajl vec postoji
	cout << "Unesite ime fajla gde ce se cuvati podaci: "; cin.getline(imeFajla, MAX, '\n');
	proveraFajla();

	//Upit za korisnika da unese broj osoba
	do
	{
	cout << "Do koliko clanova ce sadrzati baza?\nNAPOMENA: baza moze da sadrzi do 50 clanova: "; cin >> n;
	if (n > 50) { cout << "Uneli ste prevelik broj!"; }
	else if (n < 1) { cout << "Uneli ste premali broj!"; }
	} 
	while (n < 0 || n > 50);

	cin.ignore();

	bazaZaposlenih = new zaposleni[n];
	if (bazaZaposlenih == NULL) { cerr << "Greska u dodeli memorije!"; exit(1); }
	
	//Inicijalizacija idZaposlenog svake strukture na 0 za kasniju upotrebu
	for (i = 0; i < n; i++) { bazaZaposlenih[i].idZaposlenog = 0; }
	
	//Pocetak unosenja podataka
	if (unosPodataka(bazaZaposlenih, n))
	{
		cout << "Unos podataka uspesno zavrsen. Da li zelite da prikazete podatke? (D/N): "; cin >> c;
		switch (toupper(c))
		{
		case 'D':
			prikazPodataka(bazaZaposlenih, n);
			break;
		case 'N':
			break;
		default:
			break;
		}
	}
	else { cerr << "Greska! Neuspesan unos podataka."; exit(1); }
	delete[] bazaZaposlenih;
	cout << "Da li zelite da sifrujete unete podatke (D/N): "; cin >> c;

	switch (toupper(c))
	{
	case 'D':
		sifrovanje();
		break;
	case 'N':
		cout << "Izlaz iz programa..." << endl;	exit(0);
	default:
		break;
	}
	return 0;
}

/* Funkcija proverava da li fajl postoji, i ako postoji nudi opcije korisniku, u suprotnom se vraća u main() funkciju
 * Funkcija se poziva iz glavne main() funkcije.
 */
void proveraFajla(char *imeF)
{
	char c;
	//Ako postoji fajl, otvara ga u režimu za čitanje, a ako ne postoji ne kreira fajl.
	f.open(imeF, ios::in | ios::binary | ios::_Nocreate);
	if (f.is_open())
	{
		cout << "\nUPOZORENJE: Fajl vec postoji!" << endl;
		f.close();
		do
		{
			cout << "\nDa li zelite da sifrujete (S)," << endl << "desifrujete (D)," << endl << "izmenite sadrzaj (I)," << endl << "ili da obrisete fajl (O)?" << endl << "Unos: ";
			cin >> c;
			cin.ignore();
		} while (toupper(c) != 'D' && toupper(c) != 'O' && toupper(c) != 'I' && toupper(c) != 'S');
		switch (toupper(c))
		{
		case 'S':
			switch (sifrovanje())
			{
			case 0: cerr << "Greska u sifrovanju!" << endl; exit(1);
			case 1: cout << "Fajl uspesno sifrovan" << endl;
			default: break;
			};
			break;
		case 'D':
			switch (desifrovanje())
			{
			case 0: cerr << "Greska u desifrovanju!" << endl; exit(1);
			case 1: cout << "Fajl uspesno desifrovan" << endl; break;
			case 2: cout << "Fajl nije sifrovan"; break;
			default: break;
			};
			break;
		case 'I':
			desifrovanje();
			if (izmenaPodataka()){ cout << "Podaci uspesno izmenjeni." << endl; }
			else { cerr << "Greska u izmeni podataka!" << endl; exit(1); }
			break;
		case 'O':
			if (remove(imeF) != 0) { cerr << "Greska! Neuspesno brisanje fajla."; exit(1); }
			else { cout << "Fajl uspesno obrisan." << endl;  }
		default:
			break;
		}
		exit(0);
	}
	else { cout << endl << "Fajl ne postoji." << endl; }
}

/* Upis podataka u datoteku.
 * Vraća 1 ako su podaci uspešno upisani, u suprotnom 0 ako je došlo do greške.
 * Funkcija se poziva iz glavne main() funkcije.
 */
int unosPodataka(zaposleni* osoba, int n, char* imeF)
{
	int i = 0;
	//Upis podataka u strukturu i fajl:
	f.open(imeF, ios::out | ios::binary | ios::trunc);
	if (!f) { cerr << "Greska! Neuspesno otvaranje datoteke"; return 0; }
	f.seekp(0, ios::beg);
	do
	{
		osoba[i].idZaposlenog++; //idZap se automatski inkrementira
		cout << "Unos podataka za zaposlenog ciji je ID: " << osoba[i].idZaposlenog << endl;
		cout << "Unesite ime zaposlenog: "; cin.getline(osoba[i].ime, BROJ);
		cout << "Unesite prezime zaposlenog: "; cin.getline(osoba[i].prezime, BROJ);

		//Unos kontaktnih informacija:
		cout << "Unesite broj telefona: "; cin.getline(osoba[i].kt.tel, 13);
		cout << "Unesite E-mail: "; cin.getline(osoba[i].kt.mail, 80);

		//Unos datuma rođenja
		cout << "Unesite datum dan, mesec i godinu rodjenja, po sledecem formatu; DD MM YYYY: ";
		cin >> osoba[i].datRodjenja.dan;
		cin >> osoba[i].datRodjenja.mesec;
		cin >> osoba[i].datRodjenja.godina;

		cin.ignore();

		//Unos mesta stanovanja:
		cout << endl << "Unos adrese za zaposlenog ciji je ID: " << osoba[i].idZaposlenog << endl;
		cout << "Unesite ime ulice: "; cin.getline(osoba[i].adresa.imeUlice, MAX);
		cout << "Unesite broj objekta: "; cin.getline(osoba[i].adresa.broj, BROJ);
		cout << "Unesite grad: "; cin.getline(osoba[i].adresa.grad, BROJ);
		cout << "Unesite opstinu: "; cin.getline(osoba[i].adresa.mesto, BROJ);

		f.write((char*)&osoba[i], sizeof(zaposleni));
		i++;
	} while (i < n);

	f.close();

	return 1;
}

/*Funkcija prikazuje sve unete podatke u konzoli u koliko to korisnik želi.
 * Funkcija se poziva iz glavne main() funkcije.
 */
void prikazPodataka(zaposleni* osoba, int n, char* imeF)
{
	int i;

	citanjePodataka(osoba, n);

	//Prikazuje informacije redom od prve do poslednje unete strukture 
	for (i = 0; i < n; i++)
	{
		f.read((char*)&osoba[i], sizeof(zaposleni));

		cout << "ID: " << osoba[i].idZaposlenog << endl
			<< "Ime: " << osoba[i].ime << endl
			<< "Prezime: " << osoba[i].prezime << endl
			<< "Telefon: " << osoba[i].kt.tel << endl
			<< "E-mail: " << osoba[i].kt.mail  << endl;
		cout << "Datum rodjenja: " << osoba[i].datRodjenja.dan << '/' << osoba[i].datRodjenja.mesec << '/' << osoba[i].datRodjenja.godina << endl;
		cout << "Adresa stanovanja: " << osoba[i].adresa.imeUlice << ' ' << osoba[i].adresa.broj << ", " << osoba[i].adresa.grad << ", " << osoba[i].adresa.mesto << endl << endl;
	}
}

/* Funkcija učitava podatke iz fajla u privremene strukture, zatim korisnik odabira šta želi da uradi sa podacima
 * Vraća 1 ako su podaci uspešno upisani, u suprotnom 0 ako je došlo do greške.
 * Funkcija se poziva iz pomoćne funkcije proverafajla()
 */
int izmenaPodataka(char* imeF)
{
	int id = 1, c;
	char unos;
	zaposleni* tempOsoba;

	tempOsoba = new zaposleni[50];
	if (tempOsoba == NULL) { cerr << "Greska! Neuspesna dodela memorije." << endl; return 0; }

	cout << "Citanje baze..." << endl;
	 id = citanjePodataka(tempOsoba, id);
	do
	{
		cout << "Unesite ID osobe cije podatke zelite da izmenite, ili bilo koji karakter (A-Z) da zavrsite izmene: "; cin >> c;
		if (cin.fail()) { break; } //Proverava da li unos odgovara tipu promenljive, odnosno da li je uspesno unet podatak iz komandne linije u promenljivu
		else if (c <= 0) { cout << "Unesite broj veci od 0!" << endl; }
		else if ( c > 0  && c < 51)
		{
			c -= 1;
			cout << "Unesite koji tip podatka zelite da izmenite: Ime (I), Prezime (P), Telefon (T), E-mail (E),\nDatum rodjenja (D), Mesto stanovanja (M): "; cin >> unos;
			cin.ignore();
			//Početak izmene podataka
			switch (toupper(unos))
			{
			case 'I':
				cout << "Trenutno ime: " << tempOsoba[c].ime << endl; 
				cout << "Novo ime: "; cin.getline(tempOsoba[c].ime, BROJ + 1, '\n');
				break;
			case 'P':
				cout << "Trenutno prezime: " << tempOsoba[c].prezime << endl;
				cout << "Novo prezime: "; cin.getline(tempOsoba[c].prezime, BROJ + 1, '\n');
				break;
			case 'T':
				cout << "Trenutni broj telefona: " << tempOsoba[c].kt.tel << endl;
				cout << "Novi broj telefona: "; cin.getline(tempOsoba[c].kt.tel, BROJ + 1, '\n');
				break;
			case 'E':
				cout << "Trenutni E-mail: " << tempOsoba[c].kt.tel << endl;
				cout << "Novi E-mail: "; cin.getline(tempOsoba[c].kt.mail, BROJ + 1, '\n');
				break;
			case 'D':
				cout << "Treutni datum rodjenja: " << tempOsoba[c].datRodjenja.dan << '/' << tempOsoba[c].datRodjenja.mesec << '/' << tempOsoba[c].datRodjenja.godina << endl;
				cout << "Novi datum rodjenja (Format: DD MM YYYY): "; cin >> tempOsoba[c].datRodjenja.dan; cin >> tempOsoba[c].datRodjenja.mesec; cin >> tempOsoba[c].datRodjenja.godina;
				cin.ignore();
				break;
			case 'M':
				cout << "Trenutno mesto stanovanja: " << tempOsoba[c].adresa.imeUlice << ' ' << tempOsoba[c].adresa.broj << ", " << tempOsoba[c].adresa.grad << ", " << tempOsoba[c].adresa.mesto << endl;
				cout << "Novo mesto stanovanja: " << endl; 
				cout << "Ime ulice: "; cin.getline(tempOsoba[c].adresa.imeUlice, MAX + 1, '\n');
				cout << "Broj objekta: "; cin.getline(tempOsoba[c].adresa.broj, BROJ + 1, '\n');
				cout << "Grad: "; cin.getline(tempOsoba[c].adresa.grad, BROJ + 1, '\n');
				cout << "Opstina: "; cin.getline(tempOsoba[c].adresa.mesto, BROJ + 1, '\n');
				break;
			default: break;
			}
		}
	} while (1);
	
	cout << "Podaci uspesno izmenjeni. Upis u fajl...";
	upisPodataka(tempOsoba, id, 0, imeF);

	delete[] tempOsoba;
	return 1;
}

/* Upit za korisnika na koji način želi da šifruje podatke, ili da izbaci informacije o dostupnim vrstana šifrovanja.
 * Funkcija se poziva iz pomoćne funkcije proveraFajla()
 */
int sifrovanje(char* imeF)
{
	char unos;
	int odstup;
	char sifra[MAX];
	do
	{
		//Upit za korisnika na koji način želi da šifruje podatke
		cout << endl << "Na koji nacin zelite da sifrujete fajl?" << endl
			<< "1. Cezarovo sifrovanje" << endl
			<< "2. Viznerovo sifrovanje" << endl
			<< "Unesite redni broj, ili '?' ako zelite da vidite primere sifrovanja: "; cin >> unos;

		switch (unos)
		{
		case '1':
			cout << "Unesite koliko karaktera ce biti odstup u tekstu: "; cin >> odstup;
			if(E_cezar(odstup)) cout << "Uspesno sifrovanje uz pomoc Cezarovog sifrovanja." << endl;;
			return 1;
		case '2':
			cout << "Unesite sifru (Jedna rec): "; cin >> sifra;
			if (E_vizner(sifra)) cout << "Uspesno sifrovanje uz pomoc Vezarovog sifrovanja." << endl;
			return 1;
		case '?':
			info();
			break;
		default:
			break;
		}
	} while (unos != '1' && unos != '2');

	return 0;
}

/* Izbacuje primere šifrovanja u konzolu i vraća program u sifrovanje().
 * Funkcija se poziva iz pomoćne funkcije sifrovanje()
 */
void info()
{
	cout << endl;
	cout << "Cezarovo sifrovanje" << endl
		 << "Svako slovo teksta koji se sifruje vraca se za n karaktera unazad. Primer:" << endl
		 << "\n'The quick brown fox jumps over the lazy dog', sa pomerajem od 3 bice:" << endl
		 <<   "'Wkh txlfn eurzq ira mxpsv ryhu wkh odcb grj'" << endl;

	cout << "\nViznerovo sifrovanje" << endl
		<< "Svako slovo teksta se zamenjuje za sumu odgovarajucih slova iz teksta i sifre. Primer" << endl
		<< "\n'Almost before we knew it, he had left the ground' sa sifrom 'lemon':" << endl
		<<   "'Lpycfe fqtbci is xyii wg, si toq wirh gsi sfbfrp'" << endl;
}

/* Funkcija za šifrovanje podataka uz pomoć Cezarove enkripcije 
 * (Više o ovoj metoni šifrovanja na : https://en.wikipedia.org/wiki/Caesar_cipher)
 * Funkcija se poziva iz pomoćne funkcije sifrovanje() i vraća 1 u koliko je šifrovanje uspešno, u suprotnom 0.
 */
int E_cezar(int odstup, char* imeF)
{
	int i, id = 1;
	char identifikator = '1';

	zaposleni *tempOsoba;

	tempOsoba = new zaposleni[50];
	if (tempOsoba == NULL) { cerr << "Greska! Neuspesna dodela memorije."; return 0; }

	cout << "Ucitavanje baze za sifrovanje..." << endl;
	id = citanjePodataka(tempOsoba, id);

	for (i = 0; i < id; i++)
	{
		//Pocetak sifrovanja
		cezarSifrovanje(tempOsoba[i].ime, odstup);
		cezarSifrovanje(tempOsoba[i].prezime, odstup);
		cezarSifrovanje(tempOsoba[i].kt.tel, odstup);
		cezarSifrovanje(tempOsoba[i].kt.mail, odstup);

		cezarSifrovanje(tempOsoba[i].datRodjenja.godina, odstup);
		cezarSifrovanje(tempOsoba[i].datRodjenja.mesec, odstup);
		cezarSifrovanje(tempOsoba[i].datRodjenja.dan, odstup);

		cezarSifrovanje(tempOsoba[i].adresa.imeUlice, odstup);
		cezarSifrovanje(tempOsoba[i].adresa.broj, odstup);
		cezarSifrovanje(tempOsoba[i].adresa.grad, odstup);
		cezarSifrovanje(tempOsoba[i].adresa.mesto, odstup);
	}

	upisPodataka(tempOsoba, id, identifikator, imeF);
	
	delete[] tempOsoba;

	return 1;
}

/* Vižnerova šifra je modifikacija Cezarove šifre.
 * (Više na: https://en.wikipedia.org/wiki/Vigen%C3%A8re_cipher)
 * Funkcija se poziva iz pomoćne funkcije sifrovanje() i vraća 1 u koliko je šifrovanje uspešno, u suprotnom 0.
 */
int E_vizner(char* sifra, char* imeF)
{

	char identifikator = '2';
	char* kljuc;
	int i, j = 0, id = 1;

	zaposleni* tempOsoba;

	tempOsoba = new zaposleni[50];
	kljuc = new char[MAX + 1];
	if (tempOsoba == NULL || kljuc == NULL) { cerr << "Greska! Neuspesna dodela memorije." << endl; return 0; }

	generisiKljuc(sifra, kljuc);

	cout << "Ucitavanje baze za sifrovanje..." << endl;
	id = citanjePodataka(tempOsoba, id);

	//Početak šifrovanja. Svi podaci se čifruju pojedinačno.
	for (i = 0; i < id; i++)
	{
		viznerSifrovanje(tempOsoba[i].ime, kljuc);
		viznerSifrovanje(tempOsoba[i].prezime, kljuc);
		viznerSifrovanje(tempOsoba[i].kt.tel, kljuc);
		viznerSifrovanje(tempOsoba[i].kt.mail, kljuc);

		viznerSifrovanje(tempOsoba[i].datRodjenja.godina, kljuc);
		viznerSifrovanje(tempOsoba[i].datRodjenja.mesec, kljuc);
		viznerSifrovanje(tempOsoba[i].datRodjenja.dan, kljuc);

		viznerSifrovanje(tempOsoba[i].adresa.imeUlice, kljuc);
		viznerSifrovanje(tempOsoba[i].adresa.broj, kljuc);
		viznerSifrovanje(tempOsoba[i].adresa.grad, kljuc);
		viznerSifrovanje(tempOsoba[i].adresa.mesto, kljuc);
	}

	//Upis u binarnu datoteku
	upisPodataka(tempOsoba, id, identifikator);

	delete[] tempOsoba;
	delete[] kljuc;

	return 1;
}

/* Pomoćna funkcija koja vrši šifrovanje samo za E_cezar()
 * formula E(x) = (x + n) mod m, gde je:
 * x: karakter čistog, nešifrovanog, teksta, n: ofset ili odstup u tekstu, m: broj dostupnih različitih karaktera.
 */
char* cezarSifrovanje(char *tekst, int odstup)
{
	int i;
	for (i = 0; i < (int)strlen(tekst); i++)
	{
		//sifrovanje velikih slova:
		if (isupper(tekst[i]))
		{
			tekst[i] = char(int(tekst[i] + odstup - 65) % 26 + 65);
		}

		//sifrovanje malih slova
		else if (islower(tekst[i]))
		{
			tekst[i] = char(int(tekst[i] + odstup - 97) % 26 + 97);
		}

		//sifrovanje brojeva
		else if (isalnum(tekst[i]))
		{
			tekst[i] = char(int(tekst[i] + odstup - 48) % 10 + 48);
		}
		//Sifrovanje znaka '@' i '.' postavlja vrednost na '=' i '+'
		else if (tekst[i] == '@' || tekst[i] == '.')
		{
			tekst[i] = char(int(tekst[i] - 3));
		}
	}

	return tekst;
}

/* Pomoćna funkcija koja vrši šifrovanje samo za E_vizner()
 * formula E(x) = (p - k) mod m, gde je:
 * p: karakter čistog, nešifrovanog, teksta, k: karakter ključa, m: broj dostupnih različitih karaktera.
 */
char* viznerSifrovanje(char* tekst, char* kljuc)
{
	int i;
	for (i = 0; i < (int)strlen(tekst); i++)
	{
		//Sifrovanje znaka '@' i '.' postavlja vrednost na '=' i '+'
		if (tekst[i] == '@' || tekst[i] == '.')
		{
		tekst[i] = char(int(tekst[i] - 3));
		}

		//sifrovanje velikih slova:
		else if (isupper(tekst[i]))
		{
			if (!isalpha(kljuc[i])) { kljuc[i] += 16; } //Ako je kljuc sadrži numeričke karaktere, program zahteva korekciju karaktera
			kljuc[i] = toupper(kljuc[i]);
			tekst[i] = (char)(((int)tekst[i] - 65 + (int)kljuc[i] - 65) % 26) + 65;
		}

		//sifrovanje malih slova
		else if (islower(tekst[i]))
		{
			if (!isalpha(kljuc[i])) { kljuc[i] += 48; } //Ako je kljuc sadrži numeričke karaktere, program zahteva korekciju karaktera
			kljuc[i] = tolower(kljuc[i]);
			tekst[i] = (char)(((int)tekst[i] - 97 + (int)kljuc[i] - 97) % 26) + 97;
		}

		//sifrovanje brojeva
		else if (isalnum(tekst[i]))
		{
			tekst[i] = (char)(((int)tekst[i] - 48 + (int)kljuc[i] - 48) % 10) + 48;
		}
	}
	return tekst;
}

/* Pomoćna funkcija za prepoznavanje šifrovanja. Otvara fajl i traži 'identifikator'
 * koji označava koje je šifrovanje u pitanju.
 * Funkcija se poziva iz pomoćne funkcije proveraFajla() i funkcije izmenaPodataka()
 * U zavisnosti od vrste šifrovanja ili greške, vraća određeni broj:  
 * 0 - Greška
 * 1 - Fajl uspešno šifrovan
 * 2 - Fajl nije šifrovan
 */
int desifrovanje(char* imeF)
{
	char sifra[MAX];
	char id;
	int odstup;

	//Otvara fajl u rezimu za citanje i cita poslednji karakter u fajlu da bi odredio vrstu sifrovanja.
	f.open(imeF, ios::in | ios::binary);
	if (!f) { cerr << "Greska! Neuspesno otvaranje datoteke" << endl; return 0; }

	//Pomera pokazivač fajla na poslednji bajt u datoteci i upisuje tu vrednost u promenljivu id
	f.seekg(-1, ios::end);
	f.read((char *)&id, 1);
	f.close();

	//U zavisnosti od identifikatora koji je očitan, pokreće određenu vrstu dešifrovanja.
	switch (id)
	{
	case '1':
		cout << "Cezarovo sifrovanje..." << endl;
		cout << "Unesite odstup: "; cin >> odstup;
		
		if (D_cezar(odstup)) { return 1; }
		else { return 0; }
		break;
	case '2':
		cout << "Viznerovo sifrovanje..." << endl;
		cout << "Unesite sifru: "; cin >> sifra;
		if (D_vizner(sifra)) { return 1; }
		else { return 0; }
		break;
	default:
		return 2;
		break;
	}
}

/* Funkcija za dešifrovanje Cezarovog šifrovanja.
 * Funkcija se poziva iz pomoćne funkcije desifrovanje()
 * Vraca 1 ako je uspešno šifrovanje, u suprotnom 0.
 */
int D_cezar(int odstup, char* imeF)
{
	int i, id = 1;
	
	zaposleni* tempOsoba;

	tempOsoba = new zaposleni[50];
	if (tempOsoba == NULL) { cerr << "Greska! Neuspesna dodela memorije." << endl; return 0; }

	cout << "Ucitavanje baze za desifrovanje..." << endl;
	id = citanjePodataka(tempOsoba, id);

	//Početak dešifrovanja podataka preko Cezarovog dešifrovanja. Svaki podatak se dešifruje zasebno
	for (i = 0; i < id; i++)
	{
		cezarDesifrovanje(tempOsoba[i].ime, odstup);
		cezarDesifrovanje(tempOsoba[i].prezime, odstup);
		cezarDesifrovanje(tempOsoba[i].kt.tel, odstup);
		cezarDesifrovanje(tempOsoba[i].kt.mail, odstup);

		cezarDesifrovanje(tempOsoba[i].datRodjenja.godina, odstup);
		cezarDesifrovanje(tempOsoba[i].datRodjenja.mesec, odstup);
		cezarDesifrovanje(tempOsoba[i].datRodjenja.dan, odstup);

		cezarDesifrovanje(tempOsoba[i].adresa.imeUlice, odstup);
		cezarDesifrovanje(tempOsoba[i].adresa.broj, odstup);
		cezarDesifrovanje(tempOsoba[i].adresa.grad, odstup);
		cezarDesifrovanje(tempOsoba[i].adresa.mesto, odstup);
	}

	//Upis u binarnu datoteku
	upisPodataka(tempOsoba, id, 0);

	delete[] tempOsoba;

	return 1;
}

/* Funkcija za dešifrovanje Vižnerovog šifrovanja.
 * Funkcija se poziva iz pomoćne funkcije desifrovanje()
 * Vraca 1 ako je uspešno šifrovanje, u suprotnom 0.
 */
int D_vizner(char* sifra, char* imeF)
{
	int i, id = 1;
	char* kljuc;

	zaposleni* tempOsoba;

	tempOsoba = new zaposleni[50];
	kljuc = new char[MAX + 1];
	if (tempOsoba == NULL || kljuc == NULL) { cerr << "Greska! Neuspesna dodela memorije." << endl; return 0; }

	generisiKljuc(sifra, kljuc);

	cout << "Ucitavanje baze za desifrovanje..." << endl;
	id = citanjePodataka(tempOsoba, id);

	//Početak dešifrovanja podataka preko Vižnerovo dešifrovanja. Svaki podatak se dešifruje zasebno
	for (i = 0; i < id; i++)
	{
		viznerDesifrovanje(tempOsoba[i].ime, kljuc);
		viznerDesifrovanje(tempOsoba[i].prezime, kljuc);
		viznerDesifrovanje(tempOsoba[i].kt.tel, kljuc);
		viznerDesifrovanje(tempOsoba[i].kt.mail, kljuc);

		viznerDesifrovanje(tempOsoba[i].datRodjenja.godina, kljuc);
		viznerDesifrovanje(tempOsoba[i].datRodjenja.mesec, kljuc);
		viznerDesifrovanje(tempOsoba[i].datRodjenja.dan, kljuc);

		viznerDesifrovanje(tempOsoba[i].adresa.imeUlice, kljuc);
		viznerDesifrovanje(tempOsoba[i].adresa.broj, kljuc);
		viznerDesifrovanje(tempOsoba[i].adresa.grad, kljuc);
		viznerDesifrovanje(tempOsoba[i].adresa.mesto, kljuc);
	}

	//Upis u binarnu datoteku
	upisPodataka(tempOsoba, id, 0, imeF);

	delete[] tempOsoba;
	delete[] kljuc;

	return 1;
}

/*Dešifrovanje se vrši na sličan način kao šifrovanje: D(x) = (x - n) mod m.
 * x: Karakter iz šifrovanog teksta, n: odstup ili ofset, m: broj dostupnih različitih karaktera
 * 48, 65, 97 služe za korekciju pomeraja kroz ASCII tabelu
 * Vraća šifrovan tekst.
 * Poziva se iz D_vizner()
 */
char* cezarDesifrovanje(char *tekst, int odstup)
{
	int i;
	for (i = 0; i < (int)strlen(tekst); i++)
	{
		//Dešifrovanje Velikih slova
		if (isupper(tekst[i]))
		{
			tekst[i] = char(int(tekst[i] - (odstup - 26) - 65) % 26 + 65);
		}

		//Dešifrovanje malih slova
		else if (islower(tekst[i]))
		{
			tekst[i] = char(int(tekst[i] - (odstup - 26) - 97) % 26 + 97);
		}

		//Dešifrovanje brojeva
		else if (isalnum(tekst[i]))
		{
			tekst[i] = char(int(tekst[i] - (odstup - 10) - 48) % 10 + 48);
		}
		else if (tekst[i] == '=' ||tekst[i] == '+')
		{
			tekst[i] = char(int(tekst[i] + 3));
		}
	}
	return tekst;
}

/*Dešifrovanje viznerove šifre se vrši pomoću formule D(x) = (c - k + 26) mod m
 *gde je: c: šifrovan karakter, k: karakter ključa, m: broj dostupnih različitih karaktera
 * 48, 65, 97 služe za korekciju pomeraja kroz ASCII tabelu
 * Vraća šifrovan tekst.
 * Poziva se iz D_vizner()
 */
char* viznerDesifrovanje(char *tekst, char *kljuc)
{
	int i;
	for (i = 0; i < (int)strlen(tekst); i++)
	{
		//Dešifrovanje znakova '@' i '.'
		if (tekst[i] == '=' || tekst[i] == '+')
		{
			tekst[i] = char(int(tekst[i] + 3));
		}
		//sifrovanje velikih slova:
		else if (isupper(tekst[i]))
		{
			if (!isalpha(kljuc[i])) { kljuc[i] += 16; } //Korekcija ako šifra sadrži numeričke karaktere
			kljuc[i] = toupper(kljuc[i]);
			tekst[i] = (char)((((int)tekst[i] - 65 - (int)(kljuc[i] - 65)) + 26) % 26) + 65;
		}

		//sifrovanje malih slova
		else if (islower(tekst[i]))
		{
			if (!isalpha(kljuc[i])) { kljuc[i] += 48; } //Korekcija ako šifra sadrži numeričke karaktere
			kljuc[i] = tolower(kljuc[i]);
			tekst[i] = (char)((((int)tekst[i] - 97 - (int)(kljuc[i] - 97)) + 26) % 26) + 97;
		}

		//sifrovanje brojeva
		else if (isalnum(tekst[i]))
		{
			tekst[i] = (char)((((int)tekst[i] - 48 - (int)(kljuc[i] - 48)) + 10) % 10) + 48;
			if (tekst[i] < 48) { tekst[i] += 10; } //Korekcija za numeričke karaktere ako šifra sadrži tekstualne karaktere
		}
	}

	return tekst;
}

/* Pomoćna funkcija za generisanje ključa za vižnerovo šifrovanje.
 * Poziva se iz E_vizner() i D_vizner()
 * Vraća ključ dužine MAX koji je sastavljen ponavljanjem jedne reči.
 */
char* generisiKljuc(char * sifra, char* kljuc)
{
	int i, j = 0;
	//Generisanje ključa dužine MAX uz pomoć šifre. Šifra se upisuje karakter po karakter dok i nije jednako MAX
	for (i = 0; i < MAX; i++)
	{
		kljuc[i] = char(sifra[j]);
		if (j == strlen(sifra) - 1)
		{
			j = -1;
		}
		j++;
	}
	return kljuc;
}

/* Pomoćna funkcija koja služi za upis podataka u datoteku.
 * Poziva se iz E_cezar, D_cezar, E_viner, D_vizner i izmenaPodataka
 */
void upisPodataka(zaposleni *osoba, int j, int identifikator, char *imeF)
{
	int i;
	f.open(imeF, ios::out | ios::trunc | ios::binary);
	if (!f) { cerr << "Greska! Neuspesno otvaranje datoteke" << endl; exit(1); }

	//Upisuje podatke u datoteku
	for (i = 0; i < j; i++)
	{
		f.write((char*)&osoba[i], sizeof(zaposleni));
	}

	//Proverava da li postoji identifikator, ako postoji onda ga upisuje.
	if (identifikator != 0)
	{
		f.write((char *)&identifikator, sizeof(char));
	}
	f.close();
}

/* Funkcija obavlja ucitavanje podataka u privremenu strukturu.
 * Poziva se iz: E_cezar(), D_cezar(), E_vizner(), D_vizner(), izmenaPodataka() i prikazPodataka()
 */
int citanjePodataka(zaposleni* osoba, int j, char* imeF)
{
	int i, brojac = 0;

	//Pocetak citanja podataka
	f.open(imeF, ios::in | ios::binary);
	if (!f) { cerr << "Greska! Neuspesno otvaranje datoteke" << endl; exit(1); }

	//Petlja čita podatke dok ne dođe to nepostojećeg idZaposlenog, tad prekida petlju.
	for (i = 0; i < 50; i++)
	{
		f.read((char*)&osoba[i], sizeof(zaposleni));
		if (osoba[i].idZaposlenog < 0) { break; }
		brojac++;
	}
	//Informacija za korisnika koliko ima članova baze koji su upisani u datoteku. 
	cout << "Broj ucitanih radnika: "<< brojac << endl;

	f.close();

	return brojac;
}