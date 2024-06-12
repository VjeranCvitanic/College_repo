#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_ID_LEN (50)
#define MAX_DESCRIPTION_LEN 100
#define MAX_NAME_LEN 20

struct _client;
typedef struct _client* ClientP;
typedef struct _client
{
	char id[MAX_ID_LEN];
	char firstName[MAX_NAME_LEN];
	char lastName[MAX_NAME_LEN];
}Client;

struct _date;
typedef struct _date* DateP;
typedef struct _date
{
	int year;
	int month;
	int day;
}Date;


struct _transaction;
typedef struct _transaction* TransactionP;
typedef struct _transaction
{
	char description[MAX_DESCRIPTION_LEN];
	float amount;
	bool processed;
	ClientP client;
	DateP date;
	TransactionP next;
}Transaction;



ClientP Create_Client(char id[], char firstName[], char lastName[]);
TransactionP Create_Transaction(char description[], float amount, bool processed, ClientP client, DateP date);
DateP Create_Date(int year, int month, int day);

int Read(char fileName[], TransactionP first[]);

int InsertTransaction(TransactionP first[], TransactionP q);

int HashFunction(char id[MAX_ID_LEN]);

int datecmp(DateP d1, DateP d2);

int InsertAfter(TransactionP head, TransactionP q);

int Print(TransactionP first[]);

int FindConsumer(char id[], TransactionP first[]);

int DeleteAll(TransactionP first[]);

int DeleteAfter(TransactionP first);

#endif
