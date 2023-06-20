---
title: 操作系统课设
date: 2022-11-26
tags:
  - C/C++
---

库及宏定义

```c
#include <iostream>
using namespace std;
#include <vector>
#include <map>
#include <ctime>

#define WRITE 0
#define READ 1
#define EXEC 2
```

## 目录管理

> 二级目录

以及目录为用户名，二级目录只存文件

```c
// 一个父文件夹一个索引表，索引表键为文件名，值为文件 INode
static map<string, map<string, INode *>> indexTable;
// 当前目录
static string user;
```

索引节点

```c
// 索引节点
struct INode {
	int type;
	int indexBlockNum; // 索引盘块号
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
```

初始化

```c
void init() {
	user = "root";
	indexTable[user] = map<string, INode *>();
	initMem(); // 初始化内存块
}
```

帮助文档

```c
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
```

显示当前路径

```c
void pwd() {
	cout << "/" << user;
}
```

显示所有用户

```c
void su() {
	for (auto it : indexTable) {
		cout << it.first << endl;
	}
}
```

切换用户

```c
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
```

显示当前目录下所有文件

```c
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
```

增删用户

```c
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
```

获取用户输入

```c
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
```

执行用户命令

```c
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
```

Main 函数

```c
int main() {
	init();
	while (flag) {
		readline();
	}
}
```

## 空闲磁盘管理

> 位示图

模拟磁盘

```c
// 模拟磁盘，一个 char 一字节，一个块 40 个字节，共 1024 个块
static vector<vector<char>> disk(1024, vector<char>(40, '#'));
```

位示图

```c
// 位示图
static vector<vector<int>> bitmap(64, vector<int>(16, 0));
```

根据数量获取空闲磁盘块，返回起始位置

```c
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
```

释放磁盘块

```c
void free(int index) {
	disk[index] = vector<char>(40, '#');
	bitmap[index / 16][index % 16] = 0;
}

void free(vector<int> index) {
	for (auto i : index) {
		free(i);
	}
}
```

查看磁盘内容，查看位示图

```c
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
	cout << "\n已使用盘块数: " << count << "\t空闲盘块: " << 1024 - count << endl;
}
```

## 文件组织

> 二级索引

向磁盘中写索引

```c
void writeIndex(int begin, int size, int index) {
	vector<char> &block = disk[index];
	for (int i = 0; i < size; i++) {
		string num = to_string(begin + i);
		for (int j = 0; j < (int)num.size(); j++) {
			block[i * 4 + j] = num[j];
		}
	}
}
```

向磁盘中写内容

```c
void writeContent(string content, int size, int index) {
	vector<char> &block = disk[index];
	for (int i = 0; i < size; i++) {
		block[i] = content[i];
	}
}
```

向磁盘中写入文件

```c
void write(int firstIndexBegin, int firstIndexSize,
           int secondIndexBegin, int secondIndexSize,
           int storageBegin, int storageSize,

           int indexBlockNum, string content) {
	// 将一级索引的索引块号写进索引块
	writeIndex(firstIndexBegin, firstIndexSize, indexBlockNum);
	//cout << "nmsl" << endl;
	// 将二级索引的索引块号写入一级索引
	while (secondIndexSize > 10) {
		writeIndex(secondIndexBegin, 10, firstIndexBegin);
		secondIndexBegin++;
		secondIndexSize -= 10;
		firstIndexBegin++;
	}
	writeIndex(secondIndexBegin, secondIndexSize, firstIndexBegin);
	// 将存储盘块块号写入二级索引
	while (storageSize > 10) {
		writeIndex(storageBegin, 10, secondIndexBegin);
		storageBegin++;
		storageSize -= 10;
		secondIndexBegin++;
	}
	writeIndex(storageBegin, storageSize, secondIndexBegin);

	// 将内容写入存储盘块
	while (content.length() > 40) {
		writeContent(content, 40, storageBegin);
		storageBegin++;
		content.erase(0, 40);
	}
	writeContent(content, content.length(), storageBegin);

}
```

读单个磁盘块索引，即读文件的索引块

```c
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
```

