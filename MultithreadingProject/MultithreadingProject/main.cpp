#include <iostream>
#include <cstdio>
#include <thread>
#include <queue>
#include <chrono>
#include <map>
#include <string>
#include <atomic>
#include <mutex>
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

void serveCustomer(int barber);
void barberJob();
void customerJob();

//Mutexes neccesary for multithread synchronization
mutex waitingRoomMutex;
mutex couchMutex;
mutex barbersMutex[num_barbers];

atomic<int> counterPeopleServed(0); // Atomic variable playing a role of the counter
bool isBarberFree[num_barbers];

queue<int> waitingRoom; // queue with clients' ids representing waiting room
queue<int> couch; // queue with clients' ids representing couch


int main()
{
    cout << "Opening the barber shop..." << endl;

    // Initialazing threads
    thread barbers[num_barbers];
    thread customers[num_customers];

    // Creating threads that are associated with barbers and customers and assigning IDs to them


    return 0;
}