#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>

void EroareDeschidereFisier(void)
{
	printf("Eroare deschidere fisier!\n");
	exit(EXIT_FAILURE);
}

void EroareScriereFisier(void)
{
	printf("Eroare scriere in fisier!\n");
	exit(EXIT_FAILURE);
}

void EroareAlocareMemorie(void)
{
	printf("Eroare alocare memorie!\n");
	exit(EXIT_FAILURE);
}

void Menu(void)
{
	printf("\n------------------ Meniu Interactiv ---------------------\n"
		"1. Adaugare produs\n"
		"2. Afisare produse\n"
		"3. Iesire\n"
		"---------------------------------------------------------\n"
		"Optiunea dvs: "
	);
}

typedef enum optiune {
	adaugare = 1,
	afisare = 2,
	iesire = 3
} optiune;

typedef struct produs {
	char* nume;
	float pret;
} produs;

void Citire(produs* produse, int* n)
{
	FILE* f = NULL;
	unsigned short len_nume;
	char* pnume = NULL;

	if (!(f = fopen("produse.dat", "rb")))
		// nu exista baza de date
		return;

	printf("\npozitia curenta (offset): %d\n", ftell(f));

	// atata timp cat se mai pot citi noi elemente
	for (;;) {
		// daca fread nu reuseste sa citeasca elem. dimensiune
		// s-a ajuns la sfarsit de fisier
		if (!(fread(&len_nume, sizeof(unsigned short), 1, f))) {
			// inchidem fisierul, iesim din functie
			fclose(f);
			return;
		}

		pnume = (char*)malloc((len_nume + 1) * sizeof(char));
		if (!pnume)
			EroareAlocareMemorie();

		fread(pnume, sizeof(char), len_nume, f);	// citire caractere nume
		pnume[len_nume] = 0;
		
		produse[*n].nume = pnume;
		
		fread(&produse[*n].pret, sizeof(float), 1, f);		// citire pret
		(*n)++;
	}

	fclose(f);
}

void Adaugare(produs* produse, int* n)
{
	produs p;
	char nume[1000];

	printf("Nume: ");
	getchar();
	fgets(nume, 1000, stdin);
	nume[strcspn(nume, "\r\n")] = 0;
	if (!(p.nume = (char*)malloc((strlen(nume) + 1) * sizeof(char))))
		EroareAlocareMemorie();

	strcpy(p.nume, nume);

	printf("Pret: ");
	scanf("%g", &(p.pret));

	produse[*n] = p;
	(*n)++;
}

void Afisare(produs* produse, int n)
{
	int i;

	for (i = 0; i < n; i++) {
		printf("%s: %g\n", produse[i].nume, produse[i].pret);
	}

	printf("\n");
}

void Scriere(produs* produse, int n)
{
	FILE* f;
	int i;
	unsigned short len_nume;	// nr. caractere nume

	if (!(f = fopen("produse.dat", "wb")))
		EroareDeschidereFisier();

	for (i = 0; i < n; i++) {
		len_nume = (unsigned short)strlen(produse[i].nume);
		fwrite(&len_nume, sizeof(unsigned short), 1, f);	// scriere lungime sir in fisier
		fwrite(produse[i].nume, sizeof(char), len_nume, f);	// scriere sir, fara terminator
		fwrite(&produse[i].pret, sizeof(float), 1, f);
	}

	fclose(f);
}

void Eliberare(produs* produse, int n)
{
	int i;

	for (i = 0; i < n; i++)
		free(produse[i].nume);
}


int main(int argc, char** argv)
{
	optiune opt;
	produs produse[10];
	int n = 0;

	Citire(produse, &n);
	

	do
	{
		Menu();
		scanf("%d", &opt);

		switch (opt)
		{
		case adaugare:
		{
			Adaugare(produse, &n);
		}
		break;

		case afisare:
		{
			Afisare(produse, n);
		}
		break;

		case iesire:
		{
			Scriere(produse, n);
			Eliberare(produse, n);
		}
		break;

		default:
			printf("Optiune nedefinita!\n");
			break;
		}

	} while (opt != 3);

	return 0;
}