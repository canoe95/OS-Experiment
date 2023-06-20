#include <iostream>
using namespace std;
#include <vector>
#include <map>
#include <ctime>

#define WRITE 0
#define READ 1
#define EXEC 2

// �����ڵ�
struct INode {
	int type;
	int indexBlockNum; // �����̿��
	int size;
	int permission;
	string name;
	string time;

	INode() {}

	INode(int size, string name, char *time, int index) {
		this->indexBlockNum = index;
		this->size = size;
		this->name = name;
		this->time = time;
		this->time[this->time.length() - 1] = '\0';
	}
};

// �ڴ��
struct Block {
	int id;
	string fileName;
	int clk;
	bool work;
	vector<char> content;
	vector<int> index;
};

// ģ��64�ڴ�
static Block memory[64];

// һ�����ļ���һ���������������Ϊ�ļ�����ֵΪ�ļ� INode
static map<string, map<string, INode *>> indexTable;
// ��ǰĿ¼
static string user;


// ģ����̣�һ�� char һ�ֽڣ�һ���� 40 ���ֽڣ��� 1024 ����
static vector<vector<char>> disk(1024, vector<char>(40, '#'));

// λʾͼ
static vector<vector<int>> bitmap(64, vector<int>(16, 0));





// ��ʼ���ڴ�
void initMem() {
	for (int i = 0; i < 64; i++) {
		memory[i].clk = 1;
		memory[i].content = vector<char>(40, '#');
		memory[i].id = -1;
		memory[i].index = vector<int>();
		memory[i].fileName = "";
		memory[i].work = 0;
	}
}

// �ж��Ƿ����ڴ�
bool inMem(string name) {
	int i;
	for (i = 0; i < 64; i++) {
		if (memory[i].fileName == name) {
			break;
		}
	}
	return i != 64;
}

// �����ڴ�
void close(string name) {
	int i;
	for (i = 0; i < 64; i++) {
		if (memory[i].fileName == name) {
			break;
		}
	}
	if (i == 64) {
		cout << "no such file in memory!\n";
		return;
	}
	for (int j = 0; j < 8; j++) {
		Block &block = memory[i + j];
		block.clk = 1;
		block.work = 0;
		block.fileName = "";
		block.id = -1;
		block.index = vector<int>();
		block.content = vector<char>(40, '#');
	}
	cout << "file close success!\n";
}

// �����ڴ��
int allocate() {
	int m = -1;
	for (int i = 0; i < 64; i += 8) {
		if (memory[i].work == 0) {
			m = i;
			break;
		}
	}
	if (m == -1) {
		cout << "no free memory\n";
		return m;
	}
	return m;
}

vector<int> readStorage(int index);

void callIn(int memBegin, vector<int> indexNums, string name) {
	int size = indexNums.size() > 8 ? 8 : indexNums.size();
	for (int i = 0; i < size; i++) {
		Block &cur = memory[memBegin + i];
		cur.id = i;
		cur.clk = 1;
		cur.fileName = name;
		cur.work = 1;
		cur.content = disk[indexNums[i]];
		cur.index = indexNums;
	}
}

// ���ļ������ļ��˸�ҳ�����ڴ�
void open(string name) {
	if (inMem(name)) {
		cout << "file has been in the memory!\n";
		return;
	}
	map<string, INode *> table = indexTable[user];
	if (!table.count(name)) {
		cout << "no such file!\n";
		return;
	}
	INode *i = table[name];
	vector<int> index = readStorage(i->indexBlockNum);
	int memNum = allocate();
	if (memNum == -1) {
		return;
	}
	callIn(memNum, index, name);
	cout << "file open success!\n";
}

int point[8];

// ��ҳ
void adjust(int begin, int num, int id) {
	int n = point[begin / 8];
	while (1) {
		if (memory[begin + n % 8].clk) {
			memory[begin + n % 8].clk = 0;
		} else {
			memory[begin + n % 8].clk = 1;
			break;
		}
		n++;
	}
	point[begin / 8] = n % 8 + 1;
	memory[begin + n % 8].id = id;
	memory[begin + n % 8].content = disk[num];
}

