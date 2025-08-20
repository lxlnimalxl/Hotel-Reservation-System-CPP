#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <iomanip>
#include <cstdlib>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <cctype>

using namespace std;

// ANSI color codes
const string RED = "\033[31m";
const string GREEN = "\033[32m";
const string YELLOW = "\033[33m";
const string BLUE = "\033[34m";
const string CYAN = "\033[36m";
const string MAGENTA = "\033[35m";
const string RESET = "\033[0m";
const string BOLD = "\033[1m";
const string UNDERLINE = "\033[4m";

void clearScreen() {
    cout << "\033[2J\033[H" << flush; // Cross-platform clear screen
}

string getCurrentDateTime() {
    time_t now = time(nullptr);
    struct tm timeinfo;
    char buffer[80];
    #ifdef _WIN32
        localtime_s(&timeinfo, &now);
    #else
        localtime_r(&now, &timeinfo);
    #endif
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
    return string(buffer);
}

class Room {
public:
    int number;
    string type;
    double price;
    bool isAvailable;

    Room(int num, string t, double p, bool avail = true) 
        : number(num), type(t), price(p), isAvailable(avail) {}
};

class Reservation {
public:
    string guestName;
    int roomNumber;
    string checkInDate;
    string checkOutDate;

    Reservation(string name, int room, string in, string out)
        : guestName(name), roomNumber(room), checkInDate(in), checkOutDate(out) {}
};

class Hotel {
private:
    vector<Room> rooms;
    vector<Reservation> reservations;
    const string ROOMS_FILE = "rooms.txt";
    const string RESERVATIONS_FILE = "reservations.txt";

    void loadingAnimation(const string& message, int durationMs = 1000) {
        cout << CYAN << BOLD << message << " " << RESET;
        string dots[] = {"*  ", "** ", "***", "   "};
        for (int i = 0; i < 12; ++i) {
            cout << "\b\b\b" << YELLOW << dots[i % 4] << RESET << flush;
            this_thread::sleep_for(chrono::milliseconds(durationMs / 12));
        }
        cout << endl;
        this_thread::sleep_for(chrono::milliseconds(200));
        clearScreen();
    }

    void spinnerAnimation(const string& message, int spins = 3) {
        const char spinner[] = {'|', '/', '-', '\\'};
        cout << BLUE << BOLD << message << " " << RESET;
        for (int i = 0; i < spins * 4; ++i) {
            cout << "\b" << MAGENTA << spinner[i % 4] << RESET << flush;
            this_thread::sleep_for(chrono::milliseconds(60));
        }
        cout << "\b" << GREEN << BOLD << "Completed!" << RESET << endl;
        this_thread::sleep_for(chrono::milliseconds(300));
        clearScreen();
    }

    void displayASCIIArt() {
        clearScreen();
        cout << YELLOW << BOLD << R"(
============================================================
       _          _ _       
      | |__   ___| | | ___  
      | '_ \ / __| | |/ _ \ 
      | | | | (__| | | (_) |
      |_| |_| \___|_|_|\___/
============================================================
        )" << RESET << endl;
        cout << CYAN << UNDERLINE << "Luxury Hotel Reservation System" << RESET << endl;
        cout << MAGENTA << "Current Time: " << getCurrentDateTime() << RESET << endl;
        cout << GREEN << "Starting system..." << RESET << endl;
        loadingAnimation("Initializing", 1500);
    }

    void loadRooms() {
        ifstream file(ROOMS_FILE);
        if (file.is_open()) {
            rooms.clear();
            int num;
            string type;
            double price;
            int availInt;
            while (file >> num >> type >> price >> availInt) {
                if (num > 0 && price >= 0) {
                    rooms.emplace_back(num, type, price, static_cast<bool>(availInt));
                }
            }
            file.close();
            if (rooms.empty()) {
                initializeDefaultRooms();
                cout << YELLOW << "=> Rooms file was empty or corrupted. Initialized default rooms." << RESET << endl;
            } else {
                cout << GREEN << "=> Rooms loaded from " << ROOMS_FILE << RESET << endl;
            }
        } else {
            initializeDefaultRooms();
            cout << GREEN << "=> Created new rooms file: " << ROOMS_FILE << RESET << endl;
        }
        this_thread::sleep_for(chrono::milliseconds(500));
    }

