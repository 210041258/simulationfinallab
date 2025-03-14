// event cpp

#include "event.h"
#include "server.h"
#include "scheduler.h"

void
Event :: activate (double t) {
    Scheduler &s = Scheduler :: instance ();
    stime_ = Scheduler :: now ();
    rtime_ = stime_ + t;
    s.schedule (this);
}

void
Event :: cancel () {
    Scheduler::cancel(this);
}


void
ArrivalEvent :: handle () {
    server->updateStat();
    server->arrivalHandler ();
}

void
DepartureEvent :: handle () {
    server->updateStat();
    server->departureHandler ();
}



// event h



#ifndef SORUCE_CODE_EVENT_H
#define SORUCE_CODE_EVENT_H


class Server;

class Event
{
public:
    Event (Server* s) : server (s)
    {
        stime_ = 0.0;
        rtime_ = 0.0;
    }

    inline int& eventType ()
    {
        return (eventType_);
    }
    inline double& expire ()
    {
        return (rtime_);
    }
    inline double& start ()
    {
        return (stime_);
    }

    void activate (double t);
    void cancel ();
    virtual void handle () = 0;

    Event* next_;
protected:
    Server* server;

private:
    int eventType_;	
    double stime_;
    double rtime_;
    int status_;
};

class ArrivalEvent : public Event
{
public:
    ArrivalEvent (Server* s) : Event(s) {}
    void handle ();
};

class DepartureEvent : public Event
{
public:
    DepartureEvent (Server* s) : Event(s) {}
    void handle ();
};


#endif //SORUCE_CODE_EVENT_H

 // main cpp
#include <iostream>
#include <fstream>
using namespace std;

#include "scheduler.h"
#include "serviceFacility.h"


int main ()
{
    Scheduler *sch = new Scheduler ();
    sch->initialize ();

    Server* s1 = new Server(1, 3);
    s1->createTraceFile();
    s1->arrivalMean() = 10.0;
    s1->departureMean() = 10.0;
    s1->initialize();

    Server* s2 = new Server(2, 3);
    s2->createTraceFile();
    s2->arrivalMean() = 10.0;
    s2->departureMean() = 10.0;
    s2->initialize();

    s1->initializeArrival(2);

    s1->setNext(s2, s2, s2);

    s2->setNext(nullptr, nullptr, nullptr);


    
    s1->addNextFacility(sf2, sf3, sf4);
    s1->setDeptServiceFac(deptServer);

    s2->addNextFacility(sf5, nullptr, sf5);
    s2->setDeptServiceFac(deptServer);

    sf3->addNextFacility(sf1, deptServer, deptServer);
    sf3->setDeptServiceFac(deptServer);

    sf4->addNextFacility(nullptr, sf1, sf3);
    sf4->setDeptServiceFac(deptServer);

    sf5->addNextFacility(deptServer, nullptr, sf1);
    sf5->setDeptServiceFac(deptServer);


    ServiceFacility* sf1 = new ServiceFacility(1,3,10.0,10.0);
    ServiceFacility* sf2 = new ServiceFacility(2,2,10.0,10.0);
    ServiceFacility* sf3 = new ServiceFacility(3,4,10.0,10.0);
    ServiceFacility* sf4 = new ServiceFacility(4,3,10.0,10.0);
    ServiceFacility* sf5 = new ServiceFacility(5,1,10.0,10.0);
    ServiceFacility* deptServer = new ServiceFacility(6,1,10.0,10.0);


    sch->run ();
    s1 -> report();
    s2 -> report();
    return 0;
}


// queue cpp 

#include "queue.h"

Queue :: Queue ()
{
    head_ = tail_ = 0;
    length_ = 0;
}

void
Queue :: enque (Item *im)
{
    Item* it = tail_;

    if (!tail_)
    {
        head_ = tail_ = im;
    }
    else
    {
        tail_->next_ = im;
        tail_ = im;
    }

    tail_->next_ = 0;
    ++length_;
}

Item*
Queue :: deque ()
{
    if (!head_)
        return 0;

    Item* it = head_;
    head_ = it->next_;
    if (it == tail_)
        head_ = tail_ = 0;
    --length_;

    return it;
}


