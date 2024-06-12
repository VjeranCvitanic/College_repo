#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define MAX_SIZE 1024
#define MAX_ID_LEN 8
#define MAX_NAME_LEN 256
#define MAX_DESCRIPTION_NAME 526

struct _Client;
typedef struct _Client* ClientP;
typedef struct _Client
{
	char id[MAX_ID_LEN];
	char firstName[MAX_NAME_LEN];
	char lastName[MAX_NAME_LEN];
}Client;

struct _Date;
typedef struct _Date* DateP;
typedef struct _Date
{
	int year;
	int month;
	int day;
}Date;

struct _Transaction;
typedef struct _Transaction* TransactionP;
typedef struct _Transaction
{
	char description[MAX_DESCRIPTION_NAME];
	float amount;
	bool processed;
	ClientP client;
	DateP date;
	TransactionP next;
}Transaction;


DateP CreateNewDate(int day, int month, int year);

ClientP CreateNewClient(char id[], char firstName[], char lastName[]);

TransactionP CreateNewTransaction(char description[], float amount, char processed[], ClientP client, DateP date);

int ReadFromFile(char fileName[], TransactionP array[]);

int InsertInHashTable(TransactionP array[], TransactionP newTransaction);

int Insert(TransactionP head, TransactionP newTransaction);

int datecmp(DateP d1, DateP d2);

int InsertAfter(TransactionP first, TransactionP newTransaction);

int Print(TransactionP array[]);

#endif
