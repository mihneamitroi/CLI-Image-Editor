#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

void f_exit(unsigned char **imagine, int y1_lim, int y2_lim)
{
	for (int i = 0; i < y2_lim - y1_lim; i++)
		free(imagine[i]);
	free(imagine);
}

unsigned char **load(int *format, int *latime, int *lungime, int *valmax,
					 char *fisier, int *ok_load, int *x1_lim, int *y1_lim,
					 int *x2_lim, int *y2_lim)
{
	unsigned char **imagine;
	char litera;
	FILE *in = fopen(fisier, "rb");
	if (!in) {
		*ok_load = 0;
		printf("Failed to load %s\n", fisier);
		return NULL;
	}
	fscanf(in, "%c%d%d%d%d\n", &litera, format, latime, lungime, valmax);
	// Citesc datele initiale ale pozei si in functie de ele, aloc fie o
	// matrice cu dimensiunile "lungime" si "latime", fie cu o latime de 3 ori
	// mai mare pentru formatele color.
	if (*format == 2 || *format == 5) {
		imagine = (unsigned char **)malloc(*lungime * sizeof(unsigned char *));
		for (int i = 0; i < *lungime; i++)
			imagine[i] = (unsigned char *)malloc(*latime *
						  sizeof(unsigned char));
	} else {
		imagine = (unsigned char **)malloc(*lungime * sizeof(unsigned char *));
		for (int i = 0; i < *lungime; i++)
			imagine[i] = (unsigned char *)malloc(*latime * 3 *
						  sizeof(unsigned char));
	}
	if (*format == 5) {
		for (int i = 0; i < *lungime; ++i)
			for (int j = 0; j < *latime; ++j)
				fread(&imagine[i][j], sizeof(unsigned char), 1, in);
	} else if (*format == 6) {
		for (int i = 0; i < *lungime; ++i)
			for (int j = 0; j < 3 * *latime; ++j)
				fread(&imagine[i][j], sizeof(unsigned char), 1, in);
		// Pentru formatele binar citesc char cu char din matrice cu fread.
	} else if (*format == 2) {
		for (int i = 0; i < *lungime; ++i)
			for (int j = 0; j < *latime; ++j)
				fscanf(in, "%hhu", &imagine[i][j]);
	} else {
		for (int i = 0; i < *lungime; ++i)
			for (int j = 0; j < 3 * *latime; ++j)
				fscanf(in, "%hhu", &imagine[i][j]);
		}
		// Pentru formatele text citesc char cu char cu fscanf.
	fclose(in);
	printf("Loaded %s\n", fisier);
	*ok_load = 1;
	*x1_lim = 0;
	*x2_lim = *latime;
	*y1_lim = 0;
	*y2_lim = *lungime;
	// Realizez o selectie completa si ma intorc cu poza in main.
	return imagine;
}

int numar(char *sir)
{
	char *p = sir;
	if (*p == '-')
		p++;
	while (*p) {
		if (*p >= '0' && *p <= '9')
			p++;
		else
			return 0;
	}
	return 1;
	// Functia are rolul de a determina daca un parametru este numar intreg.
}

void f_select(int latime, int lungime, char *delim,
			  int *x1_lim, int *y1_lim,
			  int *x2_lim, int *y2_lim)
{
	int x1, x2, y1, y2, ok = 1;
	if (delim) {
		if (numar(delim)) {
			x1 = atoi(delim);
			delim = strtok(NULL, "\n ");
		} else {
			printf("Invalid command\n");
			return;
		}
	} else {
		printf("Invalid command\n");
		return;
	}
	// Verific existenta celor patru parametrii dati la select pentru
	// coordonate si daca acestia exista, verific sa fie numere intregi.
	if (delim) {
		if (numar(delim)) {
			y1 = atoi(delim);
			delim = strtok(NULL, "\n ");
		} else {
			printf("Invalid command\n");
			return;
		}
	} else {
		printf("Invalid command\n");
		return;
	}
	if (delim) {
		if (numar(delim)) {
			x2 = atoi(delim);
			delim = strtok(NULL, "\n ");
		} else {
			printf("Invalid command\n");
			return;
		}
	} else {
		printf("Invalid command\n");
		return;
	}
	if (delim) {
		if (numar(delim)) {
			y2 = atoi(delim);
			delim = strtok(NULL, "\n ");
		} else {
			printf("Invalid command\n");
			return;
		}
	} else {
		printf("Invalid command\n");
		return;
	}
	if (x1 < 0 || x1 > latime || x2 < 0 || x2 > latime || y1 < 0 ||
		y1 > lungime || y2 < 0 || y2 > lungime || x2 == x1 || y2 == y1) {
		printf("Invalid set of coordinates\n");
		ok = 0;
	}
	// Verific ca selectia sa nu iasa din poza sau sa fie selectat macar un
	// pixel.
	if (ok == 1) {
		if (x1 < x2) {
			*x1_lim = x1;
			*x2_lim = x2;
		} else {
			*x1_lim = x2;
			*x2_lim = x1;
		}
		if (y1 < y2) {
			*y1_lim = y1;
			*y2_lim = y2;
		} else {
			*y1_lim = y2;
			*y2_lim = y1;
		}
		printf("Selected %d %d %d %d\n", *x1_lim, *y1_lim,
			   *x2_lim, *y2_lim);
	}
}

