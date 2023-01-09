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
	int id; // ��ʶ 
	int priority; // ���ȼ� 
	int cpu_time; // ��ռ�õ� cpu ʱ�� 
	int all_time; // ����Ҫռ�õ� cpu ʱ�� 
	int start_block; // �������ٸ�ʱ��Ƭ���������̬ 
	int block_time; //  ������ʱ��Ƭ 
	int state; // ����״̬��ִ���� -1 ������ 0������ 1
	
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


deque<PCB> queue; // ִ��˳��� 
vector<int> index; // �±�ӳ�䣬��Ϊ����Ҫ��0�����ӡ���������Ǹ������ȼ����У��� index[0] �� PCB-0 �� queue �е��±� 
int ready_count = 0; // ����������
int block_count = 0; // ����������
int end_count = 0; // ��ɽ�����
int slice_count = 0; // ��ʱ��Ƭʹ����


// PCB ���� 
bool PCB_sort(PCB p1, PCB p2){
	if(p1.state != p2.state){
		return p1.state > p2.state;
	}
	return p1.priority > p2.priority;
}


// ���� queue �� PCB ����˳�� 
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


// ����ִ�ж���˳������һ������Ҳִ���꣬˵�����н���ִ���꣬���� false 
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

// ��ӡ��ǰ���н���״̬ 
void print_table(int flag){
	cout << "System CPU Time: " << slice_count << endl; // ʹ��ʱ��Ƭ 
	
	cout << "Running Process: ";
	if(flag){
		queue[0].print_priority(); 		// �������� PCB 
	}
	cout << endl;
	
	// �������� 
	cout << "Ready Queue: ";  
	for(int i = 1; i < ready_count; i++){
		queue[i].print_priority();
		cout << "->";
	}
	cout << "null" << endl;
	
	// �������� 
	cout << "Block Queue: ";  
	for(int i = ready_count; i < ready_count + block_count; i++){
		queue[i].print_priority();
		cout << "->";
	}
	cout << "null" << endl;

	// ��ɶ��� 	
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
	// ��ִ���ֱ꣬����Ϊ -1 �������к����ж� 
	if(p.all_time <= 0){
		p.state = -1;
	} else { // ��δִ���꣬�ж��Ƿ����� 
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

// ��ʼ�� table 
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