    void initializeDefaultRooms() {
        rooms.clear();
        rooms.emplace_back(101, "Single", 100.0);
        rooms.emplace_back(102, "Double", 150.0);
        rooms.emplace_back(103, "Suite", 300.0);
        rooms.emplace_back(201, "Single", 100.0);
        rooms.emplace_back(202, "Double", 150.0);
        saveRooms();
    }

    void saveRooms() {
        ofstream file(ROOMS_FILE, ios::out | ios::trunc);
        if (file.is_open()) {
            for (const auto& room : rooms) {
                file << room.number << " " << room.type << " " << room.price << " " << room.isAvailable << endl;
            }
            file.close();
        } else {
            cout << RED << "=> Error: Could not save rooms to " << ROOMS_FILE << RESET << endl;
        }
    }

    void loadReservations() {
        ifstream file(RESERVATIONS_FILE);
        if (file.is_open()) {
            reservations.clear();
            string line;
            while (getline(file, line)) {
                if (line.empty()) continue;
                size_t pos = 0;
                string name = line.substr(0, pos = line.find('|'));
                if (pos == string::npos || name.empty()) continue;
                line.erase(0, pos + 1);
                string roomStr = line.substr(0, pos = line.find('|'));
                if (pos == string::npos) continue;
                int roomNum;
                try {
                    roomNum = stoi(roomStr);
                } catch (...) {
                    continue;
                }
                line.erase(0, pos + 1);
                string checkIn = line.substr(0, pos = line.find('|'));
                if (pos == string::npos || !validateDate(checkIn)) continue;
                line.erase(0, pos + 1);
                string checkOut = line;
                if (!validateDate(checkOut)) continue;
                reservations.emplace_back(name, roomNum, checkIn, checkOut);
            }
            file.close();
            cout << GREEN << "=> Reservations loaded from " << RESERVATIONS_FILE << RESET << endl;
        } else {
            ofstream createFile(RESERVATIONS_FILE, ios::out | ios::trunc);
            createFile.close();
            cout << GREEN << "=> Created new reservations file: " << RESERVATIONS_FILE << RESET << endl;
        }
        this_thread::sleep_for(chrono::milliseconds(500));
    }

    void saveReservations() {
        ofstream file(RESERVATIONS_FILE, ios::out | ios::trunc);
        if (file.is_open()) {
            for (const auto& res : reservations) {
                string guestName = res.guestName;
                replace(guestName.begin(), guestName.end(), ' ', '_');
                file << guestName << "|" << res.roomNumber << "|" << res.checkInDate << "|" << res.checkOutDate << endl;
            }
            file.close();
        } else {
            cout << RED << "=> Error: Could not save reservations to " << RESERVATIONS_FILE << RESET << endl;
        }
    }

    void displayStoredData() {
        spinnerAnimation("Loading stored data");
        cout << BLUE << BOLD << "+-----------------------------------+" << endl;
        cout << "|           All Rooms               |" << RESET << endl;
        cout << BLUE << "+-----------------------------------+" << RESET << endl;
        cout << CYAN << setw(10) << "Room #" << setw(12) << "Type" << setw(12) << "Price" << setw(12) << "Available" << RESET << endl;
        cout << MAGENTA << string(46, '-') << RESET << endl;
        for (const auto& room : rooms) {
            string availStr = room.isAvailable ? GREEN + string("Yes") + RESET : RED + string("No") + RESET;
            cout << setw(10) << room.number << setw(12) << room.type << setw(12) << fixed << setprecision(2) << room.price << setw(12) << availStr << endl;
        }
        cout << endl << BLUE << BOLD << "+-----------------------------------+" << endl;
        cout << "|         Reservations              |" << RESET << endl;
        cout << BLUE << "+-----------------------------------+" << RESET << endl;
        if (reservations.empty()) {
            cout << YELLOW << "No reservations found." << RESET << endl;
        } else {
            cout << CYAN << setw(20) << "Guest" << setw(12) << "Room #" << setw(15) << "Check-In" << setw(15) << "Check-Out" << RESET << endl;
            cout << MAGENTA << string(62, '-') << RESET << endl;
            for (const auto& res : reservations) {
                string guestName = res.guestName;
                replace(guestName.begin(), guestName.end(), '_', ' ');
                cout << setw(20) << guestName << setw(12) << res.roomNumber << setw(15) << res.checkInDate << setw(15) << res.checkOutDate << endl;
            }
        }
        cout << MAGENTA << "\n=> Data is auto-saved to files!" << RESET << endl;
        cout << YELLOW << "Press Enter to continue..." << RESET;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
        clearScreen();
    }

