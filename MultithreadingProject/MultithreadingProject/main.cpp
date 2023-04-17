#include <iostream>
#include <cstdio>
#include <thread>
#include <queue>
#include <chrono>
#include <map>
#include <string>
#include <atomic>
#include <mutex>
#include <condition_variable>
using namespace std;

/*
The barber shop has three stations with three barbers, a couch for four people and a waiting room.
The establishment can hold 20 people at a time and a customer will not enter a filled establishment.
While inside, the customer sits on the couch if it is free, or waits in the waiting room otherwise.
If there is a vacant barber, the customer sitting longest on the couch is selected for a haircut,
and the customer waiting longest in the waiting room sits down in his place. When the customer gets
a haircut he has to pay. The fee is paid at the only counter manned by the currently vacant barber.
Barbers divide their time between cutting customers, sleeping ( in case there are no customers) and
handling the cash register. The program is supposed to end after serving 50 people.
*/

const int num_barbers = 3;
const int max_people = 20;
const int couch_capacity = 4;
const int num_customers = 50;

const int waiting_room_capacity = max_people - couch_capacity;


void barberJob(int barberId);
void customerJob(int customerId);

//Mutexes neccesary for multithread synchronization
mutex waitingRoomMutex;
mutex couchMutex;
mutex barbersMutex[num_barbers];
mutex cashRegisterMutex;

condition_variable waitingRoomC;
condition_variable couchC;
condition_variable barberC[num_barbers];
condition_variable cashRegisterC;

atomic<int> counterPeopleServed(0); // Atomic variable playing a role of the counter
bool isBarbershopClosed = false;

queue<int> waitingRoom; // queue with clients' ids representing waiting room
queue<int> couch; // queue with clients' ids representing couch
int cashRegister = -1;


int main()
{
    cout << "Opening the barber shop..." << endl;

    // Initialazing threads
    thread barbers[num_barbers];
    thread customers[num_customers];

    // Creating threads that are associated with barbers and customers and assigning IDs to them
    for (int i = 0; i < num_barbers; i++) {
        barbers[i] = thread(barberJob, i);
    }
    for (int i = 0; i < num_customers; i++) {
        customers[i] = thread(customerJob, i);
        this_thread::sleep_for(chrono::milliseconds(1)); // 1 customer per second
    }


    // Joining threads

    for (int i = 0; i < num_customers; i++) {
        customers[i].join();
    }

    while (counterPeopleServed < num_customers) {
        this_thread::sleep_for(chrono::seconds(1));
    }


    isBarbershopClosed = true;
    for (int i = 0; i < num_barbers; i++) {
        barbers[i].join();
    }

    cout << "Barber shop is closing, all customers have been served!" << endl;


    return 0;
}

void customerJob(int customerId) {
    unique_lock<mutex> lockForWaiting(waitingRoomMutex);


    while (waitingRoom.size() + couch.size() >= max_people || waitingRoom.size() >= waiting_room_capacity) {
        cout << "Barber shop is full. Customer is waiting outdoors..." << endl;
        waitingRoomC.wait(lockForWaiting);
    }

    waitingRoom.push(customerId);
    cout << "Customer has entered the shop (waiting room), ID: " << customerId << endl;

    lockForWaiting.unlock();

    unique_lock<mutex> lockForCouch(couchMutex);
    while (couch.size() >= couch_capacity || customerId != waitingRoom.front()) {
        couchC.wait(lockForCouch);
    }

    lockForWaiting.lock();    // Locking waiting room for safe access to memory in order to transfer client from waiting room to the couch
    waitingRoom.pop();

    lockForWaiting.unlock();

    couch.push(customerId);

    if (waitingRoom.size() < waiting_room_capacity)
        waitingRoomC.notify_one();

    cout << "Customer is now sitting on the couch, ID: " << customerId << endl;

    lockForCouch.unlock();


}

void barberJob(int barberId) {

    while (!isBarbershopClosed) {
        unique_lock<mutex> lockForCouch(couchMutex);

        if (couch.empty()) {
            lockForCouch.unlock();
            cout << "Barber is sleeping... (ID: " << barberId << ")" << endl;
            this_thread::sleep_for(chrono::milliseconds(250));
            continue;
        }

        int customerId = couch.front();


        couch.pop();

        cout << "Barber (ID: " << barberId << ") is now cutting hair of the customer (ID: " << customerId << ")." << endl;

        couchC.notify_all();
        lockForCouch.unlock();

        this_thread::sleep_for(chrono::milliseconds(300));

        unique_lock<mutex> lockForRegister(cashRegisterMutex);
        while (cashRegister != -1) {
            lockForRegister.unlock();
            cashRegisterC.wait(lockForRegister);
        }

        cout << "Moving customer to the cash register (ID customer: " << customerId << "; ID barber: " << barberId << ")." << endl;
        cashRegister = customerId;

        cout << "Customer is paying..." << endl;
        this_thread::sleep_for(chrono::milliseconds(100));

        cashRegister = -1;

        lockForRegister.unlock();
        cashRegisterC.notify_one();

        cout << "Customer is leaving the barber shop (ID: " << customerId << ")." << endl;
        ++counterPeopleServed;
    }
}