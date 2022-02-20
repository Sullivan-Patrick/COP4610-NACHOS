#include "copyright.h"
#include "system.h"
#include "synch.h"
#include "elevator.h"


int nextPersonID = 1;
Lock *personIDLock = new Lock("PersonIDLock");


ELEVATOR *e;


void ELEVATOR::start() {

    while(1) {

        // A. Wait until hailed

        // B. , loop doing the following
        for (int i = 0; i < N; i++) {
            //4. Go to next floor
            currentFloor = i;
            printf("Elevator arrives on floor %d", currentFloor);

            //0. Acquire elevatorLock
            elevatorLock->Acquire();

            //1. Signal persons inside elevator to get off (leaving->broadcast(elevatorLock))
            leaving[i]->Broadcast(elevatorLock);

            //2. Signal persons atFloor to get in, one at a time, checking occupancyLimit each time
            while(personsWaiting[i] > 0 && occupancy < maxOccupancy) {
                // personsWaiting--;
                // occupancy++;
                entering[i]->Signal(elevatorLock);
            }

            //2.5 Release elevatorLock
            elevatorLock->Release();

            //3. Spin for some time

            for(int j =0 ; j< 1000000; j++) {
                currentThread->Yield();
            }

        }

        for (int i = N-2; i > 0; i--) {
            //4. Go to next floor
            currentFloor = i;
            printf("Elevator arrives on floor %d", currentFloor);

            //0. Acquire elevatorLock
            elevatorLock->Acquire();

            //1. Signal persons inside elevator to get off (leaving->broadcast(elevatorLock))
            leaving[i]->Broadcast(elevatorLock);

            //2. Signal persons atFloor to get in, one at a time, checking occupancyLimit each time
            while(personsWaiting[i] > 0 && occupancy < maxOccupancy) {
                // personsWaiting--;
                // occupancy++;
                entering[i]->Signal(elevatorLock);
            }

            //2.5 Release elevatorLock
            elevatorLock->Release();

            //3. Spin for some time

            for(int j =0 ; j< 1000000; j++) {
                currentThread->Yield();
            }
        }
    }
}

void ElevatorThread(int numFloors) {

    printf("Elevator with %d floors was created!\n", numFloors);

    e = new ELEVATOR(numFloors);

    e->start();


}

ELEVATOR::ELEVATOR(int numFloors) {
    N = numFloors;
    currentFloor = 1;
    entering = new Condition*[numFloors]; //entering[i] is condition to call when arriving on floor i
    // Initialize entering
    for (int i = 0; i < numFloors; i++) {
        entering[i] = new Condition("Entering " + i);
    }
    personsWaiting = new int[numFloors]; //personswaiting[i] = num people waiting on floor i
    elevatorLock = new Lock("ElevatorLock");

    // Initialize leaving
    leaving = new Condition*[numFloors];
    for (int i = 0; i < numFloors; i++) {
        leaving[i] = new Condition("Leaving " + i);
    }
}


void Elevator(int numFloors) {
    // Create Elevator Thread
    Thread *t = new Thread("Elevator");
    t->Fork(ElevatorThread, numFloors);
}


void ELEVATOR::hailElevator(Person *p) {
    // 1. Increment waiting persons atFloor
    personsWaiting[p->atFloor]++;

    // 2. Hail Elevator
    // 2.5 Acquire elevatorLock;
    elevatorLock->Acquire();

    // 3. Wait for elevator to arrive atFloor [entering[p->atFloor]->wait(elevatorLock)]
    entering[p->atFloor]->Wait(elevatorLock);

    // 5. Get into elevator
    printf("Person %d got into the elevator.\n", p->id);
    // 6. Decrement persons waiting atFloor [personsWaiting[atFloor]++]
    personsWaiting[p->atFloor]--;
    // 7. Increment persons inside elevator [occupancy++]
    occupancy++;

    // 8. Wait for elevator to reach toFloor [leaving[p->toFloor]->wait(elevatorLock)]
    leaving[p->toFloor]->Wait(elevatorLock);

    // 9. Get out of the elevator
    printf("Person %d got out of the elevator.\n", p->id);

    // 10. Decrement persons inside elevator
    occupancy--;

    // 11. Release elevatorLock;
    elevatorLock->Release();
}

void PersonThread(int person) {

    Person *p = (Person *)person;

    printf("Person %d wants to go from floor %d to %d\n", p->id, p->atFloor, p->toFloor);

    e->hailElevator(p);
}

int getNextPersonID() {
    int personID = nextPersonID;
    personIDLock->Acquire();
    nextPersonID = nextPersonID + 1;
    personIDLock->Release();
    return personID;
}


void ArrivingGoingFromTo(int atFloor, int toFloor) {


    // Create Person struct
    Person *p = new Person;
    p->id = getNextPersonID();
    p->atFloor = atFloor;
    p->toFloor = toFloor;

    // Creates Person Thread
    Thread *t = new Thread("Person " + p->id);
    t->Fork(PersonThread, (int)p);

}