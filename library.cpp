#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <ctime>
#include <algorithm>
#include <iomanip>

using namespace std;

//person class
class Person
{
private:
    string name;
    string email;
    string address;

public:
    Person() : name(""), email(""), address("") {}
    Person(const string &fullName, const string &mail, const string &addr)
        : name(fullName), email(mail), address(addr) {}

    // Getter methods
    string getName() const { return name; }
    string getEmail() const { return email; }
    string getAddress() const { return address; }

    // Setter methods
    void setName(const string &fullName) { name = fullName; }
    void setEmail(const string &mail) { email = mail; }
    void setAddress(const string &addr) { address = addr; }
};

//Member class
class Member
{
private:
    int memberID;
    Person person;
    vector<int> borrowedBooks;
    vector<time_t> dueDates;

public:
    static int nextMemberID;

    Member() : memberID(0) {}
    Member(const Person &p)
        : memberID(generateRandomID()), person(p) {}

    int generateRandomID()
    {
        return rand() % 900 + 100; //random 3 digit ID
    }

    void borrowBook(int bookID, time_t dueDate)
    {
        borrowedBooks.push_back(bookID);
        dueDates.push_back(dueDate);
    }

    void returnBook(int bookID)
    {
        auto it = find(borrowedBooks.begin(), borrowedBooks.end(), bookID);
        if (it != borrowedBooks.end())
        {
            int index = distance(borrowedBooks.begin(), it);
            borrowedBooks.erase(it);
            dueDates.erase(dueDates.begin() + index);
        }
    }

    time_t getDueDateForBook(int bookID)
    {
        auto it = find(borrowedBooks.begin(), borrowedBooks.end(), bookID);
        if (it != borrowedBooks.end())
        {
            int index = distance(borrowedBooks.begin(), it);
            return dueDates[index];
        }
        return time_t();
    }

    const vector<int> &getBorrowedBooks() const
    {
        return borrowedBooks;
    }

    // Getter methods
    int getMemberID() const { return memberID; }
    Person getPerson() const { return person; }

    // Setter methods
    void setMemberID(int id) { memberID = id; }
    void setPerson(const Person &p) { person = p; }
};

int Member::nextMemberID = 100;

//Book class
class Book
{
private:
    int BookID;
    string BookName, AuthorFirstName, AuthorLastName, BookType;
    int PageCount;

public:
    // Getter methods
    int getBookID() const { return BookID; }
    string getBookName() const { return BookName; }
    string getAuthorFirstName() const { return AuthorFirstName; }
    string getAuthorLastName() const { return AuthorLastName; }
    string getBookType() const { return BookType; }
    int getPageCount() const { return PageCount; }

    // Setter methods
    void setBookID(int id) { BookID = id; }
    void setBookName(const string &name) { BookName = name; }
    void setAuthorFirstName(const string &first) { AuthorFirstName = first; }
    void setAuthorLastName(const string &last) { AuthorLastName = last; }
    void setBookType(const string &type) { BookType = type; }
    void setPageCount(int count) { PageCount = count; }
};

// Function declarations
void loadBooksFromFile(vector<Book> &books);
void displayAvailableBooks(const vector<Book> &books);
void saveMembersToFile(const vector<Member> &members);
void loadMembersFromFile(vector<Member> &members);
void displayMemberInformation(const vector<Member> &members);
void issueBookToMember(vector<Member> &members, const vector<Book> &books, int memberID, int bookID);
void returnBookFromMember(vector<Member> &members, int memberID, int bookID);
void displayBooksBorrowedByMember(const vector<Member> &members, int memberID, const vector<Book> &books);
float calculateFine(time_t dueDate, time_t returnDate);
void handleBookReturn(vector<Member> &members, int memberID, int bookID);

