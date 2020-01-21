/*
 *  *****   main.cpp   *****
 *
 *  Created on: April 28, 2019
 *      Author: Vagner Machado
 *      Class : Algorithms
 *   Professor: Phillips
 *
 *   Queens College - Spring 2019
 */
#include <cstddef>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

class Node
{
public:
	int jobID;
	int jobTime;
	int dependentCount;
	Node * next;
	Node(int jobID, int jobTime, int dependentCount);
	Node();
	string printNode();
};

class LinkedList
{
public:
	Node * listHead;
	LinkedList();
	bool isEmpty();
	Node * removeHead();
	void insert(Node * x);
	string printLinkedList();
};

class Jobs
{
public:
	int jobTime;
	int onWhichProc;
	int onOpen;
	int parentCount;
	int dependentCount;
	Jobs(int owp, int open);
	Jobs();
	void printJob(int job);
};

class Proc
{
public:
	int doWhichJob;
	int timeRemain;
	Proc(int dwj, int tr);
	Proc();
	void printProc(int proc);
};


class Scheduling
{
public:
	int currentTime;
	int numNodes;
	int numProcs;
	int procUsed;
	Jobs * jobArray;
	Proc * procArray;
	LinkedList open;
	int ** adjMatrix;
	int * parentCountArray;
	int * dependentCountArray;
	int * onGraphArray;
	int totalJobTimes;
	int ** scheduleTable;
	void initialization(ifstream &  inOne, ifstream & inTwo);
	void loadMatrix(ifstream & inOne);
	int constructJobArray(ifstream & inTwo, int ** matrix);
	void computeParentCount (int ** matrix, int * parentCountAry);
	void computeDependentCount (int ** matrix, int * dependentCountAry);
	void loadOpen();
	int findOrphan();
	void openInsert(Node * n);
	void printList(ofstream & outTwo);
	void printScheduleTable(ofstream & outOne);
	int findProcessor();
	void putJobOnTable(int availProc, int currTime, int joID, int joTime);
	bool checkCycle();
	bool isGraphEmpty();
	void updateProcTime();
	int findDoneProc();
	void deleteEdge(int joID);
	void loadProcArray();

	/** AUX METHODS **/
	void printJobArray();
	void printProcArray();
	void printAdjMatrix();
	void printScheduleTable();
	void printParentCount();
	void printDependentCount();
	void printOnGraphArray();
};

int main (int argc, char ** argv)
{
	cout << "\n\n*** Vagner's Job Scheduler ***\n\n";
	ifstream inFile1;    // graph
	ifstream inFile2;    // times
	ofstream outFile1;   //table
	ofstream outFile2;   //debug
	Scheduling schedule;
	inFile1.open(argv[1]);
	inFile2.open(argv[2]);
	outFile1.open(argv[3]);
	outFile2.open(argv[4]);
	schedule.initialization(inFile1, inFile2);
	schedule.procUsed = 0;
	schedule.currentTime = 1;
	while(!schedule.isGraphEmpty())
	{
		schedule.loadOpen();
		schedule.printList(outFile2);
		schedule.loadProcArray();
		if(schedule.checkCycle())
		{
			string warn =  "\n *** ERROR: Input invalid, the graph has a cycle ***\n";
			cout << warn;
			outFile1 << warn;
			outFile2 << warn;
			inFile1.close();
			inFile2.close();
			outFile1.close();
			outFile2.close();
			return 1;
		}
		schedule.printScheduleTable(outFile1);
		schedule.currentTime++;
		schedule. updateProcTime();
		int jobID = schedule.findDoneProc();
		while (jobID > 0)
		{
			schedule.onGraphArray[jobID] = 0;
			schedule.deleteEdge(jobID);
			jobID = schedule.findDoneProc();
		}
	}
	schedule.printScheduleTable(outFile1);
	inFile1.close();
	inFile2.close();
	outFile1.close();
	outFile2.close();
	cout << "\n\n*** Job Scheduling Finished ***\n\n";
	return 0;
}

/**** NODE METHODS ****/

Node::Node(int id, int time, int dependent)
{
	jobID = id;
	jobTime = time;
	dependentCount = dependent;
	next = NULL;
}