void printContent(int begin, int id) {
	// �����˸��ڴ�飬�ҵ� id ���ϵ�ҳ
	for (int i = 0; i < 8; i++) {
		Block b = memory[begin + i];
		if (b.id == id) {
			for (auto c : b.content) {
				cout << c;
			}
			return;
		}
	}
	// ����ܵ����˵��û�� id ���ϵ�ҳ����Ҫ��ҳ
	int num = memory[begin].index[id]; // �ҵ��� id ��ҳ������̿��
	// �� disk[num] �����ڴ棬�ڴ����ʼ��Ϊ begin�����Ϊ 8
	adjust(begin, num, id);
	printContent(begin, id);
}

// ��ȡ�����ļ�
void cat(string name) {
	map<string, INode *> table = indexTable[user];
	if (!table.count(name)) {
		cout << "no such file!\n";
		return;
	}
	if (!inMem(name)) {
		open(name);
	}
	int i; // �ļ����ڴ����ʼ�ڴ���
	for (i = 0; i < 64; i++) {
		if (memory[i].fileName == name) {
			break;
		}
	}

	//for (auto i : memory[i].index) { cout << i << endl; }

	// �ļ�ҳ��С
	int size = memory[i].index.size();

	for (int j = 0; j < size; j++) {
		// j Ϊҳ��
		printContent(i, j);
	}
	cout << endl;
}

void init() {
	user = "root";
	indexTable[user] = map<string, INode *>();
	initMem(); // ��ʼ���ڴ��
}

void man() {
	cout << "su: switch user\
			\nuseradd: create user\
			\nuserdel: delete user\
			\ndf: check the disk condition\
			\nls: display the dir\
			\nvi: create/change the file\
			\nrm: remove file\
			\npwd: display the current dir path\
			\nopen: open the file, put it into memory\
			\nclose: close the file, put it out memory\
			\ncat: check the file content\
			\nexit: exit the system\n";
}

void pwd() {
	cout << "/" << user;
}

void su() {
	for (auto it : indexTable) {
		cout << it.first << endl;
	}
}

void su(string u) {
	if (user != u && indexTable.count(u)) {
		initMem();
		user = u;
		pwd();
		cout << endl;
		return;
	}
	cout << "no such user!\n";
}

void ls() {
	cout << "name\t\tsize\t\tlast write time\t\t\tpermission\n";
	map<string, INode *> files = indexTable[user];
	for (auto it : files) {
		INode *f = it.second;
		cout << f->name << "\t\t" << f->size << "\t\t"
		     << f->time << "\t";
		switch (f->permission) {
			case WRITE:
				cout << "write\n";
				break;
			case READ:
				cout << "read\n";
				break;
			case EXEC:
				cout << "exec\n";
				break;
		}
	}
}

void useradd(string user) {
	if (indexTable.count(user)) {
		cout << "user have existed, add failed!\n";
		return;
	}
	indexTable[user] = map<string, INode *>();
	cout << "user create success!\n";
}

void userdel(string name) {
	if (!indexTable.count(name)) {
		cout << "user do not exist, del failed!\n";
		return;
	}
	if (name == "root") {
		cout << "root cannot be deleted!\n";
		return;
	}
	if (user == name) {
		user = "root";
	}
	indexTable.erase(name);
	cout << "user delete success!\n";
}

int getSpareBlock(int number) {
	int i, j;
	int count = 0;
	for (i = 0; i < (int)bitmap.size(); i++) {
		for (j = 0; j < (int)bitmap[0].size(); j++) {
			if (bitmap[i][j] == 0) {
				count++;
			} else if (bitmap[i][j] == 1) {
				count = 0;
			}
			if (count == number) {
				int begin = i * 16 + j - number + 1;
				for (int k = 0; k < number; k++) {
					bitmap[(begin + k) / 16][(begin + k) % 16] =  1;
				}
				return begin;
			}
		}
	}
	return -1;
}

void writeIndex(int begin, int size, int index) {
	vector<char> &block = disk[index];
	for (int i = 0; i < size; i++) {
		string num = to_string(begin + i);
		for (int j = 0; j < (int)num.size(); j++) {
			block[i * 4 + j] = num[j];
		}
	}
}

void writeContent(string content, int size, int index) {
	vector<char> &block = disk[index];
	for (int i = 0; i < size; i++) {
		block[i] = content[i];
	}
}

