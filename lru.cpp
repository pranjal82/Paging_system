#include <iostream>
#include <vector>
#include <unordered_map>
#include <list>
#include <queue>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <ctime>
#include <bits/stdc++.h>

using namespace std;



struct Process {
    int pid;
    int size;  // in KB
    vector<pair<int,pair<int,bool>>>PageTable;
    
};

class PagingSystem {
private:
    int M, V, P;  // Main memory, Virtual memory, and Page size
    vector<pair<int,int>>physicalMemory;
    vector<pair<int,int>> virtualMemory;
    map<int, Process> processMap;
    int global_pid;
    map<int,string>fileName;
    int free_phy=0,free_vir=0;
    map<int,int>memory;
  
    map<string,int>name_pid;

    

    // For LRU
    list<int>lru;

    // For FIFO
    queue<int> fifoQueue;

public:

    PagingSystem(int mainMemSize, int virtMemSize, int pageSize);
    void loadProcess(const vector<string>& filename);
    void runProcess(int pid);
    void killProcess(int pid);
    void printPageTable(int pid, const string& filename);
    void listProcesses();
    void printMemory(int memloc, int length);
   
    bool isValidAddress(int pid, int addr);
    
    int move_pg_phy_mem(int pid, int logicalPage);
    void exitSystem();
    void printAllPageTables(const string& filename);
    int fillPM();
    int fillVM();
};

PagingSystem::PagingSystem(int mainMemSize, int virtMemSize, int pageSize)
    : M(mainMemSize), V(virtMemSize), P(pageSize), global_pid(1) {
   physicalMemory.resize(M / P, {-1,-1});  // Initialize pages in physical memory
    virtualMemory.resize(V / P, {-1,-1});   // Initialize pages in virtual memory
   cerr<<"M= "<<M<<" V= "<<V<<" P= "<<P<<" done\n";
   free_phy=M/P;
   free_vir=V/P;
   cerr<<"Initial free pages : "<<"phy mem = "<<free_phy<<" Vir mem = "<<free_vir<<endl;
}

