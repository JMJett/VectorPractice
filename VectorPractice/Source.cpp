#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <queue>

using namespace std;

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
	int core;
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

//Following a method I found to define how the priority queue sorts user-defined objects
class CompareStarts {
public:
	bool operator()(Event event1, Event event2)
	{
		if (event2.time < event1.time) return true;
		return false;
	}
};

//Function prototypes
void readInput(std::vector<Event> & event);
void buildProcessTable(vector<Event> & event, vector<Process> & process);
void getReq(int pid);
void CpuReq(int pid);
void DiskReq(int pid);
void IOReq(int pid);
void CpuComp(int pid);
void DiskComp(int pid);
void IOComp(int pid);
void PrintTable();

//declare variables, hopefully the names are self explanatory
//Globals are bad practice, but this program is small enough in scope that they help more than hurt

int myClock = 0;
Event seed; //Used to start the event queue
Event current;
vector<Event> dataTable;
vector<Process> processTable;
queue<Event> highPriority;
queue<Event> lowPriority;
queue<Event> diskQueue;
priority_queue<Event, vector<Event>, CompareStarts> parentQueue;
Device dTable[4]; //0 = Core0, 1 = Core1, 2 = Disk, 3 = IO

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



void getReq(int pid) {
	if (processTable[pid].steps.size() == 0) {
		processTable[pid].state = "TERMINATED";
		PrintTable();
	}
	else {
		if (processTable[pid].steps[0].operation == "CORE") {
			CpuReq(pid);
		}
		else if (processTable[pid].steps[0].operation == "DISK") {
			DiskReq(pid);
		}
		else
			IOReq(pid);
	}
	
};

void StartReq(int pid) {
	processTable[pid].priority = 'h';
	getReq(pid);
};

void CpuReq(int pid) {
	if (!dTable[0].busy) {
		dTable[0].busy = true;
		dTable[0].completionTime = myClock + processTable[pid].steps[0].time;
		processTable[pid].coretimes += processTable[pid].steps[0].time;
		processTable[pid].core = 0;
		seed.operation = processTable[pid].steps[0].operation;
		seed.time = dTable[0].completionTime;
		seed.process = pid;
		parentQueue.push(seed);
		processTable[pid].steps.erase(processTable[pid].steps.begin());

	}
	else if (!dTable[1].busy) {
		dTable[1].busy = true;
		dTable[1].completionTime = myClock + processTable[pid].steps[0].time;
		processTable[pid].coretimes += processTable[pid].steps[0].time;
		processTable[pid].core = 1;
		seed.operation = processTable[pid].steps[0].operation;
		seed.time = dTable[1].completionTime;
		seed.process = pid;
		parentQueue.push(seed);
		processTable[pid].steps.erase(processTable[pid].steps.begin());
	}
	else {
		if (processTable[pid].priority == 'h') {
			seed.operation = processTable[pid].steps[0].operation;
			seed.time = processTable[pid].steps[0].time;
			seed.process = pid;
			highPriority.push(seed);
			processTable[pid].steps.erase(processTable[pid].steps.begin());

		}
		else {
			seed.operation = processTable[pid].steps[0].operation;
			seed.time = processTable[pid].steps[0].time;
			seed.process = pid;
			lowPriority.push(seed);
			processTable[pid].steps.erase(processTable[pid].steps.begin());

		}
	}
	
};

void CpuComp(int pid) {
	int core = processTable[pid].core;
	myClock = dTable[core].completionTime;
	if (highPriority.empty() && lowPriority.empty())
	{
		dTable[core].busy = false;
	}
	else if (highPriority.empty()){
		current = lowPriority.front();
		lowPriority.pop();
		dTable[core].completionTime = myClock + current.time;
		processTable[pid].coretimes += current.time;
		processTable[pid].core = core;
		current.time = dTable[core].completionTime;
		parentQueue.push(current);
	}
	else {
		current = highPriority.front();
		highPriority.pop();
		dTable[core].completionTime = myClock + current.time;
		processTable[pid].coretimes += current.time;
		processTable[pid].core = core;
		current.time = dTable[core].completionTime;
		parentQueue.push(current);
	}
	getReq(pid);
};

void DiskReq(int pid) {
	if (processTable[pid].steps[0].time == 0)
		processTable[pid].priority = 'l';
	if (!dTable[2].busy) {
		dTable[2].busy = true;
		dTable[2].completionTime = myClock + processTable[pid].steps[0].time;
		seed.operation = processTable[pid].steps[0].operation;
		seed.time = dTable[2].completionTime;
		seed.process = pid;
		parentQueue.push(seed);
		processTable[pid].steps.erase(processTable[pid].steps.begin());
	}
	else {
		seed.operation = processTable[pid].steps[0].operation;
		seed.time = processTable[pid].steps[0].time;
		seed.process = pid;
		diskQueue.push(seed);
		processTable[pid].steps.erase(processTable[pid].steps.begin());
	}
};

void DiskComp(int pid) {
	myClock = dTable[2].completionTime;
	if (diskQueue.empty()) {
		dTable[2].busy = false;
	}
	else {
		current = diskQueue.front();
		dTable[2].completionTime = myClock + current.time;
		current.time = dTable[2].completionTime;
		parentQueue.push(current);
	}
	processTable[pid].priority = 'l';
	getReq(pid);
};

void IOReq(int pid) {

};


void IOComp(int pid) {
	processTable[pid].steps.erase(processTable[pid].steps.begin());
	getReq(pid);
};

void PrintTable() {

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
		current = parentQueue.top();
		int pid = current.process;
		parentQueue.pop();
		if (current.operation == "Start") {
			myClock = current.time;
			StartReq(pid);
		}
		else if(current.operation == "CORE") {
			CpuComp(pid);
		}
		else if (current.operation == "DISK") {
			DiskComp(pid);
		}
		else
			IOComp(pid);
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

