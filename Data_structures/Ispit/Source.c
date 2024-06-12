#define _CRT_SECURE_NO_WARNINGS
#include "Functions.h"

int main()
{
	Transaction head[7] = { 0 };
	TransactionP first[7] = { 0 };
	for (int i = 0; i < 7; i++)
	{
		head[i].next = NULL;
		first[i] = &head[i];
	}

	ReadFromFile("transactions.txt", first);

	Print(first);

	return EXIT_SUCCESS;
}