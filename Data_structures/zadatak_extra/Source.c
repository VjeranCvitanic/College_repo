#define _CRT_SECURE_NO_WARNINGS
#include "Functions.h"

int main()
{
	//GenerateRandomPeople("List_of_people.txt");
	int br = 0, br2 = 0;
	Person head = { .date.day = 0, .date.month = 0, .date.year = 0, .left = NULL, .numOfDoses = 0, .name = "", .OIB = -1, .right = NULL, .surname = "", .vaccine = "" };

	Position first = &head;

	Node listHead = { .next = NULL, .date.day = 0, .date.month = 0, .date.year = 0, .numOfDoses = 0, .OIB = 0, .vaccine = "" };
	ListPosition listFirst = &listHead;

	if (ReadFromFirstFile("Vaccinated.txt", listFirst, &br) == EXIT_SUCCESS)
		first = ReadFromSecondFile("List_of_people.txt", first, listFirst, &br2);

	Print(first);

	printf("\n\n");

	printf("Percentage of vaccinated people(%d) in the country(%d) is %.2f %", br, br2, 100 * (double)br / br2);

	printf("\n\n");

	return 0;
}