#include <bits/stdc++.h>
using namespace std;
vector<string> tables;
class SQL
{
public:
    vector<string> keywords = {"select", "create", "table", "alter", "drop", "desc", "describe", "*", "insert", "delete", "where", "orderby", "from", "values", "into", "and", "or", "between", "in", "references", "set", "distinct", "null", "not", "(", ")", ";"};
    vector<string> types = {"int", "varchar", "float"};
    vector<string> command;
    vector<string> constraints = {"primary_key", "foreign_key", "unique", "check", "not_null", "default", "references"};
    ifstream ip_file;
    ofstream op_file, datatype, col_names, constraint, constrLength;

    SQL(string query)
    {
        read_schema();
        command = split(query);
        // for (int i = 0; i < command.size(); i++)
        // {
        //     cout << command[i] << endl;
        // }
        execute(command);
    }

    vector<string> split(string s)
    {
        stringstream stringStream(s);
        string line;
        vector<string> wordVector;
        while (getline(stringStream, line))
        {
            size_t prev = 0, pos;
            while ((pos = line.find_first_of(" (),#", prev)) != string ::npos)
            {
                if (pos > prev)
                    wordVector.push_back(line.substr(prev, pos - prev));
                prev = pos + 1;
            }
            if (prev < line.length())
                wordVector.push_back(line.substr(prev, string ::npos));
        }

        // for (int i = 0; i < wordVector.size(); i++)
        // {
        //     cout << wordVector[i] << " ";
        // }
        return wordVector;
    }

    bool isNumber(const string &str)
    {
        for (char const &c : str)
        {
            if (std::isdigit(c) == 0)
                return false;
        }
        return true;
    }

    int getIndex(vector<string> v, string K)
    {
        auto it = find(v.begin(), v.end(), K);
        if (it != v.end())
        {
            int index = it - v.begin();
            return index;
        }
        return -1;
    }