// queue h

#ifndef SORUCE_CODE_QUEUE_H
#define SORUCE_CODE_QUEUE_H


typedef struct ItemType
{
    int id_;
    double itemArrivalTime;
    ItemType *next_;
} Item; 


class Queue
{
public:
    Queue ();
    inline int& length ()
    {
        return (length_);
    }
    void enque (Item *im);
    Item* deque ();

private:
    int length_;
    Item *head_;
    Item *tail_;
};


#endif //SORUCE_CODE_QUEUE_H


// scheulkar cpp

#include "scheduler.h"
#include "event.h"

#include <iostream>
#include <fstream>
using namespace std;

Event* Scheduler :: eventList_;
double Scheduler :: clock_;
Scheduler* Scheduler::instance_;

Scheduler :: Scheduler () {
    eventList_ = 0;
}

double
Scheduler :: now () {
    return clock_;
}


Scheduler&
Scheduler :: instance () {
    return (*instance_);
}

void
Scheduler :: schedule (Event* e) {
    addEvent (e);
}


void
Scheduler :: updateClock (double t) {
    clock_ = t;
}

void
Scheduler :: initialize () {
    clock_ = 0;
}

void
Scheduler :: addEvent (Event *e) {

    Event *current;
    Event *previous;

    
    if (!eventList_) {
        eventList_ = e;
        e->next_ = 0;

        return;
    }

    
    previous = eventList_;
    if (e->expire () < eventList_->expire()) {
        e->next_ = eventList_;
        eventList_ = e;

        return;
    }

   
    current = previous -> next_;
    while (current != 0) {
        if (e->expire() < current->expire ()) {
            e->next_ = current;
            previous->next_=e;
            return;
        } else {
            current = current->next_;
            previous = previous->next_;
        }
    }

    
    previous->next_ = e;
    e->next_ = 0;

    return;
}

Event*
Scheduler :: removeEvent () {
    Event* temp;

    temp = eventList_;
    eventList_ = eventList_->next_;

    return temp;
}

void
Scheduler :: cancel (Event *e) {
    Event* current;
    Event* previous;

    previous = eventList_;
    if (previous->eventType ()!= e->eventType ()) {
        eventList_ = eventList_->next_;
        return;
    }
    current = previous->next_;
    while (current->next_!=0) {
        if (current->eventType ()== e->eventType ()) {
            previous->next_ = current->next_;
            return;
        } else {
            previous = current;
            current = current->next_;
        }
    }
}

void
Scheduler :: trigger () {
    Event* temp;

    temp = removeEvent ();
    temp->handle ();
}


void
Scheduler :: run () {
    Event * temp;

    while (eventList_ != 0) {
        temp = removeEvent ();

        updateClock (temp->expire ());

        temp->handle ();
    }
}



// scehualr .h



#ifndef SORUCE_CODE_SCHEDULER_H
#define SORUCE_CODE_SCHEDULER_H

#include "event.h"

class Scheduler
{
public:
    Scheduler ();
    void trigger ();
    void run ();
    void initialize ();

    //protected:
    static double now ();
    static Scheduler& instance ();
    void schedule (Event *e);
    void cancel (Event *e);

private:
    void addEvent (Event *e);
    Event* removeEvent ();
    void updateClock (double t);

public:
    static double clock_;
    static Event* eventList_;
    static Scheduler* instance_;
};



#endif //SORUCE_CODE_SCHEDULER_H


// server cpp

#include "server.h"
#include "scheduler.h"

#include <malloc.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
using namespace std;

Server :: Server (int serverId, int stationCount) : a_(this), d_(this)
{
    queue_ = new Queue();
    serverId_ = serverId;
    stationCount_ = stationCount;
}

double
Server :: exponential(double mean) {

    double r = (double)rand()/(RAND_MAX + 1.0);
    double ex = -log (r)/mean;
    return ex;
}

void
Server :: initialize () {
    status_ = 0;
    itemArrived_ = 0;
    timeLastEvent = 0.0;

    areaQueue_ = 0.0;
    areaServer_ = 0.0;
    areaSystem_ = 0.0;

    totalQueueDelay_ = 0.0;
    totalSystemDelay_ = 0.0;
    totalCustomerServed = 0;
}

