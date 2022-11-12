#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <cmath>

using namespace std;

/*
0 - Student ID.
-------------------------------------------------------------------------------------------------------------------------------------------------
1 - Student Age (1: 18-21, 2: 22-25, 3: above 26).
2 - Sex (1: female, 2: male).
3 - Graduated high-school type: (1: private, 2: state, 3: other).
4 - Scholarship type: (1: None, 2: 25%, 3: 50%, 4: 75%, 5: Full).
5 - Additional work: (1: Yes, 2: No).
6 - Regular artistic or sports activity: (1: Yes, 2: No).
7 - Do you have a partner: (1: Yes, 2: No).
8 - Total salary if available (1: USD 135-200, 2: USD 201-270, 3: USD 271-340, 4: USD 341-410, 5: above 410).
9 - Transportation to the university: (1: Bus, 2: Private car/taxi, 3: bicycle, 4: Other).
10 - Accommodation type in Cyprus: (1: rental, 2: dormitory, 3: with family, 4: Other).
11 - Mothers education: (1: primary school, 2: secondary school, 3: high school, 4: university, 5: MSc., 6: Ph.D.).
12 - Fathers education: (1: primary school, 2: secondary school, 3: high school, 4: university, 5: MSc., 6: Ph.D.).
13 - Number of sisters/brothers (if available): (1: 1, 2:, 2, 3: 3, 4: 4, 5: 5 or above).
14 - Parental status: (1: married, 2: divorced, 3: died - one of them or both).
15 - Mothers occupation: (1: retired, 2: housewife, 3: government officer, 4: private sector employee, 5: self-employment, 6: other).
16 - Fathers occupation: (1: retired, 2: government officer, 3: private sector employee, 4: self-employment, 5: other).
17 - Weekly study hours: (1: None, 2: <5 hours, 3: 6-10 hours, 4: 11-20 hours, 5: more than 20 hours).
18 - Reading frequency (non-scientific books/journals): (1: None, 2: Sometimes, 3: Often).
19 - Reading frequency (scientific books/journals): (1: None, 2: Sometimes, 3: Often).
20 - Attendance to the seminars/conferences related to the department: (1: Yes, 2: No).
21 - Impact of your projects/activities on your success: (1: positive, 2: negative, 3: neutral).
22 - Attendance to classes (1: always, 2: sometimes, 3: never).
23 - Preparation to midterm exams 1: (1: alone, 2: with friends, 3: not applicable).
24 - Preparation to midterm exams 2: (1: closest date to the exam, 2: regularly during the semester, 3: never).
25 - Taking notes in classes: (1: never, 2: sometimes, 3: always).
26 - Listening in classes: (1: never, 2: sometimes, 3: always).
27 - Discussion improves my interest and success in the course: (1: never, 2: sometimes, 3: always).
28 - Flip-classroom: (1: not useful, 2: useful, 3: not applicable).
29 - Cumulative grade point average in the last semester (/4.00): (1: <2.00, 2: 2.00-2.49, 3: 2.50-2.99, 4: 3.00-3.49, 5: above 3.49).
30 - Expected Cumulative grade point average in the graduation (/4.00): (1: <2.00, 2: 2.00-2.49, 3: 2.50-2.99, 4: 3.00-3.49, 5: above 3.49).
31 - Course ID.
-------------------------------------------------------------------------------------------------------------------------------------------------
32 - OUTPUT Grade (0: Fail, 1: DD, 2: DC, 3: CC, 4: CB, 5: BB, 6: BA, 7: AA). (0-2 - fail, 3-7 - success)
*/

const size_t NUM_OF_ATTRIBUTES = 32;
const size_t CLASS_INDEX = NUM_OF_ATTRIBUTES - 1;
const size_t BARRIER_GRADE = 3;

typedef struct {
    string student_id;
    string attributes[NUM_OF_ATTRIBUTES];
} Record;

void printRecord(const Record& record) {
    cout << record.student_id << ": ";
    for (const string& attribute : record.attributes) cout << attribute << "; ";
    cout << endl;
}

Record readRecord(const string& line) {
    Record record;
    size_t attr_n = 0;

    for (size_t i = 0; i < line.length(); i++) {
        string token;
        for (size_t j = i; j < line.length() && line[j] != ';'; i++, j++) token += line[j];
        if (attr_n == 0) record.student_id = token;
        else record.attributes[attr_n - 1] = token;
        attr_n++;
    }

    return record;
}

int getGrade(const Record& record) {
    return stoi(record.attributes[CLASS_INDEX]);
}

void evaluateByGrade(Record &record, int barrier) {
    record.attributes[CLASS_INDEX] = getGrade(record) >= barrier ? "1" : "0";
}

vector<Record> readFile(const string& filename) {
    ifstream fin(filename);

    vector<Record> records;
    string line;
    size_t line_i = 0;

    while (getline(fin, line)) {
        if (line_i > 0) records.push_back(readRecord(line));
        line_i++;
    }

    fin.close();

    for (auto &record : records) evaluateByGrade(record, BARRIER_GRADE);

    return records;
}

vector<size_t> getRandomAttributes(size_t N) {
    vector<size_t> attributes;
    set<size_t> used;
    srandom(time(nullptr));
    size_t attr = random() % N;

    for (size_t i = 0; i < (size_t) ceil(sqrt(N)); i++) {
        while (used.find(attr) != used.end() && !used.empty()) attr = random() % N;
        used.insert(attr);
        attributes.push_back(attr);
    }

    return attributes;
}


double get_info(const vector<Record>& T, size_t attr) {
    map<int, int> attr_freq;
    double entropy = 0;
    for (const Record& record : T) attr_freq[stoi(record.attributes[attr])]++;
    for (auto Class : attr_freq)
        entropy += (attr_freq[Class.first] / (double) T.size()) * log2(attr_freq[Class.first] / (double) T.size());
    return -entropy;
}

double get_info_x(const vector<Record>& T, size_t attr) {
    map<int, vector<Record>> subsets;
    double entropy = 0;
    for (const Record& record : T) subsets[stoi(record.attributes[attr])].push_back(record);
    for (const auto& subset_pair : subsets)
        entropy += (((double) subset_pair.second.size() / (double) T.size()) * get_info(subset_pair.second, CLASS_INDEX));
    return entropy;
}

double get_split_info(const vector<Record>& T, size_t attr) {
    map<int, int> subset_lengths;
    double length_ratio, info = 0;
    for (const Record& record : T) subset_lengths[stoi(record.attributes[attr])]++;
    for (auto subset_pair : subset_lengths) {
        length_ratio = (double) subset_pair.second / (double) T.size();
        info += (length_ratio * log2(length_ratio));
    }
    return -info;
}

double gain_ratio(const vector<Record>& T, size_t attr) {
    return get_info(T, attr) * get_info_x(T, attr) / get_split_info(T, attr);
}

size_t find_optimal_split(const vector<Record>& T) {
    size_t optimal = 0;
    for (size_t i = 0; i < NUM_OF_ATTRIBUTES - 1; i++)
        if (gain_ratio(T, i) > gain_ratio(T, optimal))
            optimal = i;
    return optimal;
}

int main() {
    vector<Record> dataset = readFile("/home/yars/CLionProjects/ai_lab3/DATA.csv");
    cout << find_optimal_split(dataset);
    return 0;
}
