// Copyright 2017 Vlad-Stefan DIEACONU, 311CA

#include "./arena.h"
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define GESTIONSIZE (unsigned int) (3 * sizeof(int))
#define INDEXSIZE (unsigned int) sizeof(int)


// Scrie valoarea unui int, 'valoarea', incepand de la pozitia 'pozitie'.
void write_int(Arena *arena, int pozitie, int valoare) {
    int *a = (int*) (arena->arena + pozitie);
    *a = valoare;
}


// Citeste valoarea unui int incepand de la pozitia 'pozitie'.
int read_int(Arena *arena, int pozitie) {
    int *a = (int*) (arena->arena + pozitie);
    return *a;
}


// Initializeaza o arena de N octeti.
void INITIALIZE(Arena *arena, int N) {
    // Seteaza dimensiunea arenei
    arena->dimensiune = N;
    // Alocarea arenei si setarea octetilor pe zero
    arena->arena = (unsigned char *) calloc(N, sizeof(unsigned char));
    if (!arena->arena) {
        exit(EXIT_FAILURE);
    }

    // Initalizarea lantului de blocuri (setarea indicelui de start pe 0)
    write_int(arena, 0, 0);
}


// Elibereaza memoria alocata la initializare si incheie programul.
void FINALIZE(Arena *arena) {
    free(arena->arena);
    exit(EXIT_SUCCESS);
}


// Afiseaza intreaga harta a memoriei, asa cum se gaseste in acel moment.
void DUMP(Arena *arena) {
    for (unsigned int i = 0; i < arena->dimensiune; i += 16) {
        // Indicele curent in format hexazecimal, cu 8 cifre hexa majuscule
        printf("%08X\t", i);

        // 16 octeti separati printr-un spatiu in format hexazecimal
        // cu 2 cifre hexa majuscule
        for (unsigned int j = i; j < MIN(i + 16, arena->dimensiune); ++j) {
            printf("%02X ", arena->arena[j]);
            // spatiu suplimentare intre octetul 8 si octetul 9
            if ((j + 1) % 8 == 0) {
                printf(" ");
            }
        }

        printf("\n");
    }

    // Dimensiunea arenei in format hexazecimal, cu 8 cifre hexa majuscule
    printf("%08X\n", arena->dimensiune);
}


// Aloca 'size' octeti de memorie in arena
// si afiseaza indexul de inceput al blocului alocat.
void ALLOC(Arena *arena, int size) {
    // blocul curent, initial este blocul de start
    unsigned int current_block = 0;
    // blocul urmator
    unsigned int next_block = read_int(arena, current_block);

    // daca arena este goala si este suficient spatiu
    if (next_block == 0 &&
        arena->dimensiune >= size + GESTIONSIZE + INDEXSIZE) {
        // rezerv zona de memorie imediat urmatoare blocului de start
        unsigned int new_block = INDEXSIZE;

        // blocul de start pointeaza la noul bloc
        write_int(arena, 0, new_block);

        // sectiunea de gestiune a noului bloc
        // primul octet al noului bloc pointeaza la blocul urmator, adica zero
        write_int(arena, new_block, 0);
        // al doilea octet al noului bloc pointeaza la blocul anterior
        write_int(arena, new_block + 4, current_block);
        // lungimea totala a blocului
        write_int(arena, new_block + 8, size + GESTIONSIZE);

        // indexul de inceput al blocului alocat in arena
        printf("%d\n", 16);
        return;

    // daca sunt blocuri alocate in arena
    } else {
        // daca este suficient spatiu intre blocul de start si urmatorul bloc
        if (next_block >= size + GESTIONSIZE + INDEXSIZE) {
            // rezerv zona de memorie pentru noul bloc
            unsigned int new_block = INDEXSIZE;

            // blocul de start pointeaza la noul bloc
            write_int(arena, current_block, new_block);
            // al doilea octet al lui next_block pointeaza la noul bloc
            write_int(arena, next_block + 4, new_block);

            // sectiunea de gestiune a noului bloc
            // primul octet al noului bloc pointeaza la blocul urmator
            write_int(arena, new_block, next_block);
            // al doilea octet al noului bloc pointeaza la blocul anterior
            write_int(arena, new_block + 4, current_block);
            // lungimea totala a blocului
            write_int(arena, new_block + 8, size + GESTIONSIZE);

            // indexul de inceput al blocului alocat in arena
            printf("%d\n", 16);
            return;

        } else {
            // updatez current_block si next_block
            current_block = next_block;
            next_block = read_int(arena, current_block);

            // parcurg memoria
            while (next_block != 0) {
                // dimensiunea blocului curent
                int len_current_block = read_int(arena, current_block + 8);

                // daca spatiul dintre blocul curent si cel urmator e suficient
                // pentru noul bloc atunci aloc memorie pentru el
                if (next_block - (current_block + len_current_block) >=
                    size + GESTIONSIZE) {
                    // rezerv zona de memorie pentru noul bloc
                    unsigned int new_block = current_block + len_current_block;

                    // blocul curent pointeaza la noul bloc
                    write_int(arena, current_block, new_block);
                    // al doilea octet al lui next_block pointeaza la noul bloc
                    write_int(arena, next_block + 4, new_block);

                    // sectiunea de gestiune a noului bloc
                    // primul octet al noului bloc pointeaza la blocul urmator
                    write_int(arena, new_block, next_block);
                    // al doilea octet al noului bloc pointeaza
                    // la blocul anterior
                    write_int(arena, new_block + 4, current_block);
                    // lungimea totala a blocului
                    write_int(arena, new_block + 8, size + GESTIONSIZE);

                    // indexul de inceput al blocului alocat in arena
                    printf("%d\n", new_block + GESTIONSIZE);
                    return;
                }

                // updatez current_block si next_block
                current_block = next_block;
                next_block = read_int(arena, current_block);
            }

            // dimensiunea blocului curent
            int len_current_block = read_int(arena, current_block + 8);

            // daca este spatiu suficient dupa ultimul bloc alocat din arena
            if (arena->dimensiune - (current_block + len_current_block)
                                >= size + GESTIONSIZE) {
                // rezerv zona de memorie pentru noul bloc
                unsigned int new_block = current_block + len_current_block;

                // blocul curent pointeaza la noul bloc
                write_int(arena, current_block, new_block);

                // sectiunea de gestiune a noului bloc
                // primul octet al noului bloc pointeaza la zero
                write_int(arena, new_block, 0);
                // al doilea octet al noului bloc pointeaza
                // la blocul anterior
                write_int(arena, new_block + 4, current_block);
                // lungimea totala a blocului
                write_int(arena, new_block + 8, size + GESTIONSIZE);

                // indexul de inceput al blocului alocat in arena
                printf("%d\n", new_block + GESTIONSIZE);
                return;
            }
        }
    }

    printf("%d\n", 0);
    return;
}