// Function to load the process into memory
void PagingSystem::loadProcess(const vector<string>& filenames) {
    for (const auto& filename : filenames) {
    ifstream infile(filename);
    if (!infile.is_open()) {
        cout << filename << " could not be loaded - file does not exist\n";
        continue;
    }

    Process newProcess;
    newProcess.pid = global_pid;
    infile >> newProcess.size;
   
    //pid_name[newProcess.pid]=filename;

    int numPages = (newProcess.size * 1024) / P;
    bool memoryFull = false;
    cerr<<"Num Pages= "<<numPages<<endl;
    newProcess.PageTable.resize(numPages);

    //  int availablePhysicalPages = (M / P) - physicalMemory.size();
    //     int availableVirtualPages = (V / P) - virtualMemory.size();
      cerr<<" free pages : "<<"phy mem = "<<free_phy<<" Vir mem = "<<free_vir<<endl;
        
        if (numPages > (free_phy + free_vir)) {
              cerr << "Memory is full, cannot load process " << filename << "\n";
                    memoryFull = true;
                    continue;
        }

        else {
            for (int i = 0; i < numPages; ++i) {
       // PageTableEntry entry = {i, -1, false};  // Initialize page table entry
         newProcess.PageTable[i]={-1,{-1,false}};
         pair<int,pair<int,bool>>p=newProcess.PageTable[i];
        
        if (free_phy <=0) {
                // if (virtualMemory.size() >= V / P) {
                //     cerr << "Memory is full, cannot load process " << filename << "\n";
                //     memoryFull = true;
                //     break;
                // }
                // else {
                  //  entry.physicalPage = virtualMemory.size();  // Add to swap memory
                  //  virtualMemory.push_back(0);  // Initialize in swap memory with zero
                    p.second.first=fillVM();
                    virtualMemory[p.second.first]={newProcess.pid,i};
                    // newProcess.pageTable.push_back(entry);
                    newProcess.PageTable[i]=p;
                    free_vir--;
                    cerr << "Process "<<filename<<" page " << i << " loaded into swap memory at vpn "<<p.second.first<<endl;
               // }
        }
        else {
                //cout<<"Size b4 inserting "<<physicalMemory.size()<<endl;
              //  entry.physicalPage = physicalMemory.size();  // Add to physical memory
              p.first=fillPM();
              physicalMemory[p.first]={newProcess.pid,i};
              p.second.second=true;
              free_phy--;
             //   physicalMemory[entry.physicalPage]=0;  // Initialize in physical memory with zero
               // cout<<"Size after inserting "<<physicalMemory.size()<<endl;
              //  entry.isInMemory=true;

             //   accessPageLRU(entry.physicalPage);
           //   fifoQueue.push(p.first);
              lru.push_back(p.first);
              newProcess.PageTable[i]=p;
              cerr<<"Inserting process "<<newProcess.pid<<" page "<<i<<" at ppn "<<p.first<<endl;
              cerr<<"Check ----->  "<<physicalMemory[p.first].first<<" "<<physicalMemory[p.first].second<<endl;
                
            }
            cerr<<"After page insertion "<<"Free phy_pages= "<<free_phy<<" free vp_pages = "<<free_vir<<endl;
    }

      if (!memoryFull) {
        cerr<<"Check mem full\n";
            processMap[newProcess.pid] = newProcess;  
            cout << filename << " is loaded and is assigned process id " << newProcess.pid << "\n";  
             // Only increment the PID if the process loaded successfully 
             fileName[newProcess.pid]=filename;  
              name_pid[filename]=newProcess.pid;
        }
         global_pid++;
        }


   

    }
    cerr<<"Printing Phy Mem\n";
    for(auto it1:physicalMemory){
        cerr<<it1.first<<" "<<it1.second<<endl;
    }
}

int PagingSystem::fillPM(){
    for(int i=0;i<physicalMemory.size();i++){
        if(physicalMemory[i].first==-1 && physicalMemory[i].second==-1)return i;
    }
    cerr<<"NO FREE PAGE IN PM\n";
    return -1;
}

int PagingSystem::fillVM(){
    for(int i=0;i<virtualMemory.size();i++){
        if(virtualMemory[i].first==-1 && virtualMemory[i].second==-1)return i;
    }
    cerr<<"NO FREE PAGE IN PM\n";
    return -1;
}

