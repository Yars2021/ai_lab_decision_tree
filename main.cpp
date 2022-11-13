#include <iostream>
#include <fstream>
#include <utility>
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

void printRecord(const Record& record, ostream& sout) {
    sout << record.student_id << ": ";
    for (const string& attribute : record.attributes) sout << attribute << "; ";
    sout << endl;
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
    return get_info(T, CLASS_INDEX) * get_info_x(T, attr) / get_split_info(T, attr);
}

class Node {
private:
    int split_attr;
    vector<Record> subset;
    map<int, Node*> children;
public:
    explicit Node(vector<Record> sub);
    void set_split_attr(size_t split);
    void addChild(int key, Node *child);
    size_t get_split_attr() const;
    vector<Record> getSubset();
    map<int, Node*> getChildren();
    void find_optimal_split(const vector<size_t>& attributes);
    void find_optimal_split(const vector<size_t>& attributes, set<size_t>& used);
    void branch();
    void branch_children(const vector<size_t>& attributes, size_t depth_limit);
    void branch_children(const vector<size_t>& attributes, set<size_t>& used, size_t depth_limit);
    void print(bool print_subsets, size_t offset, ostream& sout) const;
    ~Node();
};

Node::Node(vector<Record> sub) {
    this->split_attr = 0;
    this->subset = std::move(sub);
    this->children = *new map<int, Node*>;
}

void Node::set_split_attr(size_t split) {
    this->split_attr = split;
}

void Node::addChild(int key, Node *child) {
    this->children[key] = child;
}

size_t Node::get_split_attr() const {
    return this->split_attr;
}

vector<Record> Node::getSubset() {
    return this->subset;
}

map<int, Node *> Node::getChildren() {
    return this->children;
}

void Node::find_optimal_split(const vector<size_t>& attributes) {
    size_t optimal = attributes[0];
    for (size_t attribute : attributes)
        if (gain_ratio(this->getSubset(), attribute) > gain_ratio(this->getSubset(), optimal))
            optimal = attribute;
    this->set_split_attr(optimal);
}

void Node::find_optimal_split(const vector<size_t>& attributes, set<size_t>& used) {
    size_t optimal = attributes[0];
    for (size_t attribute : attributes)
        if (gain_ratio(this->getSubset(), attribute) > gain_ratio(this->getSubset(), optimal))
            if (used.empty() || (used.find(attribute) == used.end()))
                optimal = attribute;
    used.insert(optimal);
    this->set_split_attr(optimal);
}

void Node::branch() {
    map<int, vector<Record>> subsets;
    for (const Record& record : this->getSubset())
        subsets[stoi(record.attributes[this->get_split_attr()])].push_back(record);
    for (const auto& attr_pair : subsets)
        this->addChild(attr_pair.first, new Node(attr_pair.second));
}

void Node::branch_children(const vector<size_t>& attributes, size_t depth_limit) {
    for (auto child_pair : this->children) {
        child_pair.second->find_optimal_split(attributes);
        child_pair.second->branch();
        if (depth_limit > 0 && child_pair.second->getSubset().size() > 1)
            child_pair.second->branch_children(attributes, depth_limit - 1);
    }
}

void Node::branch_children(const vector<size_t>& attributes, set<size_t>& used, size_t depth_limit) {
    for (auto child_pair : this->children) {
        child_pair.second->find_optimal_split(attributes, used);
        child_pair.second->branch();
        if (depth_limit > 0 && child_pair.second->getSubset().size() > 1)
            child_pair.second->branch_children(attributes, used, depth_limit - 1);
    }
}

void Node::print(bool print_subsets, size_t offset, ostream& sout) const {
    string tab = string(2 * offset, ' ');
    sout << tab << "Attribute index: " << this->split_attr << endl;
    sout << tab << "Entropy: " << get_info(this->subset, this->split_attr) << endl;
    if (print_subsets) {
        sout << tab << "Subset: " << endl;
        for (const auto& record : this->subset) {
            sout << tab;
            printRecord(record, sout);
        }
    }
    sout << "******************************************************************************************************************" << endl;
    if (!this->children.empty())
        for (auto child_pair : this->children) {
            sout << tab << "Parent attribute value: " << child_pair.first << endl;
            child_pair.second->print(print_subsets, offset + 1, sout);
        }
}

Node::~Node() {
    this->split_attr = 0;
    for (auto child_pair : this->children)
        delete child_pair.second;
}

pair<vector<Record>, vector<Record>> train_test_split(const vector<Record>& dataset, double test_percent) {
    vector<Record> train, test;

    for (size_t i = 0; i < dataset.size(); i++)
        if (i < (size_t)(test_percent * (double) dataset.size()))
            test.push_back(dataset[i]);
        else
            train.push_back(dataset[i]);

    return {train, test};
}

int predict_class(const Record& record, const Node& decision_tree) {
    Node current = decision_tree;
    while (!current.getChildren().empty()) {
        current = *current.getChildren()[stoi(record.attributes[current.get_split_attr()])];
    }
    return stoi(current.getSubset()[0].attributes[CLASS_INDEX]);
}

int main() {
    vector<Record> dataset = readFile("/home/yars/CLionProjects/ai_lab3/DATA.csv");
    ofstream fout("/home/yars/CLionProjects/ai_lab3/tree.txt");
    pair<vector<Record>, vector<Record>> data = train_test_split(dataset, 0.1);
    vector<size_t> attr = getRandomAttributes(NUM_OF_ATTRIBUTES);

    cout << "Selected attributes: " << endl;
    for (size_t a : attr) cout << a << ' ';
    cout << endl << endl;

    Node *root = new Node(dataset);
    root->find_optimal_split(attr);
    root->branch();
    root->branch_children(attr, 10);
    root->print(true, 0, fout);

    double correct = 0;
    for (const auto& rec : data.second) {
        cout << predict_class(rec, *root) << ' ' << rec.attributes[CLASS_INDEX] << endl;
        if (predict_class(rec, *root) == stoi(rec.attributes[CLASS_INDEX]))
            correct++;
    }

    cout << correct / (double)(data.second.size()) << endl;

    delete root;
    fout.close();
    return 0;
}