    bool validateDate(const string& date) {
        if (date.length() != 10 || date[4] != '-' || date[7] != '-') return false;
        try {
            int year = stoi(date.substr(0, 4));
            int month = stoi(date.substr(5, 2));
            int day = stoi(date.substr(8, 2));
            if (year < 2025 || month < 1 || month > 12 || day < 1 || day > 31) return false;
            return true;
        } catch (...) {
            return false;
        }
    }

    bool roomExists(int roomNum) {
        return any_of(rooms.begin(), rooms.end(), [roomNum](const Room& r) { return r.number == roomNum; });
    }

    void displayHelp() {
        spinnerAnimation("Loading help");
        cout << BLUE << BOLD << "+-----------------------------------+" << endl;
        cout << "|           Help Guide              |" << RESET << endl;
        cout << BLUE << "+-----------------------------------+" << RESET << endl;
        cout << CYAN << "1. View Available Rooms" << RESET << ": Show rooms ready for booking.\n";
        cout << CYAN << "2. Reserve a Room" << RESET << ": Book a room with guest details.\n";
        cout << CYAN << "3. View Reservations" << RESET << ": List all current bookings.\n";
        cout << CYAN << "4. Check Out" << RESET << ": Free a room after guest stay.\n";
        cout << CYAN << "5. Add New Room" << RESET << ": Add a new room to the system.\n";
        cout << CYAN << "6. Search Reservation" << RESET << ": Find booking by guest or room.\n";
        cout << CYAN << "7. Help" << RESET << ": Show this guide.\n";
        cout << CYAN << "8. Exit" << RESET << ": Save data and exit.\n";
        cout << MAGENTA << "=> Tip: Use 'b' to return to the menu at any prompt!" << RESET << endl;
        cout << YELLOW << "Press Enter to return..." << RESET;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
        clearScreen();
    }

    void searchReservation() {
        string input;
        cout << BLUE << BOLD << "+-----------------------------------+" << endl;
        cout << "|       Search Reservation          |" << RESET << endl;
        cout << BLUE << "+-----------------------------------+" << RESET << endl;
        cout << YELLOW << "Enter guest name or room number (or 'b' to return): " << RESET;
        getline(cin, input);
        if (input == "b" || input == "B") {
            clearScreen();
            return;
        }
        spinnerAnimation("Searching reservations");
        bool found = false;
        int roomNum = 0;
        try {
            roomNum = stoi(input);
        } catch (...) {
            // Input is not a number, treat as guest name
        }
        string searchName = input;
        transform(searchName.begin(), searchName.end(), searchName.begin(), ::tolower);
        cout << CYAN << setw(20) << "Guest" << setw(12) << "Room #" << setw(15) << "Check-In" << setw(15) << "Check-Out" << RESET << endl;
        cout << MAGENTA << string(62, '-') << RESET << endl;
        for (const auto& res : reservations) {
            string guestName = res.guestName;
            string guestNameLower = guestName;
            transform(guestNameLower.begin(), guestNameLower.end(), guestNameLower.begin(), ::tolower);
            replace(guestName.begin(), guestName.end(), '_', ' ');
            if (guestNameLower.find(searchName) != string::npos || (roomNum != 0 && res.roomNumber == roomNum)) {
                cout << GREEN << setw(20) << guestName << setw(12) << res.roomNumber << setw(15) << res.checkInDate << setw(15) << res.checkOutDate << RESET << endl;
                found = true;
            }
        }
        if (!found) {
            cout << RED << "No reservations found for '" << input << "'." << RESET << endl;
        }
        cout << YELLOW << "Press Enter to return..." << RESET;
        cin.get();
        clearScreen();
    }

public:
    Hotel() {
        loadRooms();
        loadReservations();
    }

    ~Hotel() {
        saveRooms();
        saveReservations();
        cout << GREEN << "=> All data saved successfully!" << RESET << endl;
        this_thread::sleep_for(chrono::milliseconds(500));
    }

