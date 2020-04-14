#ifndef ARENA_H_   /* Include guard */
#define ARENA_H_

// Copyright 2017 Vlad-Stefan DIEACONU, 311CA 2017

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  unsigned char *arena;
  unsigned  int dimensiune;
} Arena;

void INITIALIZE(Arena *arena, int N);
void FINALIZE(Arena *arena);
void DUMP(Arena *arena);
void ALLOC(Arena *arena, int size);
void FREE(Arena *arena, int index);
void FILL(Arena *arena, int index, int size, int value);
void SHOW(Arena *arena, char* info);

void ALLOCALIGNED(Arena *arena, int size, int align);

#endif  // ARENA_H_
