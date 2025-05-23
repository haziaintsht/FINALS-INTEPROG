#include <iostream>  
#include <iomanip>
#include <cstring>
#include <cctype>
#include <stdexcept>
#include <string>
#include <fstream>
#include <sstream>
#include <regex>
#include <ctime>

using namespace std;

bool isNumber(const string &s) {
    if (s.empty()) return false;
    for (size_t i=0; i < s.length(); i++) {
        if (!isdigit(s[i])) return false;
    }
    return true;
}

const int MAX_MOVIES = 10;
const int MAX_SCREENINGS = 50;
const int MAX_BOOKINGS = 60;
const int MAX_USERS = 20;
const int MAX_SEATS = 30;

bool running = true;


class IBookingModificationStrategy;
class RegularUser;  
class Admin;
class Booking;
class Screening;
class Movie;

string buildScreeningDateTime(const string& date, int hour, int minute, int durationMinutes) {
    int year, month, day;
    sscanf(date.c_str(), "%d-%d-%d", &year, &month, &day);

    struct tm startTime = {};
    startTime.tm_year = year - 1900;
    startTime.tm_mon = month - 1;
    startTime.tm_mday = day;
    startTime.tm_hour = hour;
    startTime.tm_min = minute;

    time_t startEpoch = mktime(&startTime);
    time_t endEpoch = startEpoch + durationMinutes * 60;

    struct tm* endTime = localtime(&endEpoch);

    char startBuf[20], endBuf[20];
    strftime(startBuf, sizeof(startBuf), "%Y-%m-%d %H:%M", &startTime);
    strftime(endBuf, sizeof(endBuf), "%H:%M", endTime);

    return string(startBuf) + " - " + string(endBuf);
}

string buildScreeningDateTime(const string& date, int durationMinutes) {
    int year, month, day;
    sscanf(date.c_str(), "%d-%d-%d", &year, &month, &day);

    struct tm startTime = {};
    startTime.tm_year = year - 1900;
    startTime.tm_mon = month - 1;
    startTime.tm_mday = day;
    startTime.tm_hour = 12;  // Fixed 12:00 noon
    startTime.tm_min = 0;

    time_t startEpoch = mktime(&startTime);
    time_t endEpoch = startEpoch + durationMinutes * 60;

    struct tm* endTime = localtime(&endEpoch);

    char startBuf[6], endBuf[6];
    strftime(startBuf, sizeof(startBuf), "%H:%M", &startTime);
    strftime(endBuf, sizeof(endBuf), "%H:%M", endTime);

    return date + " " + string(startBuf) + " - " + string(endBuf);
}


string toUpperStr(const string &s) {
    string result = s;
    for (size_t i=0; i < s.length(); i++) {
        result[i] = toupper(result[i]);
    }
    return result;
}

void clearInput() {
    cin.clear();
    cin.ignore(10000, '\n');
}

// Exception class
class InputException : public exception {
    string message;
public:
    InputException(const string &msg) : message(msg) {}
    const char* what() const noexcept override {
        return message.c_str();
    }
};


class IBookingModificationStrategy {
public:
    virtual void modifyBooking(Booking* booking) = 0;
    virtual ~IBookingModificationStrategy() {}
};


class BookingModificationStrategy : public IBookingModificationStrategy {
private:
    class CinemaBookingSystem* system;
public:
    BookingModificationStrategy(class CinemaBookingSystem* sys);
    void modifyBooking(Booking* booking) override;
};

// Movie class
class Movie {
private:
    int id;
    char name[50];
    char genre[20];
    int duration; // minutes
    double cost;
public:
    Movie() : id(0), duration(0), cost(0.0) { name[0] = genre[0] = '\0'; }
    Movie(int id_, const char* n, const char* g, int d, double c) : id(id_), duration(d), cost(c) {
        strncpy(name, n, 49); name[49] = '\0';
        strncpy(genre, g, 19); genre[19] = '\0';
    }
    int getId() const { return id; }
    const char* getName() const { return name; }
    const char* getGenre() const { return genre; }
    int getDuration() const { return duration; }
    double getCost() const { return cost; }

    void setName(const char* n) { strncpy(name, n, 49); name[49] = '\0'; }
    void setGenre(const char* g) { strncpy(genre, g, 19); genre[19] = '\0'; }
    void setDuration(int d) { duration = d; }
    void setCost(double c) { cost = c; }

    void display() const {
        cout << setw(4) << id << " | "
             << setw(20) << left << name
             << setw(10) << genre
 << setw(8) << duration
<< fixed << setprecision(2)
<< setw(8) << cost
             << "\n";
    }
};


class Screening {
private:
    int id;
    Movie* movie;
    char datetime[25];
    char cinemaHall[10];
    bool seats[MAX_SEATS];
    int seatCapacity;
public:
    Screening() : id(0), movie(nullptr), seatCapacity(MAX_SEATS) {
        datetime[0] = cinemaHall[0] = '\0';
        for (int i = 0; i < seatCapacity; i++) seats[i] = false;
    }
    Screening(int id_, Movie* m, const char* dt, const char* ch) : id(id_), movie(m), seatCapacity(MAX_SEATS) {
        strncpy(datetime, dt, 24); datetime[24] = '\0';
        strncpy(cinemaHall, ch, 9); cinemaHall[9] = '\0';
        for (int i=0; i < seatCapacity; i++) seats[i] = false;
    }