    void displayMenu() {
        int availableRooms = count_if(rooms.begin(), rooms.end(), [](const Room& r) { return r.isAvailable; });
        cout << BLUE << BOLD << "+-----------------------------------+" << endl;
        cout << "|     Luxury Hotel Reservation      |" << RESET << endl;
        cout << BLUE << "+-----------------------------------+" << RESET << endl;
        cout << MAGENTA << "Time: " << getCurrentDateTime() << RESET << endl;
        cout << MAGENTA << "Rooms: " << rooms.size() << " total, " << availableRooms << " available" << RESET << endl;
        cout << MAGENTA << "Reservations: " << reservations.size() << RESET << endl;
        cout << endl;
        cout << GREEN << "1. View Available Rooms" << RESET << endl;
        cout << GREEN << "2. Reserve a Room" << RESET << endl;
        cout << GREEN << "3. View All Reservations" << RESET << endl;
        cout << GREEN << "4. Check Out a Guest" << RESET << endl;
        cout << GREEN << "5. Add New Room" << RESET << endl;
        cout << GREEN << "6. Search Reservation" << RESET << endl;
        cout << GREEN << "7. Help" << RESET << endl;
        cout << RED << "8. Exit" << RESET << endl;
        cout << YELLOW << "Enter choice (1-8): " << RESET;
    }

    void viewAvailableRooms() {
        spinnerAnimation("Loading available rooms");
        cout << BLUE << BOLD << "+-----------------------------------+" << endl;
        cout << "|        Available Rooms            |" << RESET << endl;
        cout << BLUE << "+-----------------------------------+" << RESET << endl;
        cout << CYAN << setw(10) << "Room #" << setw(12) << "Type" << setw(12) << "Price" << RESET << endl;
        cout << MAGENTA << string(34, '-') << RESET << endl;
        bool hasAvailable = false;
        for (const auto& room : rooms) {
            if (room.isAvailable) {
                cout << GREEN << setw(10) << room.number << setw(12) << room.type << setw(12) << fixed << setprecision(2) << room.price << RESET << endl;
                hasAvailable = true;
            }
        }
        if (!hasAvailable) {
            cout << YELLOW << "No rooms available." << RESET << endl;
        }
        cout << MAGENTA << "\n=> Tip: Reserve a room with option 2." << RESET << endl;
        cout << YELLOW << "Press Enter to return (or 'b' to go back): " << RESET;
        string input;
        getline(cin, input);
        if (input == "b" || input == "B") {
            clearScreen();
            return;
        }
        clearScreen();
    }

    void reserveRoom() {
        string input;
        cout << BLUE << BOLD << "+-----------------------------------+" << endl;
        cout << "|         Reserve a Room            |" << RESET << endl;
        cout << BLUE << "+-----------------------------------+" << RESET << endl;
        cout << YELLOW << "Enter room number (or 'b' to return): " << RESET;
        getline(cin, input);
        if (input == "b" || input == "B") {
            clearScreen();
            return;
        }
        int roomNum;
        try {
            roomNum = stoi(input);
            if (roomNum <= 0) throw out_of_range("Negative room number");
        } catch (...) {
            cout << RED << "Invalid room number." << RESET << endl;
            this_thread::sleep_for(chrono::milliseconds(1000));
            clearScreen();
            return;
        }
        if (!roomExists(roomNum)) {
            cout << RED << "Room does not exist." << RESET << endl;
            this_thread::sleep_for(chrono::milliseconds(1000));
            clearScreen();
            return;
        }
        auto it = find_if(rooms.begin(), rooms.end(), [roomNum](const Room& r) { return r.number == roomNum && r.isAvailable; });
        if (it != rooms.end()) {
            string name, checkIn, checkOut;
            cout << YELLOW << "Enter guest name: " << RESET;
            getline(cin, name);
            if (name.empty()) {
                cout << RED << "Guest name cannot be empty." << RESET << endl;
                this_thread::sleep_for(chrono::milliseconds(1000));
                clearScreen();
                return;
            }
            cout << YELLOW << "Enter check-in date (YYYY-MM-DD): " << RESET;
            getline(cin, checkIn);
            if (checkIn == "b" || checkIn == "B") {
                clearScreen();
                return;
            }
            while (!validateDate(checkIn)) {
                cout << RED << "Invalid date (use YYYY-MM-DD, year >= 2025). Try again: " << RESET;
                getline(cin, checkIn);
                if (checkIn == "b" || checkIn == "B") {
                    clearScreen();
                    return;
                }
            }
            cout << YELLOW << "Enter check-out date (YYYY-MM-DD): " << RESET;
            getline(cin, checkOut);
            if (checkOut == "b" || checkOut == "B") {
                clearScreen();
                return;
            }
            while (!validateDate(checkOut)) {
                cout << RED << "Invalid date (use YYYY-MM-DD, year >= 2025). Try again: " << RESET;
                getline(cin, checkOut);
                if (checkOut == "b" || checkOut == "B") {
                    clearScreen();
                    return;
                }
            }
            cout << MAGENTA << "\n+-----------------------------------+" << endl;
            cout << "|       Reservation Preview         |" << RESET << endl;
            cout << MAGENTA << "+-----------------------------------+" << RESET << endl;
            cout << CYAN << "Guest: " << name << endl;
            cout << "Room #: " << roomNum << endl;
            cout << "Check-In: " << checkIn << endl;
            cout << "Check-Out: " << checkOut << RESET << endl;
            cout << YELLOW << "Confirm? (y/n or 'b' to cancel): " << RESET;
            string confirm;
            getline(cin, confirm);
            if (confirm == "y" || confirm == "Y") {
                it->isAvailable = false;
                reservations.emplace_back(name, roomNum, checkIn, checkOut);
                saveRooms();
                saveReservations();
                loadingAnimation("Confirming reservation");
                cout << GREEN << "=> Room reserved successfully!" << RESET << endl;
            } else {
                cout << YELLOW << "=> Reservation cancelled." << RESET << endl;
            }
        } else {
            cout << RED << "Room not available or invalid." << RESET << endl;
        }
        this_thread::sleep_for(chrono::milliseconds(1000));
        clearScreen();
    }

