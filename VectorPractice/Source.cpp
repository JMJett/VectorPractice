#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <queue>

using namespace std;

//declare variables, hopefully the names are self explanatory
//Globals are bad practice, but this program is small enough in scope that they help more than hurt

int myClock = 0;
Event seed; //Used to start the event queue
vector<Event> dataTable;
vector<Process> processTable;
queue<Event> highPriority;
queue<Event> lowPriority;
priority_queue<Event, vector<Event>, CompareStarts> parentQueue;



//Defines an event as, for example, operation = CORE, time = 800, process 0
class Event {
public:
	string operation;
	int time;
	int process;
	Event();
};

//default constructor for testing
Event::Event() {
	operation = "test";
	time = 9;
	process = 900;
}

//defining the process table
class Process {
public:
	int startTime;
	int firstLine;
	int lastLine;
	int currentLine;
	int coretimes;
	int busytime;
	char priority;
	vector<Event> steps;
	string state;
	string IOComp;
};

//define the device table
class Device {
public:
	bool busy;
	int process;
	int completionTime;
};

//Use input redirection to read the input file into a table of events
void readInput(std::vector<Event> & event) {
	int i = 0;
	Event line;
	while(cin)
	{
		cin >> line.operation;
		if (line.operation != "END") {
			cin.sync();
			cin >> line.time;
		}
		else
			line.time = 0;
		event.push_back(line);
		i++;
	}
	event.pop_back();
}

//Take the data table built from the input and turn it into a list of the different processes
void buildProcessTable(vector<Event> & event, vector<Process> & process) {
	Process line;
	Event step;
	int j = 0;
	for (int i = 0; i < event.size(); i++) {
		if (event[i].operation == "START") {
			line.startTime = event[i].time;
			line.firstLine = i;
		}
		if (event[i].operation == "END") {
			line.lastLine = i;
			process.push_back(line);
		}
	}
	
	//Give each process a sublist of its computation steps, skipping the "START" and "END" lines
	for (int i = 0; i < process.size(); i++) {
		for (int j = process[i].firstLine + 1; j < process[i].lastLine; j++) {
			step.operation = event[j].operation;
			step.time = event[j].time;
			process[i].steps.push_back(step);
		}
	}
}

//Following a template I found to define how the priority queue sorts user-defined objects
class CompareStarts {
public:
	bool operator()(Event event1, Event event2)
	{
		if (event2.time < event1.time) return true;
		return false;
	}
};

void getReq(int pid) {
	if (processTable[pid].steps.size() == empty) {
		processTable[pid].state = "TERMINATED";
	}
	else {
		if (processTable[pid].steps.begin().operation == "CORE") {
			CpuReq(pid);
		}
		else if (processTable[pid].steps.begin().operation == "DISK") {
			DiskReq(pid);
		}
		else
			IOReq(pid);
	}
	processTable[pid].steps.erase(processTable[pid].steps.begin());
};

void StartReq(int pid) {
	getReq(pid);
};

void CpuReq(int pid) {

};

void DiskReq(int pid) {

};

void IOReq(int pid) {

};


int main() {
	cout << "Hello, World!" << endl;
	
	

	//build initial tables
	readInput(dataTable);
	buildProcessTable(dataTable, processTable);

	//seed event queue with the start of each process
	for (int i = 0; i < processTable.size(); i++) {
		seed.operation = "Start";
		seed.time = processTable[i].startTime;
		seed.process = i;
		parentQueue.push(seed);
	}
	
	while (!parentQueue.empty()) {
		Event w = parentQueue.top();
		cout << w.operation << " " << w.time << " Process #: " << w.process << endl;
		parentQueue.pop();
	}

	/*while (!parentQueue.empty()) {
		Event current = parentQueue.top();
		if (current.operation == "Start") {
			myClock = current.time;
			StartReq(current.process)
		}
	}*/


	cout << endl << endl << endl;

	for (int i = 0; i < processTable.size(); i++) {
		cout << processTable[i].startTime << " " << processTable[i].firstLine << " " << processTable[i].lastLine << endl;
	}
	for (int i = 0; i < dataTable.size(); i++) {
		cout << dataTable[i].operation << " " << dataTable[i].time << endl;
	}
	cout << " " << endl << endl;

	for (int i = 0; i < processTable[0].steps.size(); i++) {
		cout << processTable[0].steps[i].operation << " " << processTable[0].steps[i].time << endl;
	}
	system ("PAUSE");
	return 0;
}