    int getId() const { return id; }
    Movie* getMovie() const { return movie; }
    const char* getDateTime() const { return datetime; }
    const char* getCinemaHall() const { return cinemaHall; }
    int getSeatCapacity() const { return seatCapacity; }

    bool isSeatAvailable(int seatNum) {
        if (seatNum < 1 || seatNum > seatCapacity) return false;
        return !seats[seatNum - 1];
    }

    bool bookSeats(const int seatNums[], int count) {
        for (int i=0; i<count; i++) {
            int s = seatNums[i];
            if (s < 1 || s > seatCapacity || seats[s-1]) return false;
        }
        for (int i=0; i<count; i++) {
            seats[seatNums[i]-1] = true;
        }
        return true;
    }

    void cancelSeats(const int seatNums[], int count) {
        for (int i=0; i<count; i++) {
            int s = seatNums[i];
            if (s >=1 && s <= seatCapacity) seats[s-1] = false;
        }
    }

    void display() const {
        cout << setw(4) << id << " | "
             << setw(20) << left << movie->getName()
             << setw(15) << datetime
             << setw(10) << cinemaHall
             << "Seats: " << seatCapacity << "\n";
    }
};


class User {
protected:
    char username[20];
    char password[20];
public:
    User() { username[0] = password[0] = '\0'; }
    virtual ~User() {}
    virtual void displayDashboard() = 0;
    virtual void login() = 0;
    virtual void signup() = 0;
    const char* getPassword() const { return password; }
    const char* getUsername() const { return username; }
    bool checkPassword(const char* pass) const {
        return strcmp(password, pass) == 0;
    }
    void setUsername(const char* user) {
        strncpy(username, user, 19);
        username[19] = '\0';
    }
    void setPassword(const char* pass) {
        strncpy(password, pass, 19);
        password[19] = '\0';
    }
};


class Booking {
private:
    int id;
    class RegularUser* user;
    Screening* screening;
    int seatNumbers[MAX_SEATS];
    int seatCount;

public:
    Booking() : id(0), user(nullptr), screening(nullptr), seatCount(0) {}
    Booking(int id_, RegularUser* u, Screening* s, const int seats[], int count) : id(id_), user(u), screening(s), seatCount(count) {
        for (int i=0; i < count; i++) seatNumbers[i] = seats[i];
    }
    int getId() const { return id; }
    Screening* getScreening() const { return screening; }
    RegularUser* getUser() const { return user; }
    int getSeatCount() const { return seatCount; }
    const int* getSeats() const { return seatNumbers; }
    void display() const;

    void changeBooking(Screening* newScreening, const int newSeats[], int newCount);
};


void Booking::display() const {
    cout << "Booking ID: " << id << "\n"
         << "Movie: " << screening->getMovie()->getName() << "\n"
         << "Date & Time: " << screening->getDateTime() << "\n"
         << "Seats: ";
    for (int i = 0; i < seatCount; i++) {
        cout << seatNumbers[i];
        if (i < seatCount - 1) cout << ", ";
    }
    cout << "\n";
}

void Booking::changeBooking(Screening* newScreening, const int newSeats[], int newCount) {
    if (!newScreening->bookSeats(newSeats, newCount)) {
        throw InputException("Failed to book requested seats for modified booking.");
    }
    screening->cancelSeats(seatNumbers, seatCount);
    screening = newScreening;
    seatCount = newCount;
    for (int i = 0; i < newCount; i++) {
        seatNumbers[i] = newSeats[i];
    }
}

// RegularUser class
class RegularUser : public User {
private:
    class CinemaBookingSystem* system;

public:
    RegularUser();
    RegularUser(const RegularUser& other);
    RegularUser& operator=(const RegularUser& other);
    ~RegularUser() {}

    void login() override;
    void signup() override;
    void displayDashboard() override;
    void bookTicket();
    void modifyBooking();
    void cancelBooking();
    void viewMyBookings();
};

// CinemaBookingSystem Singleton
class CinemaBookingSystem {
private:
    Movie movies[MAX_MOVIES];
    int movieCount;
    Screening screenings[MAX_SCREENINGS];
    int screeningCount;
    Booking bookings[MAX_BOOKINGS];
    int bookingCount;
    RegularUser* users[MAX_USERS];
    int userCount;
    User* currentUser;
    static CinemaBookingSystem* instance;
    IBookingModificationStrategy* bookingModificationStrategy;

    char adminUsername[20];
    char adminPassword[20];

    // Change the CinemaBookingSystem constructor to:
CinemaBookingSystem() : movieCount(0), screeningCount(0), bookingCount(0), userCount(0), currentUser(nullptr) {
    bookingModificationStrategy = new class BookingModificationStrategy(this);
    strncpy(adminUsername, "ADMIN", 19); adminUsername[19] = '\0';
    strncpy(adminPassword, "ADMIN123", 19); adminPassword[19] = '\0';
    
    loadUsersFromFile();  // Add this line
}
    
    // File load/save helpers
    void loadUsersFromFile() {
        ifstream inFile("users.txt");
        if (inFile.is_open()) {
            string username, password;
            while (inFile >> username >> password) {
                if (userCount >= MAX_USERS) break;
                RegularUser* user = new RegularUser();
                user->setUsername(username.c_str());
                user->setPassword(password.c_str());
                users[userCount++] = user;
            }
            inFile.close();
        }
    }