    fstream &GotoLine(fstream &file, unsigned int num)
    {
        file.seekg(ios::beg);
        for (int i = 0; i < num - 1; ++i)
        {
            file.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        return file;
    }

    // Read until no more lines in text file to read
    void execute(vector<string> s);
    void create(vector<string> s);
    void insert(vector<string> s);
    void desc(vector<string> s);
    vector<vector<string>> splitFile(fstream &, string, bool);
    bool operation(vector<int> values, string command, string table_name, int attr_no);
    bool isInteger(string command);
    bool isFloat(string command);
    bool checkDuplicates(string command, string table);
    void dropTable(string tableName);
    void replaceFiles(string fileName, string tableName);
    void select(vector<string> command);
    void fetchAll(string table);
    void alterTable(vector<string> command);
    void alterFiles(vector<string> command, int index, int flag);
    void updateFiles(string fileName, int index, vector<string> append_str);

    void read_schema()
    {
        ip_file.open("Schema.txt", ios ::in);
        // char c = ' ';
        string table;
        // vector<string> tables;
        while (getline(ip_file, table))
        {
            int i = 0;
            string temp;
            while (table[i] != '#')
            {
                temp.push_back(table[i]);
                i++;
            }
            tables.push_back(temp);
        }
        ip_file.close();
    }

    string lower(string upTxt)
    {
        std::transform(upTxt.begin(), upTxt.end(), upTxt.begin(), ::tolower);
        return upTxt;
    }
    bool notin(string s, vector<string> c)
    {
        for (int i = 0; i < c.size(); i++)
        {
            if (s == c[i])
                return false;
        }
        return true;
    }
};

void SQL ::execute(vector<string> command)
{
    if (lower(command[0]) == "create")
    {
        cout << "create()";
        create(command);
    }
    else if (lower(command[0]) == "select")
    {
        cout << "select()\n";
        select(command);
    }
    else if (lower(command[0]) == "insert")
    {
        // cout << "insert()";
        insert(command);
    }
    else if (lower(command[0]) == "desc" || lower(command[0]) == "describe")
    {
        // cout << "desc()";
        desc(command);
    }
    else if (lower(command[0]) == "update")
    {
        cout << "update()";
    }
    else if (lower(command[0]) == "delete")
    {
        cout << "delete()";
    }
    else if (lower(command[0]) == "drop")
    {
        cout << "drop()";
        dropTable(command[2]);
    }
    else if (lower(command[0]) == "alter")
    {
        cout << "alter()";
    }
    else
    {
        cout << "Error" << endl;
    }
}
void SQL::create(vector<string> command)
{
    op_file.open("Schema.txt", ios::app | ios::out);
    op_file << command[2];
    col_names.open("Column.txt", ios::app | ios::out);
    datatype.open("Datatype.txt", ios::app | ios::out);
    constraint.open("Constraint.txt", ios::app | ios::out);
    constrLength.open("ConstrLength.txt", ios::app | ios::out);
    col_names << command[2] << "#";
    datatype << command[2] << "#";
    constraint << command[2] << "#";
    constrLength << command[2] << "#";
    tables.push_back(command[2]);
    vector<string> att_name, typ, constr;
    unordered_map<string, vector<string>> att_constr;
    unordered_map<string, vector<int>> att_length;

    for (int i = 3; i < command.size(); i++)
    {
        if (notin(lower(command[i]), keywords))
        {
            if (isNumber(command[i]))
            {
                if (att_length[att_name[att_name.size() - 1]].size() == 1 && att_length[att_name[att_name.size() - 1]][0] == 0)
                {
                    att_length[att_name[att_name.size() - 1]] = {stoi(command[i])};
                }

                // id --> 4 2
                // name --> 25
                else
                    att_length[att_name[att_name.size() - 1]].push_back(stoi(command[i]));
            }
            else if (notin(lower(command[i]), types) && notin(lower(command[i]), constraints))
            {
                att_name.push_back(command[i]);
                att_constr[command[i]] = {"NA"};
                att_length[command[i]] = {0};
            }
            else if (notin(lower(command[i]), types) && !notin(lower(command[i]), constraints))
            {
                constr.push_back(command[i]);
                if (att_constr[att_name[att_name.size() - 1]].size() == 1 && att_constr[att_name[att_name.size() - 1]][0] == "NA")
                {
                    att_constr[att_name[att_name.size() - 1]] = {command[i]};
                }
                else
                    att_constr[att_name[att_name.size() - 1]].push_back(command[i]);
            }
            else
            {
                typ.push_back(command[i]);
            }
        }
    }
    cout << "Table created successfully" << endl;

    for (int i = 0; i < att_name.size(); i++)
    {
        op_file << "#" << att_name[i];
        col_names << att_name[i] << ",";
        // cout << att_name[i] << "#";
        op_file << "#" << typ[i];
        datatype << typ[i] << ",";
        // cout << typ[i] << "#";
        for (int j = 0; j < att_constr[att_name[i]].size(); j++)
            constraint << att_constr[att_name[i]][j] << "-";
        constraint << ",";

        for (int j = 0; j < att_length[att_name[i]].size(); j++)
            constrLength << att_length[att_name[i]][j] << "-";
        constrLength << ",";
    }

    op_file << endl;
    col_names << endl;
    datatype << endl;
    constraint << endl;
    constrLength << endl;
    op_file.close();
    col_names.close();
    datatype.close();
    constraint.close();
    constrLength.close();
}
void SQL::insert(vector<string> command)
{
    // op_file.open("Schema.txt", ios::app | ios::out);

    if (notin(lower(command[2]), tables))
    {
        cout << "Error : No such table created\n";
    }
    else
    {
        ofstream op_table;
        fstream f1("ConstrLength.txt");
        fstream f2("Column.txt");
        fstream f3("Constraint.txt");
        fstream f4("Datatype.txt");
        string table_name = command[2];
        vector<vector<string>> Length, Constraint, Datatype;
        op_table.open(command[2] + ".txt", ios::app | ios::out);
        unordered_map<string, vector<vector<string>>> attribute_constrints;
        map<int, vector<int>> attr_table;

        Length = splitFile(f1, command[2], true);
        Constraint = splitFile(f3, command[2], true);
        Datatype = splitFile(f4, command[2], false);

        // add values in map attr_table
        for (int i = 1; i <= Length.size(); i++)
        {
            vector<int> boolValues(11, 0);
            // Int Float Varchar Number(Length) Precisoin PK FK NotNUll Unique check default
            if (lower(Datatype[i - 1][0]) == "int")
                boolValues[0] = 1;
            else if (lower(Datatype[i - 1][0]) == "float")
                boolValues[1] = 1;
            else if (lower(Datatype[i - 1][0]) == "varchar")
                boolValues[2] = 1;

            boolValues[3] = stoi(Length[i - 1][0]);
            if (Length[i - 1].size() == 2)
                boolValues[4] = stoi(Length[i - 1][1]);

            if (lower(Constraint[i - 1][0]) == "primary_key")
                boolValues[5] = 1;
            if (lower(Constraint[i - 1][0]) == "foreign_key")
                boolValues[6] = 1;
            if (lower(Constraint[i - 1][0]) == "not_null")
                boolValues[7] = 1;
            if (lower(Constraint[i - 1][0]) == "unique")
                boolValues[8] = 1;
            if (lower(Constraint[i - 1][0]) == "check")
                boolValues[9] = 1;
            if (lower(Constraint[i - 1][0]) == "default")
                boolValues[10] = 1;

            attr_table[i] = boolValues;
        }

        // for (auto x : attr_table)
        // {
        //     cout << x.first << " ";
        //     for (auto i : x.second)
        //         cout << i << " ";
        //     cout << endl;
        // }

        bool check;
        for (int i = 4; i < command.size(); i++)
        {
            command[i].erase(
                remove(command[i].begin(), command[i].end(), '\''),
                command[i].end());
            if (command[i] == ";")
                break;

            vector<int> values = attr_table[i - 3];
            check = operation(values, command[i], table_name, i - 3);
            // cout << "Bool - " << check << endl;
            if (!check)
                break;
            op_table << command[i] << "#";
        }
        if (check)
        {
            cout << "\nData inserted successfully\n";
            op_table << "\n";
        }
        op_table.close();
    }
}
bool SQL ::isInteger(string command)
{
    return isNumber(command);
}
bool SQL ::isFloat(string command)
{
    string s;
    int flag = 0;
    for (auto i : command)
    {
        if (i != '.')
        {
            s.push_back(i);
            flag = 1;
        }
    }
    if (isNumber(command) && flag)
    {
        return true;
    }
    return false;
}
bool SQL ::checkDuplicates(string command, string table_name)
{
    ifstream table;
    table.open(table_name + ".txt", ios::in);
    string attr;
    vector<string> values;
    // vector<string> tables;
    while (getline(table, attr))
    {
        int i = 0;
        string temp;
        while (attr[i] != '#')
        {
            temp.push_back(attr[i]);
            i++;
        }
        values.push_back(temp);
    }
    values.push_back(command);
    set<string> value_set;
    for (auto i : values)
    {
        value_set.insert(i);
    }
    if (value_set.size() != values.size())
        return true; // if duplicate found
    return false;    // if duplicate not found
}

bool SQL::operation(vector<int> values, string command, string table_name, int attr_no)
{
    if (values[0] == 1)
    {
        if (!isInteger(command))
        {
            cout << "Error " << command << " is not of type integer" << endl;
            return false;
        }
    }

    else if (values[1] == 1)
    {
        if (!isFloat(command))
        {
            cout << "Error " << command << " is not of type float" << endl;
            return false;
        }
    }
    // else if (values[2] == 1)
    // {
    // // isVarchar(command);
    // }

    if (values[3])
    {
        int t = command.size();
        if (values[1] == 1)
            t--;

        if (t > values[3])
        {
            cout << "Error : Length Constraint Violated due to " << command << endl;
            return false;
        }
    }

    if (values[4])
    {
        vector<string> temp = split(command);
        if (temp[1].size() > values[4])
        {
            cout << "Error : Precision Constraint Violated due to " << command << endl;
            return false;
        }
    }

    if (values[5] == 1)
    {
        if (lower(command) == "na" || lower(command) == "null")
        {
            cout << "Error : Null value detected - This attribute can't be null\n";
            return false;
        }
        else if (checkDuplicates(command, table_name))
        {
            cout << "Error : Your entered key is already exists in the table " << endl;
            return false;
        }
    }

    // if (values[6] == 1)
    // {
    // }
    // // checkForignkey

    if (values[7] == 1)
    {
        if (lower(command) == "na" || lower(command) == "null")
        {
            cout << "Error : Null value detected - This attribute can't be null\n";
            return false;
        }
    }
    // checkNull(command)

    // if (values[8] == 1)
    // {
    // }
    // // checkDuplicates(command);

    // if (values[9] == 1)
    // {
    // }
    // // checkcheck(command)

    // if (values[10] == 1)
    // {
    // }
    // // checkdefault
    return true;
}

vector<vector<string>> SQL::splitFile(fstream &f1, string s, bool b) // if b=true, that means its for files constraint.txt and constraintLength.txt, else it is for datatype.txt
{

    vector<vector<string>> Length;
    int index = getIndex(tables, s);
    GotoLine(f1, index + 1);
    string fline;
    vector<string> l, l1;
    f1 >> fline;
    l = split(fline);
    if (b)
    {
        for (int i = 1; i < l.size(); i++)
        {
            stringstream stringStream(l[i]);
            string line;
            vector<string> wordVector;
            while (getline(stringStream, line))
            {
                size_t prev = 0, pos;
                while ((pos = line.find_first_of("-", prev)) != string ::npos)
                {
                    if (pos > prev)
                        wordVector.push_back(line.substr(prev, pos - prev));
                    prev = pos + 1;
                }
                if (prev < line.length())
                    wordVector.push_back(line.substr(prev, string ::npos));
            }
            Length.push_back(wordVector);
        }
    }
    else
    {
        for (int i = 1; i < l.size(); i++)
        {
            Length.push_back({l[i]});
        }
    }
    return Length;
}

void SQL::desc(vector<string> command)
{
    string table = command[1];
    if (notin(lower(table), tables))
    {
        cout << "Error : No such table created\n";
    }
    else
    {
        // map<string, string> info;
        ifstream ip_file1, ip_file2, ip_file3;
        ip_file1.open("Column.txt", ios::in);
        vector<string> columns, datatype, cons;
        ip_file2.open("Datatype.txt", ios::in);
        ip_file3.open("Constraint.txt", ios::in);
        string temp;
        while (getline(ip_file1, temp))
        {
            int i = 0;
            string temp1;
            while (temp[i] != '#')
            {
                temp1.push_back(table[i]);
                i++;
            }
            if (temp1 == table)
            {
                columns = split(temp.substr(i + 1));
            }
        }
        while (getline(ip_file2, temp))
        {
            int i = 0;
            string temp1;
            while (temp[i] != '#')
            {
                temp1.push_back(table[i]);
                i++;
            }
            if (temp1 == table)
            {
                datatype = split(temp.substr(i + 1));
            }
        }
        while (getline(ip_file3, temp))
        {
            int i = 0;
            string temp1;
            while (temp[i] != '#')
            {
                temp1.push_back(table[i]);
                i++;
            }
            if (temp1 == table)
            {
                cons = split(temp.substr(i + 1));
            }
        }

        map<int, vector<string>> tempmap;
        for (int i = 0; i < cons.size(); i++)
        {
            vector<string> str;
            stringstream stringStream(cons[i]);
            string line;
            while (getline(stringStream, line))
            {
                size_t prev = 0, pos;
                while ((pos = line.find_first_of("-", prev)) != string ::npos)
                {
                    if (pos > prev)
                        str.push_back(line.substr(prev, pos - prev));
                    prev = pos + 1;
                }
                if (prev < line.length())
                    str.push_back(line.substr(prev, string ::npos));
            }
            tempmap[i] = str;
        }

        // cout << "\tTable -  " << table << endl;
        cout << "-------------------------------------------------------------\n";
        cout << "TABLE\t|  COLUMN NAME\t|  DATATYPE\t|  CONSTRAINTS\t" << endl;
        cout << "-------------------------------------------------------------\n";
        cout << table;
        for (int i = 0; i < columns.size(); i++)
        {
            if (columns[i].size() > 4 && datatype[i].size() > 4)
            {
                cout << "\t|  " << columns[i] << "\t|  " << datatype[i] << "\t| ";
                for (int j = 0; j < tempmap[i].size(); j++)
                {
                    cout << "  " << tempmap[i][j] << " ";
                }
                cout << "\n-------------------------------------------------------------\n";
            }
            else if (columns[i].size() > 4 && datatype[i].size() <= 4)
            {
                cout << "\t|  " << columns[i] << "\t|  " << datatype[i] << "\t\t|";
                for (int j = 0; j < tempmap[i].size(); j++)
                {
                    cout << "  " << tempmap[i][j] << " ";
                }

                cout << "\n-------------------------------------------------------------\n";
            }
            else if (columns[i].size() <= 4 && datatype[i].size() > 4)
            {
                cout << "\t|  " << columns[i] << "\t\t|  " << datatype[i] << "\t|";
                for (int j = 0; j < tempmap[i].size(); j++)
                {
                    cout << "  " << tempmap[i][j] << " ";
                }

                cout << "\n-------------------------------------------------------------\n";
            }
            else
            {
                cout << "\t|  " << columns[i] << "\t\t|  " << datatype[i] << "\t\t|";
                for (int j = 0; j < tempmap[i].size(); j++)
                {
                    cout << "  " << tempmap[i][j] << " ";
                }

                cout << "\n-------------------------------------------------------------\n";
            }
        }
        ip_file1.close();
        ip_file2.close();
        ip_file3.close();
    }
}

void SQL::dropTable(string tableName)
{
    if (notin(lower(tableName), tables))
    {
        cout << "\nError : No such table exsits\n";
    }
    else
    {
        replaceFiles("Schema.txt", tableName);
        replaceFiles("Column.txt", tableName);
        replaceFiles("Constraint.txt", tableName);
        replaceFiles("ConstrLength.txt", tableName);
        replaceFiles("Datatype.txt", tableName);
        cout << "\nTable deleted successfully" << endl;
    }
}

void SQL::replaceFiles(string fileName, string tableName)
{
    fstream f1;
    ofstream temp;
    string line;
    f1.open(fileName);
    temp.open("temp.txt");
    int index = getIndex(tables, tableName);
    int count = 0;

    char *writable = new char[fileName.size() + 1];
    copy(fileName.begin(), fileName.end(), writable);
    writable[fileName.size()] = '\0'; // don't forget the terminating 0

    while (getline(f1, line))
    {
        if (count == index)
        {
            count++;
        }
        else
        {
            temp << line << endl;
            count++;
        }
    }

    temp.close();
    f1.close();
    remove(writable);
    rename("temp.txt", writable);
    delete[] writable;
}

void SQL::select(vector<string> command)
{
    string table;
    vector<string>::iterator itr = find(command.begin(), command.end(), "from");
    if (itr != end(command))
    {
        table = command[itr - command.begin() + 1]; // itr-vector.begin gives index of "from" element
    }
    else
    {
        cout << "Syntax Error";
    }

    // select * from table_name
    if (command[1] == "*")
    {
        fetchAll(table);
    }
}

void SQL::fetchAll(string table)
{
    if (notin(lower(table), tables))
    {
        cout << "Error : No such table exists\n";
    }
    else
    {
        ifstream f1;
        fstream f2;
        f1.open(table + ".txt");
        f2.open("Column.txt");
        int index = getIndex(tables, table);
        GotoLine(f2, index + 1);
        string line;
        f2 >> line;
        vector<string> columns = split(line);
        string temp;
        for (int i = 1; i < columns.size(); i++)
        {
            cout << columns[i] << "\t|\t";
        }
        cout << endl;
        while (getline(f1, temp))
        {
            vector<string> records = split(temp);
            for (int i = 0; i < records.size(); i++)
            {
                cout << records[i] << "\t|\t";
            }
            cout << endl;
        }
        f1.close();
        f2.close();
    }
}
void SQL ::updateFiles(string fileName, int index, vector<string> append_str)
{
    ofstream temp;
    string line;
    fstream f1;
    f1.open(fileName);
    temp.open("temp.txt");
    int count = 0;
    char *writable = new char[fileName.size() + 1];
    copy(fileName.begin(), fileName.end(), writable);
    writable[fileName.size()] = '\0'; // don't forget the terminating 0

    while (getline(f1, line))
    {
        if (count == index)
        {
            count++;
            if (append_str.size() == 1)
            {
                temp << line << append_str[0] << endl; // Check if "," is required or not
            }
            else
            {
                temp << line;
                for (int i = 0; i < append_str.size(); i++)
                {
                    temp << append_str[i] << "-";
                }
                temp << endl;
            }
        }
        else
        {
            temp << line << endl;
            count++;
        }
    }

    temp.close();
    f1.close();
    remove(writable);
    rename("temp.txt", writable);
    delete[] writable;
}
void SQL ::alterFiles(vector<string> command, int index, int flag) // flag = 0 for add 1 for drop 2 for rename 3 for modify
{
    // command starts from 4 from previous command
    if (flag == 0) // col_name datatype constraint/constraints
    {
        // Adding new col
        
        updateFiles("Column.txt", index, {command[0]});
        updateFiles("Datatype.txt", index, {command[1]});
        if (command.size() == 3)
        {
            if (isNumber(command[3]))
            {
                updateFiles("ConstrLength.txt", index, {command[2]});
            }
            updateFiles("Constraint.txt", index, {command[2]});
        }
        else if (command.size() > 3)
        {
            if (isNumber(command[3]))
            {
                if (command.size() == 5)
                    updateFiles("ConstrLength.txt", index, {command[3], command[4]});
                else
                    updateFiles("ConstrLength.txt", index, {command[3], "NA"});
            }
            else
            {
                updateFiles("ConstrLength.txt", index, vector<string>(command.begin() + 3, command.end()));
            }
        }
        else
        {
            updateFiles("Constraint.txt", index, {"NA"});
            updateFiles("ConstrLength.txt", index, {"0"});
        }
    }
    else if (flag == 1)
    {
        // Drop column
        // Alter table table_name drop column col_name
        fstream f1, f2, f3, f4;
        f1.open("Column.txt");
        f2.open("Constraint.txt");
        f3.open("ConstrLength.txt");
        f4.open("Datatype.txt");
        vector<vector<string>> Columns, Constraint, Constrlength, Datatype;
        Columns = splitFile(f1, tables[index], false);     // Check this
        Constraint = splitFile(f2, tables[index], true);   // Check this
        Constrlength = splitFile(f3, tables[index], true); // Check this
        Datatype = splitFile(f4, tables[index], false);    // Check this
        vector<string> columns;
        for (auto i : Columns)
        {
            columns.push_back(i[0]);
        }
        int index_1 = -1;
        for (int i = 0; i < columns.size(); i++)
        {
            if (columns[i] == command[0])
            {
                index_1 = i;
            }
        }
        auto it = columns.begin() + index_1;
        auto it1 = Constraint.begin() + index_1;
        auto it2 = Constrlength.begin() + index_1;
        auto it3 = Datatype.begin() + index_1;
        columns.erase(it);
        Constraint.erase(it1);
        Constrlength.erase(it2);
        Datatype.erase(it3);
        string replace_col, replace_cons, replace_len, replace_dt;
        replace_col = tables[index] + "#";
        replace_cons = tables[index] + "#";
        replace_len = tables[index] + "#";
        replace_dt = tables[index] + "#";
        for (auto i : columns)
        {
            replace_col += i + ",";
        }
        for (auto i : Datatype)
        {
            for (auto j : i)
                replace_dt += j + ",";
        }
        for (auto i : Constraint)
        {
            for (auto j : i)
                replace_cons += j + "-";
            replace_cons += ",";
        }
        for (auto i : Constrlength)
        {
            for (auto j : i)
                replace_len += j + "-";
            replace_len += ",";
        }
        ofstream temp;
        ofstream temp1;
        ofstream temp2;
        ofstream temp3;
        string line;
        string line1;
        string line2;
        string line3;
        string fileName = "Column.txt";
        string fileName1 = "Constraint.txt";
        string fileName2 = "ConstrLength.txt";
        string fileName3 = "Datatype.txt";
        temp.open("temp.txt");
        temp1.open("temp1.txt");
        temp2.open("temp2.txt");
        temp3.open("temp3.txt");
        int count = 0;
        int count1 = 0;
        int count2 = 0;
        int count3 = 0;
        char *writable = new char[fileName.size() + 1];
        copy(fileName.begin(), fileName.end(), writable);
        writable[fileName.size()] = '\0'; // don't forget the terminating 0
        char *writable1 = new char[fileName1.size() + 1];
        copy(fileName1.begin(), fileName1.end(), writable1);
        writable1[fileName1.size()] = '\0'; // don't forget the terminating 0
        char *writable2 = new char[fileName2.size() + 1];
        copy(fileName2.begin(), fileName2.end(), writable2);
        writable2[fileName2.size()] = '\0'; // don't forget the terminating 0
        char *writable3 = new char[fileName3.size() + 1];
        copy(fileName3.begin(), fileName3.end(), writable3);
        writable3[fileName3.size()] = '\0'; // don't forget the terminating 0
        while (getline(f1, line))
        {
            if (count == index)
            {
                count++;
                temp << replace_col << endl;
            }
            else
            {
                temp << line << endl;
                count++;
            }
        }
        while (getline(f2, line1))
        {
            if (count1 == index)
            {
                count1++;
                temp1 << replace_cons;
            }
            else
            {
                temp1 << line1 << endl;
                count1++;
            }
        }
        while (getline(f3, line2))
        {
            if (count2 == index)
            {
                count2++;
                temp2 << replace_len;
            }
            else
            {
                temp2 << line2 << endl;
                count2++;
            }
        }
        while (getline(f4, line3))
        {
            if (count3 == index)
            {
                count3++;
                temp3 << replace_dt;
            }
            else
            {
                temp3 << line3 << endl;
                count3++;
            }
        }
        temp.close();
        temp1.close();
        temp2.close();
        temp3.close();
        f1.close();
        f2.close();
        f3.close();
        f4.close();
        remove(writable);
        remove(writable1);
        remove(writable2);
        remove(writable3);
        rename("temp.txt", writable);
        rename("temp1.txt", writable1);
        rename("temp2.txt", writable2);
        rename("temp3.txt", writable3);
        delete[] writable;
        delete[] writable1;
        delete[] writable2;
        delete[] writable3;
    }
    else if (flag == 2)
    {}
    else
    {}
}
void SQL ::alterTable(vector<string> command)
{
    // Alter table table_name add col_name datatype
    // Alter table table_name drop column col_name
    // Alter table table_name rename col_name to new_col_name
    // Alter table table_name modify col_name new_datatype
    string table = command[2];

    if (notin(lower(table), tables))
    {
        cout << "Error : No such table created\n";
    }
    else
    {
        int index = getIndex(tables, table);
        string types = command[3];
        fstream f2("Column.txt");
        vector<vector<string>> columns = splitFile(f2, table, false);
        vector<string> Columns_main;
        f2.close();
        for (int i = 0; i < columns.size(); i++)
        {
            Columns_main.push_back(columns[i][0]);
        }
        if (lower(types) == "add")
        {
            if (notin(lower(command[4]), constraints))
            {
                if (notin(lower(command[4]), Columns_main))
                {
                    alterFiles(vector<string>(command.begin() + 4, command.end()), index, 0);
                    // Add col to files schema, column, constraint, constrlength, datatype
                }
                else
                {
                    cout << "Error : Column name already exists" << endl;
                }
            }
            else
            {
                alterFiles(vector<string>(command.begin() + 4, command.end()), index, 0);
            }
        }
        else if (lower(types) == "drop")
        {
            if (command.size() < 6)
            {
                cout << "Error: Mention Column name" << endl;
            }
            if (notin(lower(command[5]), Columns_main))
            {
                cout << "Error : No such column exists" << endl;
            }
            else
            {
                // delete col from files schema, column, constraint, constrlength, datatype
                alterFiles({command[5]}, index, 1);
            }
        }
        else if (lower(types) == "rename")
        {
            if (notin(lower(command[4]), Columns_main))
            {
                cout << "Error : No such column exists" << endl;
            }
            else
            {
                // update col_name to files schema, column, constraint, constrlength, datatype
                alterFiles(vector<string>(command.begin() + 4, command.end()), index, 2);
            }
        }
        else if (lower(types) == "modify")
        {
            if (notin(lower(command[4]), Columns_main))
            {
                cout << "Error : No such column exists" << endl;
            }
            else
            {
                // add constraints to col in files schema, column, constraint, constrlength, datatype
                alterFiles(vector<string>(command.begin() + 4, command.end()), index, 3);
            }
        }
        else
        {
            cout << "Syntax Error" << endl;
        }
    }
}

int main()
{
    string s = "alter table demo add marks int";
    //"drop table stud";
    // "select * from stud ;"
    // "insert into demo values (345135699999,'abcdef');";
    //"insert into stud values (11817, 'john');";
    //"create table stud(grno int primary_key, name varchar(20));";
    //"create table phase(phaseno int(3,2) primary_key, cardname varchar(20));"
    // cout << "Enter the SQL command :- ";
    // getline(cin, s);
    SQL sql(s);
    return 0;
}