int PagingSystem::move_pg_phy_mem(int pid, int pageno) {

    if(processMap.find(pid)==processMap.end()){
        cout<<"Process pid "<<pid<<" not in process map how can I move page to phy memory\n";
        return -1;
    }
    cerr<<"pid= "<<pid<<" process page = "<<pageno<<endl;
   
    Process& process = processMap[pid];
    pair<int,pair<int,bool>>&p= process.PageTable[pageno];

    // cerr<<"PHY MEM CHECK !!\n";
    // for(auto it:physicalMemory){
    //     cerr<<it.first<<" "<<it.second<<endl;
    // }
    
    // cout<<"Process pid = "<<pid<<" proces pageno = "<<pageno<<endl;
    // cout<<p.first<<" "<<p.second.first<<" "<<p.second.second<<endl;

    if (p.second.second) return p.first;  // Already in physical memory
     
  

    // Swap a page out if necessary
    // if (physicalMemory.size() >= M / P) {
    //     replacePageLRU(logicalPage);  // Or use FIFO
    //     //replacePageFIFO(logicalPage);
    // }
 cerr<<"Moving page to Physical memory "<<"pid= "<<pid<<" pageno "<<pageno<<endl;
    // Move the page from swap to physical memory
   // int pmindex=fifoQueue.front();
   int ind=fillPM();
   if(ind!=-1){
    physicalMemory[ind]={pid,pageno};
    p.first=ind;
    p.second.first=-1;
    p.second.second=true;
    return ind;
   }
   else {
       int pmindex=lru.front();
   cerr<<"LRU phy mem page "<<pmindex<<endl;
   lru.pop_front();
   lru.remove(pmindex);
       int vmindex=p.second.first;
    //cerr<<"Details -> "<<physicalMemory[pmindex].first<<" "<<physicalMemory[pmindex].second<<endl;
    if(physicalMemory[pmindex].first==-1 && physicalMemory[pmindex].second==-1){
        physicalMemory[pmindex]=virtualMemory[vmindex];
         p.second.second=true;
         p.second.first=-1;
         p.first=pmindex;
         virtualMemory[vmindex]={-1,-1};
         return p.first;
         
    }
    Process& process1 = processMap[physicalMemory[pmindex].first];
    cerr<<"Process1 : "<<process1.pid<<endl;
    pair<int,pair<int,bool>>&p1= process1.PageTable[physicalMemory[pmindex].second];
    //fifoQueue.pop();
   
    cerr<<"Swapping physical page "<<pmindex<<" with virtual mem page "<<pageno<<endl;
    cerr<<"PM index = "<<pmindex<<" VM index = "<<vmindex<<endl;
    pair<int,int>temp;
    temp=virtualMemory[vmindex];
    virtualMemory[vmindex]=physicalMemory[pmindex];
    physicalMemory[pmindex]=temp;
    //cerr<<"Swap done\n";

    p.second.second=true;
    p.second.first=-1;
    p.first=pmindex;
   // cerr<<"updated VM wala page joh PM me aya\n";

    p1.second.second=false;
   // cerr<<"Step 1 \n";
    p1.second.first=vmindex;
    // cerr<<"Step 1 \n";
    p1.first=-1;
   // cerr<<"updated PM wala page joh VM me gya\n";
   

    // int swapIndex = entry.physicalPage;
    // entry.physicalPage = physicalMemory.size();
    // physicalMemory[entry.physicalPage]=virtualMemory[swapIndex];
    // virtualMemory[swapIndex] = 0;  // Clear swap space after moving the page
    // entry.isInMemory = true;

    cerr << "Page " << pageno << " of process " << pid << " moved from swap to physical memory\n";
    return p.first ;
   }
  
}


