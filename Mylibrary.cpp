#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <map>
#include <iomanip>

struct Student {
    std::string name;
    int id;
};

struct Book {
    std::string title;
    std::string author;
    int id;
    bool available;
    time_t returnDate;
    int studentId; // Added field to associate with a student
};

std::vector<Book> books;
std::map<int, Student> students;
const std::string BOOKS_DATA_FILE = "library_data.txt";
const std::string STUDENTS_DATA_FILE = "students_data.txt";
const std::string ADMIN_USERNAME = "admin";
const std::string ADMIN_PASSWORD = "pass";
const double FINE_PER_DAY = 1.0;

void ClearConsole() {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

void PauseConsole() {
    std::cout << "\nPress Enter to continue...";
    std::cin.get();
}

void LoadData() {
    std::ifstream booksFile(BOOKS_DATA_FILE);
    if (!booksFile) {
        std::cerr << "Error: Could not open books data file." << std::endl;
        return;
    }

    Book book;
    while (booksFile >> book.id >> book.available >> book.returnDate >> book.studentId) {
        booksFile.ignore(); // Ignore the newline character
        std::getline(booksFile, book.title);
        std::getline(booksFile, book.author);
        books.push_back(book);
    }
    booksFile.close();

    std::ifstream studentsFile(STUDENTS_DATA_FILE);
    if (!studentsFile) {
        std::cerr << "Error: Could not open students data file." << std::endl;
        return;
    }

    Student student;
    while (studentsFile >> student.id) {
        studentsFile.ignore(); // Ignore the newline character
        std::getline(studentsFile, student.name);
        students[student.id] = student;
    }
    studentsFile.close();
}

void SaveData() {
    std::ofstream booksFile(BOOKS_DATA_FILE);
    if (!booksFile) {
        std::cerr << "Error: Could not open books data file." << std::endl;
        return;
    }

    for (const Book& book : books) {
        booksFile << book.id << " " << book.available << " " << book.returnDate << " " << book.studentId << "\n";
        booksFile << book.title << "\n";
        booksFile << book.author << "\n";
    }
    booksFile.close();

    std::ofstream studentsFile(STUDENTS_DATA_FILE);
    if (!studentsFile) {
        std::cerr << "Error: Could not open students data file." << std::endl;
        return;
    }

    for (const auto& student : students) {
        studentsFile << student.first << "\n";
        studentsFile << student.second.name << "\n";
    }
    studentsFile.close();
}

int GenerateUniqueBookID() {
    int maxID = 0;
    for (const Book& book : books) {
        if (book.id > maxID) {
            maxID = book.id;
        }
    }
    return maxID + 1;
}

int GenerateUniqueStudentID() {
    int maxID = 0;
    for (const auto& student : students) {
        if (student.first > maxID) {
            maxID = student.first;
        }
    }
    return maxID + 1;
}

void AddBook() {
    ClearConsole();
    while (true) {
        Book newBook;
        std::cin.ignore(); // Clear the input buffer
        std::cout << "Add New Book\n";
        std::cout << "=============================\n";
        std::cout << "Enter book title: ";
        std::getline(std::cin, newBook.title);
        std::cout << "Enter author name: ";
        std::getline(std::cin, newBook.author);
        newBook.available = true;
        newBook.returnDate = 0;
        newBook.studentId = -1;
        newBook.id = GenerateUniqueBookID();
        books.push_back(newBook);
        SaveData();
        std::cout << "\nBook added successfully. ID: " << newBook.id << "\n";

        char choice;
        std::cout << "Do you want to add more books? (y/n): ";
        std::cin >> choice;
        if (choice != 'y' && choice != 'Y') {
            break;
        }
    }
}

void ListBooks() {
    ClearConsole();
    std::cout << "Library Books\n";
    std::cout << "====================================================================================================\n";
    std::cout << std::setw(10) << "ID" << std::setw(30) << "Title" << std::setw(25) << "Author" << std::setw(15) << "Available" << std::setw(20) << "Student ID" << std::setw(15) << "Days Late" << std::setw(15) << "Fine\n";
    std::cout << "====================================================================================================\n";
    for (const Book& book : books) {
        std::cout << std::setw(10) << book.id << std::setw(30) << book.title << std::setw(25) << book.author
                  << std::setw(15) << (book.available ? "Yes" : "No") << std::setw(20) << (book.studentId != -1 ? std::to_string(book.studentId) : "") << std::setw(15);
        if (!book.available) {
            time_t now = time(nullptr);
            double daysLate = difftime(now, book.returnDate) / (24 * 3600); // Calculate days late
            if (daysLate > 0) {
                double fine = daysLate * FINE_PER_DAY;
                std::cout << daysLate << std::setw(15) << "$" << fine << "\n";
            } else {
                std::cout << "0" << std::setw(15) << "-\n";
            }
        } else {
            std::cout << "-" << std::setw(15) << "-\n";
        }
    }
    std::cout << "====================================================================================================\n";
    PauseConsole();
}

void ListStudentsWithBooks() {
    ClearConsole();
    std::cout << "Students and Their Books\n";
    std::cout << "==============================================================================================\n";
    std::cout << std::setw(15) << "Student ID" << std::setw(30) << "Student Name" << std::setw(15) << "Book ID" << std::setw(30) << "Book Title\n";
    std::cout << "==============================================================================================\n";
    for (const auto& student : students) {
        const Student& s = student.second;
        bool studentFound = false;
        for (const Book& book : books) {
            if (book.studentId == s.id) {
                studentFound = true;
                std::cout << std::setw(15) << s.id << std::setw(30) << s.name << std::setw(15) << book.id << std::setw(30) << book.title << "\n";
            }
        }
        if (!studentFound) {
            std::cout << std::setw(15) << s.id << std::setw(30) << s.name << std::setw(15) << "-" << std::setw(30) << "No Books\n";
        }
    }
    std::cout << "==============================================================================================\n";
    PauseConsole();
}

void AddStudent() {
    ClearConsole();
    Student newStudent;
    std::cin.ignore(); // Clear the input buffer
    std::cout << "Add New Student\n";
    std::cout << "=============================\n";
    std::cout << "Enter student name: ";
    std::getline(std::cin, newStudent.name);
    newStudent.id = GenerateUniqueStudentID();
    students[newStudent.id] = newStudent;
    SaveData();
    std::cout << "\nStudent added successfully. ID: " << newStudent.id << "\n";
    PauseConsole();
}

void LendBook() {
    ClearConsole();
    int bookId, studentId;
    std::cout << "Lend Book\n";
    std::cout << "=============================\n";
    std::cout << "Enter book ID: ";
    std::cin >> bookId;
    std::cout << "Enter student ID: ";
    std::cin >> studentId;

    auto bookIter = std::find_if(books.begin(), books.end(), [bookId](const Book& book) {
        return book.id == bookId && book.available;
    });

    auto studentIter = students.find(studentId);

    if (bookIter != books.end() && studentIter != students.end()) {
        Book& book = *bookIter;
        Student& student = studentIter->second;
        book.available = false;
        time_t now = time(nullptr);
        book.returnDate = now + (15 * 24 * 3600); // 15 days from now
        book.studentId = student.id;
        SaveData();
        std::cout << "\nBook lent successfully to student: " << student.name << "\n";
    } else {
        std::cout << "\nBook or student not found or book already lent.\n";
    }
    PauseConsole();
}

void DepositBook() {
    ClearConsole();
    int bookId;
    std::cout << "Deposit Book\n";
    std::cout << "=============================\n";
    std::cout << "Enter book ID: ";
    std::cin >> bookId;

    auto bookIter = std::find_if(books.begin(), books.end(), [bookId](const Book& book) {
        return book.id == bookId && !book.available;
    });

    if (bookIter != books.end()) {
        Book& book = *bookIter;
        time_t now = time(nullptr);
        double daysLate = difftime(now, book.returnDate) / (24 * 3600); // Calculate days late
        if (daysLate > 0) {
            double fine = daysLate * FINE_PER_DAY;
            std::cout << "Book is " << daysLate << " days late.\n";
            std::cout << "Fine: $" << fine << "\n";
        }
        book.available = true;
        book.returnDate = 0;
        book.studentId = -1;
        SaveData();
        std::cout << "\nBook deposited successfully.\n";
    } else {
        std::cout << "\nBook not found or already deposited.\n";
    }
    PauseConsole();
}

void ClearStudentData() {
    ClearConsole();
    students.clear();

    // Clear associated student IDs from borrowed books
    for (Book& book : books) {
        book.studentId = -1;
        book.available = true;
        book.returnDate = 0;
    }

    SaveData();
    std::cout << "All student data cleared and borrowed book records reset.\n";
    PauseConsole();
}

void ClearBookData() {
    ClearConsole();
    books.clear();

    // Clear associated book IDs from students
    for (auto& student : students) {
        student.second.id = -1;
    }

    SaveData();
    std::cout << "All book data cleared and student records reset.\n";
    PauseConsole();
}

bool AdminLogin() {
    std::string username, password;
    ClearConsole();
    std::cout << "Admin Login\n";
    std::cout << "=============================\n";
    std::cout << "Username: ";
    std::cin >> username;
    std::cout << "Password: ";
    std::cin >> password;
    return (username == ADMIN_USERNAME && password == ADMIN_PASSWORD);
}

void AdminMenu() {
    if (!AdminLogin()) {
        std::cout << "Login failed. Access denied.\n";
        PauseConsole();
        return;
    }

    int choice;
    while (true) {
        ClearConsole();
        std::cout << "Admin Menu\n";
        std::cout << "=============================\n";
        std::cout << "1. Add Book\n";
        std::cout << "2. List Books\n";
        std::cout << "3. List Students and Their Books\n";
        std::cout << "4. Add Student\n";
        std::cout << "5. Lend Book\n";
        std::cout << "6. Deposit Book\n";
        std::cout << "7. Clear Student Data\n";
        std::cout << "8. Clear Book Data\n";
        std::cout << "9. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        std::cin.ignore(); // Ignore the newline character
        switch (choice) {
            case 1:
                AddBook();
                break;
            case 2:
                ListBooks();
                break;
            case 3:
                ListStudentsWithBooks();
                break;
            case 4:
                AddStudent();
                break;
            case 5:
                LendBook();
                break;
            case 6:
                DepositBook();
                break;
            case 7:
                ClearStudentData();
                break;
            case 8:
                ClearBookData();
                break;
            case 9:
                SaveData();
                std::cout << "\nExiting. Goodbye!\n";
                return;
            default:
                std::cout << "\nInvalid choice. Please try again.\n";
                PauseConsole();
        }
    }
}

int main() {
    LoadData();

    int choice;
    while (true) {
        ClearConsole();
         std::cout << R"(
  ____  _               __      
 / ___|| |_ __ _ _ __   | |
 \___ \| __/ _` | '__|  |
  ___) | || (_| | |     | 
 |____/ \__\__,_|_|     |  
                                                                     
)";
        std::cout << "Library Management System\n";
        std::cout << "=============================\n";
        std::cout << "1. Admin\n";
        std::cout << "2. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        std::cin.ignore(); // Ignore the newline character
        switch (choice) {
            case 1:
                AdminMenu();
                break;
            case 2:
                SaveData();
                std::cout << "\nExiting. Goodbye!\n";
                return 0;
            default:
                std::cout << "\nInvalid choice. Please try again.\n";
                PauseConsole();
        }
    }

    return 0;
}