void select_all(int latime, int lungime,
				int *x1_lim, int *y1_lim, int *x2_lim, int *y2_lim)
{
	*x1_lim = 0;
	*x2_lim = latime;
	*y1_lim = 0;
	*y2_lim = lungime;
}

unsigned char **rotate_90(unsigned char **imagine, int x1_lim, int y1_lim,
						  int x2_lim, int y2_lim, int nr_culori,
						  int selectie_completa)
{
	unsigned char **matrice_rotita;
	matrice_rotita = (unsigned char **)malloc((x2_lim - x1_lim) *
											  sizeof(unsigned char *));
	for (int i = 0; i < x2_lim - x1_lim; ++i) {
		matrice_rotita[i] = (unsigned char *)malloc((y2_lim - y1_lim)
													* nr_culori *
													sizeof(unsigned char));
		for (int j = 0; j < (y2_lim - y1_lim) * nr_culori; j = j + nr_culori)
			for (int k = 0; k < nr_culori; ++k) {
				matrice_rotita[i][j + k] = imagine
										   [y2_lim - 1 - j / nr_culori]
										   [(x1_lim + i) * nr_culori + k];
			}
	}
	// Aloc si completez in acelasi timp o matrice ce are ca lungime latimea
	// vechii matrici si invers (dreptunghi rasturnat).
	if (selectie_completa == 0) {
		for (int i = 0; i < x2_lim - x1_lim; ++i)
			for (int j = 0; j < nr_culori * (y2_lim - y1_lim); ++j)
				imagine[i + y1_lim]
				[j + nr_culori * x1_lim] = matrice_rotita[i][j];
		f_exit(matrice_rotita, x1_lim, x2_lim);
		return imagine;
		// Daca este selectata doar o portiune de poza, ma intorc cu acea
		// sectiune. Alfel, pastrez toata poza intoarsa.
	}
	f_exit(imagine, y1_lim, y2_lim);
	return matrice_rotita;
}

void rotate(unsigned char ***imagine, int format, int *latime, int *lungime,
			int x1_lim, int y1_lim, int *x2_lim, int *y2_lim)
{
	int unghi, selectie_completa = 0, e_patrat = 0;
	scanf("%d", &unghi);
	if (unghi % 90 == 0 && unghi / 90 >= -4 && unghi / 90 <= 4) {
		if (*latime == *x2_lim - x1_lim && *lungime == *y2_lim - y1_lim)
			selectie_completa = 1;
		else
			if (*x2_lim - x1_lim == *y2_lim - y1_lim)
				e_patrat = 1;
	// Verific daca unghiul este multiplu de 90 si se afla intre -360 si 360,
	// apoi verific daca cumva este selectata toata poza sau daca selectia
	// reprezinta un patrat.
		if (selectie_completa == 1 || e_patrat == 1) {
			printf("Rotated %d\n", unghi);
			while (unghi < 0)
				unghi = unghi + 360;
			// O rotatie la -270 de grade echivaleaza cu una la 90, lucru care
			// e valabil si pentru perechile -180, 180 si -90, 270.
			if (unghi == 360)
				unghi = 0;
			int nr_culori = 1;
			if (format == 3 || format == 6)
				nr_culori = 3;
			while (unghi > 0) {
				*imagine = rotate_90(*imagine, x1_lim, y1_lim, *x2_lim,
									 *y2_lim, nr_culori, selectie_completa);
				if (selectie_completa == 1) {
					*latime = *y2_lim - y1_lim;
					*lungime = *x2_lim - x1_lim;
					*x2_lim = *latime;
					*y2_lim = *lungime;
				}
				unghi = unghi - 90;
				// Atat timp cat unghiul inca are o valoare pozitiva aplic
				// functia de rotire pentru cate 90 de grade si interschimb
				// lungimea cu latimea.
			}
		} else {
			printf("The selection must be square\n");
		}
	} else {
		printf("Unsupported rotation angle\n");
	}
}