Node::Node()
{
	jobID = -1;
	jobTime = -1;
	dependentCount = -1;
	next = NULL;
}

string Node::printNode()
{
	stringstream result;
	result << "\n     Job ID: " <<  jobID;
	result << "\n   Job Time: " << jobTime;
	result << "\n Dependents: " << dependentCount;
	return result.str();
}

/**** LINKED LIST METHODS ****/

LinkedList::LinkedList()
{
	listHead = NULL;
}

bool LinkedList::isEmpty()
{
	return listHead == NULL;
}

Node * LinkedList::removeHead()
{
	Node * temp = listHead;
	listHead = listHead->next;
	return temp;
}

void LinkedList::insert(Node * x)
{
	if (listHead == NULL) //first
	{
		listHead = x;
		return;
	}
	else if (listHead->dependentCount <= x->dependentCount) //start
	{
		x->next = listHead;
		listHead = x;
		return;
	}
	else
	{
		Node * trav = listHead;
		while (x->dependentCount < trav->dependentCount)
		{
			if(trav->next == NULL)   //end
			{
				trav->next = x;
				x->next = NULL;
				return;
			}
			else if (trav->next->dependentCount <= x->dependentCount) //middle
			{
				x->next = trav->next;
				trav->next = x;
				return;
			}
			trav = trav->next;
		}
	}
}

string LinkedList::printLinkedList()
{
	stringstream result;
	Node * trav = listHead;
	while (trav != NULL)
	{
		result << trav->printNode() << "\n";
		trav = trav->next;
	}
	return result.str();
}

/**** JOBS METHODS ****/

Jobs::Jobs(int owp, int open)
{
	onWhichProc = owp;
	onOpen = open;
}

Jobs::Jobs()
{
	jobTime = 0,
			onWhichProc = 0;
	onOpen = 0;
	parentCount = 0;
	dependentCount = 0;
}

void Jobs::printJob(int job)
{
	cout << "\n\n*** JOB NUMBER " << job
			<< " *** \n      Job Time: " << jobTime
			<< "\n   On Processor: " << onWhichProc
			<< "\n        On Open: " << onOpen
			<< "\n   Parent Count: " << parentCount
			<< "\nDependent Count: " << dependentCount;
}


/**** PROC METHODS ****/

Proc::Proc(int dwj, int tr)
{
	doWhichJob = dwj;
	timeRemain = tr;
}

Proc::Proc()
{
	doWhichJob = -1;
	timeRemain = -1;
}

void Proc::printProc(int proc)
{
	cout << "\n\n *** PROCESSOR NUMBER " << proc << "***"
			<< "\n     Doing Job: " << doWhichJob
			<< "\nTime Remaining: " << timeRemain;
}

/**** SCHEDULING METHODS ****/

void Scheduling::initialization(ifstream &  inOne, ifstream & inTwo)
{
	open = LinkedList();
	numProcs = -1;
	inOne >> numNodes;
	cout << " \n*** How many processors are being used? ***\n";
	while (numProcs <= 0)
	{
		cin >> numProcs;
		if (numProcs <= 0)
			cout << "\n*** ERROR: We need one or more processors ***\n\nEnter number of processors:  ";
		else
			if (numProcs > numNodes)
			{
				numProcs = numNodes;   //unlimited
				cout << "\n *** Using unlimited processors ***\n";
			}
			else
				cout << "\n *** Using " << numProcs << " processors ***\n";
	}

	//adjacent matrix
	adjMatrix = new int * [numNodes + 1];
	for(int i = 1; i < numNodes + 1; i++)
		adjMatrix[i] = new int [numNodes + 1];

	for(int i = 1; i < numNodes + 1; i++)
		for(int j = 1; j < numNodes + 1; j++)
			adjMatrix[i][j] = 0;

	//on graph array
	onGraphArray = new int[numNodes + 1];
	for(int j = 1; j < numNodes + 1; j++)
		onGraphArray[j] = 1;

	// proc array
	procArray = new Proc [numProcs + 1];
	for (int i = 0; i < numProcs + 1; i++)
		procArray[i] = Proc();

	// job array, parent count and dependent array
	jobArray = new Jobs[numNodes + 1];
	parentCountArray =  new int [numNodes + 1];
	dependentCountArray = new int [numNodes + 1];
	for (int i = 1; i < numNodes + 1; i++)
	{
		jobArray[i] = Jobs();
		parentCountArray[i] =  0;
		dependentCountArray[i] = 0;
	}

	loadMatrix(inOne);
	computeParentCount(adjMatrix, parentCountArray);
	computeDependentCount(adjMatrix, dependentCountArray);
	totalJobTimes = constructJobArray(inTwo, adjMatrix);

	//schedule table
	scheduleTable = new int * [numProcs + 1];
	for(int i = 1; i < numProcs + 1; i++)
		scheduleTable[i] = new int[totalJobTimes + 1];

	for(int i = 1; i < numProcs + 1; i++)
		for(int j = 1; j < totalJobTimes + 1; j++)
			scheduleTable[i][j] = 0;



}

