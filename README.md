# DARIC_Map
Mapper for DARIC

## Table of contents
1. [Directory Structure](#directory-structure)
2. [Getting Started](#getting-started)
    1. [Hardware pre-requisities](#hardware-pre-requisities)
    2. [Software pre-requisites](#software-pre-requisites)
    3. [Installation](#installation)
    4. [Running example](#running-example)
    5. [Data formats](#data-formats)
# Directory Structure
```
RF-Map
│───README.md
│───build.sh
│───run.sh
│───data
│     └───test.txt
│───log
│     └───test.log
│───CGRA.cpp
│───CGRA.h
│───config.h
│───DFG.cpp
│───DFG.h
│───GraphRegister.cpp
│───GraphRegister.h
│───main.cpp
│───Path.cpp
│───Path.h
│───Register.cpp
│───Register.h
│───tool.cpp
└───tool.h
```

# Getting start
## Hardware pre-requisities
* Ubuntu 20.04.5
* Intel(R) Xeon(R) CPU E5-2650 v4 @ 2.20GHz
## Software pre-requisities
* c++11
* gflags
* glog

## Installation
First, install the gflags
```
git clone https://github.com/gflags/gflags.git
mkdir build
cd build
cmake -DBUILD_SHARED_LIBS=ON -DBUILD_STATIC_LIBS=ON -DINSTALL_HEADERS=ON -DINSTALL_SHARED_LIBS=ON -DINSTALL_STATIC_LIBS=ON ..
make
sudo make install
```
Then, install the glog
```
git clone https://github.com/google/glog.git
cd glog
mkdir build
cmake ..
make
sudo make install
```
Finally, compile this project and you can get ./rf_tcad file
```
./build
```
## Running Example
You can run the example and get the final results in log/test.log
```
./run.sh start
```
Please note that in ./run.sh, you need to modify the following parameters for the DFG that requires map:
* --II (The II of Dfg after scheduling.)
* --childNum (The maximum number of child nodes in DFG.)
* --pea_column (The number of columns of processing element array in RF-CGRA.)
* --pea_row (The number of rows of processing element array in RF-CGRA.)

## Data Formats

Format of DFG scheduling that try to map
```
|----------|------------|-------------|------------|------------|-------------|-------------|
|node index|  time step |node's type  |child node 1|child node 2|edge 1's dif |edge 2's dif |
|----------|------------|-------------|------------|------------|-------------|-------------|
```
An example
```
0,0,1,1,2,3,4,5,6,7,8,9,10,15,17,0,2,256,258,512,514,2,1,514,513,0,512
1,1,0,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,0,0,0,0
2,1,0,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,0,0,0,0
3,1,0,12,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,0,0,0,0
4,1,0,12,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,0,0,0,0
5,2,0,13,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,0,0,0,0
6,2,0,13,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,0,0,0,0
7,1,0,14,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,0,0,0,0
8,1,0,14,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,0,0,0,0
9,1,0,16,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,0,0,0,0
10,1,0,16,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,0,0,0,0
11,2,0,18,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,0,0,0,0
12,2,0,18,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,0,0,0,0
13,3,0,21,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,0,0,0,0
14,2,0,19,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,0,0,0,0
15,1,0,19,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,0,0,0,0
16,2,0,20,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,0,0,0,0
17,2,0,20,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,0,0,0,0
18,3,0,21,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,0,0,0,0
19,3,0,22,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,0,0,0,0
20,3,0,22,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,0,0,0,0
21,4,0,23,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,0,0,0,0
22,4,0,23,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,0,0,0,0
23,5,0,24,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,0,0,0,0
24,6,2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,0,0,0,0
```
Format of DFG mapping
```
----------------------------------------[0]----------------------------------------
|   bank    |   bank    |   LU    |   SU    |   PE    |   PE    |   PE    |   PE    |  
|   bank    |   bank    |   LU    |   SU    |   PE    |   PE    |   PE    |   PE    |   
|   bank    |   bank    |   LU    |   SU    |   PE    |   PE    |   PE    |   PE    |  
|   bank    |   bank    |   LU    |   SU    |   PE    |   PE    |   PE    |   PE    |   
------------------------------------------------------------------------------------
Path[0]:(dependent edge)
pre: (Label of the precursor node) ------  pos: (Label of the successor node) -----  latency: (The latency of edge)
Label of registers
```
An example
```
---------------------------------------[0]---------------------------------------
|       |       |0      |       |       |       |5      |11     |17     |       |
|       |       |       |       |       |       |14     |6      |16     |       |
|       |       |       |       |       |       |12     |21     |       |       |
|       |       |       |       |       |24     |22     |       |       |       |
---------------------------------------[1]---------------------------------------
|       |       |       |       |       |       |1      |15     |2      |10     |
|       |       |       |       |       |       |8      |7      |9      |4      |
|       |       |       |       |       |       |3      |13     |18     |20     |
|       |       |       |       |       |       |19     |23     |       |       |

--------------------------------------------
Path[0]:
pre:0  ------  pos:1  -----  latency:1
2 8 126 127
Path[1]:
pre:0  ------  pos:15  -----  latency:1
2 8 14 132 133
Path[2]:
pre:0  ------  pos:8  -----  latency:3
2 128 38 156 157
Path[3]:
pre:0  ------  pos:2  -----  latency:5
2 128 8 134 20 138 139
Path[4]:
pre:0  ------  pos:7  -----  latency:5
2 128 8 134 44 162 163
Path[5]:
pre:0  ------  pos:3  -----  latency:513
2 123 1 (251*2+1+2)  122 9 129 39 159 68 186 187
Path[6]:
pre:0  ------  pos:4  -----  latency:517
2 123 1 (251*2+1+2)  122 9 129 15 135 21 141 26 146 56 174 175
Path[7]:
pre:0  ------  pos:5  -----  latency:1026
2 123 1 (251*2+1+2)  122 3 150 (255*2+1+2)  4 136 16 129 6 7
Path[8]:
pre:0  ------  pos:17  -----  latency:1026
2 123 1 (251*2+1+2)  122 3 150 (255*2+1+2)  4 136 16 140 18 19
Path[9]:
pre:0  ------  pos:10  -----  latency:1027
2 123 1 (251*2+1+2)  122 3 150 (255*2+1+2)  4 136 16 140 27 144 145
Path[10]:
pre:0  ------  pos:9  -----  latency:1029
2 123 1 (251*2+1+2)  122 3 150 (255*2+1+2)  4 136 16 140 20 170 50 168 169
Path[11]:
pre:0  ------  pos:6  -----  latency:1030
2 123 1 (251*2+1+2)  122 3 150 (255*2+1+2)  4 136 16 130 10 137 17 164 42 43
Path[12]:
pre:1  ------  pos:11  -----  latency:1
127 134 12 13
Path[13]:
pre:2  ------  pos:11  -----  latency:1
139 134 12 13
Path[14]:
pre:3  ------  pos:12  -----  latency:1
187 66 67
Path[15]:
pre:4  ------  pos:12  -----  latency:1
175 170 164 158 188 66 67
Path[16]:
pre:5  ------  pos:13  -----  latency:1
7 14 44 74 192 193
Path[17]:
pre:6  ------  pos:13  -----  latency:1
43 74 192 193
Path[18]:
pre:7  ------  pos:14  -----  latency:1
163 158 36 37
Path[19]:
pre:8  ------  pos:14  -----  latency:1
157 36 37
Path[20]:
pre:9  ------  pos:16  -----  latency:1
169 48 49
Path[21]:
pre:10  ------  pos:16  -----  latency:1
145 140 170 48 49
Path[22]:
pre:11  ------  pos:18  -----  latency:1
13 20 50 80 198 199
Path[23]:
pre:12  ------  pos:18  -----  latency:1
67 74 80 198 199
Path[24]:
pre:13  ------  pos:21  -----  latency:1
193 72 73
Path[25]:
pre:14  ------  pos:19  -----  latency:1
37 68 98 216 217
Path[26]:
pre:15  ------  pos:19  -----  latency:2
133 128 158 188 98 216 217
Path[27]:
pre:16  ------  pos:20  -----  latency:1
49 56 86 204 205
Path[28]:
pre:17  ------  pos:20  -----  latency:1
19 26 57 86 204 205
Path[29]:
pre:18  ------  pos:21  -----  latency:1
199 194 72 73
Path[30]:
pre:19  ------  pos:22  -----  latency:1
217 96 97
Path[31]:
pre:20  ------  pos:22  -----  latency:1
205 200 194 188 218 96 97
Path[32]:
pre:21  ------  pos:23  -----  latency:1
73 104 222 223
Path[33]:
pre:22  ------  pos:23  -----  latency:1
97 104 222 223
Path[34]:
pre:23  ------  pos:24  -----  latency:1
223 95
--------------------------------------------

```

# Reference
```
@inproceedings{inproceedings,
	author = {Liu, Dajiang and Mou, Di and Zhu, Rong and Zhuang, Yan and Shang, Jiaxing and Zhong, Jiang and Yin, Shouyi},
	year = {2023},
	month = {07},
	pages = {1-6},
	title = {DARIC: A Data Reuse-Friendly CGRA for Parallel Data Access via Elastic FIFOs},
	doi = {10.1109/DAC56929.2023.10247862}
}
```

