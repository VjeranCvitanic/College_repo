#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAX_LINE 27
#define MAX_NAME 15
#define MAX_SIZE 1024
//#define OIB_SIZE 12

typedef struct _Date
{
	int day;
	int month;
	int year;
}Date;

struct _Node;
typedef struct _Node* ListPosition;
typedef struct _Node
{
	int OIB;
	char vaccine[MAX_NAME];
	int numOfDoses;
	Date date;
	ListPosition next;
}Node;

struct _Person;
typedef struct _Person* Position;
typedef struct _Person
{
	int OIB;
	char name[MAX_NAME];
	char surname[MAX_NAME];
	char vaccine[MAX_NAME];
	Date date;
	int numOfDoses;

	Position left;
	Position right;
}Person;

Position CreateNewPerson(char name[MAX_NAME], char surname[MAX_NAME], int OIB, char vaccine[MAX_NAME], int year, int month, int day, int numOfDoses);

Position Insert(Position current, Position q);

int GenerateRandomPeople(char fileName[MAX_LINE]);

int ReadFromFirstFile(char fileName[MAX_LINE], ListPosition listFirst, int* br);

Position ReadFromSecondFile(char fileName[MAX_LINE], Position first, ListPosition listFirst, int* br);

int Find(ListPosition current, int OIB);

Position AddName(Position first, ListPosition listFirst, int OIB, char name[MAX_NAME], char surname[MAX_NAME]);

int Print(Position first);

ListPosition CreateNewListElement(int OIB, char vaccine[MAX_NAME], int numOfDoses, int day, int month, int year);

int InsertInList(ListPosition head, int OIB, char vaccine[MAX_NAME], int numOfDoses, int day, int month, int year);


#endif 