void Scheduling::printJobArray()
{
	//cout << "\n\n***   Job Array   ***\n";
	for (int i = 1; i < numNodes + 1; i++)
		jobArray[i].printJob(i);
}

void Scheduling::printProcArray()
{
	//cout << "\n\n***   Processor Array   ***\n";
	for (int i = 1; i < numProcs + 1; i++)
		procArray[i].printProc(i);
}

void Scheduling::printAdjMatrix()
{
	//cout << "\n\n***   Adjacent Matrix   ***\n";
	for(int i = 1; i < numNodes + 1; i++)
	{
		for(int j = 1; j < numNodes + 1; j++)
		{
			cout << adjMatrix[i][j] << "   ";
		}
		cout << "\n";
	}
}

void Scheduling::printParentCount()
{
	//cout << "\n\n*** Parent Count ***\n";
	for(int j = 1; j < numNodes + 1; j++)
	{
		cout << j << ": " << parentCountArray[j] << "\t";
	}
}

void Scheduling::printDependentCount()
{
	//cout << "\n\n*** Dependent Count ***\n";
	for(int j = 1; j < numNodes + 1; j++)
	{
		cout << j << ": " << dependentCountArray[j] << "\t";
	}
}

void Scheduling::printOnGraphArray()
{
	//cout << "\n\n*** On Graph Array ***\n";
	for(int j = 1; j < numNodes + 1; j++)
	{
		cout << j << ": " << onGraphArray[j] << "\t";
	}
}

void Scheduling::printScheduleTable()
{
	//cout << "\n***   Schedule Table   ***\n";
	for(int i = 1; i < numProcs + 1; i++)
	{
		for(int j = 1; j < totalJobTimes + 1; j++)
		{
			cout << scheduleTable[i][j] << "   ";
		}
		cout << "\n";
	}
}
void Scheduling::loadMatrix(ifstream & inOne)
{
	int row, col;
	row = col = 0;
	while (inOne >> row)
	{
		inOne >> col;
		adjMatrix[row][col] = 1;
	}
}
int Scheduling::constructJobArray(ifstream & inTwo, int ** matrix)
{
	totalJobTimes = 0;
	int nodeNumber;
	inTwo >> nodeNumber;
	int nodeID;
	int jobTime;
	while (inTwo >> nodeID)
	{
		inTwo >> jobTime;
		totalJobTimes += jobTime;
		jobArray[nodeID].jobTime = jobTime;
		jobArray[nodeID].onWhichProc = -1;
		jobArray[nodeID].onOpen = 0;
		jobArray[nodeID].parentCount = parentCountArray[nodeID];
		jobArray[nodeID].dependentCount = dependentCountArray[nodeID];
	}
	return totalJobTimes;
}
void Scheduling::computeParentCount (int ** matrix, int * parentCountAry)
{
	for(int i = 1; i < numNodes + 1; i++)
		for(int j = 1; j < numNodes + 1; j++)
			parentCountArray[i] += adjMatrix[j][i];
}
void Scheduling::computeDependentCount (int ** matrix, int * dependentCountAry)
{
	for(int i = 1; i < numNodes + 1; i++)
		for(int j = 1; j < numNodes + 1; j++)
			dependentCountArray[i] += adjMatrix[i][j];
}

