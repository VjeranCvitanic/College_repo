#include <iostream>

using namespace std;

extern "C" int potprogram_asm(int) asm("potprogram_asm");

int potprogram_c(int n);

int main()
{
	cout << "Hello" << endl;
    cout <<"ASM: " <<potprogram_asm(12) << endl;
    cout <<"C++: " <<potprogram_c(12) << endl;
    cout << "Bye" << endl;
	return 0;
}

int potprogram_c(int n)
{
    int sum = 0;
    for(int i = 0; i < n; i++)
    {
        sum += i;
    }

    return sum;
}
