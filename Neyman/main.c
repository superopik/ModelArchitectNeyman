#include <stdio.h>
// Вариант 9
// Команды:
// 1 — вывести значение аккумулятора на экран
// 2 — загрузить в аккумулятор значение из ячейки памяти
// 3 — умножить аккумулятор на значение из ячейки памяти
// 4 - безусловный переход (JUMP)
// 0 — стоп

//Принципы Фон Неймана
//1.Принцип программного управления: Процессор сам чиатет команды из файла и выполняет их
//2.Принцип однородности памяти: Мы храним команды и данные в одном массиве памяти
//3.Принцип адресности памяти: Мы указываем адрес до ячейки памяти в массиве
//4.Принцип последовательного выполнения команд: Процессор выполняет команды последовательно



int load(int* memory, int size) // Чтение данных
{
	int count = 0;
	int znach;
	while (count < size && scanf("%d", &znach) == 1) 
	{
		memory[count] = znach;
		count++;
	}
	return count;
}

void error(int err_code,int* cpu_state,int* memory) // Обработка ошибок
{
	// Коды ошибок: 1-Выход за пределы памяти 2-Неверная команда
	if ((cpu_state[5] == 1) && (err_code == 1))
	{
		printf("Ошибка: Выход за пределы памяти (повторное прерывание)\n");
		cpu_state[4] = 1;
		return;
	}
	else if ((cpu_state[5] == 2) && (err_code == 2))
	{
		printf("Ошибка: Неверная команда (повторное прерывание)\n");
		cpu_state[4] = 1;
		return;
	}
	cpu_state[5] = err_code;
	cpu_state[0] = memory[0];
	if (cpu_state[5] == 1)
	{
		printf("Ошибка: Выход за пределы памяти\n");
		cpu_state[0] = cpu_state[3];
	}
	else if (cpu_state[5] == 2)
	{
		printf("Ошибка: Неверная команда\n");
		cpu_state[0] = cpu_state[3];
	}
}

int main(void) // Вариант 9
{
	int cpu_state[8]; // Регистры процессора
	int memory[256]; // Память
	int loaded = load(memory, 256); // Чтение данных + количество загруженных элементов
	cpu_state[0] = 6; // Счётчик команд (pc)
	cpu_state[1] = 0; // аккумулятор (acc)
	cpu_state[2] = 0; // регистр команд(ir), хранит текущий опкод(команду)
	cpu_state[3] = memory[0]; // вектор прерываний(адрес обработчика ошибок)
	cpu_state[4] = 0; // Флаг остановки
	cpu_state[5] = 0; // код последней ошибки(0 = нет ошибки)
	cpu_state[6] = 0; //резерв
	cpu_state[7] = 0; //резерв
	if (cpu_state[3] < 0 || cpu_state[3] >= loaded) // Проверка выхода вектора прерываний за границы памяти 
	{
		error(1, cpu_state, memory);
		cpu_state[4] = 1;
	}
	while (cpu_state[4]==0)
	{
		
		if (cpu_state[0] < 0 || cpu_state[0] >= loaded) // проверка выхода счётчика команд за границы памяти
		{
			error(1, cpu_state, memory);
			continue;
		}

		cpu_state[2] = memory[cpu_state[0]]; // загрузка команды в регистр команд

		if (cpu_state[2] == 0) // Стоп
		{
			printf("pc=%d: команда 0 (HALT), acc=%d\n", cpu_state[0], cpu_state[1]);
			if (cpu_state[5] != 0)
			{
				printf("Программа завершена с ошибкой\n");
			}
			else
			{
				printf("Программа успешно завершена\n");
			}
			cpu_state[4] = 1;
			break;
		}


		else if (cpu_state[2] == 1) // Печать аккумулятора
		{
			printf("pc=%d: команда 1 (PRINT), acc=%d\n", cpu_state[0], cpu_state[1]);
			cpu_state[0] +=1;
			continue;
		}


		else if (cpu_state[2] == 2) // Загрузить в аккумулятор значение из памяти
		{
			if ((cpu_state[0] + 1 < 0) || (cpu_state[0] + 1 >= loaded)) // Проверка выхода счётчика команд для следующего значения за пределы памяти 
			{
				error(1, cpu_state, memory);
				continue;
			}
			else if ((memory[cpu_state[0] + 1] < 0) || (memory[cpu_state[0] + 1] >= loaded)) // Проверка выхода значения операнда за пределы памяти
			{
				error(1, cpu_state, memory);
				continue;
			}
			else
			{
				cpu_state[1] = memory[memory[cpu_state[0] + 1]];
				printf("pc=%d: команда 2 (LOAD), operand=%d, acc=%d\n", cpu_state[0], memory[cpu_state[0] + 1], cpu_state[1]);
				cpu_state[0] += 2;
				continue;
			}
		}


		else if (cpu_state[2] == 3) // Умножить аккумулятор на значение из памяти
		{
			if ((cpu_state[0] + 1 < 0) || (cpu_state[0] + 1 >= loaded)) // Проверка выхода счётчика команд для следующего значения за пределы памяти 
			{
				error(1, cpu_state, memory);
				continue;
			}
			else if ((memory[cpu_state[0] + 1] < 0) || (memory[cpu_state[0] + 1] >= loaded)) // Проверка выхода значения операнда за пределы памяти
			{
				error(1, cpu_state, memory);
				continue;
			}
			else
			{
				cpu_state[1] *= memory[memory[cpu_state[0] + 1]];
				printf("pc=%d: команда 3 (MUL), operand=%d, acc=%d\n", cpu_state[0], memory[cpu_state[0] + 1], cpu_state[1]);
				cpu_state[0] += 2;
				continue;
			}
		}


		else if (cpu_state[2] == 4) // JUMP
		{
			if ((cpu_state[0] + 1) < 0 || (cpu_state[0] + 1) >= loaded) // Проверка выхода счётчика команд для следующего значения за пределы памяти
			{
				error(1, cpu_state, memory);
				continue;
			}
			else if ((memory[cpu_state[0] + 1] < 0) || (memory[cpu_state[0] + 1] >= loaded)) // Проверка выхода значения операнда за пределы памяти
			{
				error(1, cpu_state, memory);
				continue;
			}
			else
			{
				printf("pc=%d: команда 4 (JUMP), operand=%d\n", cpu_state[0], memory[cpu_state[0] + 1]);
				cpu_state[0] = memory[cpu_state[0] + 1];
				continue;
			}
		}

		else
		{
			error(2, cpu_state, memory);
		}
	}
	return 0;
}