void Scheduling::loadOpen()
{
	int orphanNode;
	orphanNode = findOrphan();
	if (orphanNode != -1)
		while (orphanNode != -1 )
		{
			if(orphanNode > 0)
			{
				int jobID = orphanNode;
				int jobTime = jobArray[jobID].jobTime;
				Node * newNode = new Node(jobID, jobTime, dependentCountArray[jobID]);
				openInsert(newNode);
				jobArray[jobID].onOpen = 1;
			}
			orphanNode = findOrphan();
		}
}
int Scheduling::findOrphan()
{
	for (int i = 1; i < numNodes + 1; i++)
	{
		if( parentCountArray[i] <= 0 && jobArray[i].onOpen == 0 && jobArray[i].onWhichProc == -1)
			return i;
	}
	return -1;
}

void Scheduling::openInsert(Node * n)
{
	open.insert(n);
}

void Scheduling::printList(ofstream & outTwo)
{
	outTwo << "\n***************************************";
	outTwo << open.printLinkedList();;
	outTwo << "\n***************************************";

}

void Scheduling::loadProcArray()
{
	int availProc = findProcessor();
	while (availProc > 0 && !open.isEmpty() && procUsed < numProcs)
	{
		procUsed++;
		Node * newJob = open.removeHead();
		int jobID = newJob->jobID;
		int jobTime = newJob->jobTime;
		procArray[availProc].doWhichJob = jobID;
		procArray[availProc].timeRemain = jobTime;
		putJobOnTable(availProc, currentTime, jobID, jobTime);
		availProc = findProcessor();
	}
}

int Scheduling::findProcessor()
{
	for(int i = 1; i < numProcs + 1; i++)
		if(procArray[i].timeRemain <= 0)
			return i;
	return -1;
}

void Scheduling::putJobOnTable(int availProc, int currTime, int joID, int joTime)
{
	int time = currTime;
	int endTime = joTime + currTime;
	while(time < endTime)
	{
		scheduleTable[availProc][time] = joID;
		time++;
	}
}

void Scheduling::printScheduleTable(ofstream & outOne)
{
	outOne << "\n\n\t\t";
	for(int i = 1; i < totalJobTimes + 1; i++)
		if(i < 10)
			outOne << i << " -- ";
		else
			outOne << i << " - ";
	for(int i = 1; i < numProcs + 1; i++)
	{
		if (i < 10)
		outOne << "\nP(" << i << ")  | ";
		else
			outOne << "\nP(" << i << ") | ";

		for(int j = 1; j < totalJobTimes + 1; j++)
		{
			if (scheduleTable[i][j] == 0)
				outOne << '-';
			else
				outOne << scheduleTable[i][j];

			if(scheduleTable[i][j] < 10)
				outOne << "  | ";
			else
				outOne << " | ";
		}

		outOne << "\n";//------------------------------------------------";
		for(int i = 0; i < totalJobTimes * 5 + 7; i++)
			outOne << "-";
	}
}

bool Scheduling::checkCycle()
{
	if(open.isEmpty() && !isGraphEmpty())
	{
		for(int i = 1; i < numProcs + 1; i++)
			if (procArray[i].timeRemain > 0)
				return false;
		return true;
	}
	return false;
}

bool Scheduling::isGraphEmpty()
{
	for(int j = 1; j < numNodes + 1; j++)
	{
		if(onGraphArray[j] == 1)
		{
			return false;
		}
	}

	return true;
}

void Scheduling::updateProcTime()
{
	for(int i = 1; i < numProcs + 1; i++)
	{
		if (procArray[i].timeRemain > 0)
		{
			procArray[i].timeRemain--;
			if(procArray[i].timeRemain == 0)
				procUsed--;
		}
	}
}

int Scheduling::findDoneProc()
{
	for(int i = 1; i < numProcs + 1; i++)
	{
		if(procArray[i].timeRemain <= 0 && procArray[i].doWhichJob != -1)
		{
			int jobID = procArray[i].doWhichJob;
			procArray[i].doWhichJob = -1;
			return jobID;
		}
	}
	return -1;
}
void Scheduling::deleteEdge(int joID)
{

	for(int i = 1; i < numNodes + 1; i++)
	{
		if(adjMatrix[joID][i] > 0)
		{
			parentCountArray[i]--;
			jobArray[i].parentCount--;
		}
	}
}
