#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <array>
#include <algorithm>
#include <sstream>
#include <cmath>

using namespace std;
using MATRIX = vector<vector<int>>;

// used for goto
HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
COORD CursorPosition;

// used for printing all elements of a 1D vector
template <class T>
void PrintData1(vector<T> &data)
{
    cout << endl;
    for (auto num : data)
        cout << num << endl;
    cout << endl;
}

// used for printing all elements of a 2D vector
template <class T>
void PrintData2(vector<vector<T>> &data)
{
    for (auto row : data)
    {
        for (auto num : row)
            cout << num << ", ";
        cout << endl;
    }
}

class bestSplitValues
{
public:
    float resEntropy; // Entropy Value for each Node
    int Feature;      // Col in terms of data
};

class TreeNode
{
public:
    MATRIX trainData;
    bestSplitValues bestSplit;

    TreeNode *leftChild;
    TreeNode *rightChild;
    static vector<int> selectedFeature;

    TreeNode(MATRIX &data)
    {
        this->leftChild = this->rightChild = NULL;
        this->trainData = data;
        pair<int, float> r = GetInfoGain(trainData);
        this->bestSplit.Feature = r.first;
        this->bestSplit.resEntropy = r.second;
    }
    vector<pair<int, int>> CountResults(MATRIX &Data, int index)
    {
        vector<pair<int, int>> vp;
        int count0{0}, count1{0};
        if (index <= 0)
        {
            count0 = count1 = 0;
            for (int i = 0; i < Data.size(); i++)
            {
                count0 += (Data[i][0] == 0) ? 1 : 0;
                count1 += (Data[i][0] == 1) ? 1 : 0;
            }
            vp.push_back({count1, count0});
        }
        else
        {
            map<int, int> countLabels;
            map<int, int> countAll;
            for (int i = 0; i < Data.size(); i++)
            {
                countAll[Data[i][index]]++;
                countLabels[Data[i][index]] += Data[i][0];
            }
            for (auto i = countLabels.begin(), j = countAll.begin(); 
                i != countLabels.end() && j != countAll.end(); i++, j++)
            {
                count1 = i->second;
                count0 = j->second - i->second;
                vp.push_back({count1, count0});
            }
        }
        return vp;
    }
    double EntropyOfCol(pair<int, int> p)
    {
        double total = double(p.first + p.second);
        double ret = -1 * (double(p.first / total) * log2(double(p.first / total))) - 1 
                                * (double(p.second / total) * log2(double(p.second / total)));
        return (isnan(ret)) ? 0 : ret;
    }
    float ColGain(MATRIX &data, int index)
    {
        float TotalColEntropy = EntropyOfCol(CountResults(data, 0)[0]);
        if (index == 0)
        {
            return TotalColEntropy;
        }
        vector<float> cal;
        vector<pair<int, int>> vp = CountResults(data, index);
        for (auto n : vp)
        {
            int total = n.first + n.second;
            float prob = (float)total / data.size();
            cal.push_back(-1 * float(prob) * EntropyOfCol(n));
        }
        float ret = TotalColEntropy;
        for (auto n : cal)
        {
            ret += n;
        }
        return ret;
    }
    pair<int, float> GetInfoGain(MATRIX &data)
    {
        if (data.size() == 0)
        {
            return {-1, 0.0f};
        }
        vector<float> cal;
        map<int, float> calM;
        int maxIndex = -1;

        for (int i = 1; i <= data[0].size(); i++)
        {
            calM[i] = ColGain(data, i);
        }

        float MaxRet = 0.f;
        for (auto n : calM)
        {
            if (n.second > MaxRet)
            {
                MaxRet = n.second;
                maxIndex = n.first;
            }
        }
        selectedFeature.push_back(maxIndex);
        return {maxIndex, MaxRet};
    }
    vector<int> GetColumn(MATRIX &dataT, int col)
    {
        vector<int> ret;
        for (int i = 0; i < dataT.size(); i++)
        {
            ret.push_back(dataT[i][col]);
        }
        return ret;
    }
};