void write(int firstIndexBegin, int firstIndexSize,
           int secondIndexBegin, int secondIndexSize,
           int storageBegin, int storageSize,

           int indexBlockNum, string content) {
	// ��һ���������������д��������
	writeIndex(firstIndexBegin, firstIndexSize, indexBlockNum);
	//cout << "nmsl" << endl;
	// �������������������д��һ������
	while (secondIndexSize > 10) {
		writeIndex(secondIndexBegin, 10, firstIndexBegin);
		secondIndexBegin++;
		secondIndexSize -= 10;
		firstIndexBegin++;
	}
	writeIndex(secondIndexBegin, secondIndexSize, firstIndexBegin);
	// ���洢�̿���д���������
	while (storageSize > 10) {
		writeIndex(storageBegin, 10, secondIndexBegin);
		storageBegin++;
		storageSize -= 10;
		secondIndexBegin++;
	}
	writeIndex(storageBegin, storageSize, secondIndexBegin);

	// ������д��洢�̿�
	while (content.length() > 40) {
		writeContent(content, 40, storageBegin);
		storageBegin++;
		content.erase(0, 40);
	}
	writeContent(content, content.length(), storageBegin);

}

vector<int> readIndex(int index) {
	vector<int> nums;
	vector<char> indexBlock = disk[index];
	for (int i = 0; i < (int)indexBlock.size(); i += 4) {
		if (indexBlock[i] != '#') {
			string num;
			num.push_back(indexBlock[i]);
			int j = i + 1;
			while (indexBlock[j] != '#') {
				num.push_back(indexBlock[j]);
				j++;
			}
			nums.push_back(atoi(num.c_str()));
		}
	}
	return nums;
}

vector<int> readIndex(vector<int> index) {
	vector<int> nums;
	for (int i = 0; i < (int)index.size(); i++) {
		vector<int> row = readIndex(index[i]);
		for (auto it : row) {
			nums.push_back(it);
		}
	}
	return nums;
}

vector<int> readStorage(int index) {
	// �������飬�ҳ�һ������
	vector<int> firstIndex = readIndex(index);
	// ��һ���������ҳ���������
	vector<int> secondIndex = readIndex(firstIndex);
	// �������������ҳ��洢��
	vector<int> storageBlockNum = readIndex(secondIndex);
	return storageBlockNum;
}

vector<int> readStorage(int index);

void createFile(string name) {
	map<string, INode *> &table = indexTable[user];

	string content;
	cout << "please enter the file content:\n";
	getline(cin, content, '\n');

	int indexBlockNum = getSpareBlock(1);
	// content.length Ϊ�ļ���ռ�ֽ�������λ B��һ���̿� 40B����������̿�����
	int size = content.length();
	//cout << content << " " << size << " " << indexBlockNum << endl;
	int storageSize = size / 40 + 1;
	// һ���̿�� 4B��һ���̿�����¼ 10 ���̿�ţ�secondIndexSize ���̿���Լ�¼�ļ�����ĸ����̿�
	int secondIndexSize = storageSize / 10 + 1;
	int firstIndexSize = secondIndexSize / 10 + 1;

	int firstIndexNum = getSpareBlock(firstIndexSize);
	int secondIndexNum = getSpareBlock(secondIndexSize);
	int storageBlockNum = getSpareBlock(storageSize);

	write(firstIndexNum, firstIndexSize, secondIndexNum, secondIndexSize,
	      storageBlockNum, storageSize, indexBlockNum, content);


	time_t now = time(0);
	INode *file = new INode(size, name, ctime(&now), indexBlockNum);
	table[name] = file;
}

void free(int index) {
	disk[index] = vector<char>(40, '#');
	bitmap[index / 16][index % 16] = 0;
}

void free(vector<int> index) {
	for (auto i : index) {
		free(i);
	}
}

void rm(string name) {
	map<string, INode *> &table = indexTable[user];
	if (!table.count(name)) {
		cout << "no such file!\n";
		return;
	}
	// �������̺�
	int index = table[name]->indexBlockNum;
	// һ�������̺�
	vector<int> firstIndex = readIndex(index);
	// ���������̺�
	vector<int> secondIndex = readIndex(firstIndex);
	// �洢�̺�
	vector<int> storage = readIndex(secondIndex);
	free(index);
	free(firstIndex);
	free(secondIndex);
	free(storage);
	table.erase(name);
	cout << "file delete success!\n";
}

