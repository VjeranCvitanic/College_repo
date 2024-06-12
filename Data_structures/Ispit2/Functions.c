#define _CRT_SECURE_NO_WARNINGS
#include "Functions.h"

ClientP Create_Client(char id[], char firstName[], char lastName[])
{
	ClientP q = NULL;
	q = (ClientP)malloc(sizeof(Client));
	if (!q)
		return NULL;

	strcpy(q->id, id);
	strcpy(q->firstName, firstName);
	strcpy(q->lastName, lastName);

	return q;
}

TransactionP Create_Transaction(char description[], float amount, bool processed, ClientP client, DateP date)
{
	TransactionP q = NULL;
	q = (TransactionP)malloc(sizeof(Transaction));
	if (!q)
		return NULL;

	q->processed = processed;
	q->amount = amount;
	strcpy(q->description, description);
	q->client = client;
	q->date = date;
	q->next = NULL;

	return q;
}

DateP Create_Date(int year, int month, int day)
{
	DateP q = NULL;
	q = (DateP)malloc(sizeof(Date));
	if (!q)
		return NULL;

	q->day = day;
	q->month = month;
	q->year = year;

	return q;
}

int Read(char fileName[], TransactionP first[])
{
	FILE* fp = fopen(fileName, "r");

	ClientP client = NULL;
	TransactionP transaction = NULL;
	DateP date = NULL;
	char description[MAX_DESCRIPTION_LEN] = { 0 };
	float amount = 0;
	char bool_var[7] = { 0 };
	bool processed = false;
	char id[MAX_ID_LEN] = { 0 };
	char firstName[MAX_NAME_LEN] = { 0 };
	char lastName[MAX_NAME_LEN] = { 0 };
	int year = 0;
	int month = 0;
	int day = 0;

	if (!fp)
		return EXIT_FAILURE;

	while (!feof(fp))
	{
		fscanf(fp, "%s %f %s %s %s %s %d-%d-%d\n", description, &amount, bool_var, id, firstName, lastName, &year, &month, &day);
		client = Create_Client(id, firstName, lastName);
		date = Create_Date(year, month, day);

		if (strcmp(bool_var, "true") == 0)
			processed = true;

		transaction = Create_Transaction(description, amount, processed, client, date);
		InsertTransaction(first, transaction);
	}

	fclose(fp);

	return EXIT_SUCCESS;
}

int InsertTransaction(TransactionP first[], TransactionP q)
{
	int hash_id = HashFunction(q->client->id);

	TransactionP head = first[hash_id];

	while (head->next && datecmp(head->next->date, q->date) < 0)
	{
		head = head->next;
	}

	if (head->next && datecmp(head->next->date, q->date) == 0)
	{
		while (head->next && datecmp(head->next->date, q->date) == 0 && strcmp(head->next->client->lastName, q->client->lastName) < 0)
		{
			head = head->next;
		}

		if (head->next && strcmp(head->next->client->lastName, q->client->lastName) == 0)
		{
			while (head->next && datecmp(head->next->date, q->date) == 0 && strcmp(head->next->client->lastName, q->client->lastName) == 0 && strcmp(head->next->client->lastName, q->client->lastName) < 0)
			{
				head = head->next;
			}
		}
	}


	InsertAfter(head, q);

	return EXIT_SUCCESS;
}

int HashFunction(char id[MAX_ID_LEN])
{
	char* p = NULL;
	p = id;
	int sum = 0;


	while (strlen(p) > 0)
	{
		sum += *p;
		p += 1;
	}

	return sum % 7;
}

int datecmp(DateP d1, DateP d2)
{
	if (d1->year == d2->year)
	{
		if (d1->month == d2->month)
		{
			if (d1->day == d2->day)
			{
				return 0;
			}

			else if (d1->day > d2->day)
			{
				return 1;
			}

			else
				return -1;
		}

		else if (d1->month > d2->month)
			return 1;

		else
			return -1;
	}

	else if (d1->year > d2->year)
	{
		return 1;
	}

	else 
		return -1;
}

int InsertAfter(TransactionP head, TransactionP q)
{
	if (!head || !q)
		return EXIT_FAILURE;

	q->next = head->next;
	head->next = q;

	return EXIT_SUCCESS;
}

int Print(TransactionP first[])
{
	TransactionP head = NULL;
	char bool_var[7] = "false";

	for (int i = 0; i < 7; i++)
	{
		head = first[i]->next;

		while (head)
		{
			if (head->processed)
				strcpy(bool_var,"true");
			printf("%s %f %s %s %s %s %d-%d-%d\n", head->description, head->amount, bool_var, head->client->id, head->client->firstName, head->client->lastName, head->date->year, head->date->month, head->date->day);
			head = head->next;
		}

		printf("\n\n\n\n");
	}

	return EXIT_SUCCESS;
}


int FindConsumer(char id[], TransactionP first[])
{
	int id_val = HashFunction(id);
	char bool_var[7] = "false";

	TransactionP head = first[id_val]->next;

	while (head)
	{
		if (strcmp(head->client->id, id) == 0)
		{
			if (head->processed)
				strcpy(bool_var, "true");
			printf("%s %f %s %s %s %s %d-%d-%d\n", head->description, head->amount, bool_var, head->client->id, head->client->firstName, head->client->lastName, head->date->year, head->date->month, head->date->day);
		}

		head = head->next;
	}

	return EXIT_SUCCESS;
}

int DeleteAll(TransactionP first[])
{
	for (int i = 0; i < 7; i++)
	{
		while (first[i]->next)
			DeleteAfter(first[i]);
	}

	return EXIT_SUCCESS;
}

int DeleteAfter(TransactionP first)
{
	TransactionP temp = first->next;
	first->next = first->next->next;

	free(temp);

	return EXIT_SUCCESS;
}