vector<string> GetColumn(vector<vector<string>> &dataT, int col)
{
    vector<string> ret;
    for (int i = 0; i < dataT.size(); i++)
    {
        ret.push_back(dataT[i][col]);
    }
    return ret;
}
vector<int> TreeNode::selectedFeature;

bool isIdentical(string c, vector<string> &alreadyE)
{
    for (auto n : alreadyE)
    {
        if (c == n)
            return true;
    }
    alreadyE.push_back(c);
    return false;
}

class Data_Retrieval
{
public:
    Data_Retrieval(){

    };
    vector<vector<string>> RETRIEVE_DATA(string filename)
    {
        // Code For Converting .csv data to strings (vector:2D)
        vector<vector<string>> record;
        vector<string> row;
        string line, data;

        fstream file(filename, ios::in);
        if (file.is_open())
        {
            while (getline(file, line))
            {
                row.clear();

                stringstream s(line);
                while (getline(s, data, ','))
                {
                    row.push_back(data);
                }
                record.push_back(row);
            }
        }
        else
        {
            cout << "Could not Open file";
        }
        return record;
    }
    MATRIX Bit_Mask(vector<vector<string>> &data)
    {

        MATRIX cvt(data.size(), vector<int>(data[0].size() - 1, 0));
        int index = 1;
        int row = 0, col = 0;

        for (index = 1; index < data[0].size(); index++)
        {
            row = 0;
            int val = 0;
            vector<string> temp;
            map<string, int> mp;
            for (int i = 0; i < data.size(); i++)
            {
                if (isIdentical(data[i][index], temp) == 0)
                {
                    mp[data[i][index]] = i;
                }
                cvt[row][col] = mp[data[i][index]];
                row++;
            }
            col++; // next Col
        }
        return cvt;
    }
};

bool remove_column(MATRIX &a, int pos)
{
    bool success = pos < a[0].size() && a.size() > 0;
    if (success)
    {
        MATRIX temp;
        vector<int> temp2;
        for (int i = 0; i < a.size(); i++)
        {
            temp2.clear();
            for (int j = 0; j < a[0].size(); j++)
            {
                if (j + 1 == pos)
                {
                    continue;
                }
                temp2.push_back(a[i][j]);
            }
            temp.push_back(temp2);
        }
        a.clear();
        a.resize(temp.size(), vector<int>(temp[0].size()));
        for (int i = 0; i < temp.size(); i++)
        {
            for (int j = 0; j < temp[0].size(); j++)
            {
                a[i][j] = temp[i][j];
            }
        }
    }

    else
    {
        a.clear();
        a.resize(0, vector<int>(0));
    }
    return success;
}

class TwoMatrix
{
public:
    MATRIX accepted;
    MATRIX rejected;
};

int DATACOUNTER = 1;
TwoMatrix Split_Data(MATRIX &tempData, int f)
{
    DATACOUNTER++;
    MATRIX Accepted;
    MATRIX Rejected;
    Accepted.clear();
    Rejected.clear();
    for (int i = 0; i < tempData.size(); i++)
    {
        if (tempData[i][f] == 0)
            Accepted.push_back(tempData[i]);
        else if (tempData[i][f] == 1)
            Rejected.push_back(tempData[i]);
    }
    TwoMatrix tw;
    if (remove_column(Accepted, f))
    {
        tw.accepted = Accepted;
    }
    else
    {
        tw.accepted.clear();
        tw.accepted.resize(0, vector<int>(0));
    }

    if (remove_column(Rejected, f))
    {
        tw.rejected = Rejected;
    }
    else
    {
        tw.rejected.clear();
        tw.rejected.resize(0, vector<int>(0));
    }
    return tw;
}

class DecisionTree
{
public:
    Data_Retrieval DR;
    TreeNode *root;
    vector<vector<string>> DataInString;
    DecisionTree(string filename = "TEMP.csv")
    {
        // This Constructor Will Fetch The Data and convert to integers
        this->DataInString = DR.RETRIEVE_DATA(filename);
        MATRIX DataForTraining = DR.Bit_Mask(DataInString);
        TreeNode *rootC = new TreeNode(DataForTraining);
        pair<int, float> r = rootC->GetInfoGain(rootC->trainData);
        rootC->bestSplit.Feature = r.first;
        rootC->bestSplit.resEntropy = r.second;
        this->root = rootC;
        ConstructTree(root);
    }

