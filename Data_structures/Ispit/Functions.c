#define _CRT_SECURE_NO_WARNINGS
#include "Functions.h"

DateP CreateNewDate(int day, int month, int year)
{
	DateP q = NULL;
	q = (DateP)malloc(sizeof(Date));
	if (!q)
	{
		perror("Couldn't allocate memory!\n");
		return NULL;
	}

	q->day = day;
	q->month = month;
	q->year = year;

	return q;
}

ClientP CreateNewClient(char id[], char firstName[], char lastName[])
{
	ClientP q = NULL;
	q = (ClientP)malloc(sizeof(Client));
	if (!q)
	{
		perror("Couldn't allocate memory!\n");
		return NULL;
	}

	strcpy(q->id, id);
	strcpy(q->firstName, firstName);
	strcpy(q->lastName, lastName);

	return q;
}

TransactionP CreateNewTransaction(char description[], float amount, char processed[], ClientP client, DateP date)
{
	TransactionP q = NULL;
	q = (TransactionP)malloc(sizeof(Transaction));
	if (!q)
	{
		perror("Couldn't allocate memory!\n");
		return NULL;
	}

	strcpy(q->description, description);
	q->amount = amount;
	q->client = client;
	q->date = date;
	q->next = NULL;
	if (strcmp(processed, "true") || strcmp(processed, "True") || strcmp(processed, "TRUE"))
		q->processed = true;
	else q->processed = false;

	return q;
}

int ReadFromFile(char fileName[], TransactionP array[])
{
	FILE* fp = NULL;
	char buffer[MAX_SIZE] = { 0 };
	char* p = NULL;
	char desc[MAX_DESCRIPTION_NAME] = { 0 };
	char firstName[MAX_NAME_LEN] = { 0 };
	char lastName[MAX_NAME_LEN] = { 0 };
	float amount = 0.0;
	char id[MAX_ID_LEN] = { 0 };
	char processed[6] = { 0 };
	int year = 0, month = 0, day = 0;
	DateP newDate = NULL;
	ClientP newClient = NULL;
	TransactionP newTransaction = NULL;

	fp = fopen(fileName, "r");
	if (!fp)
	{
		printf("Couldn't open the file!\n");
		return EXIT_FAILURE;
	}

	p = buffer;
	while (!feof(fp))
	{
		fgets(p, MAX_SIZE, fp);

		if (sscanf(p, "%s %f %s %s %s %s %d - %d - %d", desc, &amount, processed, id, firstName, lastName, &year, &month, &day) == 9)
		{
			newDate = CreateNewDate(day, month, year);
			newClient = CreateNewClient(id, firstName, lastName);
			if (newDate != NULL && newClient != NULL)
				newTransaction = CreateNewTransaction(desc, amount, processed, newClient, newDate);

			if (newTransaction != NULL)
			{
				InsertInHashTable(array, newTransaction);
			}

		}
	}
	
	fclose(fp);

	return EXIT_SUCCESS;
}

int InsertInHashTable(TransactionP array[], TransactionP newTransaction)
{
	int hashID = 0;
	char* p = NULL;
	int sum = 0;

	p = newTransaction->client->id;

	while (strlen(p) > 0)
	{
		sum += *p;
		p += 1;
	}

	sum = sum % 7;

	Insert(array[sum], newTransaction);

	return EXIT_SUCCESS;
}

int Insert(TransactionP head, TransactionP newTransaction)
{
	TransactionP first = head;

	while (first->next && datecmp(first->next->date, newTransaction->date) > 0)
	{
		first = first->next;
	}

	if (!first->next)
	{
		InsertAfter(first, newTransaction);
		return EXIT_SUCCESS;
	}




	//...


	InsertAfter(first, newTransaction);
	return EXIT_SUCCESS;

	/*if (first->next)
	{
		while (first->next && datecmp(first->next->date, newTransaction->date == 0))
		{
			if (strcmp(first->client->lastName, newTransaction->client->lastName) > 0)
				first = first->next;

			else if (strcmp(first->client->lastName, newTransaction->client->lastName) == 0)
			{
				if (strcmp(first->client->firstName, newTransaction->client->firstName) > 0)
					first = first->next;

				else
				{
					InsertAfter(first, newTransaction);
					break;
				}
			}

			else
			{
				InsertAfter(first, newTransaction);
				break;
			}
		}
	}*/
	

	return EXIT_SUCCESS;
}

int datecmp(DateP d1, DateP d2)
{
	if (!d1 || !d2)return EXIT_FAILURE;
	if (d1->year == d2->year)
	{
		if (d1->month == d2->month)
		{
			return d1->month - d2->month;
		}

		return d1->month - d2->month;
	}

	return d1->year - d2->year;
}

int InsertAfter(TransactionP first, TransactionP newTransaction)
{
	if (!first || !newTransaction)return EXIT_FAILURE;

	newTransaction->next = first->next;
	first->next = newTransaction;

	return EXIT_SUCCESS;
}

int Print(TransactionP array[])
{
	char processed[6] = { 0 };

	for (int i = 0; i < 7; i++)
	{
		TransactionP first = array[i]->next;
		while (first)
		{
			if (first->processed)strcpy(processed, "true");
			else strcpy(processed, "false");
			printf("%s %f %s %s %s %s %d - %d - %d\n", first->description, first->amount, processed, first->client->id, first->client->firstName, first->client->lastName, first->date->year, first->date->month, first->date->day);
			first = first->next;
		}
	}

	return EXIT_SUCCESS;
}