    void saveUsersToFile() {
        ofstream outFile("users.txt");
        if (outFile.is_open()) {
            for (int i = 0; i < userCount; i++) {
                outFile << users[i]->getUsername() << " " << users[i]->getPassword() << endl;
            }
            outFile.close();
        }
    }

public:
    ~CinemaBookingSystem() {
        delete bookingModificationStrategy;
        for (int i = 0; i < userCount; i++) {
            delete users[i];
        }
    }

    static CinemaBookingSystem* getInstance() {
        if (!instance) {
            instance = new CinemaBookingSystem();
        }
        return instance;
    }

    bool validateAdmin(const char* user, const char* pass) {
        return (strcasecmp(user, adminUsername) == 0) && (strcmp(pass, adminPassword) == 0);
    }

    void setAdminUsername(const char* user) {
        strncpy(adminUsername, user, 19); adminUsername[19] = '\0';
    }

    void setAdminPassword(const char* pass) {
        strncpy(adminPassword, pass, 19); adminPassword[19] = '\0';
    }

     void saveUsersToFilePublic() {
        saveUsersToFile();
    }
    void addMovie(const char* name, const char* genre, int duration, double cost) {
        if (movieCount >= MAX_MOVIES) throw InputException("Movie limit reached.");
        int newId = movieCount + 1;
        movies[movieCount++] = Movie(newId, name, genre, duration, cost);
    }

    void editMovie(int id, const char* name, const char* genre, int duration, double cost) {
        Movie* m = findMovieById(id);
        if (!m) throw InputException("Movie not found.");
        m->setName(name);
        m->setGenre(genre);
        m->setDuration(duration);
        m->setCost(cost);
    }

    void deleteMovie(int id) {
        int idx = -1;
        for (int i = 0; i < movieCount; i++) {
            if (movies[i].getId() == id) {
                idx = i;
                break;
            }
        }
        if (idx == -1) throw InputException("Movie not found.");
        
        for (int i = 0; i < screeningCount;) {
            if (screenings[i].getMovie()->getId() == id) {
                deleteScreening(screenings[i].getId());
            } else {
                i++;
            }
        }
        
        for (int i = idx; i < movieCount - 1; i++) {
            movies[i] = movies[i + 1];
        }
        movieCount--;
    }

    void displayMovies() const {
        cout << "+----+----------------------+----------+--------+--------+\n";
        cout << "| ID | Name                 | Genre    |Duration| Cost   |\n";
        cout << "+----+----------------------+----------+--------+--------+\n";
        for (int i = 0; i < movieCount; i++) {
            movies[i].display();
        }
        cout << "+----+----------------------+----------+--------+--------+\n";
    }

    Movie* findMovieById(int id) const {
        for (int i = 0; i < movieCount; i++) {
            if (movies[i].getId() == id) return const_cast<Movie*>(&movies[i]);
        }
        return nullptr;
    }

void addScreening(int movieId, const char* datetime, const char* hall) {
    if (screeningCount >= MAX_SCREENINGS) throw InputException("Screening limit reached.");
    Movie* m = findMovieById(movieId);
    if (!m) throw InputException("Movie not found for screening.");
    
    // Ensure datetime is formatted correctly
    if (strlen(datetime) < 16) {
        throw InputException("Invalid datetime format.");
    }

    int newId = screeningCount + 1;
    screenings[screeningCount++] = Screening(newId, m, datetime, hall);
}


    void editScreening(int id, int movieId, const char* datetime, const char* hall) {
        Screening* s = findScreeningById(id);
        if (!s) throw InputException("Screening not found.");
        Movie* m = findMovieById(movieId);
        if (!m) throw InputException("Movie not found for screening.");
        *s = Screening(id, m, datetime, hall);
    }

    void deleteScreening(int id) {
        int idx = -1;
        for (int i = 0; i < screeningCount; i++) {
            if (screenings[i].getId() == id) {
                idx = i;
                break;
            }
        }
        if (idx == -1) throw InputException("Screening not found.");
        
        for (int i = 0; i < bookingCount;) {
            if (bookings[i].getScreening()->getId() == id) {
                cancelBookingByIndex(i);
            } else {
                i++;
            }
        }
        
        for (int i = idx; i < screeningCount - 1; i++) {
            screenings[i] = screenings[i + 1];
        }
        screeningCount--;
    }

void displayScreenings() const {
    cout << "+----+----------------------+-----------------------------+-----------+------------------------+\n";
    cout << "| ID | Movie Name           | Date & Time                 | Hall      | Seat Capacity  |\n";
    cout << "+----+----------------------+-----------------------------+-----------+------------------------+\n";
    for (int i = 0; i < screeningCount; i++) {
        string dateTimeStr(screenings[i].getDateTime());

        // Extract start and end times
        string startTime, endTime;
        size_t dashPos = dateTimeStr.find(" - ");
        if (dashPos != string::npos) {
            startTime = dateTimeStr.substr(0, dashPos); // Get the start time
            endTime = dateTimeStr.substr(dashPos + 3); // Get the end time (after " - ")
        } else {
            startTime = "Invalid Date"; // Fallback for invalid date
            endTime = "Invalid Time"; // Fallback for invalid time
        }

        // Displaying the screening information
        cout << "|" << setw(4) << screenings[i].getId() << " "
             << "| " << setw(20) << left << screenings[i].getMovie()->getName()
             << "| " << setw(19) << left << startTime + " - " + endTime // Displaying only the relevant time range
             << "| " << setw(5) << "Cinema Hall: " <<screenings[i].getCinemaHall()
             << "| " << setw(15) << "Seats: " << screenings[i].getSeatCapacity() << " |\n";
    }
    cout << "+----+----------------------+-----------------------------+-----------+------------------------+\n";
}