void PagingSystem::runProcess(int pid) {
     cerr<<"curr pid to be run : "<<pid<<endl;

    if (processMap.find(pid) == processMap.end()) {
        cout << "Invalid PID given"<<pid<<" \n";
        return;
    }
   

    Process& process = processMap[pid];
    ifstream infile(fileName[pid]);  // Sample executable file for the process
    string line;
    vector<string> vec;
    while(infile >> line)
    {
        vec.push_back(line);
    }
    for(int i=1;i<vec.size();i++){
        cerr<<vec[i]<<endl;
        string s = vec[i];
        cerr<<"s: "<<s<<endl;
        //cerr<<"Command processing\n";
        
        if (s == "add") {
            int x, y, z;
            
            x = stoi(vec[i+1].substr(0,vec[i+1].size()-1));
            y = stoi(vec[i+2].substr(0,vec[i+2].size()-1));
            z = stoi(vec[i+3]);
            i=i+3;
      //      iss >> x >> y >> z;
            if (!isValidAddress(pid, x) || !isValidAddress(pid, y) || !isValidAddress(pid, z)) {
                cout << "Invalid Memory Address specified for process id " << pid << "\n";
                return;
            }
            int ppn_x=  move_pg_phy_mem(pid, x / P);  // Ensure the page is in physical memory
           int ppn_y= move_pg_phy_mem(pid, y / P);  // Ensure the page is in physical memory
           int ppn_z= move_pg_phy_mem(pid, z / P);  // Ensure the page is in physical memory
            int value_x = memory[ppn_x * P + x % P];
            int value_y = memory[ppn_y * P + y % P];
            int result= value_x + value_y;
              memory[ppn_z * P + z % P] = result;
            cout << "Command: add(" << x << ", " << y << ", " << z << "); Result: " << result << "\n";
            lru.remove(ppn_x);
            lru.remove(ppn_y);
            lru.remove(ppn_z);
            lru.push_back(ppn_x);
            lru.push_back(ppn_y);
            lru.push_back(ppn_z);
          //  accessPageLRU(ppn_x);
           // accessPageLRU(ppn_y);
           // accessPageLRU(ppn_z);
        } else if (s == "sub") {
            int x, y, z;
            x = stoi(vec[i+1].substr(0,vec[i+1].size()-1));
            y = stoi(vec[i+2].substr(0,vec[i+2].size()-1));
            z = stoi(vec[i+3]);
            i=i+3;
        //    iss >> x >> y >> z;
            if (!isValidAddress(pid, x) || !isValidAddress(pid, y) || !isValidAddress(pid, z)) {
                cout << "Invalid Memory Address specified for process id " << pid << "\n";
                return;
            }

          int ppn_x=  move_pg_phy_mem(pid, x / P);  // Ensure the page is in physical memory
          cerr<<"First operand\n";
           int ppn_y= move_pg_phy_mem(pid, y / P);  // Ensure the page is in physical memory
           cerr<<"Second operand\n";
           int ppn_z= move_pg_phy_mem(pid, z / P);  // Ensure the page is in physical memory

            int value_x = memory[ppn_x * P + x % P];
            int value_y = memory[ppn_y * P + y % P];
            cerr<<"VALUE_X = "<<value_x <<" VALUE_Y = "<<value_y<<endl;
            cerr<<"MEM_INDEX of X = "<<ppn_x * P + x % P<<"MEM_INDEX OF Y = "<<ppn_y * P + y % P<<endl;
            int result= value_x - value_y;
              memory[ppn_z * P + z % P] = result;

            cout << "Command: sub(" << x << ", " << y << ", " << z << "); Result: " << result<< "\n";
           // accessPageLRU(ppn_x);
            //accessPageLRU(ppn_y);
           // accessPageLRU(ppn_z);
           lru.remove(ppn_x);
           lru.remove(ppn_y);
           lru.remove(ppn_z);
           lru.push_back(ppn_x);
            lru.push_back(ppn_y);
            lru.push_back(ppn_z);

        } else if (s == "print") {
            cerr<<"Printing\n";
            int x;
            x = stoi(vec[i+1]);
            i=i+1;
       //     iss >> x;
            if (!isValidAddress(pid, x)) {
                cerr << "Invalid Memory Address specified for process id " << pid << "\n";
                return;
            }
          int ppn_x=  move_pg_phy_mem(pid, x / P);
           // int value_x = physicalMemory[ppn_x * P + x % P];
           int value_x = memory[ppn_x * P + x % P];
           
            cout << "Command: print(" << x << "); Result: " << value_x << "\n";
           // accessPageLRU(ppn_x);
           
            
        } 
        else if (s == "load") {
            cerr<<"Arg taken for load\n";
           
            int a = stoi(vec[i+1].substr(0,s.size()-1));
            int y = stoi(vec[i+2]);
            i=i+2;
           // iss >> a >> y;
           
            if (!isValidAddress(pid, y)) {
                cout << "Invalid Memory Address specified for process id " << pid << "\n";
                return;
            }
            //cout<<"Hi1\n";
            cerr<<"Sending to get page "<<y/P<<endl;
            int ppn_y = move_pg_phy_mem(pid, y / P);
           // cout<<"Hi2\n";
            
           // physicalMemory[ppn_y*P  + y % P] = a;
          cerr<<"MEMEORY INDEX = "<< ppn_y * P + y % P<<endl;
            memory[ppn_y * P + y % P]=a;
                     
            cout << "Command: load(" << a << ", " << y << "); Result: Value of " << a << " is now stored in addr(" << y << ")\n";
           // accessPageLRU(ppn_y);
           lru.remove(ppn_y);
            lru.push_back(ppn_y);
            
        }
        else {
            cout<<"Nothing\n";
        }
    }
}