// Elibereaza blocul de memorie al carei sectiuni de date incepe la
// pozitia 'index' in arena
void FREE(Arena *arena, int index) {
    unsigned int prev_block = read_int(arena, index - 2 * INDEXSIZE);
    unsigned int next_block = read_int(arena, index - 3 * INDEXSIZE);

    // primul octet al blocului prev pointeaza la blocul next
    write_int(arena, prev_block, next_block);

    // daca exista blocul next
    if (next_block != 0) {
        // al doilea octet al blocului next pointeaza la blocul prev
        write_int(arena, next_block + 4, prev_block);
    }
}


// Seteaza 'size' octeti consecutivi din arena, incepand cu indexul 'index',
// la valoarea 'value'.
void FILL(Arena *arena, int index, int size, int value) {
    memset(arena->arena + index, value, size);
}

// Afiseaza, in format zecimal, numarul de octeti liberi din arena, impreuna cu
// numarul de regiuni (zone continue) libere din arena.
void SHOW_FREE(Arena *arena) {
    // Calculul numarului de octeti liberi din arena.

    // daca arena este goala, atunci numarul de octeti liberi este egal
    // cu dimensiunea arenei - dimensiunea indexului de start
    int free_bytes = arena->dimensiune - 4;

    // blocul curent
    unsigned int current_block = read_int(arena, 0);
    // daca arena nu este goala parcurgem blocurile
    if (current_block != 0) {
        while (current_block != 0) {
            // scadem al treilea octet din sectiunea de gestine,
            // reprezentand lungimea totata a blocului ocupat
            free_bytes -= read_int(arena, current_block + 8);

            // updatez current_block
            current_block = read_int(arena, current_block);
        }
    }

    // Calculul regiunilor libere din arena.
    int free_blocks = 0;

    // blocul curent
    current_block = read_int(arena, 0);
    // daca arena este goala
    if (current_block == 0) {
        // exista doar o regiune libera
        ++free_blocks;

    } else {
        // daca exita spatiu liber intre blocul de start si current_bloc
        if (current_block > INDEXSIZE) {
            ++free_blocks;
        }

        // urmatorul bloc
        unsigned int next_block = read_int(arena, current_block);
        // parcurgem toate blocurile
        while (next_block != 0) {
            // daca intre sfarsitul blocului curent si
            // inceputul blocului next_block exista spatiu
            if (current_block + read_int(arena, current_block + 8)
                < next_block) {
                ++free_blocks;
            }

            // updatez current_block si next_block
            current_block = next_block;
            next_block = read_int(arena, current_block);
        }

        // daca intre sfarsitul ultimului bloc si finalul arenei
        // exista spatiu liber
        if (current_block + read_int(arena, current_block + 8)
                    < arena->dimensiune) {
            ++free_blocks;
        }
    }
    printf("%d blocks (%d bytes) free\n", free_blocks, free_bytes);
}