    static void ConstructTree(TreeNode *node)
    {
        pair<int, float> infoBefore = node->GetInfoGain(node->trainData);

        if (node->bestSplit.Feature > 0 && node->bestSplit.resEntropy > 0.f)
        {
            TwoMatrix children = Split_Data(node->trainData, node->bestSplit.Feature);
            system("cls");

            if (children.accepted.size() > 0)
            {
                TreeNode *rightChild = new TreeNode(children.accepted);
                node->rightChild = rightChild;
                DecisionTree::ConstructTree(node->rightChild);
            }
            if (children.rejected.size() > 0)
            {
                TreeNode *leftChild = new TreeNode(children.rejected);
                node->leftChild = leftChild;
                DecisionTree::ConstructTree(node->leftChild);
            }
        }
    }
    static int Predicts(TreeNode *node, vector<int> targets)
    {
        int obsCat = targets[node->bestSplit.Feature];
        int predict = -1;
        if (node->leftChild == NULL && (node->rightChild == NULL))
        {
            // THIS IS A LEAF NODE :: THE OUTCOME
            predict = (node->trainData)[0][0];
        }
        else if (obsCat == 0)
        {
            predict = DecisionTree::Predicts(node->rightChild, targets);
        }
        else
        {
            predict = DecisionTree::Predicts(node->leftChild, targets);
        }
        return predict;
    }
    void InOrder(TreeNode *p)
    {
        if (p != NULL)
        {
            InOrder(p->leftChild);
            cout << p->bestSplit.Feature << ", " << p->bestSplit.resEntropy << endl;
            InOrder(p->rightChild);
        }
    }
};

class menu
{
public:
    string userrest;
    int userbudget, userreserv, usertiming, usercuisine;
    DecisionTree DT;

    menu()
    {
        system("cls");
        ccolor(15);
        cout << endl
             << endl;
        system("cls");
        dname();
        userinput();
    };

    void restlist(vector<string> names);
    void loadques();
    void ccolor(int);
    void userinput();
    void showcredits();
    void dname();
    void mainmenu();
    void timingmenu();
    void reservmenu();
    void budgetmenu();
    void cuisinemenu();
    void PrintList(vector<string> &data);
};

void gotoXY(int x, int y)
{
    CursorPosition.X = x;
    CursorPosition.Y = y;
    SetConsoleCursorPosition(console, CursorPosition);
}

int main(int argc, char const *argv[])
{
    menu M;
}

