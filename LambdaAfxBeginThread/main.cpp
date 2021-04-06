#include <stdio.h>
#include "LambdaThread.h"

void multiplicationTable(int value, int count);

int main()
{
	CWinThread* pThread1 = _AfxBeginThread([]() -> void
	{
		multiplicationTable(3, 10000);
	});

	CWinThread* pThread2 = _AfxBeginThread([](int value, int count) -> void
	{
		multiplicationTable(value, count);
	}, 4, 10000);


	WaitForSingleObject(pThread1->m_hThread, 100000);
	WaitForSingleObject(pThread2->m_hThread, 100000);

	return 0;
}


void multiplicationTable(int value, int count)
{
	for (int i = 0; i < count; i++)
		printf("%d * %d = %d\n", value, i, value * i);
}
