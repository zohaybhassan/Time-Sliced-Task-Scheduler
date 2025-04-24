#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

using namespace std;

struct MLTask {
    int taskID;
    string taskType;
    int computationTime;
    int priority;
    int timesScheduled;
    MLTask* next;

    MLTask(int id, string type, int time, int prio) {
        taskID = id;
        taskType = type;
        computationTime = time;
        priority = prio;
        timesScheduled = 0;
        next = nullptr;
    }
};

class PriorityQueue {
private:
    MLTask* front;

public:
    PriorityQueue() {
        front = nullptr;
    }

    ~PriorityQueue() {
        makeNull();
    }

    bool isEmpty() {
        return front == nullptr;
    }

    void enqueue(MLTask* newTask) {
        if (isEmpty()) {
            front = newTask;
            return;
        }

        if (newTask->priority < front->priority) {
            newTask->next = front;
            front = newTask;
            return;
        }

        MLTask* current = front;
        while (current->next != nullptr && current->next->priority <= newTask->priority) {
            current = current->next;
        }

        newTask->next = current->next;
        current->next = newTask;
    }

    MLTask* dequeue() {
        if (isEmpty()) {
            cout << "Queue is empty!" << endl;
            return nullptr;
        }

        MLTask* temp = front;
        front = front->next;
        temp->next = nullptr;
        return temp;
    }

    MLTask* search(int id) {
        MLTask* current = front;

        while (current != nullptr) {
            if (current->taskID == id) {
                return current;
            }
            current = current->next;
        }

        return nullptr;
    }

    bool removeTask(int id) {
        if (isEmpty()) {
            return false;
        }

        if (front->taskID == id) {
            MLTask* temp = front;
            front = front->next;
            delete temp;
            return true;
        }

        MLTask* current = front;
        while (current->next != nullptr && current->next->taskID != id) {
            current = current->next;
        }

        if (current->next != nullptr) {
            MLTask* temp = current->next;
            current->next = temp->next;
            delete temp;
            return true;
        }

        return false;
    }

    void makeNull() {
        while (!isEmpty()) {
            MLTask* temp = dequeue();
            delete temp;
        }
    }

    void displayAll() {
        if (isEmpty()) {
            cout << "Queue is empty!" << endl;
            return;
        }

        cout << "-----------------------------------------------------------------------" << endl;
        cout << "| " << left << setw(8) << "Task ID" << "| "
            << setw(25) << "Task Type" << "| "
            << setw(15) << "Comp. Time" << "| "
            << setw(10) << "Priority" << "| "
            << setw(10) << "Times" << " |" << endl;
        cout << "-----------------------------------------------------------------------" << endl;

        MLTask* current = front;
        while (current != nullptr) {
            cout << "| " << left << setw(8) << current->taskID << "| "
                << setw(25) << current->taskType << "| "
                << setw(15) << current->computationTime << "| "
                << setw(10) << current->priority << "| "
                << setw(10) << current->timesScheduled << " |" << endl;
            current = current->next;
        }
        cout << "-----------------------------------------------------------------------" << endl;
    }
};

class TaskScheduler {
private:
    PriorityQueue taskQueue;
    const int TIME_SLICE = 3;

public:
    TaskScheduler() {}

    void addProcessesFromFile(const string& filename) {
        ifstream file(filename);

        if (!file.is_open()) {
            cout << "Error: Can't open file " << filename << endl;
            return;
        }

        int id, time, priority;
        string type;

        while (file >> id >> ws) {
            getline(file, type, ',');
            file >> time >> priority;

            MLTask* newTask = new MLTask(id, type, time, priority);
            taskQueue.enqueue(newTask);

            cout << "Added Task: " << id << ", " << type << ", " << time << ", " << priority << endl;
        }

        file.close();
    }