void menu::PrintList(vector<string> &data)
{

    int i = 6;
    for (auto num : data)
    {
        i++;
        gotoXY(47, i);
        cout << num << endl;
    }
    cout << endl;
}
void menu::restlist(vector<string> names)
{

    int menu_item = 0, run, x = 7;
    bool running = true;

    gotoXY(50, 5);
    cout << "All Restaurants";
    gotoXY(45, 7);
    cout << "->";

    while (running)
    {
        gotoXY(47, 7);
        sort(names.begin(), names.end());
        auto it = unique(names.begin(), names.end());
        names.erase(it, names.end());
        PrintList(names);

        system("pause>nul"); // the >nul bit causes it the print no message

        if (GetAsyncKeyState(VK_DOWN) && x != 35) // down button pressed
        {
            gotoXY(45, x);
            cout << "  ";
            x++;
            gotoXY(45, x);
            cout << "->";
            menu_item++;
            continue;
        }

        if (GetAsyncKeyState(VK_UP) && x != 7) // up button pressed
        {
            gotoXY(45, x);
            cout << "  ";
            x--;
            gotoXY(45, x);
            cout << "->";

            menu_item--;
            continue;
        }

        if (GetAsyncKeyState(VK_RETURN))
        { // Enter key pressed

            switch (menu_item)
            {
            case 0:
            {
                userrest = "Al Kabab";
                system("cls");
                dname();
                cuisinemenu();
                break;
            }
            case 1:
            {
                userrest = "Aussies";
                system("cls");
                dname();
                cuisinemenu();
                break;
            }
            case 2:
            {
                userrest = "Burger o'clock";
                system("cls");
                dname();
                cuisinemenu();
                break;
            }
            case 3:
            {
                userrest = "Coconut Grove";
                system("cls");
                dname();
                cuisinemenu();
                break;
            }
            case 4:
            {
                userrest = "Dominos";
                system("cls");
                dname();
                cuisinemenu();
                break;
            }
            case 5:
            {
                userrest = "Ghalib";
                system("cls");
                dname();
                cuisinemenu();
                break;
            }
            case 6:
            {
                userrest = "Lal Qila";
                system("cls");
                dname();
                cuisinemenu();
                break;
            }
            case 7:
            {
                userrest = "Meat the Cheese";
                system("cls");
                dname();
                cuisinemenu();
                break;
            }
            case 8:
            {
                userrest = "No Lies Fries";
                system("cls");
                dname();
                cuisinemenu();
                break;
            }
            case 9:
            {
                userrest = "Oh my Grill";
                system("cls");
                dname();
                cuisinemenu();
                break;
            }
            case 10:
            {
                userrest = "SAGKH";
                system("cls");
                dname();
                cuisinemenu();
                break;
            }
            case 11:
            {
                userrest = "Xander's";
                system("cls");
                dname();
                cuisinemenu();
                break;
            }
            default:
            {
                cuisinemenu();
                break;
            }
            }
        }
    }
    gotoXY(20, 21);
}
void menu::dname()
{
    ccolor(4);
    gotoXY(43, 0);
    cout << "Restaurant Recommender";
    ccolor(15);
}
void menu::ccolor(int clr)
{

    HANDLE hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, clr);
}
void menu::userinput()
{

    cout << endl
         << endl
         << "\t\t\t\t\t\tWelcome User!";
    Sleep(1000);
    system("cls");
    dname();
    cout << endl
         << endl
         << "\t\t";
    loadques();
    Sleep(500);
    system("cls");
    dname();
    mainmenu();
}
void menu::loadques()
{
    string message = "Just answer a few questions for us and we will pick out the perfect restaurant for you!!";
    int sleep = 0 + (rand() % 100);
    for(int i = 0; i < message.length(); i++) {
        cout << message[i];
        if(message[i] == ' ') continue;
        Sleep(sleep);
    }
}
void menu::showcredits()
{
    cout << "by ";
    ccolor(10);
    cout << "Khair";
    Sleep(500);
    cout << " ";
    ccolor(12);
    cout << "Faseeh";
    Sleep(500);
    cout << " ";
    ccolor(14);
    cout << "Anas";
    Sleep(500);
    ccolor(15);
}
bool sortcol(const vector<string> &v1, const vector<string> &v2)
{
    return v1[0] < v2[0];
}
void printacceptedreslist(vector<vector<string>> data, int uc)
{

    string dupcheck;
    sort(data.begin(), data.end(), sortcol);
    string cuisine;
    if (uc == 1)
    {
        cuisine = "Desi";
    }
    else
    {
        cuisine = "American";
    }
    int j = 11;

    for (int i = 0; i < data.size(); i++)
    {
        if (data[i][4] == cuisine && data[i][1] == "1" && data[i][0] != dupcheck)
        {
            gotoXY(38, j++);
            cout << data[i][0] << endl;
            dupcheck = data[i][0];
        }
    }
}
void menu::budgetmenu()
{
    int menu_item = 0, run, x = 7;
    bool running = true;

    gotoXY(45, 5);
    cout << "What Is Your Budget";
    gotoXY(45, 7);
    cout << "->";

    while (running)
    {
        gotoXY(47, 7);
        cout << "1) Above 5000 rupees";
        gotoXY(47, 8);
        cout << "2) Below 5000 rupees";
        gotoXY(47, 9);
        cout << "3) Go Back To Main Menu";
        system("pause>nul"); // the >nul bit causes it the print no message

        if (GetAsyncKeyState(VK_DOWN) && x != 9) // down button pressed
        {
            gotoXY(45, x);
            cout << "  ";
            x++;
            gotoXY(45, x);
            cout << "->";
            menu_item++;
            continue;
        }

        if (GetAsyncKeyState(VK_UP) && x != 7) // up button pressed
        {
            gotoXY(45, x);
            cout << "  ";
            x--;
            gotoXY(45, x);
            cout << "->";

            menu_item--;
            continue;
        }

        int predict = -1;

        if (GetAsyncKeyState(VK_RETURN))
        { // Enter key pressed
            vector<int> ans;
            switch (menu_item)
            {

            case 0:
            {
                system("cls");
                dname();
                userbudget = 1;
                gotoXY(20, 21);
                system("cls");
                ans.push_back(userreserv);
                ans.push_back(userbudget);
                ans.push_back(usercuisine);
                ans.push_back(usertiming);
                running = false;
                break;
            }
            case 1:
            {
                system("cls");
                dname();
                userbudget = 0;
                gotoXY(20, 21);
                system("cls");
                ans.push_back(userreserv);
                ans.push_back(userbudget);
                ans.push_back(usercuisine);
                ans.push_back(usertiming);
                running = false;
                break;
            }
            case 2:
            {
                running = false;
                system("cls");
                dname();
                mainmenu();
                break;
            }
            }
            predict = DT.Predicts(DT.root, ans);
            if (predict == 0)
            {
                string printcuisine;
                if (usercuisine = 1)
                    printcuisine = "Desi";
                else
                    printcuisine = "American";

                system("cls");
                dname();
                gotoXY(39, 4);
                cout << userrest << " is not recommended for you! " << endl
                     << endl;
                gotoXY(37, 9);
                cout << "Below are some " << printcuisine << " recommendation for you : " << endl;
                printacceptedreslist(DT.DataInString, usercuisine);
                exit(0);
            }
            else if (predict == 1)
            {
                system("cls");
                dname();
                gotoXY(35, 4);
                cout << userrest << " is recommended for you. you are good to go!" << endl;
                exit(0);
            }
            else
            {
                cout << "out of order " << endl;
                mainmenu();
            }
        }
    }

    gotoXY(20, 21);
}
void menu::reservmenu()
{
    int menu_item = 0, run, x = 7;
    bool running = true;

    gotoXY(45, 5);
    cout << "What You Prefer Reservation";
    gotoXY(45, 7);
    cout << "->";

    while (running)
    {
        gotoXY(47, 7);
        cout << "1) Yes";
        gotoXY(47, 8);

        cout << "2) No";
        gotoXY(47, 9);
        cout << "3) Go Back To Main Menu";
        system("pause>nul"); // the >nul bit causes it the print no message

        if (GetAsyncKeyState(VK_DOWN) && x != 9) // down button pressed
        {
            gotoXY(45, x);
            cout << "  ";
            x++;
            gotoXY(45, x);
            cout << "->";
            menu_item++;
            continue;
        }

        if (GetAsyncKeyState(VK_UP) && x != 7) // up button pressed
        {
            gotoXY(45, x);
            cout << "  ";
            x--;
            gotoXY(45, x);
            cout << "->";

            menu_item--;
            continue;
        }

        if (GetAsyncKeyState(VK_RETURN))
        { // Enter key pressed

            switch (menu_item)
            {

            case 0:
            {
                system("cls");
                dname();
                userreserv = 1;
                timingmenu();
                running = false;
                break;
            }
            case 1:
            {
                system("cls");
                dname();
                userreserv = 0;
                timingmenu();
                running = false;
                break;
            }
            case 2:
            {
                running = false;
                system("cls");
                dname();
                mainmenu();
                break;
            }
            }
        }
    }

    gotoXY(20, 21);
}
void menu::timingmenu()
{
    int menu_item = 0, run, x = 7;
    bool running = true;

    gotoXY(45, 5);
    cout << "What Timing Would You Prefer";
    gotoXY(45, 7);
    cout << "->";

    while (running)
    {
        gotoXY(47, 7);
        cout << "1) Lunch";
        gotoXY(47, 8);
        cout << "2) Dinner";
        gotoXY(47, 9);
        cout << "3) Go Back to Main Menu";
        system("pause>nul"); // the >nul bit causes it the print no message

        if (GetAsyncKeyState(VK_DOWN) && x != 9) // down button pressed
        {
            gotoXY(45, x);
            cout << "  ";
            x++;
            gotoXY(45, x);
            cout << "->";
            menu_item++;
            continue;
        }

        if (GetAsyncKeyState(VK_UP) && x != 7) // up button pressed
        {
            gotoXY(45, x);
            cout << "  ";
            x--;
            gotoXY(45, x);
            cout << "->";

            menu_item--;
            continue;
        }

        if (GetAsyncKeyState(VK_RETURN))
        { // Enter key pressed

            switch (menu_item)
            {

            case 0:
            {
                system("cls");
                dname();
                usertiming = 1;
                budgetmenu();
                running = false;
                break;
            }
            case 1:
            {
                system("cls");
                dname();
                usertiming = 0;
                budgetmenu();
                running = false;
                break;
            }
            case 2:
            {
                running = false;
                system("cls");
                dname();
                mainmenu();
                break;
            }
            }
        }
    }

    gotoXY(20, 21);
}
void menu::cuisinemenu()
{
    int menu_item = 0, run, x = 7;
    bool running = true;

    gotoXY(45, 5);
    cout << "What Cuisine Would You Prefer";
    gotoXY(45, 7);
    cout << "->";

    while (running)
    {
        gotoXY(47, 7);
        cout << "1) American ";
        gotoXY(47, 8);
        cout << "2) Desi";
        gotoXY(47, 9);
        cout << "3) Main Menu";
        system("pause>nul"); // the >nul bit causes it the print no message

        if (GetAsyncKeyState(VK_DOWN) && x != 9) // down button pressed
        {
            gotoXY(45, x);
            cout << "  ";
            x++;
            gotoXY(45, x);
            cout << "->";
            menu_item++;
            continue;
        }

        if (GetAsyncKeyState(VK_UP) && x != 7) // up button pressed
        {
            gotoXY(45, x);
            cout << "  ";
            x--;
            gotoXY(45, x);
            cout << "->";

            menu_item--;
            continue;
        }

        if (GetAsyncKeyState(VK_RETURN))
        { // Enter key pressed

            switch (menu_item)
            {

            case 0:
            {
                system("cls");
                dname();
                usercuisine = 0;
                reservmenu();
                running = false;
                break;
            }
            case 1:
            {
                system("cls");
                dname();
                usercuisine = 1;
                reservmenu();
                running = false;
                break;
            }
            case 2:
            {
                running = false;
                system("cls");
                dname();
                mainmenu();
                break;
            }
            }
        }
    }

    gotoXY(20, 21);
}
void menu::mainmenu()
{

    int menu_item = 0, run, x = 7;
    bool running = true;

    gotoXY(50, 5);
    cout << "Main Menu";
    gotoXY(45, 7);
    cout << "->";

    while (running)
    {
        gotoXY(47, 7);
        cout << "1) View All Restaurants !";
        gotoXY(47, 8);
        cout << "2) Quit Program";

        system("pause>nul"); // the >nul bit causes it the print no message

        if (GetAsyncKeyState(VK_DOWN) && x != 8) // down button pressed
        {
            gotoXY(45, x);
            cout << "  ";
            x++;
            gotoXY(45, x);
            cout << "->";
            menu_item++;
            continue;
        }

        if (GetAsyncKeyState(VK_UP) && x != 7) // up button pressed
        {
            gotoXY(45, x);
            cout << "  ";
            x--;
            gotoXY(45, x);
            cout << "->";

            menu_item--;
            continue;
        }

        if (GetAsyncKeyState(VK_RETURN))
        { // Enter key pressed

            switch (menu_item)
            {

            case 0:
            {
                gotoXY(47, 16);
                cout << "Finding Restaurants !";
                Sleep(100);
                cout << " \b.";
                for (int i = 0; i < 3; i++)
                {
                    Sleep(400);
                    cout << " \b.";
                }
                system("cls");
                dname();
                vector<string> gg = GetColumn(DT.DataInString, 0);
                restlist(gg);
                cuisinemenu();
                break;
            }
            case 1:
            {
                gotoXY(47, 16);
                cout << "The program has now terminated!!";
                exit(0);
                running = false;
            }
            }
        }
    }

    gotoXY(20, 21);
}