void PagingSystem::killProcess(int pid) {
   if (processMap.find(pid) != processMap.end()) {
        Process& process = processMap[pid];

        // Free all physical memory and swap memory occupied by this process
        for (const auto& entry : process.PageTable) {
            if (entry.second.second==true) {
                physicalMemory[entry.first] = {-1,-1};
                 // Free physical memory but don't nullify content
            } else {
                virtualMemory[entry.second.first] = {-1,-1};  // Free swap memory but don't nullify content
            }
        }

        processMap.erase(pid);  // Remove the process from the process map
        cout << "killed " << pid << "\n";
    } else {
        cerr << "Invalid PID given for killing \n";
    }
}

// Helper to validate memory addresses
bool PagingSystem::isValidAddress(int pid, int addr) {
    Process& process = processMap[pid];
    int pageIndex = addr / P;
    if (pageIndex >= process.PageTable.size()) {
        return false;
    }
    return true;
}

// List all processes in memory
void PagingSystem::listProcesses() {
    cout<<"Listing all processes\n";
    for (const auto& [pid, process] : processMap) {
        cout << pid << " ";
    }
    cout << "\n";
}

// Print memory from `memloc` to `memloc + length - 1`
void PagingSystem::printMemory(int memloc, int length) {
    for (int i = memloc; i < memloc + length; ++i) {
        cout << "Value of " << i << ": " << memory[i] << "\n";
    }
}


void PagingSystem::exitSystem() {
    // Clean up all allocated memory
    physicalMemory.clear();
    virtualMemory.clear();
    processMap.clear();

    cout << "System exited and all memory cleaned up.\n";
    exit(0);
}

void PagingSystem::printPageTable(int pid, const string& filename) {
    ofstream outfile(filename, ios::app);  // Append to the file
    if (!outfile.is_open()) {
        cerr << "Unable to open " << filename << "\n";
        return;
    }

    cerr << "inside the pte function\n";

    // Get the current date and time
    time_t now = time(0);
    char* dt = ctime(&now);
    outfile << "Page Table for PID " << pid << " at " << dt << "\n";

    if (processMap.find(pid) == processMap.end()) {
        cerr << "Invalid PID\n";
        return;
    }

    Process& process = processMap[pid];
    int i=0;
    for (auto it : process.PageTable) {
       // cerr << "Entry received => " << entry.logica << endl;
       pair<int,pair<int,bool>>p=it;
       int x,y,z;
       x=i;
       i++;
       if(it.second.second==true){
        y=it.first;
       }
       else y=it.second.first;
       z=it.second.second;
       outfile<<x<<" "<<y<<" "<<z<<endl;
        
    }
    outfile.close();
}

void PagingSystem::printAllPageTables(const string& filename) {
    for (const auto& [pid, process] : processMap) {
       /// printPageTable(pid, filename);
        ofstream outfile(filename, ios::app);  // Append to the file
    if (!outfile.is_open()) {
        cerr << "Unable to open " << filename << "\n";
        return;
    }

    cerr << "inside the pte function\n";

    // Get the current date and time
    time_t now = time(0);
    char* dt = ctime(&now);
    outfile << "Page Table for PID " << pid << " at " << dt << "\n";

    if (processMap.find(pid) == processMap.end()) {
        cerr << "Invalid PID\n";
        return;
    }

    //Process& process = processMap[pid];
    int i=0;
    for (auto it : process.PageTable) {
       // cerr << "Entry received => " << entry.logica << endl;
       pair<int,pair<int,bool>>p=it;
       int x,y,z;
       x=i;
       i++;
       if(it.second.second==true){
        y=it.first;
       }
       else y=it.second.first;
       z=it.second.second;
       outfile<<pid<<" "<<x<<" "<<y<<" "<<z<<endl;
        
    }
    outfile.close();
    }
}