// Function definitions
void loadBooksFromFile(vector<Book> &books)
{
    ifstream file("library_books.csv");

    if (file.is_open())
    {
        Book book;
        string line;
        getline(file, line); //This will avoid the Header
        while (getline(file, line))
        {
            stringstream ss(line);
            int id, pageCount;
            string bookName, authorFirstName, authorLastName, bookType;

            ss >> id;
            ss.ignore(); 
            getline(ss, bookName, ',');
            ss >> pageCount;
            ss.ignore();
            getline(ss, authorFirstName, ',');
            getline(ss, authorLastName, ',');
            getline(ss, bookType);

            book.setBookID(id);
            book.setBookName(bookName);
            book.setPageCount(pageCount);
            book.setAuthorFirstName(authorFirstName);
            book.setAuthorLastName(authorLastName);
            book.setBookType(bookType);

            books.push_back(book);
        }

        file.close();
        cout << "Book details loaded from file.\n";
    }
    else
    {
        cout << "Error: Unable to open the file for reading.\n";
    }
}

void displayAvailableBooks(const vector<Book> &books)
{
    cout << "Available Books:\n";
    for (const auto &book : books)
    {
        cout << "ID: " << book.getBookID()
             << ", Title: " << book.getBookName()
             << ", Page count: " << book.getPageCount()
             << ", Author: " << book.getAuthorFirstName() << " " << book.getAuthorLastName()
             << ", Type: " << book.getBookType() << "\n";
    }
}

void saveMembersToFile(const vector<Member> &members)
{
    ofstream file("member_details.txt");

    if (file.is_open())
    {
        for (const auto &member : members)
        {
            file << "Member," << member.getMemberID() << ","
                 << member.getPerson().getName() << ","
                 << member.getPerson().getEmail() << ","
                 << member.getPerson().getAddress() << "\n";
        }

        file << "NextMemberID," << Member::nextMemberID << "\n";

        file.close();
        cout << "Member details saved to file.\n";
    }
    else
    {
        cout << "Error: Unable to open the file for writing.\n";
    }
}


void loadMembersFromFile(vector<Member> &members)
{
    ifstream file("member_details.txt");

    if (file.is_open())
    {
        members.clear();

        string line;
        while (getline(file, line))
        {
            stringstream ss(line);
            string type;
            ss >> type;

            if (type == "Member")
            {
                int memberID;
                string fullName, email, address;

                ss >> memberID;
                ss.ignore();
                getline(ss, fullName, ',');
                getline(ss, email, ',');
                getline(ss, address);

                Person person(fullName, email, address);
                Member member(person);
                member.setMemberID(memberID);

                members.push_back(member);
            }
            else if (type == "NextMemberID")
            {
                int nextID;
                ss >> nextID;
                Member::nextMemberID = nextID;
            }
        }

        file.close();
        cout << "Member details loaded from file.\n";
    }
    else
    {
        cout << "No saved member data found.\n";
    }
}

void displayMemberInformation(const vector<Member> &members)
{
    if (members.empty())
    {
        cout << "No members found.\n";
    }
    else
    {
        cout << "\n---------------------------\n";
        cout << "Member Information:\n";
        for (const auto &member : members)
        {
            Person person = member.getPerson();
            cout << "ID: " << member.getMemberID()
                 << ", Name: " << person.getName()
                 << ", Address: " << person.getAddress()
                 << ", Email: " << person.getEmail() << "\n";
        }
    }
}

void issueBookToMember(vector<Member> &members, const vector<Book> &books, int memberID, int bookID)
{
    for (auto &member : members)
    {
        if (member.getMemberID() == memberID)
        {
            time_t now = time(nullptr);
            time_t dueDate = now + (3 * 24 * 60 * 60);

            member.borrowBook(bookID, dueDate);

            cout << "\n---------------------------\n";
            cout << "Book issued successfully to member " << memberID << ".\n";
            saveMembersToFile(members);
            return;
        }
    }

    cout << "Member not found.\n";
}

void displayBooksBorrowedByMember(const vector<Member> &members, int memberID, const vector<Book> &books)
{
    for (const auto &member : members)
    {
        if (member.getMemberID() == memberID)
        {
            cout << "\n---------------------------\n";
            cout << "Books borrowed by member " << memberID << ":\n";

            const vector<int> &borrowedBooks = member.getBorrowedBooks();

            for (int bookID : borrowedBooks)
            {
                auto it = find_if(books.begin(), books.end(), [bookID](const Book &b)
                                  { return b.getBookID() == bookID; });

                if (it != books.end())
                {
                    cout << "ID: " << it->getBookID()
                         << ", Title: " << it->getBookName()
                         << ", Author: " << it->getAuthorFirstName() << " " << it->getAuthorLastName()
                         << ", Type: " << it->getBookType() << "\n";
                }
            }
            return;
        }
    }

    cout << "Member not found.\n";
}

