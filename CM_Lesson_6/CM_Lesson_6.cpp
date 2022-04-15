#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <random>
#include <queue>
#include "pcout.h"

using namespace std;

//***********************************************************************************************
// 1.
// Создайте потокобезопасную оболочку для объекта cout.Назовите ее pcout.Необходимо,
// чтобы несколько потоков могли обращаться к pcout и информация выводилась в консоль.
// Продемонстрируйте работу pcout.
//***********************************************************************************************



void v1()
{
    for (int i = 0; i < 20; ++i)
        pcout(__FUNCTION__);
}
void v2()
{
    for (int i = 0; i < 20; ++i)
        pcout(__FUNCTION__);

}
void v3()
{
    for (int i = 0; i < 20; ++i)
        pcout(__FUNCTION__);
}


void task_1()
{
    thread t1(v1);
    thread t2(v2);
    thread t3(v3);

    t1.join();
    t2.join();
    t3.join();

}

//***********************************************************************************************



//***********************************************************************************************
// 2.
// Реализовать функцию, возвращающую i - ое простое число(например, миллионное простое
// число равно 15485863).Вычисления реализовать во вторичном потоке.В консоли отображать
// прогресс вычисления(в основном потоке).
//***********************************************************************************************

bool isSimple(int num)
{
    for (int i = 2; i < num; ++i)
    {
        if ((num % i) == 0)
            return false;
    }
    return true;
}

mutex m;
queue<pair<int, int>> state;
condition_variable stateCond;

void genSimpleNum(int n)
{
    int counter = 1;
    int number = 0;
    while (number < n)
    {
        if (isSimple(counter))
        {
            ++number;
            lock_guard<mutex> lg(m);
            state.push(pair(number, counter));
            stateCond.notify_one();
            
        }
        ++counter;
    }
}

//***********************************************************************************************

void task_2()
{
    int numbersToFind = 10000;
    //int numbersToFind = 10;
    thread genSimpleNumThread(genSimpleNum, numbersToFind);

    pair<int, int> currentState;

    while (true)
    {
        unique_lock<mutex> lk(m);
        stateCond.wait(lk, []() {return !state.empty(); });
        currentState = state.front();
        state.pop();
        lk.unlock();
        if (!(currentState.first % 10))
            cout << "\r" << "Найдено " << currentState.first << " простых чисел из " << numbersToFind;

            if (currentState.first == numbersToFind)
                break;
    }

    genSimpleNumThread.join();
    cout << endl;
    cout << numbersToFind << "-е просто число: " << currentState.second << endl;

}

//***********************************************************************************************
// 3.
// Промоделировать следующую ситуацию.Есть два человека(2 потока) : хозяин и вор.Хозяин
// приносит домой вещи(функция добавляющая случайное число в вектор с периодичностью 1
// раз в секунду).При этом у каждой вещи есть своя ценность.Вор забирает вещи(функция,
// которая находит наибольшее число и удаляет из вектора с периодичностью 1 раз в 0.5
// секунд), каждый раз забирает вещь с наибольшей ценностью.
//***********************************************************************************************

vector <int> stuff;
mutex stuffMutex;

const chrono::seconds OWNER_PERIOD = 1s;
const chrono::milliseconds THIEF_PERIOD = 500ms;

void ownerFunction()
{
    random_device rd;
    mt19937 gen{ rd() };
    uniform_int_distribution<int> d{ 1, 10 };

    while (true)
    {
        unique_lock<mutex> lk(stuffMutex);
        stuff.push_back(d(gen));
        lk.unlock();

        this_thread::sleep_for(OWNER_PERIOD);
    }
}

void thiefFunction()
{
    while (true)
    {
        unique_lock<mutex> lk(stuffMutex);
        if (stuff.size() > 0)
        {
            auto it = max_element(stuff.begin(), stuff.end());
            stuff.erase(it);
        }
        lk.unlock();
        this_thread::sleep_for(THIEF_PERIOD);
    }
}

void task_3()
{
    thread ownerThread(ownerFunction);
    thread thiefThread(thiefFunction);
    ownerThread.join();
    thiefThread.join();
}
//***********************************************************************************************






int main()
{
    setlocale(LC_ALL, "Russian");

    task_1();
    //task_2();
    //task_3();
}