int main(int argc, char* argv[]) {
    

    int M = 0, V = 0, P = 0;
    string infile, outfile;

    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        cout << "Processing argument: " << argv[i] << "\n";  // Debug print to show arguments

        if (string(argv[i]) == "-M") {
            M = stoi(argv[++i]) * 1024;  // Convert KB to bytes
            cout << "Main memory size: " << M << " bytes\n";  // Debugging print
        } else if (string(argv[i]) == "-V") {
            V = stoi(argv[++i]) * 1024;  // Convert KB to bytes
            cout << "Virtual memory size: " << V << " bytes\n";  // Debugging print
        } else if (string(argv[i]) == "-P") {
            P = stoi(argv[++i]);         // Page size in bytes
            cout << "Page size: " << P << " bytes\n";  // Debugging print
        } else if (string(argv[i]) == "-i") {
            infile = argv[++i];           // Input file
            cout << "Infile: " << infile << "\n";  // Debugging print
        } else if (string(argv[i]) == "-o") {
            outfile = argv[++i];          // Output file
            cout << "Outfile: " << outfile << "\n";  // Debugging print
        }
    }

    // Check if the required values are provided
    if (M == 0 || V == 0 || P == 0 || infile.empty() || outfile.empty()) {
        cerr << "Invalid or missing arguments.\n";
        return 1;
    }
    
    cout<<"Initialise\n";
    // Initialize PagingSystem with the specified memory sizes
    PagingSystem ps(M, V, P);

    // Open the input file
    ifstream inFile(infile);
    if (!inFile.is_open()) {
        cerr << "Unable to open infile: " << infile << "\n";
        return 1;
    }
    

    // Open the output file
    ofstream outFile(outfile);
    if (!outFile.is_open()) {
        cerr << "Unable to open outfile: " << outfile << "\n";
        return 1;
    }

    outFile << "Print" << endl;

    // Redirect std::cout to outfile
    streambuf* coutBuf = cout.rdbuf();  // Save old buffer
    cout.rdbuf(outFile.rdbuf());        // Redirect cout to outfile

    cerr<<"Reading from infile\n";
    string command;
    while (getline(inFile, command)) {
        istringstream iss(command);
        string cmd;
        iss >> cmd;
        cerr<<"Cmd = "<<cmd<<endl;
        if (cmd == "load") {
            
            vector<string> files;
            string file;
            while (iss >> file) {
                files.push_back(file);
                cerr<<"Pushing "<<file<<" in files\n";
            }
            ps.loadProcess(files);
        } else if (cmd == "run") {
            int pid;
            iss >> pid;
            cerr<<"send pid to run "<<pid<<endl;
            ps.runProcess(pid);
        } else if (cmd == "kill") {
            int pid;
            iss >> pid;
            ps.killProcess(pid);
        } else if (cmd == "listpr") {
            ps.listProcesses();
        } else if (cmd == "pte") {
            cerr << "Printing pte\n";
            int pid;
            string filename;
            iss >> pid >> filename;
            ps.printPageTable(pid, filename);
        } else if (cmd == "pteall") {
            cerr << "Printing pteall\n";
            string filename;
            iss >> filename;
            ps.printAllPageTables(filename);
        } else if (cmd == "print") {
            int memloc, length;
            iss >> memloc >> length;
            ps.printMemory(memloc, length);
        } else if (cmd == "exit") {
            ps.exitSystem();
        }
    }

    // Restore original std::cout buffer
    cout.rdbuf(coutBuf);

    // Close files
    inFile.close();
    outFile.close();

    return 0;
}