// Afiseaza: numarul de octeti folositi din arena (numai sectiunile de date),
// eficienta utilizarii = numarul de octeti folositi / numarul de octeti
// rezervati (care nu sunt liberi),
// fragmentarea = numarul de zone libere - 1 / numarul de blocuri alocate.
void SHOW_USAGE(Arena *arena) {
    int all_used_bytes = 4;
    int data_used_bytes = 0;
    int free_blocks = 0;
    int used_blocks = 0;

    // blocul curent
    unsigned int current_block;

    // Calculul nuumarului de octeti folositi din arena si al blocurilor alocate

    // blocul curent, initial e primul bloc
    current_block = read_int(arena, 0);
    // altfel daca arena nu este goala parcurgem blocurile
    if (current_block != 0) {
        while (current_block != 0) {
            ++used_blocks;
            all_used_bytes += read_int(arena, current_block + 8);
            data_used_bytes += read_int(arena, current_block + 8)
                                    - GESTIONSIZE;
            // updatez current_block
            current_block = read_int(arena, current_block);
        }
    }
    printf("%d blocks (%d bytes) used\n", used_blocks, data_used_bytes);

    unsigned int efficiency = 100 * data_used_bytes / all_used_bytes;
    printf("%d%% efficiency\n", efficiency);

    // Calculul regiunilor libere
    // blocul curent, initial e primul bloc
    current_block = read_int(arena, 0);
    // daca arena este goala
    if (current_block == 0) {
        printf("0%% fragmentation\n");

    } else {
        // daca exita spatiu liber intre blocul de start si current_bloc
        if (current_block > INDEXSIZE) {
            ++free_blocks;
        }

        // urmatorul bloc
        unsigned int next_block = read_int(arena, current_block);
        // parcurgem toate blocurile
        while (next_block != 0) {
            // daca intre sfarsitul blocului curent si
            // inceputul blocului next_block exista spatiu
            if (current_block + read_int(arena, current_block + 8)
                < next_block) {
                ++free_blocks;
            }

            // updatez current_block si next_block
            current_block = next_block;
            next_block = read_int(arena, current_block);
        }

        /// daca intre sfarsitul ultimului bloc si finalul arenei
        // exista spatiu liber
        if (current_block + read_int(arena, current_block + 8)
            < arena->dimensiune) {
            ++free_blocks;
        }

        printf("%d%% fragmentation\n", 100 * (free_blocks - 1) / used_blocks);
    }
}


// Afiseaza pe cate o linie, zonele libere ai alocate, in ordinea in care sunt
// asezate in arena.
void SHOW_ALLOCATIONS(Arena *arena) {
    // indexul de start
    printf("OCCUPIED 4 bytes\n");

    // blocul curent
    unsigned int current_block = read_int(arena, 0);
    // daca arena este goala
    if (current_block == 0) {
        printf("FREE %d bytes\n", arena->dimensiune - 4);
        return;

    } else {
        // blocul urmator
        unsigned int next_block = read_int(arena, current_block);

        // daca e spatiu intre blocul de start si urmatorul bloc
        if (current_block > 4) {
            printf("FREE %d bytes\n", current_block - 4);
        }

        // parcurgem blocurile
        while (next_block != 0) {
            printf("OCCUPIED %d bytes\n", read_int(arena, current_block + 8));
            // daca intre sfarsitul blocului curent si
            // inceputul blocului next_block exista spatiu
            if ((current_block + read_int(arena, current_block + 8))
                        < next_block) {
                printf("FREE %d bytes\n", next_block
                    - (current_block + read_int(arena, current_block + 8)));
            }

            // updatez current_block si next_block
            current_block = next_block;
            next_block = read_int(arena, current_block);
        }

        /// daca intre sfarsitul ultimului bloc si finalul arenei
        // exista spatiu liber
        unsigned int len_current_block = read_int(arena, current_block + 8);
        printf("OCCUPIED %d bytes\n", len_current_block);
        if ((current_block + len_current_block) < arena->dimensiune) {
            printf("FREE %d bytes\n", arena->dimensiune -
                (current_block + len_current_block));
        }
    }
}

// Afiseaza informatii statistice despre starea arenei.
void SHOW(Arena *arena, char* info) {
    if (strncmp(info, "FREE", 4) == 0) {
        SHOW_FREE(arena);

    } else if (strncmp(info, "USAGE", 5) == 0) {
        SHOW_USAGE(arena);

    } else if (strncmp(info, "ALLOCATIONS", 11) == 0) {
        SHOW_ALLOCATIONS(arena);
    }
}