unsigned char **crop(unsigned char **imagine, int format, int *latime,
					 int *lungime, int x1_lim, int y1_lim, int x2_lim,
					 int y2_lim)
{
	unsigned char **imagine_aux;
	if (format == 2 || format == 5) {
		imagine_aux = (unsigned char **)malloc((y2_lim - y1_lim) *
											   sizeof(unsigned char *));
		for (int i = 0; i < y2_lim - y1_lim; i++)
			imagine_aux[i] = (unsigned char *)malloc((x2_lim - x1_lim)
													 * sizeof(unsigned char));
		for (int i = 0; i < y2_lim - y1_lim; ++i)
			for (int j = 0; j < x2_lim - x1_lim; j++)
				imagine_aux[i][j] = imagine[i + y1_lim][j + x1_lim];
	} else {
		imagine_aux = (unsigned char **)malloc((y2_lim - y1_lim) *
											   sizeof(unsigned char *));
		for (int i = 0; i < y2_lim - y1_lim; i++)
			imagine_aux[i] = (unsigned char *)malloc(3 * (x2_lim - x1_lim)
													 * sizeof(unsigned char));
		for (int i = 0; i < y2_lim - y1_lim; ++i)
			for (int j = 0; j < 3 * (x2_lim - x1_lim); j++)
				imagine_aux[i][j] = imagine[i + y1_lim][j + 3 * x1_lim];
	}
	// Utilizez o matrice auxiliara in care, in functie de caz, copiez fie
	// o matrice simpla, fie una cu numar triplu de coloane.
	f_exit(imagine, 0, *lungime);
	*latime = x2_lim - x1_lim;
	*lungime = y2_lim - y1_lim;
	printf("Image cropped\n");
	return imagine_aux;
	// Renunt la imaginea initiala si intorc in main sectiunea decupata.
}

void grayscale(unsigned char **imagine, int format, int x1_lim,
			   int y1_lim, int x2_lim, int y2_lim)
{
	if (format == 3 || format == 6) {
		unsigned char r, g, b;
		for (int i = y1_lim; i < y2_lim; ++i)
			for (int j = x1_lim; j < x2_lim; j++) {
				// Stochez valorile aferente fiecarei culori si apoi
				// modific facand aproximarea cu functia round. "j" ia valori
				// din 3 in 3 deoarece stochez matricea pentru format color
				// cu un numar triplu de coloane.
				r = imagine[i][3 * j];
				g = imagine[i][3 * j + 1];
				b = imagine[i][3 * j + 2];
				double gri = (r + g + b) / 3;
				imagine[i][3 * j] = (unsigned char)round(gri);
				imagine[i][3 * j + 1] = (unsigned char)round(gri);
				imagine[i][3 * j + 2] = (unsigned char)round(gri);
			}
		printf("Grayscale filter applied\n");
	} else {
		printf("Grayscale filter not available\n");
	}
}

void sepia(unsigned char **imagine, int format, int valmax,
		   int x1_lim, int y1_lim, int x2_lim, int y2_lim)
{
	if (format == 3 || format == 6) {
		unsigned char r, g, b;
		for (int i = y1_lim; i < y2_lim; ++i)
			for (int j = x1_lim; j < x2_lim; j++) {
				// Stochez valorile aferente fiecarei culori sa le am mai
				// tarziu si apoi verific daca valoarea calculata depaseste
				// valoarea maxima precizata.
				r = imagine[i][3 * j];
				g = imagine[i][3 * j + 1];
				b = imagine[i][3 * j + 2];
				if (0.393 * r + 0.769 * g + 0.189 * b > valmax)
					imagine[i][3 * j] = valmax;
				else
					imagine[i][3 * j] = round(0.393 * r +
										0.769 * g + 0.189 * b);
				if (0.349 * r + 0.686 * g + 0.168 * b > valmax)
					imagine[i][3 * j + 1] = valmax;
				else
					imagine[i][3 * j + 1] = round(0.349 * r
											+ 0.686 * g + 0.168 * b);
				if (0.272 * r + 0.534 * g + 0.131 * b > valmax)
					imagine[i][3 * j + 2] = valmax;
				else
					imagine[i][3 * j + 2] = round(0.272 * r
											+ 0.534 * g + 0.131 * b);
			}
		printf("Sepia filter applied\n");
	} else {
		printf("Sepia filter not available\n");
	}
}