    void viewReservations() {
        spinnerAnimation("Fetching reservations");
        cout << BLUE << BOLD << "+-----------------------------------+" << endl;
        cout << "|       All Reservations            |" << RESET << endl;
        cout << BLUE << "+-----------------------------------+" << RESET << endl;
        if (reservations.empty()) {
            cout << YELLOW << "No reservations found." << RESET << endl;
        } else {
            cout << CYAN << setw(20) << "Guest" << setw(12) << "Room #" << setw(15) << "Check-In" << setw(15) << "Check-Out" << RESET << endl;
            cout << MAGENTA << string(62, '-') << RESET << endl;
            for (const auto& res : reservations) {
                string guestName = res.guestName;
                replace(guestName.begin(), guestName.end(), '_', ' ');
                cout << GREEN << setw(20) << guestName << setw(12) << res.roomNumber << setw(15) << res.checkInDate << setw(15) << res.checkOutDate << RESET << endl;
            }
        }
        cout << MAGENTA << "\n=> Tip: Use option 6 to search for a specific reservation." << RESET << endl;
        cout << YELLOW << "Press Enter to return..." << RESET;
        cin.get();
        clearScreen();
    }

    void checkOut() {
        string input;
        cout << BLUE << BOLD << "+-----------------------------------+" << endl;
        cout << "|         Check Out Guest          |" << RESET << endl;
        cout << BLUE << "+-----------------------------------+" << RESET << endl;
        cout << YELLOW << "Enter room number (or 'b' to return): " << RESET;
        getline(cin, input);
        if (input == "b" || input == "B") {
            clearScreen();
            return;
        }
        int roomNum;
        try {
            roomNum = stoi(input);
            if (roomNum <= 0) throw out_of_range("Negative room number");
        } catch (...) {
            cout << RED << "Invalid room number." << RESET << endl;
            this_thread::sleep_for(chrono::milliseconds(1000));
            clearScreen();
            return;
        }
        if (!roomExists(roomNum)) {
            cout << RED << "Room does not exist." << RESET << endl;
            this_thread::sleep_for(chrono::milliseconds(1000));
            clearScreen();
            return;
        }
        auto roomIt = find_if(rooms.begin(), rooms.end(), [roomNum](const Room& r) { return r.number == roomNum && !r.isAvailable; });
        if (roomIt != rooms.end()) {
            auto resIt = find_if(reservations.begin(), reservations.end(), [roomNum](const Reservation& res) { return res.roomNumber == roomNum; });
            if (resIt != reservations.end()) {
                string guestName = resIt->guestName;
                replace(guestName.begin(), guestName.end(), '_', ' ');
                cout << MAGENTA << "\n+-----------------------------------+" << endl;
                cout << "|        Check-Out Preview         |" << RESET << endl;
                cout << MAGENTA << "+-----------------------------------+" << RESET << endl;
                cout << CYAN << "Guest: " << guestName << endl;
                cout << "Room #: " << roomNum << endl;
                cout << "Check-In: " << resIt->checkInDate << endl;
                cout << "Check-Out: " << resIt->checkOutDate << RESET << endl;
                cout << YELLOW << "Confirm? (y/n or 'b' to cancel): " << RESET;
                string confirm;
                getline(cin, confirm);
                if (confirm == "y" || confirm == "Y") {
                    roomIt->isAvailable = true;
                    reservations.erase(resIt);
                    saveRooms();
                    saveReservations();
                    loadingAnimation("Processing check-out");
                    cout << GREEN << "=> Checked out successfully!" << RESET << endl;
                } else {
                    cout << YELLOW << "=> Check-out cancelled." << RESET << endl;
                }
            } else {
                cout << RED << "No reservation found for this room." << RESET << endl;
            }
        } else {
            cout << RED << "Room is already available or invalid." << RESET << endl;
        }
        this_thread::sleep_for(chrono::milliseconds(1000));
        clearScreen();
    }

