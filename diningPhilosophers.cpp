#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>
#include <iostream>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
using namespace std;

QMutex ctrl; // control output stream

class Monitor
{
	int ticketCounter;
	int numForks;
	int capacity;
	bool *forks;
	QMutex mutex;
	QWaitCondition wc;

public:
	int prio;
	Monitor(int n)
	{
		forks = new bool[n];
		ticketCounter = 0;
		capacity = n;
		prio = 0;
		numForks = n;

		for (int i = 0; i < n; i++)
		{
			forks[i] = true;
		}
	}

	~Monitor()
	{
		delete[] forks;
	}

	int getTicket(int i)
	{
		mutex.lock();
		int myticket = ticketCounter++;
		ctrl.lock();
		cout << "Philosopher " << i << " got ticket " << myticket << endl;
		ctrl.unlock();
		mutex.unlock();
		return myticket;
	}

	void getForks(int i, int currentTick)
	{
		mutex.lock();
		while (numForks < 2 || !forks[i] || !forks[(i + 1) % capacity] || currentTick > prio + 1)
		{
			wc.wait(&mutex);
		}

		forks[i] = false;
		forks[(i + 1) % capacity] = false;
		numForks -= 2;
		ctrl.lock();
		cout << "Philosopher " << i << " got forks" << endl;
		ctrl.unlock();
		mutex.unlock();
	}

	void releaseForks(int i)
	{
		mutex.lock();
		forks[i] = true;
		forks[(i + 1) % capacity] = true;
		numForks += 2;
		ctrl.lock();
		cout << "Philosopher " << i << " released the forks" << endl;
		ctrl.unlock();
		wc.wakeAll();
		mutex.unlock();
	}
};

class Philosopher : public QThread
{
	Monitor *monitor = nullptr;
	int id;
	int ticket;

public:
	Philosopher(int i, Monitor *m) : id(i), monitor(m)
	{
		srand(time(0));
	}

	void think()
	{
		srand(time(0));
		usleep(rand() % 1000000);
		return;
	}

	void eat()
	{
		srand(time(0));
		usleep(rand() % 1000000);
		return;
	}

	void run()
	{
		think();
		ticket = monitor->getTicket(id);
		monitor->getForks(id, ticket);
		eat();
		monitor->prio = ticket; // last philo eating ticket number
		ctrl.lock();
		cout << "Philosopher " << id << " ate" << endl;
		ctrl.unlock();
		monitor->releaseForks(id);
	}
};

int main(int argc, char *argv[])
{
	srand(time(0));
	if (argc != 2)
	{
		cout << "Usage: <philosophers>" << endl;
		return 0;
	}

	int nPhilosophers = atoi(argv[1]);
	Monitor monitor(nPhilosophers);
	Philosopher *p[nPhilosophers];

	for (int i = 0; i < nPhilosophers; i++)
	{
		p[i] = new Philosopher(i, &monitor);
		p[i]->start();
	}

	for (int i = 0; i < nPhilosophers; i++)
	{
		p[i]->wait();
	}

	return 0;
}