void rewriteFile(string name) {
	rm(name);
	createFile(name);
}

void df();

void vi(string name) {
	map<string, INode *> table = indexTable[user];
	if (!table.count(name)) {
		createFile(name);
		//df();
		cout << "file create success!\n";
		return;
	}
	rewriteFile(name);
}


void df() {
	cout << "disk matrix\n";
	for (auto row : disk) {
		for (auto c : row) {
			cout << c;
		}
		cout << endl;
	}

	cout << "bitmap\n";
	int count = 0;
	for (auto row : bitmap) {
		for (int e : row) {
			count += e;
			cout << e << "\t";
		}
		cout << endl;
	}
	cout << "\n��ʹ���̿���: " << count << "\t�����̿�: " << 1024 - count << endl;
}


void exec(vector<string> commands);

void readline() {
	cout << "\nroot@localhost:";
	pwd();
	cout << "$ ";
	string str;
	getline(cin, str, '\n');
	int start = 0, end = 0, count = 0;
	vector<string> commands;
	while (start < (int)str.length()) {
		while (str[start] == ' ') {
			start++;
		}
		end = start;
		while (end < (int)str.length() && str[end] != ' ') {
			end++;
		}
		string command = str.substr(start, end - start);
		commands.push_back(command);
		start = end + 1;
	}
	exec(commands);
}

static int flag = true;

void exec(vector<string> commands) {
	int n = commands.size();
	if (n > 2) {
		cout << "no such command, maybe u need input \"man\" to get help\n";
		return;
	}
	string first = commands[0];
	if (first == "man") {
		if (n > 1) {
			cout << "no such command, maybe u need input \"man\" to get help\n";
			return;
		}
		man();
	} else if (first == "pwd") {
		if (n > 1) {
			cout << "no such command, maybe u need input \"man\" to get help\n";
			return;
		}
		pwd();
		cout << endl;
	} else if (first == "ls") {
		if (n > 1) {
			cout << "no such command, maybe u need input \"man\" to get help\n";
			return;
		}
		ls();
	} else if (first == "format") {
		if (n > 1) {
			cout << "no such command, maybe u need input \"man\" to get help\n";
			return;
		}
		init();
	} else if (first == "df") {
		if (n > 1) {
			cout << "no such command, maybe u need input \"man\" to get help\n";
			return;
		}
		df();
	} else if (first == "su") {
		if (n > 2) {
			cout << "no such command, maybe u need input \"man\" to get help\n";
			return;
		}
		if (n == 1) {
			su();
			return;
		}
		su(commands[1]);
	} else if (first == "vi") {
		if (n != 2) {
			cout << "no such command, maybe u need input \"man\" to get help\n";
			return;
		}
		vi(commands[1]);
	} else if (first == "rm") {
		if (n != 2) {
			cout << "no such command, maybe u need input \"man\" to get help\n";
			return;
		}
		rm(commands[1]);
	} else if (first == "open") {
		if (n != 2) {
			cout << "no such command, maybe u need input \"man\" to get help\n";
			return;
		}
		open(commands[1]);
	} else if (first == "cat") {
		if (n != 2) {
			cout << "no such command, maybe u need input \"man\" to get help\n";
			return;
		}
		cat(commands[1]);
	} else if (first == "close") {
		if (n != 2) {
			cout << "no such command, maybe u need input \"man\" to get help\n";
			return;
		}
		close(commands[1]);
	} else if (first == "useradd") {
		if (n != 2) {
			cout << "no such command, maybe u need input \"man\" to get help\n";
			return;
		}
		useradd(commands[1]);
	} else if (first == "userdel") {
		if (n != 2) {
			cout << "no such command, maybe u need input \"man\" to get help\n";
			return;
		}
		userdel(commands[1]);
	} else if (first == "exit") {
		if (n > 1) {
			cout << "no such command, maybe u need input \"man\" to get help\n";
			return;
		}
		flag = false;
	} else {
		cout << "no such command, maybe u need input \"man\" to get help\n";
	}
}


int main() {
	init();
	while (flag) {
		readline();
	}
}