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

atomic<int> counterPeopleServed(0); // Atomic variable playing a role of the counter
bool isBarberFree[num_barbers];

queue<int> waitingRoom; // queue with clients' ids representing waiting room
queue<int> couch; // queue with clients' ids representing couch


//int main()
//{
//    cout << "Opening the barber shop..." << endl;
//
//    // Initialazing threads
//    thread barbers[num_barbers];
//    thread customers[num_customers];
//
//    // Creating threads that are associated with barbers and customers and assigning IDs to them
//    for (int i = 0; i < num_barbers; i++) {
//        barbers[i] = thread(barberJob, i); 
//    }
//    for (int i = 0; i < num_customers; i++) {
//        customers[i] = thread(customerJob, i);
//        this_thread::sleep_for(chrono::seconds(rand() % 5 + 1)); // Random arrival time with 1 to 5 seconds inbetween
//    }
//
//
//
//
//    return 0;
//}

void customerJob(int customerId) {
    unique_lock<mutex> lock_for_waiting(waitingRoomMutex);

    while (waitingRoom.size() >= waiting_room_capacity)
        waitingRoomC.wait(lock_for_waiting);

    waitingRoom.push(customerId);
    cout << "Customer has entered the shop (waiting room), ID: " << customerId <<  endl;

    lock_for_waiting.unlock();

    unique_lock<mutex> lock_for_couch(couchMutex);

    while (couch.size() >= couch_capacity)
        couchC.wait(lock_for_couch);


    lock_for_couch.lock();   // Locking waiting room for safe access to memory in order to transfer client from waiting room to the couch
    waitingRoom.pop();
    lock_for_waiting.unlock();

    couch.push(customerId);

    cout << "Customer is now sitting on the couch, ID: " << customerId << endl;
    
    lock_for_couch.unlock();

    unique_lock<mutex> lock_for_barber(cashRegisterMutex);
    
    // Checking if there are vacant barbers
    while (true) {
        for (int i = 0; i < num_barbers; i++) {
            unique_lock<mutex> barbersAccessLock(barbersMutex[i]);

            /*if (!barberC[i].notify_one())
                continue;*/


        }
    }
}

void barberJob(int barberId) {

}