    Screening* findScreeningById(int id) const {
        for (int i = 0; i < screeningCount; i++) {
            if (screenings[i].getId() == id) return const_cast<Screening*>(&screenings[i]);
        }
        return nullptr;
    }

    void addUser(RegularUser* user) {
        if (userCount >= MAX_USERS) throw InputException("User limit reached.");
        users[userCount++] = user;
        saveUsersToFile(); 
    }

    RegularUser* findUserByUsername(const char* username) const {
        for (int i = 0; i < userCount; i++) {
            if (strcmp(users[i]->getUsername(), username) == 0) return users[i];
        }
        return nullptr;
    }

    void addBooking(RegularUser* user, Screening* screening, const int seats[], int count) {
        if (bookingCount >= MAX_BOOKINGS) throw InputException("Booking limit reached.");
        if (!screening->bookSeats(seats, count)) throw InputException("Some seats are already booked or invalid.");
        int newId = bookingCount + 1;
        bookings[bookingCount++] = Booking(newId, user, screening, seats, count);
    }

    Booking* findBookingById(int id) {
        for (int i = 0; i < bookingCount; i++) {
            if (bookings[i].getId() == id) return &bookings[i];
        }
        return nullptr;
    }

    int findBookingIndexById(int id) {
        for (int i = 0; i < bookingCount; i++) {
            if (bookings[i].getId() == id) return i;
        }
        return -1;
    }

    void cancelBookingByIndex(int index) {
        bookings[index].getScreening()->cancelSeats(bookings[index].getSeats(), bookings[index].getSeatCount());
        for (int i = index; i < bookingCount - 1; i++) {
            bookings[i] = bookings[i + 1];
        }
        bookingCount--;
    }

    void displayAllBookings() const {
        if (bookingCount == 0) {
            cout << "No bookings available.\n";
            return;
        }
        cout << "+----+------------+----------------------+---------------+---------+-------+\n";
        cout << "| ID | Username   | Movie                | Date & Time   | Seats   | Count |\n";
        cout << "+----+------------+----------------------+---------------+---------+-------+\n";
        for (int i = 0; i < bookingCount; i++) {
            const Booking& b = bookings[i];
            cout << "|" << setw(3) << b.getId() << " ";
            cout << "| " << setw(10) << left << b.getUser()->getUsername();
            cout << "| " << setw(20) << b.getScreening()->getMovie()->getName();
            cout << "| " << setw(14) << b.getScreening()->getDateTime();
            cout << "| ";
            const int* seatnums = b.getSeats();
            for (int s = 0; s < b.getSeatCount(); s++) {
                cout << seatnums[s];
                if (s < b.getSeatCount() - 1) cout << ",";
            }
            cout << setw(7 - b.getSeatCount()) << " ";
            cout << "| " << setw(5) << b.getSeatCount() << " |\n";
        }
        cout << "+----+------------+----------------------+---------------+---------+-------+\n";
    }

    void generateMovieReport() const {
        cout << "--- Movie Booking Report ---\n";
        for (int i = 0; i < movieCount; i++) {
            int totalBooked = 0;
            for (int j = 0; j < bookingCount; j++) {
                if (bookings[j].getScreening()->getMovie()->getId() == movies[i].getId()) {
                    totalBooked += bookings[j].getSeatCount();
                }
            }
            cout << "Movie: " << movies[i].getName() << " - Booked Seats: " << totalBooked << "\n";
        }
    }

    void generateRevenueReport() const {
        cout << "--- Revenue Report ---\n";
        double totalRevenue = 0.0;
        for (int i = 0; i < movieCount; i++) {
            double movieRevenue = 0.0;
            for (int j = 0; j < bookingCount; j++) {
                if (bookings[j].getScreening()->getMovie()->getId() == movies[i].getId()) {
                    movieRevenue += bookings[j].getSeatCount() * movies[i].getCost();
                }
            }
            cout << "Movie: " << movies[i].getName() << " - Revenue: $" << fixed << setprecision(2) << movieRevenue << "\n";
            totalRevenue += movieRevenue;
        }
        cout << "Total Revenue: $" << fixed << setprecision(2) << totalRevenue << "\n";
    }

    IBookingModificationStrategy* getBookingModificationStrategy() {
        return bookingModificationStrategy;
    }

    Booking* getBookings() { return bookings; }
    int getBookingCount() const { return bookingCount; }
};


CinemaBookingSystem* CinemaBookingSystem::instance = nullptr;


BookingModificationStrategy::BookingModificationStrategy(class CinemaBookingSystem* sys) : system(sys) {}

