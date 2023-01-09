#include <iostream>
using namespace std;
#include <vector>
#include <map>
#include <set>
#include <stack>
#include <deque>
#include <algorithm>
#include <string> 

struct PCB {
	int id; // 标识 
	int priority; // 优先级 
	int cpu_time; // 已占用的 cpu 时间 
	int all_time; // 还需要占用的 cpu 时间 
	int start_block; // 经过多少个时间片后进入阻塞态 
	int block_time; //  阻塞的时间片 
	int state; // 进程状态，执行完 -1 、阻塞 0、就绪 1
	
	PCB() {}
	
	PCB(int i, int p, int c, int a, int sb, int bt, int s){
		id = i;
		priority = p;
		cpu_time = c;
		all_time = a;
		start_block = sb;
		block_time = bt;
		state = s;
	} 
	
	void print_priority(){
		cout << id << '[' << priority << ']'; 
	}
	
	void print(){
		cout << id << '\t'
			 << priority << "\t\t"
			 << cpu_time << "\t\t"
			 << all_time << "\t\t"
			 << start_block << "\t\t"
			 << block_time << "\t\t";
		if(state == -1){
			cout << "end\n";
		} else if(state == 0){
			cout << "block\n";
		} else if(state == 1){
			cout << "ready\n";
		}
	}
};


deque<PCB> queue; // 执行顺序表 
vector<int> index; // 下标映射，因为表需要从0按序打印，而队列是根据优先级排列，即 index[0] 是 PCB-0 在 queue 中的下标 
int ready_count = 0; // 就绪进程数
int block_count = 0; // 阻塞进程数
int end_count = 0; // 完成进程数
int slice_count = 0; // 总时间片使用数


// PCB 排序 
bool PCB_sort(PCB p1, PCB p2){
	if(p1.state != p2.state){
		return p1.state > p2.state;
	}
	return p1.priority > p2.priority;
}


// 调整 queue 中 PCB 优先顺序 
void sort_queue(){
	sort(queue.begin(), queue.end(), PCB_sort);
	ready_count = 0; block_count = 0; end_count = 0;
	for(int i = 0; i < queue.size(); i++){
		if(queue[i].state == 1){
			ready_count++;
		} else if(queue[i].state == 0){
			block_count++;
		} else if(queue[i].state == -1){
			end_count++;
		}		
	}
}


// 设置执行队列顺序，若第一个进程也执行完，说明所有进程执行完，返回 false 
int set_sequence(){
	sort_queue();
	PCB cur = queue.front();
	if(cur.state == -1){
		return 0;
	}
	return 1;
}


void print_colume(){
	cout << "ID\tPriority\tCPU Time\tAll Time\tStart Block\tBlock Time\tState" << endl;
	cout << "---------------------------------------------------------------------------------------------\n";
}

// 打印当前所有进程状态 
void print_table(int flag){
	cout << "System CPU Time: " << slice_count << endl; // 使用时间片 
	
	cout << "Running Process: ";
	if(flag){
		queue[0].print_priority(); 		// 本次运行 PCB 
	}
	cout << endl;
	
	// 就绪队列 
	cout << "Ready Queue: ";  
	for(int i = 1; i < ready_count; i++){
		queue[i].print_priority();
		cout << "->";
	}
	cout << "null" << endl;
	
	// 阻塞队列 
	cout << "Block Queue: ";  
	for(int i = ready_count; i < ready_count + block_count; i++){
		queue[i].print_priority();
		cout << "->";
	}
	cout << "null" << endl;

	// 完成队列 	
	cout << "End Queue: ";  
	for(int i = ready_count + block_count; i < queue.size(); i++){
		queue[i].print_priority();
		cout << "->";
	}
	cout << "null" << endl;
	
	cout << "=============================================================================================\n";
	print_colume();
	for(int i = 0; i < queue.size(); i++){
		//cout << index[i] << endl;
		queue[index[i]].print();
	}
	cout << "=============================================================================================\n";
} 

	
void use(PCB& p){
	p.priority -= 3;
	p.cpu_time++;
	p.all_time--;
	// 若执行完，直接置为 -1 ，不进行后续判断 
	if(p.all_time <= 0){
		p.state = -1;
	} else { // 若未执行完，判断是否阻塞 
		if(p.start_block > 0){
			p.start_block--;
		}
		if(p.start_block == 0){
			p.state = 0;
		}
	}	
}

void ready_use(PCB& p){
	p.priority++;
}

void block_use(PCB& p){
	p.block_time--;
	if(p.block_time <= 0){
		p.state = 1;
	}
}


void update_index(){
	for(int i = 0; i < queue.size(); i++){
		//cout << queue[i].id << endl;
		index[queue[i].id] = i;
	}
}


void use(){
	use(queue[0]);
	for(int i = 1; i < ready_count; i++){
		ready_use(queue[i]);
	}
	for(int i = ready_count; i < ready_count + block_count; i++){
		block_use(queue[i]);
	}
}

int run(){
	int flag = set_sequence();	
	update_index();
	print_table(flag);
	if(flag) { use(); slice_count++; }
	return flag;
}

// 初始化 table 
void init(){
	PCB p0(0,9,2,1,0,0,0), p1(1,38,3,0,-1,0,-1), p2(2,30,0,6,-1,0,1),
		p3(3,29,0,3,-1,0,1), p4(4,0,0,4,-1,0,1);
		
	queue.push_back(p0);
	queue.push_back(p1);
	queue.push_back(p2);
	queue.push_back(p3);
	queue.push_back(p4);
	
	for(int i = 0; i < queue.size(); i++){
		index.push_back(i);
	}
	
	set_sequence();
	update_index();
}


int main(){
	init();
	while(run()) {}
    return 0;
}