    void addNewRoom() {
        string input;
        cout << BLUE << BOLD << "+-----------------------------------+" << endl;
        cout << "|          Add New Room            |" << RESET << endl;
        cout << BLUE << "+-----------------------------------+" << RESET << endl;
        cout << YELLOW << "Enter new room number (or 'b' to return): " << RESET;
        getline(cin, input);
        if (input == "b" || input == "B") {
            clearScreen();
            return;
        }
        int roomNum;
        try {
            roomNum = stoi(input);
            if (roomNum <= 0) throw out_of_range("Negative room number");
        } catch (...) {
            cout << RED << "Invalid room number." << RESET << endl;
            this_thread::sleep_for(chrono::milliseconds(1000));
            clearScreen();
            return;
        }
        if (roomExists(roomNum)) {
            cout << RED << "Room number already exists." << RESET << endl;
            this_thread::sleep_for(chrono::milliseconds(1000));
            clearScreen();
            return;
        }
        string type;
        double price;
        cout << YELLOW << "Enter room type (e.g., Single, Double): " << RESET;
        getline(cin, type);
        if (type.empty()) {
            cout << RED << "Room type cannot be empty." << RESET << endl;
            this_thread::sleep_for(chrono::milliseconds(1000));
            clearScreen();
            return;
        }
        cout << YELLOW << "Enter room price: " << RESET;
        cin >> price;
        if (cin.fail() || price <= 0) {
            cout << RED << "Invalid price." << RESET << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            this_thread::sleep_for(chrono::milliseconds(1000));
            clearScreen();
            return;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the newline after cin >>
        cout << MAGENTA << "\n+-----------------------------------+" << endl;
        cout << "|         New Room Preview         |" << RESET << endl;
        cout << MAGENTA << "+-----------------------------------+" << RESET << endl;
        cout << CYAN << "Room #: " << roomNum << endl;
        cout << "Type: " << type << endl;
        cout << "Price: " << fixed << setprecision(2) << price << RESET << endl;
        cout << YELLOW << "Confirm? (y/n or 'b' to cancel): " << RESET;
        string confirm;
        getline(cin, confirm);
        if (confirm == "y" || confirm == "Y") {
            rooms.emplace_back(roomNum, type, price);
            saveRooms();
            loadingAnimation("Adding new room");
            cout << GREEN << "=> New room added successfully!" << RESET << endl;
        } else {
            cout << YELLOW << "=> Room addition cancelled." << RESET << endl;
        }
        this_thread::sleep_for(chrono::milliseconds(1000));
        clearScreen();
    }

    void run() {
        displayASCIIArt();
        displayStoredData();
        int choice;
        do {
            displayMenu();
            string input;
            getline(cin, input);
            try {
                choice = stoi(input);
                if (choice < 1 || choice > 8) throw out_of_range("Invalid choice");
            } catch (...) {
                cout << RED << "Please enter a number between 1 and 8." << RESET << endl;
                this_thread::sleep_for(chrono::milliseconds(1000));
                clearScreen();
                continue;
            }
            clearScreen();
            switch (choice) {
                case 1: viewAvailableRooms(); break;
                case 2: reserveRoom(); break;
                case 3: viewReservations(); break;
                case 4: checkOut(); break;
                case 5: addNewRoom(); break;
                case 6: searchReservation(); break;
                case 7: displayHelp(); break;
                case 8: spinnerAnimation("Saving and exiting"); break;
            }
        } while (choice != 8);
    }
};

int main() {
    #ifdef _WIN32
        system("chcp 65001 > nul");
    #endif
    Hotel hotel;
    hotel.run();
    return 0;
}