void Server:: initializeArrival(int jobId){
    currentJobId = jobId;
    double t = exponential (arrivalMean_);
    trace_ << "interarrival time " << t << endl;
    a_.activate (t);
}

void Server:: initializeDeparture(){
    double t = exponential (departureMean_);
    trace_ << "\tservice time = " << t << endl;
    d_.activate (t);
    if((!nextServer.empty()) && (nextServer[currentJobId-1] != nullptr)){
        nextServer[currentJobId-1] ->initializeArrival(currentJobId);
    }
    if ((itemArrived_ < 1000) && (serverId_ == 1)) {
        cout<<itemArrived_<<endl;
        initializeArrival(2);
    }
}

void
Server :: createTraceFile () {
    string traceName = "trace_" + to_string(serverId_) + ".out";
    trace_.open (traceName, ios::out);
    if (!trace_) {
        cout << "cannot open the trace file.\n";
    }
    trace_<< "trace file for the simulation" << endl;
    trace_ << "format of the file" << endl;
    trace_ << "<event> <time> <item id> <server status> <queue size>" << endl << endl;
}

void
Server :: arrivalHandler () {
    Item* temp;
    itemArrived_++;
    temp = (Item*) malloc (sizeof(Item));
    temp->id_ = itemArrived_;
    temp->itemArrivalTime = Scheduler::now();

    trace_ << "a\t" << Scheduler::now () << "\t" << temp->id_ << "\t" << status_ << "\t" << queue_->length() << endl;

    if (status () < stationCount_) {

        status() += 1;
        trace_ << "s\t" << Scheduler::now () << "\t" << temp->id_ << "\t" << status_ << "\t" << queue_->length() << endl;
        itemInService_ = temp;
        queueDelay_ = Scheduler::now() - itemInService_->itemArrivalTime;
        totalQueueDelay_ += queueDelay_;

        initializeDeparture();
    }
    else {
        queue_->enque(temp);
    }
}

void
Server :: departureHandler () {
    status() -= 1;
    if (queue_->length() > 0) {
        trace_ << "d\t" << Scheduler::now () << "\t" << itemInService_->id_ << "\t" << status_ << "\t" << queue_->length() << endl;
    } else {
        trace_ << "d\t" << Scheduler::now () << "\t" << itemInService_->id_ << "\t" << 0 << "\t" << queue_->length() << endl;
    }
    totalCustomerServed++;
    systemDelay_ = Scheduler::now() - itemInService_->itemArrivalTime;
    totalSystemDelay_ += systemDelay_;

    if (queue_->length() > 0) {
        itemInService_ = queue_->deque ();
        queueDelay_ = Scheduler::now() - itemInService_->itemArrivalTime;
        totalQueueDelay_ += queueDelay_;
        trace_ << "s\t" << Scheduler::now () << "\t" << itemInService_->id_ << "\t" << status_ << "\t" << queue_->length() << endl;

        initializeDeparture();
    }
    else {
        status () = 0;
        itemInService_ = nullptr;
    }
}

void Server::updateStat()
{
    double durationSinceLastEvent;

    durationSinceLastEvent = Scheduler::now() - timeLastEvent;
    timeLastEvent = Scheduler::now();

    areaQueue() += durationSinceLastEvent*(queue_->length());
    areaServer() += durationSinceLastEvent*status();
    areaSystem() += durationSinceLastEvent*(queue_->length() + status());
}

void Server::report()
{
    ofstream report_;
    string reportName = "report_" + to_string(serverId_) + ".out";
    report_.open (reportName, ios::out);
    if (!report_) {
        cout << "cannot open the report file.\n";
    }
    report_<< "Report of the simulation" << endl;
    report_<<"Traffic Intensity: "<<arrivalMean()/departureMean()<<endl;
    report_<<"Average Number of Customers in the Queue : "<<(areaQueue()/(Scheduler::now()))<<endl;
    report_<<"Average Server Utilization: "<<(areaServer()/(Scheduler::now()))/stationCount_<<endl;
    report_<<"Average Number of Customers in the System: "<<(areaSystem()/(Scheduler::now()))<<endl;
    report_<<"Average Queueing Delay: "<<(totalQueueDelay()/totalCustomerServed)<<endl;
    report_<<"Average System Delay: "<<(totalSystemDelay()/totalCustomerServed)<<endl;
}