void BookingModificationStrategy::modifyBooking(Booking* booking) {
    cout << "=== Modify Booking ===\n";
    system->displayScreenings();

    cout << "Enter new screening ID: ";
    string input;
    getline(cin, input);
    while (!isNumber(input)) {
        cout << "Invalid input. Enter numeric screening ID: ";
        getline(cin, input);
    }
    int screeningId = stoi(input);

    Screening* newScreening = system->findScreeningById(screeningId);
    if (!newScreening) {
        cout << "Screening not found, returning to dashboard.\n";
        return;
    }

    cout << "Enter number of seats to book: ";
    getline(cin, input);
    while (!isNumber(input)) {
        cout << "Invalid input. Enter numeric value: ";
        getline(cin, input);
    }
    int seatCount = stoi(input);
    if (seatCount <= 0 || seatCount > newScreening->getSeatCapacity()) {
        cout << "Invalid number of seats.\n";
        return;
    }

    int seatNums[MAX_SEATS];
    for (int i = 0; i < seatCount; i++) {
        cout << "Enter seat number " << (i + 1) << ": ";
        getline(cin, input);
        if (!isNumber(input)) {
            cout << "Invalid input. Enter numeric seat number.\n";
            i--;
            continue;
        }
        int seatNum = stoi(input);
        if (!newScreening->isSeatAvailable(seatNum)) {
            cout << "Seat not available or invalid. Try again.\n";
            i--;
            continue;
        }
        seatNums[i] = seatNum;
    }

    try {
        booking->changeBooking(newScreening, seatNums, seatCount);
        cout << "Booking modified successfully.\n";
    } catch (InputException& e) {
        cout << "Error: " << e.what() << "\n";
    }
}

// Implement RegularUser methods
RegularUser::RegularUser() { system = CinemaBookingSystem::getInstance(); }

RegularUser::RegularUser(const RegularUser& other) : User(other) {
    strncpy(username, other.username, 19); username[19] = '\0';
    strncpy(password, other.password, 19); password[19] = '\0';
    system = CinemaBookingSystem::getInstance();
}

RegularUser& RegularUser::operator=(const RegularUser& other) {
    if (this != &other) {
        User::operator=(other);
        system = CinemaBookingSystem::getInstance();
    }
    return *this;
}

void RegularUser::login() {
    string user, pass;
    cout << "Enter username: ";
    cin >> user;
    cout << "Enter password: ";
    cin >> pass;
    clearInput();
    RegularUser* found = system->findUserByUsername(user.c_str());
    if (!found) {
        cout << "User not found.\n";
        return;
    }
    if (!found->checkPassword(pass.c_str())) {
        cout << "Incorrect password.\n";
        return;
    }
    *this = *found;
    cout << "Successfully logged in!\n";
    displayDashboard();
}

void RegularUser::signup() {
    string user, pass;
    cout << "Enter username: ";
    cin >> user;
    cout << "Enter password: ";
    cin >> pass;
    clearInput();
    if (system->findUserByUsername(user.c_str())) {
        cout << "Username already taken.\n";
        return;
    }
    setUsername(user.c_str());
    setPassword(pass.c_str());
    try {
        RegularUser* newUser = new RegularUser(*this);
        system->addUser(newUser);
        cout << "Successfully signed up!\n";
    } catch (InputException& e) {
        cout << "Error: " << e.what() << "\n";
    }
}

void RegularUser::displayDashboard() {
    string input;
    int choice;
    do {
        cout << "\n==== USER DASHBOARD ====\n";
        cout << "1. Browse Movies\n";
        cout << "2. Browse Screenings\n";
        cout << "3. Book Ticket\n";
        cout << "4. Modify Booking\n";
        cout << "5. Cancel Booking\n";
        cout << "6. View My Bookings\n";
        cout << "7. Logout\n";
        cout << "Enter your Choice: ";
        getline(cin, input);
        if (input.length() != 1 || !isdigit(input[0])) {
            cout << "Invalid choice.\n";
            continue;
        }
        choice = input[0] - '0';
        switch (choice) {
            case 1: system->displayMovies(); break;
            case 2: system->displayScreenings(); break;
            case 3: bookTicket(); break;
            case 4: modifyBooking(); break;
            case 5: cancelBooking(); break;
            case 6: viewMyBookings(); break;
            case 7: cout << "Logging out now...\n"; return;
            default: cout << "Invalid choice.\n"; break;
        }
    } while (running);
}

void RegularUser::bookTicket() {
    string input;
    cout << "Enter movie ID to book: ";
    getline(cin, input);
    if (!isNumber(input)) {
        cout << "Invalid movie ID.\n";
        return;
    }
    int movieId = stoi(input);
    Movie* movie = system->findMovieById(movieId);
    if (!movie) {
        cout << "Movie not found.\n";
        return;
    }

    system->displayScreenings();
    cout << "Enter screening ID to book: ";
    getline(cin, input);
    if (!isNumber(input)) {
        cout << "Invalid screening ID.\n";
        return;
    }
    int screeningId = stoi(input);
    Screening* screening = system->findScreeningById(screeningId);
    if (!screening) {
        cout << "Screening not found.\n";
        return;
    }

    cout << "Enter number of tickets: ";
    getline(cin, input);
    if (!isNumber(input)) {
        cout << "Invalid number.\n";
        return;
    }
    int ticketCount = stoi(input);
    if (ticketCount <= 0 || ticketCount > screening->getSeatCapacity()) {
        cout << "Invalid number of tickets.\n";
        return;
    }

    int seats[MAX_SEATS];
for (int i = 0; i < ticketCount; i++) {
    cout << "Enter seat number " << (i + 1) << ": ";
    getline(cin, input);
    if (!isNumber(input)) {
        cout << "Invalid seat.\n";
        i--;
        continue;
    }
    int seatNum = stoi(input);

    // Check for duplicates in this user's current selection
    bool duplicate = false;
    for (int j = 0; j < i; j++) {
        if (seats[j] == seatNum) {
            duplicate = true;
            break;
        }
    }

    if (duplicate) {
        cout << "You've already selected that seat. Please pick a different one.\n";
        i--;
        continue;
    }

    if (!screening->isSeatAvailable(seatNum)) {
        cout << "Seat already taken. Please choose another.\n";
        i--;
        continue;
    }

    seats[i] = seatNum;
}


    try {
        system->addBooking(this, screening, seats, ticketCount);
        cout << "Booking finished.\n";
    } catch (InputException& e) {
        cout << "Booking error: " << e.what() << "\n";
    }
}

