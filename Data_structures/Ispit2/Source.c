#define _CRT_SECURE_NO_WARNINGS
#include "Functions.h"

int main()
{
	Transaction head[7] = { 0 };
	TransactionP first[7] = { 0 };
	char id[MAX_ID_LEN];

	for (int i = 0; i < 7; i++)
	{
		head[i].next = NULL;
		first[i] = &head[i];
	}
		
	Read("Text.txt", first);

	Print(first);

	printf("\n\n\n\n\n");

	printf("Enter id:\n");
	scanf("%s", id);

	FindConsumer(id, first);

	//jos za 4 i 5

	DeleteAll(first);

	return EXIT_SUCCESS;
}