void Server::setNext(Server *server1, Server* server2, Server* server3) {
    nextServer.push_back(server1);
    nextServer.push_back(server2);
    nextServer.push_back(server3);
}


// service facility cpp

#include "serviceFacility.h"

ServiceFacility::ServiceFacility(int serviceFacilityId, int serverCount, double arrivalMean, double departureMean):Server(1, 0, nullptr) {

    this->createTraceFile();
    this->arrivalMean() = arrivalMean;
    this->departureMean() = departureMean;
    this->initialize();
    serviceFacilityId_ = serviceFacilityId;
    q = new Queue ();
    if(serverCount >= 1){
        s1 = serverInitialize(1, arrivalMean, departureMean);
        if(serverCount>=2){
            s2 = serverInitialize(2, arrivalMean, departureMean);
            s1->setNext(s2);
            if(serverCount>=3){
                s3 = serverInitialize(3, arrivalMean, departureMean);
                s2->setNext(s3);
                if(serverCount>=4){
                    s4 = serverInitialize(4, arrivalMean, departureMean);
                    s3->setNext(s4);
                    s4->setNext(nullptr);
                }
                else{
                    s3->setNext(nullptr);
                    s4 = nullptr;
                }
            }
            else{
                s2->setNext(nullptr);
                s3 = nullptr;
                s4 = nullptr;
            }
        }
        else{
            s1->setNext(nullptr);
            s2 = nullptr;
            s3 = nullptr;
            s4 = nullptr;
        }
    }
}

void ServiceFacility::serviceFacilityArrival(int jobId){
    s1->initializeArrival();
    if(!nextFacility.empty()){
        if(nextFacility[jobId-1] == deptServiceFac_){
            //process departure i.e. update output statistics
        }
        else if(nextFacility[jobId-1] != nullptr){
            nextFacility[jobId-1]->serviceFacilityArrival(jobId);
        }
    }
};

Server* ServiceFacility::serverInitialize(int id, double arrivalMean, double departureMean) {
    Server* server = new Server(serviceFacilityId_, id, q);
    server->createTraceFile ();
    server->arrivalMean() = arrivalMean;
    server->departureMean() = departureMean;
    server->initialize();
}

void ServiceFacility::generateReport() {
    s1->report();
    if(s2 != nullptr) s2->report();
    if(s3 != nullptr) s3->report();
    if(s4 != nullptr) s4->report();
}

void ServiceFacility::addNextFacility(ServiceFacility *s1, ServiceFacility *s2, ServiceFacility *s3) {
    nextFacility.push_back(s1);
    nextFacility.push_back(s2);
    nextFacility.push_back(s3);
}

void ServiceFacility::setDeptServiceFac(ServiceFacility *server) {
    deptServiceFac_ = server;
}



// servicefacility h


#ifndef SORUCE_CODE_SERVICEFACILITY_H
#define SORUCE_CODE_SERVICEFACILITY_H

#include "server.h"
#include <bits/stdc++.h>

class ServiceFacility: public Server {
private:
    int serviceFacilityId_;
    Queue* q;
    Server* s1;
    Server* s2;
    Server* s3;
    Server* s4;
    ServiceFacility* deptServiceFac_;
    vector<ServiceFacility*> nextFacility; //next facility vector for job-1,2,3
    Server* serverInitialize(int id, double arrivalMean, double departureMean);
public:
    ServiceFacility(int serviceFacilityId, int serverCount, double arrivalMean, double departureMean);

    void serviceFacilityArrival(int jobId);
    void addNextFacility(ServiceFacility *s1, ServiceFacility *s2, ServiceFacility *s3);
    void setDeptServiceFac(ServiceFacility* server);
    void generateReport();
};


#endif //SORUCE_CODE_SERVICEFACILITY_H





