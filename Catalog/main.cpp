
#include <boost/filesystem/operations.hpp>
#include <fstream>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <string>

using namespace std;
using namespace boost;

uint32_t SizeDir(boost::filesystem::path dir)
{
    size_t size = 0;
    for (boost::filesystem::recursive_directory_iterator it(dir);
        it != boost::filesystem::recursive_directory_iterator();
        ++it)
    {
        if (!boost::filesystem::is_directory(*it))
            size += boost::filesystem::file_size(*it);
    }
    return size;
}

struct HumanReadable
{
    std::uintmax_t size{};
private:
    friend std::ostream& operator << (std::ostream& os, HumanReadable hr)
    {
        int i{};
        double mantissa = hr.size;

        for (; mantissa >= 1024.; mantissa /= 1024., ++i) {}
        mantissa = std::ceil(mantissa * 10.) / 10.;
        os << mantissa << " " << "BKMGTPE"[i];
        return i == 0 ? os : os << "B ";
    }

    friend std::ostream& operator << (std::ostream& os, uint32_t size)
    {
        int i{};
        double mantissa = size;
        for (; mantissa >= 1024.; mantissa /= 1024., ++i) {}
        mantissa = std::ceil(mantissa * 10.) / 10.;
        os << mantissa << " " << "BKMGTPE"[i];
        return i == 0 ? os : os << "B ";
    }
};

void CreateDir(string path, string namedir)
{
    boost::system::error_code error;
    boost::filesystem::create_directories(path + '/' + namedir, error);
    assert(!error);
}

void RemoveDir(string path, string namedir)
{
    try
    {
        if (boost::filesystem::exists(path + '/' + namedir))
            boost::filesystem::remove_all(path + '/' + namedir);
    }
    catch (boost::filesystem::filesystem_error const& e)
    {
        cout << e.what() << endl;
        std::system("pause");
    }
}

void RenameDir(string path)
{
    string old_name, new_name;
    cout << "Enter the old dir name: ";
    getline(cin, old_name);
    cout << "Enter the new dir name: ";
    getline(cin, new_name);

    try
    {
        if (boost::filesystem::exists(path + '/' + old_name))
            boost::filesystem::rename(path + '/' + old_name, path + '/' + new_name);
    }
    catch (boost::filesystem::filesystem_error const& e)
    {
        cout << e.what() << endl;
        std::system("pause");
    }
}

void PrintDir(string path)
{
    boost::filesystem::directory_iterator begin(path);
    boost::filesystem::directory_iterator end;
    boost::filesystem::path file;
    cout << "\t" << left << setfill(' ') << setw(45) << "Filename" << "Size" << right << setfill(' ') << setw(50) << "Type" << endl;
    
    for (; begin != end; ++begin)
    {
        boost::filesystem::file_status fs = boost::filesystem::status(*begin);
        file = *begin;
        cout << left << setfill('-') << setw(50) << file.filename().string();
        switch (fs.type())
        {
        case boost::filesystem::regular_file:
            cout << HumanReadable{ boost::filesystem::file_size(*begin) } << right << setfill('-') << setw(45) << "FILE" << endl;
            break;
        case boost::filesystem::symlink_file:
            cout << HumanReadable{ SizeDir(*begin) } << right << setfill('-')
                << setw(45) << "SYMLINK" << endl;
            break;
        case boost::filesystem::directory_file:
            cout << HumanReadable{ SizeDir(*begin) } << right << setfill('-')
                << setw(45) << "DIRECTORY" << endl;
            break;
        default:
            cout << HumanReadable{ SizeDir(*begin) } << right << setfill('-')
                << setw(45) << "OTHER" << endl;
            break;
        }
    }
}

void CopyFile(string path, string pathFrom, string pathTo)
{
    fstream from; 
    fstream to;
    string buff;
    from.open(pathFrom, ios::in);
    if (from.is_open())
    {
        to.open(pathTo, ios::out | ios::app);
        if (to.is_open())
        {
            cout << "\nCopy text: \n";
            while (getline(from, buff))
            {
                cout << buff << endl;
                to << buff << endl;
            }
            from.close();
            to.close();
        }
        else
            perror("FileTo: Erorr! File doesn`t open!");
    }
    else
        perror("FileFrom: Erorr! File doesn`t open!");
}

void DeleteFile(string path, string file) {
    string pathFile = path + "/" + file;
    char* char_arr;
    char_arr = &pathFile[0];

    if (remove(char_arr) != 0) {
        cout << "File is not found!" << file << "\n";
    }
    else
        cout << file << "File deleted!\n";
}

int main()
{
    string path;
    string file;
    string namedir; 
    string to;
    char switch_on;

    cout << "Enter path: ";
    getline(cin, path);

    do
    {
        std::system("cls");
        PrintDir(path);
        cout << "1 - Create dir." << endl;
        cout << "2 - Remove dir/file." << endl;
        cout << "3 - Open dir." << endl;
        cout << "4 - Rename dir." << endl;
        cout << "5 - Copy files." << endl;
        cout << "6 - Delete file!" << endl;
        cout << "7 - Exit!" << endl;
        cout << "Enter: ";
        cin >> switch_on;

        switch (switch_on)
        {
        case '1':
        {
            cin.ignore();
            cout << "Enter name of new dir: ";
            getline(cin, namedir);
            CreateDir(path, namedir);
        }break;

        case '2':
        {
            cin.ignore();
            cout << "Enter name dir for remove: ";
            getline(cin, namedir);
            RemoveDir(path, namedir);
        }break;

        case '3':
        {
            cin.ignore();
            cout << "Enter the name of dir: ";
            getline(cin, namedir);
            boost::filesystem::current_path(path + '/' + namedir);
            path = boost::filesystem::current_path().string();
        }break;

        case '4':
        {
            cin.ignore();
            RenameDir(path);
        }break;

        case '5':
        {
            cin.ignore();
            cout << "Enter file name from: ";
            getline(cin, namedir);
            cout << "Enter file name to: ";
            getline(cin, to);
            CopyFile(path, namedir, to);
        }break;

        case '6':
        {
            cin.ignore();
            DeleteFile(path, file);
        }break;

        case '7':
        {
            exit(7);
        }break;
        }
    } while (true);

    return 0;
}