void RegularUser::modifyBooking() {
    cout << "Your bookings:\n";
    Booking* allBookings = system->getBookings();
    bool haveBookings = false;
    for (int i = 0; i < system->getBookingCount(); i++) {
        if (allBookings[i].getUser() == this) {
            allBookings[i].display();
            haveBookings = true;
        }
    }
    if (!haveBookings) {
        cout << "No bookings to modify.\n";
        return;
    }

    string input;
    cout << "Enter booking ID to modify: ";
    getline(cin, input);
    if (!isNumber(input)) {
        cout << "Invalid booking ID.\n";
        return;
    }
    int bookingId = stoi(input);
    Booking* booking = system->findBookingById(bookingId);
    if (!booking || booking->getUser() != this) {
        cout << "Booking not found.\n";
        return;
    }
    system->getBookingModificationStrategy()->modifyBooking(booking);
}

void RegularUser::cancelBooking() {
    cout << "Your bookings:\n";
    Booking* allBookings = system->getBookings();
    bool haveBookings = false;
    for (int i = 0; i < system->getBookingCount(); i++) {
        if (allBookings[i].getUser() == this) {
            allBookings[i].display();
            haveBookings = true;
        }
    }
    if (!haveBookings) {
        cout << "No bookings to cancel.\n";
        return;
    }

    string input;
    cout << "Enter booking ID to cancel: ";
    getline(cin, input);
    if (!isNumber(input)) {
        cout << "Invalid input.\n";
        return;
    }
    int bookingId = stoi(input);
    int index = system->findBookingIndexById(bookingId);
    if (index == -1 || system->getBookings()[index].getUser() != this) {
        cout << "Booking not found.\n";
        return;
    }
    cout << "Are you sure to cancel (Y/N)? ";
    char confirm;
    cin >> confirm;
    clearInput();
    if (toupper(confirm) == 'Y') {
        system->cancelBookingByIndex(index);
        cout << "Booking cancelled.\n";
    } else {
        cout << "Cancellation aborted.\n";
    }
}

void RegularUser::viewMyBookings() {
    Booking* allBookings = system->getBookings();
    bool haveBookings = false;

    cout << "+------------+----------------------+---------------+---------+-------+\n";
    cout << "| Movie      | Date & Time          | Hall     | Seats     | Count |\n";
    cout << "+------------+----------------------+----------+-----------+-------+\n";

    for (int i = 0; i < system->getBookingCount(); i++) {
        if (allBookings[i].getUser() == this) {
            Screening* s = allBookings[i].getScreening();
            const int* seats = allBookings[i].getSeats();
            cout << "| " << setw(10) << left << s->getMovie()->getName()
                 << "| " << setw(20) << left << s->getDateTime()
                 << "| " << setw(8) << left << s->getCinemaHall()
                 << "| ";
            for (int j = 0; j < allBookings[i].getSeatCount(); j++) {
                cout << seats[j];
                if (j < allBookings[i].getSeatCount() - 1) cout << ",";
            }
            cout << setw(11 - allBookings[i].getSeatCount() * 2) << " ";
            cout << "| " << setw(5) << allBookings[i].getSeatCount() << " |\n";
            haveBookings = true;
        }
    }

    cout << "+------------+----------------------+----------+-----------+-------+\n";
    if (!haveBookings) {
        cout << "No bookings found.\n";
    }
}



class Admin : public User {
private:
CinemaBookingSystem* system;
bool loggedIn; 
public:
    Admin() { system = CinemaBookingSystem::getInstance();
    loggedIn = false;
}
    void login() override;
    void signup() override {}
    void displayDashboard() override;
    void logout();
    bool isLoggedIn() const { return loggedIn; }

private:
    void addMovie();
    void editMovie();
    void deleteMovie();
    void addScreening();
    void editScreening();
    void deleteScreening();
};

void Admin::login() {
    string user, pass;
    cout << "Enter admin username: ";
    cin >> user;
    cout << "Enter admin password: ";
    cin >> pass;
    clearInput();
    if (!system->validateAdmin(user.c_str(), pass.c_str())) {
        cout << "Invalid admin credentials.\n";
        return;
    }
    loggedIn = true;
    cout << "Successfully logged in as admin.\n";
    displayDashboard();

   
}

void Admin::displayDashboard() {
    string input;
    int choice;
    while (loggedIn) {
        cout << "\n=== ADMIN DASHBOARD ===\n";
        cout << "1. Add Movie\n";
        cout << "2. Edit Movie\n";
        cout << "3. Delete Movie\n";
        cout << "4. Add Screening\n";
        cout << "5. Edit Screening\n";
        cout << "6. Delete Screening\n";
        cout << "7. View All Bookings\n";
        cout << "8. Generate Movie Report\n";
        cout << "9. Generate Revenue Report\n";
        cout << "10. Logout\n";
        cout << "Enter your choice: ";
        getline(cin, input);
        
        // Modified validation to handle multi-digit numbers
        if (!isNumber(input)) {
            cout << "Invalid choice.\n";
            continue;
        }
        choice = stoi(input);
        
        switch (choice) {
            case 1: addMovie(); break;
            case 2: editMovie(); break;
            case 3: deleteMovie(); break;
            case 4: addScreening(); break;
            case 5: editScreening(); break;
            case 6: deleteScreening(); break;
            case 7: system->displayAllBookings(); break;
            case 8: system->generateMovieReport(); break;
            case 9: system->generateRevenueReport(); break;
            case 10: 
                logout();
                return;
            default:
                cout << "Invalid choice.\n";
        }
    }
}

