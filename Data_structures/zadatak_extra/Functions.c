#define _CRT_SECURE_NO_WARNINGS
#include "Functions.h"

int GenerateRandomPeople(char fileName[MAX_LINE])
{
	FILE* fp = NULL;
	fp = fopen(fileName, "w");
	srand(time(NULL));

	if (!fp)
	{
		perror("Couldn't open the file!\n");
		return EXIT_FAILURE;
	}

	//char numbers[10] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
	char names[MAX_LINE][MAX_NAME] = { "Ante", "Stipe", "Ivan", "Luka", "Jura", "Anton", "Vjeran", "Tonci", "Marko", "Marin", "Ana", "Ivana", "Irena", "Lucija", "Karla", "Petra", "Tonka", "Ivona", "Anica", "Ivo", "Anamarija", "Leona", "Kristina", "Josip", "Mijo"};
	char surnames[MAX_LINE][MAX_NAME] = { "Cvitanic", "Biuk", "Radovnikovic", "Korda", "Jakelic", "Munivrana", "Modric", "Livaja", "Kalinic", "Vukovic", "Petkovic", "Orsic", "Drmic" };

	//char rnum[OIB_SIZE] = { 0 };
	int rnum = 0;
	char rname[MAX_NAME] = { 0 };
	char rsurname[MAX_NAME] = { 0 };
	
	fprintf(fp, "NAME SURNAME   OIB\n");

	for (int i = 0; i < 80; i++)
	{
		rnum = rand() % 900000 + 10000;
	
		strcpy(rname, names[rand() % 25]);
		strcpy(rsurname, surnames[rand() % 13]);

		fprintf(fp, "%s %s   %d\n", rname, rsurname, rnum);
	}

	fclose(fp);

	return EXIT_SUCCESS;
}

Position CreateNewPerson(char name[MAX_NAME], char surname[MAX_NAME], int OIB, char vaccine[MAX_NAME], int year, int month, int day, int numOfDoses)
{
	Position q = NULL;
	q = (Position)malloc(sizeof(Person));

	if (!q)
	{
		perror("Couldn't allocate memory!\n");
		return NULL;
	}

	strcpy(q->name, name);
	strcpy(q->surname, surname);
	strcpy(q->vaccine, vaccine);
	q->date.year = year;
	q->date.month = month;
	q->date.day = day;
	q->OIB = OIB;
	q->numOfDoses = numOfDoses;
	q->left = NULL;
	q->right = NULL;

	return q;
}

Position Insert(Position current, Position q)
{
	if (current == NULL)return q;
	else if (current->OIB == -1)return q;
	else if (strcmp(current->surname, q->surname) > 0)
	{
		current->left = Insert(current->left, q);
	}

	else if (strcmp(current->surname, q->surname) < 0)
	{
		current->right = Insert(current->right, q);
	}

	else
	{
		if (strcmp(current->name, q->name) > 0)
		{
			current->left = Insert(current->left, q);
		}

		else if (strcmp(current->name, q->name) < 0)
		{
			current->right = Insert(current->right, q);
		}

		else
		{
			if (current->date.year * 10000 + current->date.month * 100 + current->date.day > q->date.year * 10000 + q->date.month * 100 + q->date.day)
			{
				current->left = Insert(current->left, q);
			}

			else if (current->date.year * 10000 + current->date.month * 100 + current->date.day < q->date.year * 10000 + q->date.month * 100 + q->date.day)
			{
				current->right = Insert(current->right, q);
			}

			else
			{
				if (strcmp(current->vaccine, q->vaccine) > 0)
				{
					current->left = Insert(current->left, q);
				}

				else current->right = Insert(current->right, q);
			}
		}
	}


	return current;
}

int ReadFromFirstFile(char fileName[MAX_LINE], ListPosition listFirst, int* br)
{
	FILE* fp = fopen(fileName, "r");
	char buffer[MAX_SIZE] = { 0 };
	int OIB = 0;
	int year = 0;
	int month = 0;
	int day = 0;
	int numOfDoses = 0;
	char vaccine[MAX_NAME] = { 0 };

	if (!fp)
	{
		perror("Couldn't open the file!\n");
		return EXIT_FAILURE;
	}

	while (!feof(fp))
	{
		fgets(buffer, MAX_SIZE, fp);
		if (sscanf(buffer, "%d %s %d %d:%d:%d\n", &OIB, vaccine, &numOfDoses, &day, &month, &year) == 6)
		{
			InsertInList(listFirst, OIB, vaccine, numOfDoses, day, month, year);
		}
		(*br)++;
	}

	return EXIT_SUCCESS;
}

Position ReadFromSecondFile(char fileName[MAX_LINE], Position first, ListPosition listFirst, int* br)
{
	FILE* fp = fopen(fileName, "r");
	char name[MAX_NAME] = { 0 };
	char surname[MAX_NAME] = { 0 };
	int OIB = 0;
	char buffer[MAX_SIZE] = { 0 };
	if (!fp)
	{
		perror("Couldn't open the file\n");
		return EXIT_FAILURE;
	}

	while (!feof(fp))
	{
		fgets(buffer, MAX_SIZE, fp);
		if (sscanf(buffer, "%s %s %d\n", name, surname, &OIB) == 3)
		{
			if (Find(listFirst, OIB) == EXIT_SUCCESS)
			{
				first = AddName(first, listFirst, OIB, name, surname);
			}
		}
		(*br)++;
	}

	fclose(fp);
	return first;
}

int Find(ListPosition current, int OIB)
{
	ListPosition temp = current;

	while (temp->next && temp->OIB != OIB)
		temp = temp->next;

	if (temp->next == NULL)return EXIT_FAILURE;

	else return EXIT_SUCCESS;
}

Position AddName(Position current, ListPosition listFirst, int OIB, char name[MAX_NAME], char surname[MAX_NAME])
{
	ListPosition temp = listFirst;
	
	while (temp->next && temp->OIB != OIB)
		temp = temp->next;

	if (!temp->next)return NULL;

	else
	{
		Position q = CreateNewPerson(name, surname, OIB, temp->vaccine, temp->date.year, temp->date.month, temp->date.day, temp->numOfDoses);
		current = Insert(current, q);
	}

	return current;
}

int Print(Position first)
{
	if (first == NULL)return EXIT_SUCCESS;

	Print(first->left);
	printf("%d %s %s %d %d:%d:%d %s\n", first->OIB, first->surname, first->name, first->numOfDoses, first->date.day, first->date.month, first->date.year, first->vaccine);
	Print(first->right);

	return EXIT_SUCCESS;
}

ListPosition CreateNewListElement(int OIB, char vaccine[MAX_NAME], int numOfDoses, int day, int month, int year)
{
	ListPosition q = (ListPosition)malloc(sizeof(Node));
	if (!q)
	{
		perror("Couldn't allocate memory!\n");
		return NULL;
	}

	q->next = NULL;
	q->date.day = day;
	q->date.month = month;
	q->date.year = year;
	q->numOfDoses = numOfDoses;
	strcpy(q->vaccine, vaccine);
	q->OIB = OIB;

	return q;
};

int InsertInList(ListPosition head, int OIB, char vaccine[MAX_NAME], int numOfDoses, int day, int month, int year)
{
	ListPosition first = head;
	ListPosition q = CreateNewListElement(OIB, vaccine, numOfDoses, day, month, year);
	
	while (first->next)
	{
		first = first->next;
	}

	q->next = first->next;
	first->next = q;

	return EXIT_SUCCESS;
}