float calculateFine(time_t dueDate, time_t returnDate)
{
    const float fineRate = 1.0;
    int daysOverdue = max(0, int((returnDate - dueDate + 86399) / (24 * 60 * 60)));
    return fineRate * daysOverdue;
}

void returnBookFromMember(vector<Member> &members, int memberID, int bookID)
{
    for (auto &member : members)
    {
        if (member.getMemberID() == memberID)
        {
            time_t dueDate = member.getDueDateForBook(bookID);
            if (dueDate != time_t())
            {
                std::string returnDateStr;
                cout << "Enter return date (DD-MM-YYYY): ";
                cin.ignore();
                getline(cin, returnDateStr);

                std::tm tm = {};
                std::istringstream ss(returnDateStr);
                ss >> std::get_time(&tm, "%d-%m-%Y");

                if (ss.fail())
                {
                    cout << "Invalid date format. Please enter date in DD-MM-YYYY format.\n";
                    return;
                }

                time_t returnDate = std::mktime(&tm);

                float fine = calculateFine(dueDate, returnDate);

                if (fine > 0.0)
                {
                    cout << "\n---------------------------\n";
                    cout << "Fine of £" << fine << " charged for overdue book.\n";
                }
                else
                {
                    cout << "\n---------------------------\n";
                    cout << "Book returned on time.\n";
                }

                member.returnBook(bookID);
                saveMembersToFile(members);
                return;
            }
            else
            {
                cout << "Member did not borrow this book.\n";
                return;
            }
        }
    }

    cout << "Member not found.\n";
}

int main()
{
    vector<Book> availableBooks;
    vector<Member> members;

    loadBooksFromFile(availableBooks);
    loadMembersFromFile(members);

    int choice;

    do
    {
        cout << "\n---------------------------\n";
        cout << "\nLibrary System Menu:\n";
        cout << "1. Display Available Books\n";
        cout << "2. Display Member Information\n";
        cout << "3. Add Member\n";
        cout << "4. Issue a Book to a Member\n";
        cout << "5. Return a Book\n";
        cout << "6. Display Books Borrowed by a Member\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        cin.ignore();

        switch (choice)
        {
        case 1:
            displayAvailableBooks(availableBooks);
            break;
        case 2:
            displayMemberInformation(members);
            break;
        case 3: {
            string name, address, email;
            cout << "Enter Name: ";
            getline(cin, name);

            cout << "Enter Address: ";
            getline(cin, address);

            cout << "Enter Email: ";
            getline(cin, email);

            Person person(name, email, address);
            Member newMember(person);
            members.push_back(newMember);
            cout << "\n---------------------------\n";
            cout << "Member successfully added. Member ID: " << newMember.getMemberID() << "\n";
            saveMembersToFile(members);
            break;
        }
        case 4:
        {
            int memberID, bookID;
            cout << "Enter Member ID: ";
            cin >> memberID;

            cout << "Enter Book ID: ";
            cin >> bookID;

            issueBookToMember(members, availableBooks, memberID, bookID);
            break;
        }
        case 5:
        {
            int memberID, bookID;
            cout << "Enter Member ID: ";
            cin >> memberID;

            cout << "Enter Book ID: ";
            cin >> bookID;

            returnBookFromMember(members, memberID, bookID);
            break;
        }
        case 6:
        {
            int memberID;
            cout << "Enter Member ID: ";
            cin >> memberID;

            displayBooksBorrowedByMember(members, memberID, availableBooks);
            break;
        }
        case 0:
            cout << "Exiting the program. Goodbye!\n";
            break;
        default:
            cout << "Invalid choice. Please enter a valid option.\n";
        }

    } while (choice != 0);

    return 0;
}