void Admin::addMovie() {
    char name[50], genre[20];
    int duration;
    double cost;
    cout << "Enter movie name: ";
    cin.getline(name, 50);
cout << "Choose genre:\n";
cout << "1. Action\n";
cout << "2. Comedy\n";
cout << "3. Drama\n";
cout << "4. Horror\n";
cout << "5. Sci-Fi\n";
cout << "6. Romance\n";
string genreChoice;
getline(cin, genreChoice);
if (genreChoice == "1") strcpy(genre, "Action");
else if (genreChoice == "2") strcpy(genre, "Comedy");
else if (genreChoice == "3") strcpy(genre, "Drama");
else if (genreChoice == "4") strcpy(genre, "Horror");
else if (genreChoice == "5") strcpy(genre, "Sci-Fi");
else if (genreChoice == "6") strcpy(genre, "Romance");
else strcpy(genre, "Unknown");

    cout << "Enter duration (minutes): ";
    string input;
    getline(cin, input);
    while (!isNumber(input)) {
        cout << "Invalid input. Enter numeric duration: ";
        getline(cin, input);
    }
    duration = stoi(input);
    cout << "Enter cost: ";
    getline(cin, input);
    try {
        cost = stod(input);
    } catch (...) {
        cout << "Invalid cost input.\n";
        return;
    }
    try {
        system->addMovie(name, genre, duration, cost);
        cout << "Movie added successfully.\n";
    } catch (InputException& e) {
        cout << "Error: " << e.what() << "\n";
    }
}

void Admin::editMovie() {
    system->displayMovies();
    cout << "Enter movie ID to edit: ";
    string input;
    getline(cin, input);
    if (!isNumber(input)) {
        cout << "Invalid movie ID.\n";
        return;
    }
    int movieId = stoi(input);
    Movie* m = system->findMovieById(movieId);
    if (!m) {
        cout << "Movie not found.\n";
        return;
    }
    char name[50], genre[20];
    int duration;
    double cost;
    cout << "Enter new name: ";
    cin.getline(name, 50);
cout << "Enter New genre:\n";
cout << "1. Action\n";
cout << "2. Comedy\n";
cout << "3. Drama\n";
cout << "4. Horror\n";
cout << "5. Sci-Fi\n";
cout << "6. Romance\n";
string genreChoice;
getline(cin, genreChoice);
if (genreChoice == "1") strcpy(genre, "Action");
else if (genreChoice == "2") strcpy(genre, "Comedy");
else if (genreChoice == "3") strcpy(genre, "Drama");
else if (genreChoice == "4") strcpy(genre, "Horror");
else if (genreChoice == "5") strcpy(genre, "Sci-Fi");
else if (genreChoice == "6") strcpy(genre, "Romance");
else strcpy(genre, "Unknown");

    cout << "Enter new duration (minutes): ";
    getline(cin, input);
    while (!isNumber(input)) {
        cout << "Invalid input. Enter numeric duration: ";
        getline(cin, input);
    }
    duration = stoi(input);
    cout << "Enter new cost: ";
    getline(cin, input);
    try {
        cost = stod(input);
    } catch (...) {
        cout << "Invalid cost input.\n";
        return;
    }
    try {
        system->editMovie(movieId, name, genre, duration, cost);
        cout << "Movie edited successfully.\n";
    } catch (InputException& e) {
        cout << "Error: " << e.what() << "\n";
    }
}

void Admin::deleteMovie() {
    system->displayMovies();
    cout << "Enter movie ID to delete: ";
    string input;
    getline(cin, input);
    if (!isNumber(input)) {
        cout << "Invalid input.\n";
        return;
    }
    int movieId = stoi(input);
    cout << "Are you sure to delete this movie? (Y/N): ";
    char c;
    cin >> c;
    clearInput();
    if (toupper(c) != 'Y') {
        cout << "Deletion cancelled.\n";
        return;
    }
    try {
        system->deleteMovie(movieId);
        cout << "Movie deleted.\n";
    } catch (InputException& e) {
        cout << "Error: " << e.what() << "\n";
    }
}