    void executeNextProcess() {
        if (taskQueue.isEmpty()) {
            cout << "No tasks to execute!" << endl;
            return;
        }

        MLTask* currentTask = taskQueue.dequeue();
        currentTask->timesScheduled++;

        cout << "\n---------- EXECUTING TASK ----------" << endl;
        cout << "Task ID: " << currentTask->taskID << endl;
        cout << "Task Type: " << currentTask->taskType << endl;
        cout << "Times Scheduled: " << currentTask->timesScheduled - 1 << endl;
        cout << "Remaining Time: " << currentTask->computationTime << " units" << endl;

        int timeToAllocate = TIME_SLICE;

        if (currentTask->timesScheduled >= 5) {
            int multiplier = currentTask->timesScheduled - 3;
            if (multiplier > 1) {
                timeToAllocate = TIME_SLICE * multiplier;
                cout << "Adaptive Scheduling: Using " << timeToAllocate << " units" << endl;
            }
        }

        if (currentTask->computationTime <= timeToAllocate) {
            cout << "Task completed after using " << currentTask->computationTime << " time units!" << endl;
            delete currentTask;
        }
        else {
            currentTask->computationTime -= timeToAllocate;
            cout << "Task needs " << currentTask->computationTime << " more units to complete." << endl;

            if (currentTask->priority < 6) {
                currentTask->priority++;
                cout << "Priority reduced to: " << currentTask->priority << endl;
            }

            taskQueue.enqueue(currentTask);
            cout << "Task added back to queue." << endl;
        }
    }

    void searchTask(int id) {
        MLTask* task = taskQueue.search(id);
        if (task != nullptr) {
            cout << "\nTask found:" << endl;
            cout << "ID: " << task->taskID << ", Type: " << task->taskType << endl;
            cout << "Time: " << task->computationTime << ", Priority: " << task->priority << endl;
        }
        else {
            cout << "Task " << id << " not found!" << endl;
        }
    }

    void removeTask(int id) {
        if (taskQueue.removeTask(id)) {
            cout << "Task " << id << " removed!" << endl;
        }
        else {
            cout << "Task " << id << " not found!" << endl;
        }
    }

    void clearAllTasks() {
        taskQueue.makeNull();
        cout << "All tasks cleared!" << endl;
    }

    void displayAllTasks() {
        cout << "\nTasks in Queue:" << endl;
        taskQueue.displayAll();
    }

    bool isQueueEmpty() {
        return taskQueue.isEmpty();
    }
};

int main() {
    TaskScheduler scheduler;
    int choice;
    string filename;
    int taskId;

    cout << "ML Task Scheduler" << endl;
    cout << "================" << endl;

    while (true) {
        cout << "\nMenu:" << endl;
        cout << "1. Add Tasks from File" << endl;
        cout << "2. Execute Next Task" << endl;
        cout << "3. Search Task" << endl;
        cout << "4. Remove Task" << endl;
        cout << "5. Display All Tasks" << endl;
        cout << "6. Clear All Tasks" << endl;
        cout << "7. Run Simulation" << endl;
        cout << "8. Exit" << endl;
        cout << "Choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            cout << "Enter filename: ";
            cin >> filename;
            scheduler.addProcessesFromFile(filename);
            break;

        case 2:
            scheduler.executeNextProcess();
            break;

        case 3:
            cout << "Enter Task ID: ";
            cin >> taskId;
            scheduler.searchTask(taskId);
            break;

        case 4:
            cout << "Enter Task ID: ";
            cin >> taskId;
            scheduler.removeTask(taskId);
            break;

        case 5:
            scheduler.displayAllTasks();
            break;

        case 6:
            scheduler.clearAllTasks();
            break;

        case 7:
            cout << "\nRunning simulation..." << endl;
            while (!scheduler.isQueueEmpty()) {
                scheduler.executeNextProcess();
                scheduler.displayAllTasks();
                cout << "\nPress Enter to continue...";
                cin.ignore();
                cin.get();
            }
            cout << "Simulation complete!" << endl;
            break;

        case 8:
            cout << "Exiting program!" << endl;
            return 0;

        default:
            cout << "Invalid choice!" << endl;
        }
    }

    return 0;
}