void save(unsigned char **imagine, int format, int valmax,
		  int latime, int lungime)
{
	char *fisier, parametru[100], *p;
	fgets(parametru, 100, stdin);
	fisier = strtok(parametru, "\n ");
	// Aici stochez numele fisierului, iar pointerul p preia un posibil
	// "ascii".
	p = strtok(NULL, "\n ");
	FILE *out = fopen(fisier, "wb");
	format = format % 3;
	if (format == 0)
		format = format + 3;
	int k = 1;
	if (format == 3)
		k = 3;
	// Fac ca formatele 2 si 5, respectiv 3 si 6 sa fie echivalente pentru
	// numarul de coloane pe care il are matricea pe care o voi salva.
	if (p) {
		// Daca primesc inca un parametru dupa numele fisierului, verific daca
		// e "ascii" si in caz pozitiv scriu cu fprintf, altfel cu fwrite.
		if (strcmp(p, "ascii") == 0) {
			fprintf(out, "P%d\n%d %d\n%d\n", format, latime, lungime, valmax);
			for (int i = 0; i < lungime; ++i) {
				for (int j = 0; j < k * (latime); ++j)
					fprintf(out, "%d ", imagine[i][j]);
				fprintf(out, "\n");
			}
		} else {
			format = format + 3;
			fprintf(out, "P%d\n%d %d\n%d\n", format, latime, lungime,
					valmax);
			for (int i = 0; i < lungime; ++i)
				for (int j = 0; j < k * (latime); ++j)
					fwrite(&imagine[i][j], sizeof(char), 1, out);
		}
	} else {
		// Daca nu mai primesc niciun parametru dupa fisier scriu direct binar.
		format = format + 3;
		fprintf(out, "P%d\n%d %d\n%d\n", format, latime, lungime, valmax);
		for (int i = 0; i < lungime; ++i)
			for (int j = 0; j < k * (latime); ++j)
				fwrite(&imagine[i][j], sizeof(char), 1, out);
	}
	fclose(out);
	printf("Saved %s\n", fisier);
}

int main(void)
{
	unsigned char **imagine = NULL;
	char comanda[100], fisier[100], gunoi[100];
	int format, latime, lungime, valmax, ok_exit = 1;
	int ok_load = 0, x1_lim, y1_lim, x2_lim, y2_lim;
	do {
		scanf("%s", comanda);
		if (strcmp(comanda, "LOAD") == 0) {
			scanf("%s", fisier);
			if (ok_load == 1)
				f_exit(imagine, 0, lungime);
			imagine = load(&format, &latime, &lungime, &valmax,
						   fisier, &ok_load, &x1_lim, &y1_lim,
						   &x2_lim, &y2_lim);
		} else if (strcmp(comanda, "SELECT") == 0) {
			char parametru[100], *delim;
			fgets(parametru, 100, stdin);
			delim = strtok(parametru, "\n ");
			if (ok_load == 0) {
				printf("No image loaded\n");
			} else {
				if (strcmp(delim, "ALL") == 0) {
					select_all(latime, lungime, &x1_lim, &y1_lim,
							   &x2_lim, &y2_lim);
					printf("Selected ALL\n");
				} else {
					f_select(latime, lungime, delim, &x1_lim,
							 &y1_lim, &x2_lim, &y2_lim);
				}
			}
		} else if (strcmp(comanda, "ROTATE") == 0) {
			if (ok_load == 0) {
				printf("No image loaded\n");
				fgets(gunoi, 100, stdin);
			} else {
				rotate(&imagine, format, &latime, &lungime, x1_lim,
					   y1_lim, &x2_lim, &y2_lim);
			}
		} else if (strcmp(comanda, "CROP") == 0) {
			if (ok_load == 0) {
				printf("No image loaded\n");
				fgets(gunoi, 100, stdin);
			} else {
				imagine = crop(imagine, format, &latime, &lungime,
							   x1_lim, y1_lim, x2_lim, y2_lim);
				select_all(latime, lungime, &x1_lim, &y1_lim, &x2_lim,
						   &y2_lim);
			}
		} else if (strcmp(comanda, "GRAYSCALE") == 0) {
			if (ok_load == 0)
				printf("No image loaded\n");
			else
				grayscale(imagine, format, x1_lim, y1_lim, x2_lim, y2_lim);
		} else if (strcmp(comanda, "SEPIA") == 0) {
			if (ok_load == 0)
				printf("No image loaded\n");
			else
				sepia(imagine, format, valmax, x1_lim, y1_lim, x2_lim, y2_lim);
		} else if (strcmp(comanda, "SAVE") == 0) {
			if (ok_load == 0) {
				printf("No image loaded\n");
				fgets(gunoi, 100, stdin);
			} else {
				save(imagine, format, valmax, latime, lungime);
			}
		} else if (strcmp(comanda, "EXIT") == 0) {
			if (ok_load == 0)
				printf("No image loaded\n");
			else
				f_exit(imagine, 0, lungime);
			ok_exit = 0;
		} else {
			printf("Invalid command\n");
			fgets(gunoi, 100, stdin);
		}
	} while (ok_exit == 1);
	return 0;
	// In main am verificat primul parametru pe care il primeste o comanda, iar
	// apoi am apelat functia de care aveam nevoie. Daca nu aveam o poza incar-
	// cata, citesc restul de parametrii de care nu e nevoie cu fgets.
}