void Admin::addScreening() {
    system->displayMovies();
    cout << "Enter movie ID to add screening for: ";
    string input;
    getline(cin, input);
    if (!isNumber(input)) {
        cout << "Invalid movie ID.\n";
        return;
    }
    int movieId = stoi(input);
char hall[10];
char datetime[50];
cout << "Enter screening date (YYYY-MM-DD): ";
getline(cin, input);
while (!regex_match(input, regex("^\\d{4}-\\d{2}-\\d{2}$"))) {
    cout << "Invalid format. Please enter date as YYYY-MM-DD: ";
    getline(cin, input);
}
string date = input;

// Start hour
cout << "Enter start hour (0-23): ";
getline(cin, input);
while (!regex_match(input, regex("^\\d{1,2}$")) || stoi(input) < 0 || stoi(input) > 23) {
    cout << "Invalid hour. Please enter a number between 0 and 23: ";
    getline(cin, input);
}
int hour = stoi(input);

// Start minute (optional)
cout << "Enter start minutes (0-59) [optional]: ";
getline(cin, input);
int minute = 0;
if (!input.empty()) {
    while (!regex_match(input, regex("^\\d{1,2}$")) || stoi(input) < 0 || stoi(input) > 59) {
        cout << "Invalid minutes. Please enter 0–59 or leave blank: ";
        getline(cin, input);
    }
    if (!input.empty()) minute = stoi(input);
}

// Get movie to retrieve duration
Movie* movie = system->findMovieById(movieId);
if (!movie) {
    cout << "Movie not found.\n";
    return;
}
string fullDateTime = buildScreeningDateTime(date, hour, minute, movie->getDuration());
strncpy(datetime, fullDateTime.c_str(), sizeof(datetime)-1);
datetime[sizeof(datetime)-1] = '\0';

cout << "Enter cinema hall: ";
cin.getline(hall, 10);

try {
    system->addScreening(movieId, datetime, hall);
    cout << "Screening added.\n";
} catch (InputException& e) {
    cout << "Error: " << e.what() << "\n";
}

}

void Admin::editScreening() {
    system->displayScreenings();
    cout << "Enter screening ID to edit: ";
    string input;
    getline(cin, input);
    if (!isNumber(input)) {
        cout << "Invalid screening ID.\n";
        return;
    }
    int screeningId = stoi(input);
    Screening* s = system->findScreeningById(screeningId);
    if (!s) {
        cout << "Screening not found.\n";
        return;
    }
    system->displayMovies();
    cout << "Enter new movie ID: ";
    getline(cin, input);
    if (!isNumber(input)) {
        cout << "Invalid movie ID.\n";
        return;
    }
    int movieId = stoi(input);
    char hall[10];
char datetime[50];
cout << "Enter screening date (YYYY-MM-DD): ";
getline(cin, input);
while (!regex_match(input, regex("^\\d{4}-\\d{2}-\\d{2}$"))) {
    cout << "Invalid format. Please enter date as YYYY-MM-DD: ";
    getline(cin, input);
}
string date = input;

// Start hour
cout << "Enter start hour (0-23): ";
getline(cin, input);
while (!regex_match(input, regex("^\\d{1,2}$")) || stoi(input) < 0 || stoi(input) > 23) {
    cout << "Invalid hour. Please enter a number between 0 and 23: ";
    getline(cin, input);
}
int hour = stoi(input);

// Start minute (optional)
cout << "Enter start minutes (0-59) [optional]: ";
getline(cin, input);
int minute = 0;
if (!input.empty()) {
    while (!regex_match(input, regex("^\\d{1,2}$")) || stoi(input) < 0 || stoi(input) > 59) {
        cout << "Invalid minutes. Please enter 0–59 or leave blank: ";
        getline(cin, input);
    }
    if (!input.empty()) minute = stoi(input);
}

// Get movie to retrieve duration
Movie* movie = system->findMovieById(movieId);
if (!movie) {
    cout << "Movie not found.\n";
    return;
}
string fullDateTime = buildScreeningDateTime(date, hour, minute, movie->getDuration());
strncpy(datetime, fullDateTime.c_str(), sizeof(datetime)-1);
datetime[sizeof(datetime)-1] = '\0';

cout << "Enter cinema hall: ";
cin.getline(hall, 10);

try {
    system->addScreening(movieId, datetime, hall);
    cout << "Screening added.\n";
} catch (InputException& e) {
    cout << "Error: " << e.what() << "\n";
}

}

void Admin::deleteScreening() {
    system->displayScreenings();
    cout << "Enter screening ID to delete: ";
    string input;
    getline(cin, input);
    if (!isNumber(input)) {
        cout << "Invalid input.\n";
        return;
    }
    int screeningId = stoi(input);
    cout << "Are you sure to delete this screening? (Y/N): ";
    char c;
    cin >> c;
    clearInput();
    if (toupper(c) != 'Y') {
        cout << "Deletion cancelled.\n";
        return;
    }
    try {
        system->deleteScreening(screeningId);
        cout << "Screening deleted.\n";
    } catch (InputException& e) {
        cout << "Error: " << e.what() << "\n";
    }
}
void Admin::logout() {
    loggedIn = false;
    cout << "Logging out...\n";

}
    
 


int main() {
    CinemaBookingSystem* system = CinemaBookingSystem::getInstance();
    string input;
    int choice;
    while (running) {
        cout << "====WELCOME TO GROUP 4 CINEMA BOOKING SYSTEM====\n";
        cout << "1. Regular user Log in\n";
        cout << "2. Regular user Sign up\n";
        cout << "3. Admin Log in\n";
        cout << "4. Exit\n";
        cout << "Enter choice: ";
        getline(cin, input);
        if (input.length() != 1 || !isdigit(input[0])) {
            cout << "Invalid input.\n";
            continue;
        }
        choice = input[0] - '0';
        try {
            if (choice == 1) {
                RegularUser user;
                user.login();
            } else if (choice == 2) {
                RegularUser user;
                user.signup();
            } else if (choice == 3) {
                Admin admin;
                admin.login();
                // After logout, it will automatically return here
            } else if (choice == 4) {
                running = false;
            } else {
                cout << "Invalid choice.\n";
            }
        } catch (InputException& e) {
            cout << "Error: " << e.what() << endl;
        }
    }
    delete CinemaBookingSystem::getInstance(); 
    return 0;
}