读多个磁盘块索引，读文件的一级索引和二级索引

```c
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
```

通过索引块读文件的存储块，返回盘块号

```c
vector<int> readStorage(int index) {
	// 读索引块，找出一级索引
	vector<int> firstIndex = readIndex(index);
	// 读一级索引，找出二级索引
	vector<int> secondIndex = readIndex(firstIndex);
	// 读二级索引，找出存储块
	vector<int> storageBlockNum = readIndex(secondIndex);
	return storageBlockNum;
}
```

创建文件

```c
void createFile(string name) {
	map<string, INode *> &table = indexTable[user];

	string content;
	cout << "please enter the file content:\n";
	getline(cin, content, '\n');

	int indexBlockNum = getSpareBlock(1);
	// content.length 为文件所占字节数，单位 B，一个盘块 40B，求得所需盘块数量
	int size = content.length();
	//cout << content << " " << size << " " << indexBlockNum << endl;
	int storageSize = size / 40 + 1;
	// 一个盘块号 4B，一个盘块最多记录 10 个盘块号，secondIndexSize 个盘块可以记录文件所存的各个盘块
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
```

删除文件

```c
void rm(string name) {
	map<string, INode *> &table = indexTable[user];
	if (!table.count(name)) {
		cout << "no such file!\n";
		return;
	}
	// 索引块盘号
	int index = table[name]->indexBlockNum;
	// 一级索引盘号
	vector<int> firstIndex = readIndex(index);
	// 二级索引盘号
	vector<int> secondIndex = readIndex(firstIndex);
	// 存储盘号
	vector<int> storage = readIndex(secondIndex);
	free(index);
	free(firstIndex);
	free(secondIndex);
	free(storage);
	table.erase(name);
	cout << "file delete success!\n";
}
```

重写文件

```c
void rewriteFile(string name) {
	rm(name);
	createFile(name);
}
```

创建或修改文件

```c
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
```

## 内存管理

> Clock 调页算法

内存块

```c
// 内存块
struct Block {
	int id;
	string fileName;
	int clk;
	bool work;
	vector<char> content;
	vector<int> index;
};
```

模拟 64 块内存

```c
// 模拟64内存
static Block memory[64];
```

初始化内存

```c
// 初始化内存
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
```

判断文件是否在内存

```c
// 判断是否在内存
bool inMem(string name) {
	int i;
	for (i = 0; i < 64; i++) {
		if (memory[i].fileName == name) {
			break;
		}
	}
	return i != 64;
}
```

回收内存块

```c
// 回收内存
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
```

分配内存块

```c
// 分配内存块
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
```

将文件前八页调入内存

```c
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

// 打开文件，把文件八个页调入内存
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
```

Clock 算法调页

```c
int point[8]; // Clock 指针
// 调页
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
```

打印内存块

```c
void printContent(int begin, int id) {
	// 遍历八个内存块，找到 id 符合的页
	for (int i = 0; i < 8; i++) {
		Block b = memory[begin + i];
		if (b.id == id) {
			for (auto c : b.content) {
				cout << c;
			}
			return;
		}
	}
	// 如果跑到这里，说明没有 id 符合的页，需要调页
	int num = memory[begin].index[id]; // 找到第 id 个页的外存盘块号
	// 将 disk[num] 调入内存，内存块起始号为 begin，跨度为 8
	adjust(begin, num, id);
	printContent(begin, id);
}
```

从磁盘调入整个文件进内存并按序打印

```c

// 读取整个文件
void cat(string name) {
	map<string, INode *> table = indexTable[user];
	if (!table.count(name)) {
		cout << "no such file!\n";
		return;
	}
	if (!inMem(name)) {
		open(name);
	}
	int i; // 文件在内存的起始内存块号
	for (i = 0; i < 64; i++) {
		if (memory[i].fileName == name) {
			break;
		}
	}

	//for (auto i : memory[i].index) { cout << i << endl; }

	// 文件页大小
	int size = memory[i].index.size();

	for (int j = 0; j < size; j++) {
		// j 为页号
		printContent(i, j);
	}
	cout << endl;
}
```

