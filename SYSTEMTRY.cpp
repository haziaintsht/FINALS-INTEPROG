#include &lt;iostream&gt;
#include &lt;iomanip&gt;
#include &lt;cstring&gt;
#include &lt;cctype&gt;
#include &lt;stdexcept&gt;
#include &lt;string&gt;

using namespace std;

// Constants
const int MAX_MOVIES = 50;
const int MAX_SCREENINGS = 100;
const int MAX_BOOKINGS = 200;
const int MAX_USERS = 50;
const int MAX_SEATS = 30;

bool running = true;

// Utility functions
bool isNumber(const string &amp;s) {
    for (size_t i = 0; i &lt; s.length(); i++) {
        if (!isdigit(s[i])) return false;
    }
    return true;
}

string toUpperStr(const string &amp;s) {
    string result = s;
    for (size_t i = 0; i &lt; s.length(); i++) {
        result[i] = toupper(result[i]);
    }
    return result;
}

// Exception classes
class InputException : public exception {
    string message;
public:
    InputException(const string&amp; msg) : message(msg) {}
    const char* what() const noexcept override {
        return message.c_str();
    }
};

// Movie class
class Movie {
private:
    int id;
    char name[50];
    char genre[20];
    int duration; // in minutes
    double cost; // ticket cost per seat
public:
    Movie() : id(0), duration(0), cost(0.0) {
        name[0] = '\0';
        genre[0] = '\0';
    }
    Movie(int i, const char* n, const char* g, int d, double c) : id(i), duration(d), cost(c) {
        strncpy(name, n, 49);
        name[49] = '\0';
        strncpy(genre, g, 19);
        genre[19] = '\0';
    }
    int getId() const { return id; }
    const char* getName() const { return name; }
    const char* getGenre() const { return genre; }
    int getDuration() const { return duration; }
    double getCost() const { return cost; }

    void setName(const char* n) {
        strncpy(name, n, 49);
        name[49] = '\0';
    }
    void setGenre(const char* g) {
        strncpy(genre, g, 19);
        genre[19] = '\0';
    }
    void setDuration(int d) { duration = d; }
    void setCost(double c) { cost = c; }

    void display() const {
        cout &lt;&lt; setw(4) &lt;&lt; id &lt;&lt; " | "
             &lt;&lt; setw(20) &lt;&lt; left &lt;&lt; name
             &lt;&lt; setw(10) &lt;&lt; genre
             &lt;&lt; setw(8) &lt;&lt; duration
             &lt;&lt; setw(8) &lt;&lt; fixed &lt;&lt; setprecision(2) &lt;&lt; cost
             &lt;&lt; "\n";
    }
};

// Forward declarations
class RegularUser;

// Screening class
class Screening {
private:
    int id;
    Movie* movie;
    char datetime[25];
    char cinemaHall[10];
    bool seats[MAX_SEATS];  // true = booked, false = free
    int seatCapacity;
public:
    Screening() : id(0), movie(nullptr), seatCapacity(MAX_SEATS) {
        datetime[0] = '\0';
        cinemaHall[0] = '\0';
        for (int i = 0; i &lt; seatCapacity; i++) seats[i] = false;
    }
    Screening(int i, Movie* m, const char* dt, const char* ch) : id(i), movie(m), seatCapacity(MAX_SEATS) {
        strncpy(datetime, dt, 24);
        datetime[24] = '\0';
        strncpy(cinemaHall, ch, 9);
        cinemaHall[9] = '\0';
        for (int i = 0; i &lt; seatCapacity; i++) seats[i] = false;
    }
    int getId() const { return id; }
    Movie* getMovie() const { return movie; }
    const char* getDateTime() const { return datetime; }
    const char* getCinemaHall() const { return cinemaHall; }
    int getSeatCapacity() const { return seatCapacity; }

    bool isSeatAvailable(int seatNum) {
        if (seatNum &lt; 1 || seatNum &gt; seatCapacity) return false;
        return seats[seatNum - 1] == false;
    }

    bool bookSeats(const int seatNums[], int count) {
        for (int i = 0; i &lt; count; i++) {
            int s = seatNums[i];
            if (s &lt; 1 || s &gt; seatCapacity || seats[s - 1]) return false; // invalid or already booked
        }
        for (int i = 0; i &lt; count; i++) {
            seats[seatNums[i] - 1] = true;
        }
        return true;
    }

    void cancelSeats(const int seatNums[], int count) {
        for (int i = 0; i &lt; count; i++) {
            int s = seatNums[i];
            if (s &gt;= 1 &amp;&amp; s &lt;= seatCapacity) {
                seats[s - 1] = false;
            }
        }
    }

    void display() const {
        cout &lt;&lt; setw(4) &lt;&lt; id &lt;&lt; " | "
             &lt;&lt; setw(20) &lt;&lt; left &lt;&lt; movie-&gt;getName()
             &lt;&lt; setw(15) &lt;&lt; datetime
             &lt;&lt; setw(10) &lt;&lt; cinemaHall
             &lt;&lt; "Seat Capacity: " &lt;&lt; seatCapacity &lt;&lt; "\n";
    }
};

// Booking class
class Booking {
private:
    int id;
    RegularUser* user;
    Screening* screening;
    int seatNumbers[MAX_SEATS];
    int seatCount;
public:
    Booking() : id(0), user(nullptr), screening(nullptr), seatCount(0) {}
    Booking(int i, RegularUser* u, Screening* s, const int seats[], int count) : id(i), user(u), screening(s), seatCount(count) {
        for (int i = 0; i &lt; count; i++) seatNumbers[i] = seats[i];
    }
    int getId() const { return id; }
    Screening* getScreening() const { return screening; }
    RegularUser* getUser() const { return user; }
    int getSeatCount() const { return seatCount; }
    const int* getSeats() const { return seatNumbers; }

    void display() const;

    void changeBooking(Screening* newScreening, const int newSeats[], int newCount) {
        if (!newScreening-&gt;bookSeats(newSeats, newCount)) {
            throw InputException("Failed to book requested seats for modified booking.");
        }
        screening-&gt;cancelSeats(seatNumbers, seatCount);
        screening = newScreening;
        seatCount = newCount;
        for (int i = 0; i &lt; newCount; i++) {
            seatNumbers[i] = newSeats[i];
        }
    }
};

// Base User class
class User {
protected:
    char username[20];
    char password[20];
public:
    User() {
        username[0] = '\0';
        password[0] = '\0';
    }
    virtual ~User() {}
    virtual void displayDashboard() = 0;
    virtual void login() = 0;
    virtual void signup() = 0;
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

class CinemaBookingSystem; // forward declaration

// Strategy Pattern for booking modification
class IBookingModificationStrategy {
public:
    virtual void modifyBooking(Booking* booking) = 0;
    virtual ~IBookingModificationStrategy() {}
};

// Concrete strategy for modifying booking
class BookingModificationStrategy : public IBookingModificationStrategy {
private:
    CinemaBookingSystem* system;
public:
    BookingModificationStrategy(CinemaBookingSystem* sys) : system(sys) {}
    void modifyBooking(Booking* booking) override;
};

// Cinema Booking System Singleton
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

    CinemaBookingSystem() : movieCount(0), screeningCount(0), bookingCount(0), userCount(0), currentUser(nullptr) {
        bookingModificationStrategy = new BookingModificationStrategy(this);
        setAdminUsername("ADMIN");
        setAdminPassword("ADMIN123");
    }
public:
    ~CinemaBookingSystem() {
        delete bookingModificationStrategy;
        for (int i = 0; i &lt; userCount; i++) {
            delete users[i];
        }
    }

    static CinemaBookingSystem* getInstance() {
        if (!instance) {
            instance = new CinemaBookingSystem();
        }
        return instance;
    }

    // Admin credentials
private:
    char adminUsername[20];
    char adminPassword[20];
public:
    void setAdminUsername(const char* user) {
        strncpy(adminUsername, user, 19);
        adminUsername[19] = '\0';
    }
    void setAdminPassword(const char* pass) {
        strncpy(adminPassword, pass, 19);
        adminPassword[19] = '\0';
    }
    bool validateAdmin(const char* user, const char* pass) {
        return (strcasecmp(user, adminUsername) == 0) && (strcmp(pass, adminPassword) == 0);
    }

    void addMovie(const char* name, const char* genre, int duration, double cost) {
        if (movieCount &gt;= MAX_MOVIES) {
            throw InputException("Movie limit reached.");
        }
        int newId = movieCount + 1;
        movies[movieCount] = Movie(newId, name, genre, duration, cost);
        movieCount++;
    }

    void editMovie(int id, const char* name, const char* genre, int duration, double cost) {
        Movie* m = findMovieById(id);
        if (!m) throw InputException("Movie not found.");
        m-&gt;setName(name);
        m-&gt;setGenre(genre);
        m-&gt;setDuration(duration);
        m-&gt;setCost(cost);
    }

    void deleteMovie(int id) {
        int idx = -1;
        for (int i = 0; i &lt; movieCount; i++) {
            if (movies[i].getId() == id) {
                idx = i;
                break;
            }
        }
        if (idx == -1) throw InputException("Movie not found.");
        for (int i = idx; i &lt; movieCount - 1; i++) {
            movies[i] = movies[i + 1];
        }
        movieCount--;
        for (int i = 0; i &lt; screeningCount;) {
            if (screenings[i].getMovie()->getId() == id) {
                deleteScreening(screenings[i].getId());
            }
            else {
                i++;
            }
        }
    }

    void displayMovies() {
        cout &lt;&lt; "+----+----------------------+----------+--------+--------+\n";
        cout &lt;&lt; "| ID | Name                 | Genre    |Duration| Cost   |\n";
        cout &lt;&lt; "+----+----------------------+----------+--------+--------+\n";
        for (int i = 0; i &lt; movieCount; i++) {
            movies[i].display();
        }
        cout &lt;&lt; "+----+----------------------+----------+--------+--------+\n";
    }

    Movie* findMovieById(int id) {
        for (int i = 0; i &lt; movieCount; i++) {
            if (movies[i].getId() == id) return &amp;movies[i];
        }
        return nullptr;
    }

    void addScreening(int movieId, const char* datetime, const char* hall) {
        if (screeningCount &gt;= MAX_SCREENINGS) throw InputException("Screening limit reached.");
        Movie* m = findMovieById(movieId);
        if (!m) throw InputException("Movie not found for screening.");
        int newId = screeningCount + 1;
        screenings[screeningCount] = Screening(newId, m, datetime, hall);
        screeningCount++;
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
        for (int i = 0; i &lt; screeningCount; i++) {
            if (screenings[i].getId() == id) {
                idx = i;
                break;
            }
        }
        if (idx == -1) throw InputException("Screening not found.");
        for (int i = 0; i &lt; bookingCount;) {
            if (bookings[i].getScreening()->getId() == id) {
                cancelBookingByIndex(i);
            }
            else {
                i++;
            }
        }
        for (int i = idx; i &lt; screeningCount - 1; i++) {
            screenings[i] = screenings[i + 1];
        }
        screeningCount--;
    }

    void displayScreenings() {
        cout &lt;&lt; "+----+----------------------+---------------+----------+----------------+\n";
        cout &lt;&lt; "| ID | Movie Name           | Date &amp; Time   | Hall     | Seat Capacity  |\n";
        cout &lt;&lt; "+----+----------------------+---------------+----------+----------------+\n";
        for (int i = 0; i &lt; screeningCount; i++) {
            screenings[i].display();
        }
        cout &lt;&lt; "+----+----------------------+---------------+----------+----------------+\n";
    }

    Screening* findScreeningById(int id) {
        for (int i = 0; i &lt; screeningCount; i++) {
            if (screenings[i].getId() == id) return &amp;screenings[i];
        }
        return nullptr;
    }

    void addUser(RegularUser* user) {
        if (userCount &gt;= MAX_USERS) throw InputException("User limit reached.");
        users[userCount] = user;
        userCount++;
    }

    RegularUser* findUserByUsername(const char* username);

    void addBooking(RegularUser* user, Screening* screening, const int seats[], int count);

    Booking* findBookingById(int id);

    int findBookingIndexById(int id);

    void cancelBookingByIndex(int index);

    void displayAllBookings();

    void generateMovieReport();

    void generateRevenueReport();

    User* loginUser(int choice);

    void userSignup();

    void userDashboard(RegularUser* user);

    void adminDashboard();

    void modifyBooking(Booking* booking, RegularUser* user);

    IBookingModificationStrategy* getBookingModificationStrategy() { return bookingModificationStrategy; }

    // For RegularUser class access
    int getBookingCount() const { return bookingCount; }
    Booking* getBookings() { return bookings; }
};

CinemaBookingSystem* CinemaBookingSystem::instance = nullptr;

// Forward declaration needed for Booking class methods using RegularUser defined later
class RegularUser : public User {
private:
    CinemaBookingSystem* system;
public:
    RegularUser() { system = CinemaBookingSystem::getInstance(); }
    void login() override;
    void signup() override;
    void displayDashboard() override;
    void bookTicket();
    void modifyBooking();
    void cancelBooking();
    void viewMyBookings();
};

// Now define Booking methods that require RegularUser

void Booking::display() const {
    cout &lt;&lt; "Booking ID: " &lt;&lt; id &lt;&lt; "\n";
    cout &lt;&lt; "Movie: " &lt;&lt; screening-&gt;getMovie()-&gt;getName() &lt;&lt; "\n";
    cout &lt;&lt; "Date &amp; Time: " &lt;&lt; screening-&gt;getDateTime() &lt;&lt; "\n";
    cout &lt;&lt; "Seats: ";
    for (int i = 0; i &lt; seatCount; i++) {
        cout &lt;&lt; seatNumbers[i];
        if (i &lt; seatCount - 1) cout &lt;&lt; ", ";
    }
    cout &lt;&lt; "\n";
}

// Implement BookingModificationStrategy
void BookingModificationStrategy::modifyBooking(Booking* booking) {
    cout &lt;&lt; "=== Modify Booking ===\n";
    system-&gt;displayScreenings();
    cout &lt;&lt; "Enter new screening ID: ";
    string input;
    int screeningId;
    getline(cin, input);
    while (!isNumber(input)) {
        cout &lt;&lt; "Invalid input. Enter numeric screening ID: ";
        getline(cin, input);
    }
    screeningId = stoi(input);

    Screening* newScreening = system-&gt;findScreeningById(screeningId);
    if (!newScreening) {
        cout &lt;&lt; "Screening not found, returning to dashboard.\n";
        return;
    }

    cout &lt;&lt; "Enter number of seats to book: ";
    getline(cin, input);
    while (!isNumber(input)) {
        cout &lt;&lt; "Invalid input. Enter numeric value: ";
        getline(cin, input);
    }
    int seatCount = stoi(input);
    if (seatCount &lt;= 0 || seatCount &gt; newScreening-&gt;getSeatCapacity()) {
        cout &lt;&lt; "Invalid number of seats.\n";
        return;
    }
    int seatNums[MAX_SEATS];
    for (int i = 0; i &lt; seatCount; i++) {
        cout &lt;&lt; "Enter seat number " &lt;&lt; (i + 1) &lt;&lt; ": ";
        getline(cin, input);
        if (!isNumber(input)) {
            cout &lt;&lt; "Invalid input. Enter numeric seat number: ";
            i--;
            continue;
        }
        int seatNum = stoi(input);
        if (seatNum &lt; 1 || seatNum &gt; newScreening-&gt;getSeatCapacity() || !newScreening-&gt;isSeatAvailable(seatNum)) {
            cout &lt;&lt; "Seat not available or invalid. Try again.\n";
            i--;
            continue;
        }
        seatNums[i] = seatNum;
    }
    try {
        booking-&gt;changeBooking(newScreening, seatNums, seatCount);
        cout &lt;&lt; "Booking modified successfully.\n";
    }
    catch (InputException& e) {
        cout &lt;&lt; "Error: " &lt;&lt; e.what() &lt;&lt; "\n";
    }
}

// Implement CinemaBookingSystem methods dependent on RegularUser

RegularUser* CinemaBookingSystem::findUserByUsername(const char* username) {
    for (int i = 0; i &lt; userCount; i++) {
        if (strcmp(users[i]-&gt;getUsername(), username) == 0) return users[i];
    }
    return nullptr;
}

void CinemaBookingSystem::addBooking(RegularUser* user, Screening* screening, const int seats[], int count) {
    if (bookingCount &gt;= MAX_BOOKINGS) throw InputException("Booking limit reached.");
    if (!screening-&gt;bookSeats(seats, count)) {
        throw InputException("Some seats are already booked or invalid.");
    }
    int newId = bookingCount + 1;
    bookings[bookingCount] = Booking(newId, user, screening, seats, count);
    bookingCount++;
}

Booking* CinemaBookingSystem::findBookingById(int id) {
    for (int i = 0; i &lt; bookingCount; i++) {
        if (bookings[i].getId() == id) return &amp;bookings[i];
    }
    return nullptr;
}

int CinemaBookingSystem::findBookingIndexById(int id) {
    for (int i = 0; i &lt; bookingCount; i++) {
        if (bookings[i].getId() == id) return i;
    }
    return -1;
}

void CinemaBookingSystem::cancelBookingByIndex(int index) {
    bookings[index].getScreening()-&gt;cancelSeats(bookings[index].getSeats(), bookings[index].getSeatCount());
    for (int i = index; i &lt; bookingCount - 1; i++) {
        bookings[i] = bookings[i + 1];
    }
    bookingCount--;
}

void CinemaBookingSystem::displayAllBookings() {
    if (bookingCount == 0) {
        cout &lt;&lt; "No bookings available.\n";
        return;
    }
    cout &lt;&lt; "+----+------------+----------------------+---------------+---------+-------+\n";
    cout &lt;&lt; "| ID | Username   | Movie                | Date &amp; Time  | Seats   | Count |\n";
    cout &lt;&lt; "+----+------------+----------------------+---------------+---------+-------+\n";
    for (int i = 0; i &lt; bookingCount; i++) {
        Booking& b = bookings[i];
        cout &lt;&lt; "|" &lt;&lt; setw(3) &lt;&lt; b.getId() &lt;&lt; " ";
        cout &lt;&lt; "| " &lt;&lt; setw(10) &lt;&lt; left &lt;&lt; b.getUser()-&gt;getUsername();
        cout &lt;&lt; "| " &lt;&lt; setw(20) &lt;&lt; b.getScreening()-&gt;getMovie()-&gt;getName();
        cout &lt;&lt; "| " &lt;&lt; setw(14) &lt;&lt; b.getScreening()-&gt;getDateTime();
        cout &lt;&lt; "| ";
        for (int s = 0; s &lt; b.getSeatCount(); s++) {
            cout &lt;&lt; b.getSeats()[s];
            if (s &lt; b.getSeatCount() - 1) cout &lt;&lt; ",";
        }
        cout &lt;&lt; setw(7 - b.getSeatCount()) &lt;&lt; " ";
        cout &lt;&lt; "| " &lt;&lt; setw(5) &lt;&lt; b.getSeatCount() &lt;&lt; " |\n";
    }
    cout &lt;&lt; "+----+------------+----------------------+---------------+---------+-------+\n";
}

void CinemaBookingSystem::generateMovieReport() {
    cout &lt;&lt; "--- Movie Booking Report ---\n";
    for (int i = 0; i &lt; movieCount; i++) {
        int totalBooked = 0;
        for (int j = 0; j &lt; bookingCount; j++) {
            Booking& b = bookings[j];
            if (b.getScreening()-&gt;getMovie()-&gt;getId() == movies[i].getId()) {
                totalBooked += b.getSeatCount();
            }
        }
        cout &lt;&lt; "Movie: " &lt;&lt; movies[i].getName() &lt;&lt; " - Booked Seats: " &lt;&lt; totalBooked &lt;&lt; "\n";
    }
}

void CinemaBookingSystem::generateRevenueReport() {
    cout &lt;&lt; "--- Revenue Report ---\n";
    double totalRevenue = 0.0;
    for (int i = 0; i &lt; movieCount; i++) {
        double movieRevenue = 0.0;
        for (int j = 0; j &lt; bookingCount; j++) {
            Booking& b = bookings[j];
            if (b.getScreening()-&gt;getMovie()-&gt;getId() == movies[i].getId()) {
                movieRevenue += b.getSeatCount() * movies[i].getCost();
            }
        }
        cout &lt;&lt; "Movie: " &lt;&lt; movies[i].getName() &lt;&lt; " - Revenue: $" &lt;&lt; fixed &lt;&lt; setprecision(2) &lt;&lt; movieRevenue &lt;&lt; "\n";
        totalRevenue += movieRevenue;
    }
    cout &lt;&lt; "Total Revenue: $" &lt;&lt; fixed &lt;&lt; setprecision(2) &lt;&lt; totalRevenue &lt;&lt; "\n";
}

// Implement RegularUser methods

void RegularUser::login() {
    string inputUser, inputPass;
    cout &lt;&lt; "Enter username: ";
    cin &gt;&gt; inputUser;
    cout &lt;&lt; "Enter password: ";
    cin &gt;&gt; inputPass;

    RegularUser* foundUser = system-&gt;findUserByUsername(inputUser.c_str());
    if (!foundUser) {
        cout &lt;&lt; "User not found.\n";
        return;
    }
    if (!foundUser-&gt;checkPassword(inputPass.c_str())) {
        cout &lt;&lt; "Incorrect password.\n";
        return;
    }
    *this = *foundUser;
    cout &lt;&lt; "Successfully logged in!\n";
    displayDashboard();
}

void RegularUser::signup() {
    string inputUser, inputPass;
    cout &lt;&lt; "Enter username: ";
    cin &gt;&gt; inputUser;
    if (system-&gt;findUserByUsername(inputUser.c_str())) {
        cout &lt;&lt; "Username already in use.\n";
        return;
    }
    cout &lt;&lt; "Enter password: ";
    cin &gt;&gt; inputPass;
    setUsername(inputUser.c_str());
    setPassword(inputPass.c_str());
    try {
        RegularUser* newUser = new RegularUser();
        newUser-&gt;setUsername(inputUser.c_str());
        newUser-&gt;setPassword(inputPass.c_str());
        system-&gt;addUser(newUser);
        cout &lt;&lt; "Successfully signed up!\n";
    }
    catch (InputException& e) {
        cout &lt;&lt; "Error: " &lt;&lt; e.what() &lt;&lt; "\n";
    }
}

void RegularUser::displayDashboard() {
    int choice;
    string input;
    do {
        cout &lt;&lt; "\n=== USER DASHBOARD ===\n";
        cout &lt;&lt; setw(15) &lt;&lt; left &lt;&lt; "1. Browse Movies\n";
        cout &lt;&lt; setw(15) &lt;&lt; left &lt;&lt; "2. Browse Screenings\n";
        cout &lt;&lt; setw(15) &lt;&lt; left &lt;&lt; "3. Book Ticket\n";
        cout &lt;&lt; setw(15) &lt;&lt; left &lt;&lt; "4. Modify Booking\n";
        cout &lt;&lt; setw(15) &lt;&lt; left &lt;&lt; "5. Cancel Booking\n";
        cout &lt;&lt; setw(15) &lt;&lt; left &lt;&lt; "6. View My Bookings\n";
        cout &lt;&lt; setw(15) &lt;&lt; left &lt;&lt; "7. Logout\n";
        cout &lt;&lt; "\nEnter your Choice: ";
        cin &gt;&gt; input;
        if (input.length() != 1 || !isdigit(input[0])) {
            cout &lt;&lt; "Invalid input, choose 1-7.\n";
            continue;
        }
        choice = input[0] - '0';

        switch(choice) {
            case 1:
                system-&gt;displayMovies();
                break;
            case 2:
                system-&gt;displayScreenings();
                break;
            case 3:
                bookTicket();
                break;
            case 4:
                modifyBooking();
                break;
            case 5:
                cancelBooking();
                break;
            case 6:
                viewMyBookings();
                break;
            case 7:
                cout &lt;&lt; "Logging out...\n";
                return;
            default:
                cout &lt;&lt; "Invalid choice!\n";
        }
    } while (true);
}

void RegularUser::bookTicket() {
    string input;
    cout &lt;&lt; "Enter movie ID to book: ";
    cin &gt;&gt; input;
    if (!isNumber(input)) {
        cout &lt;&lt; "Invalid movie ID.\n";
        return;
    }
    int movieId = stoi(input);
    Movie* movie = system-&gt;findMovieById(movieId);
    if (!movie) {
        cout &lt;&lt; "Movie not found.\n";
        return;
    }

    system-&gt;displayScreenings();
    cout &lt;&lt; "Enter screening ID to book: ";
    cin &gt;&gt; input;
    if (!isNumber(input)) {
        cout &lt;&lt; "Invalid screening ID.\n";
        return;
    }
    int screeningId = stoi(input);
    Screening* screening = system-&gt;findScreeningById(screeningId);
    if (!screening) {
        cout &lt;&lt; "Screening not found.\n";
        return;
    }
    cout &lt;&lt; "Enter number of tickets: ";
    cin &gt;&gt; input;
    if (!isNumber(input)) {
        cout &lt;&lt; "Invalid number of tickets.\n";
        return;
    }
    int ticketCount = stoi(input);
    if (ticketCount &lt;= 0 || ticketCount &gt; screening-&gt;getSeatCapacity()) {
        cout &lt;&lt; "Invalid number of tickets.\n";
        return;
    }
    int seats[MAX_SEATS];
    for (int i = 0; i &lt; ticketCount; i++) {
        cout &lt;&lt; "Enter seat number " &lt;&lt; i+1 &lt;&lt; ": ";
        cin &gt;&gt; input;
        if (!isNumber(input)) {
            cout &lt;&lt; "Invalid seat number.\n";
            i--;
            continue;
        }
        int seatNum = stoi(input);
        if (!screening-&gt;isSeatAvailable(seatNum)) {
            cout &lt;&lt; "Seat not available.\n";
            i--;
            continue;
        }
        seats[i] = seatNum;
    }
    try {
        system-&gt;addBooking(this, screening, seats, ticketCount);
        cout &lt;&lt; "Booking finished!\n";
    }
    catch (InputException& e) {
        cout &lt;&lt; "Error during booking: " &lt;&lt; e.what() &lt;&lt; "\n";
    }
}

void RegularUser::modifyBooking() {
    cout &lt;&lt; "Your bookings:\n";
    bool hasBookings = false;
    Booking* allBookings = system-&gt;getBookings();
    int bookingCount = system-&gt;getBookingCount();
    for (int i = 0; i &lt; bookingCount; i++) {
        if (allBookings[i].getUser() == this) {
            allBookings[i].display();
            hasBookings = true;
        }
    }
    if (!hasBookings) {
        cout &lt;&lt; "No bookings to modify.\n";
        return;
    }
    string input;
    cout &lt;&lt; "Enter booking ID to modify: ";
    cin &gt;&gt; input;
    if (!isNumber(input)) {
        cout &lt;&lt; "Invalid booking ID.\n";
        return;
    }
    int bookingId = stoi(input);
    Booking* booking = system-&gt;findBookingById(bookingId);
    if (!booking || booking->getUser() != this) {
        cout &lt;&lt; "Booking not found.\n";
        return;
    }
    system-&gt;getBookingModificationStrategy()-&gt;modifyBooking(booking);
}

void RegularUser::cancelBooking() {
    cout &lt;&lt; "Your bookings:\n";
    bool hasBookings = false;
    Booking* allBookings = system-&gt;getBookings();
    int bookingCount = system-&gt;getBookingCount();
    for (int i = 0; i &lt; bookingCount; i++) {
        if (allBookings[i].getUser() == this) {
            allBookings[i].display();
            hasBookings = true;
        }
    }
    if (!hasBookings) {
        cout &lt;&lt; "No bookings to cancel.\n";
        return;
    }
    string input;
    cout &lt;&lt; "Enter booking ID to cancel: ";
    cin &gt;&gt; input;
    if (!isNumber(input)) {
        cout &lt;&lt; "Invalid input.\n";
        return;
    }
    int bookingId = stoi(input);
    int index = system-&gt;findBookingIndexById(bookingId);
    if (index == -1 || system-&gt;getBookings()[index].getUser() != this) {
        cout &lt;&lt; "Booking not found.\n";
        return;
    }
    cout &lt;&lt; "Are you sure to cancel this booking? (Y/N): ";
    char confirm;
    cin &gt;&gt; confirm;
    confirm = toupper(confirm);
    if (confirm == 'Y') {
        system-&gt;cancelBookingByIndex(index);
        cout &lt;&lt; "Booking cancelled.\n";
    }
    else {
        cout &lt;&lt; "Cancellation aborted.\n";
    }
}

void RegularUser::viewMyBookings() {
    cout &lt;&lt; "Your bookings:\n";
    bool hasBookings = false;
    Booking* allBookings = system-&gt;getBookings();
    int bookingCount = system-&gt;getBookingCount();
    for (int i = 0; i &lt; bookingCount; i++) {
        if (allBookings[i].getUser() == this) {
            allBookings[i].display();
            hasBookings = true;
        }
    }
    if (!hasBookings) {
        cout &lt;&lt; "No bookings found.\n";
    }
}

// Admin class

class Admin : public User {
private:
    CinemaBookingSystem* system;
public:
    Admin() { system = CinemaBookingSystem::getInstance(); }
    void login() override;
    void signup() override {}
    void displayDashboard() override;
private:
    void addMovie();
    void editMovie();
    void deleteMovie();
    void addScreening();
    void editScreening();
    void deleteScreening();
};

void Admin::login() {
    string inputUser, inputPass;
    cout &lt;&lt; "Enter admin username: ";
    cin &gt;&gt; inputUser;
    cout &lt;&lt; "Enter admin password: ";
    cin &gt;&gt; inputPass;
    if (!system-&gt;validateAdmin(inputUser.c_str(), inputPass.c_str())) {
        cout &lt;&lt; "Invalid admin credentials.\n";
        return;
    }
    cout &lt;&lt; "Successfully logged in as admin.\n";
    displayDashboard();
}

void Admin::displayDashboard() {
    string input;
    int choice;
    do {
        cout &lt;&lt; "\n=== ADMIN DASHBOARD ===\n";
        cout &lt;&lt; setw(20) &lt;&lt; left &lt;&lt; "1. Add Movie\n";
        cout &lt;&lt; setw(20) &lt;&lt; left &lt;&lt; "2. Edit Movie\n";
        cout &lt;&lt; setw(20) &lt;&lt; left &lt;&lt; "3. Delete Movie\n";
        cout &lt;&lt; setw(20) &lt;&lt; left &lt;&lt; "4. Add Screening\n";
        cout &lt;&lt; setw(20) &lt;&lt; left &lt;&lt; "5. Edit Screening\n";
        cout &lt;&lt; setw(20) &lt;&lt; left &lt;&lt; "6. Delete Screening\n";
        cout &lt;&lt; setw(20) &lt;&lt; left &lt;&lt; "7. View All Bookings\n";
        cout &lt;&lt; setw(20) &lt;&lt; left &lt;&lt; "8. Generate Movie Report\n";
        cout &lt;&lt; setw(20) &lt;&lt; left &lt;&lt; "9. Generate Revenue Report\n";
        cout &lt;&lt; setw(20) &lt;&lt; left &lt;&lt; "10. Logout\n";
        cout &lt;&lt; "Enter your choice: ";
        cin &gt;&gt; input;
        if (input.length() != 1 || !isdigit(input[0])) {
            cout &lt;&lt; "Invalid input, choose 1-10.\n";
            continue;
        }
        choice = input[0] - '0';

        switch (choice) {
            case 1: addMovie(); break;
            case 2: editMovie(); break;
            case 3: deleteMovie(); break;
            case 4: addScreening(); break;
            case 5: editScreening(); break;
            case 6: deleteScreening(); break;
            case 7: system-&gt;displayAllBookings(); break;
            case 8: system-&gt;generateMovieReport(); break;
            case 9: system-&gt;generateRevenueReport(); break;
            case 10:
                cout &lt;&lt; "Logging out...\n";
                return;
            default:
                cout &lt;&lt; "Invalid choice!\n";
        }
    } while (true);
}

void Admin::addMovie() {
    cin.ignore();
    char name[50], genre[20];
    int duration;
    double cost;
    cout &lt;&lt; "Enter the name of the movie: ";
    cin.getline(name, 50);
    cout &lt;&lt; "Enter Genre: ";
    cin.getline(genre, 20);
    cout &lt;&lt; "Enter Duration (minutes): ";
    string input;
    getline(cin, input);
    while (!isNumber(input)) {
        cout &lt;&lt; "Invalid input. Enter numeric duration: ";
        getline(cin, input);
    }
    duration = stoi(input);
    cout &lt;&lt; "Enter cost: ";
    getline(cin, input);
    try {
        cost = stod(input);
    }
    catch (...) {
        cout &lt;&lt; "Invalid cost input.\n";
        return;
    }

    try {
        system-&gt;addMovie(name, genre, duration, cost);
        cout &lt;&lt; "Movie added successfully.\n";
    }
    catch (InputException& e) {
        cout &lt;&lt; "Error: " &lt;&lt; e.what() &lt;&lt; "\n";
    }
}

void Admin::editMovie() {
    system-&gt;displayMovies();
    cout &lt;&lt; "Enter movie ID to edit: ";
    string input;
    cin &gt;&gt; input;
    if (!isNumber(input)) {
        cout &lt;&lt; "Invalid movie ID.\n";
        return;
    }
    int movieId = stoi(input);
    Movie* m = system-&gt;findMovieById(movieId);
    if (!m) {
        cout &lt;&lt; "Movie not found.\n";
        return;
    }
    cin.ignore();
    char name[50], genre[20];
    int duration;
    double cost;
    cout &lt;&lt; "Enter new name of the movie: ";
    cin.getline(name, 50);
    cout &lt;&lt; "Enter new Genre: ";
    cin.getline(genre, 20);
    cout &lt;&lt; "Enter new Duration (minutes): ";
    getline(cin, input);
    while (!isNumber(input)) {
        cout &lt;&lt; "Invalid input. Enter numeric duration: ";
        getline(cin, input);
    }
    duration = stoi(input);
    cout &lt;&lt; "Enter new cost: ";
    getline(cin, input);
    try {
        cost = stod(input);
    }
    catch (...) {
        cout &lt;&lt; "Invalid cost input.\n";
        return;
    }

    try {
        system-&gt;editMovie(movieId, name, genre, duration, cost);
        cout &lt;&lt; "Movie edited successfully.\n";
    }
    catch (InputException& e) {
        cout &lt;&lt; "Error: " &lt;&lt; e.what() &lt;&lt; "\n";
    }
}

void Admin::deleteMovie() {
    system-&gt;displayMovies();
    cout &lt;&lt; "Enter movie ID to delete: ";
    string input;
    cin &gt;&gt; input;
    if (!isNumber(input)) {
        cout &lt;&lt; "Invalid input.\n";
        return;
    }
    int movieId = stoi(input);
    cout &lt;&lt; "Are you sure you want to delete this movie? (Y/N): ";
    char confirm;
    cin &gt;&gt; confirm;
    confirm = toupper(confirm);
    if (confirm != 'Y') {
        cout &lt;&lt; "Deletion cancelled.\n";
        return;
    }
    try {
        system-&gt;deleteMovie(movieId);
        cout &lt;&lt; "Movie deleted.\n";
    }
    catch (InputException& e) {
        cout &lt;&lt; "Error: " &lt;&lt; e.what() &lt;&lt; "\n";
    }
}

void Admin::addScreening() {
    system-&gt;displayMovies();
    cout &lt;&lt; "Enter movie ID to add screening for: ";
    string input;
    cin &gt;&gt; input;
    if (!isNumber(input)) {
        cout &lt;&lt; "Invalid movie ID.\n";
        return;
    }
    int movieId = stoi(input);
    cin.ignore();
    char datetime[25], hall[10];
    cout &lt;&lt; "Enter date and time (e.g. 2023-12-31_19:00): ";
    cin.getline(datetime, 25);
    cout &lt;&lt; "Enter cinema hall: ";
    cin.getline(hall, 10);

    try {
        system-&gt;addScreening(movieId, datetime, hall);
        cout &lt;&lt; "Screening added.\n";
    }
    catch (InputException& e) {
        cout &lt;&lt; "Error: " &lt;&lt; e.what() &lt;&lt; "\n";
    }
}

void Admin::editScreening() {
    system-&gt;displayScreenings();
    cout &lt;&lt; "Enter screening ID to edit: ";
    string input;
    cin &gt;&gt; input;
    if (!isNumber(input)) {
        cout &lt;&lt; "Invalid screening ID.\n";
        return;
    }
    int screeningId = stoi(input);
    Screening* s = system-&gt;findScreeningById(screeningId);
    if (!s) {
        cout &lt;&lt; "Screening not found.\n";
        return;
    }
    system-&gt;displayMovies();
    cout &lt;&lt; "Enter new movie ID: ";
    string movieInput;
    cin &gt;&gt; movieInput;
    if (!isNumber(movieInput)) {
        cout &lt;&lt; "Invalid movie ID.\n";
        return;
    }
    int movieId = stoi(movieInput);
    cin.ignore();
    char datetime[25], hall[10];
    cout &lt;&lt; "Enter new date and time (e.g. 2023-12-31_19:00): ";
    cin.getline(datetime, 25);
    cout &lt;&lt; "Enter new cinema hall: ";
    cin.getline(hall, 10);

    try {
        system-&gt;editScreening(screeningId, movieId, datetime, hall);
        cout &lt;&lt; "Screening edited.\n";
    }
    catch (InputException& e) {
        cout &lt;&lt; "Error: " &lt;&lt; e.what() &lt;&lt; "\n";
    }
}

void Admin::deleteScreening() {
    system-&gt;displayScreenings();
    cout &lt;&lt; "Enter screening ID to delete: ";
    string input;
    cin &gt;&gt; input;
    if (!isNumber(input)) {
        cout &lt;&lt; "Invalid input.\n";
        return;
    }
    int screeningId = stoi(input);
    cout &lt;&lt; "Are you sure you want to delete this screening? (Y/N): ";
    char confirm;
    cin &gt;&gt; confirm;
    confirm = toupper(confirm);
    if (confirm != 'Y') {
        cout &lt;&lt; "Deletion cancelled.\n";
        return;
    }
    try {
        system-&gt;deleteScreening(screeningId);
        cout &lt;&lt; "Screening deleted.\n";
    }
    catch (InputException& e) {
        cout &lt;&lt; "Error: " &lt;&lt; e.what() &lt;&lt; "\n";
    }
}

// Main function
int main() {
    CinemaBookingSystem* system = CinemaBookingSystem::getInstance();
    string input;
    int choice;

    while (running) {
        cout &lt;&lt; "===WELCOME TO GROUP 4 CINEMA BOOKING SYSTEM===\n";
        cout &lt;&lt; "1. Regular user Log in\n";
        cout &lt;&lt; "2. Regular user Sign up\n";
        cout &lt;&lt; "3. Admin Log in\n";
        cout &lt;&lt; "\nEnter choice: ";
        cin &gt;&gt; input;

        if (input.length() != 1 || !isdigit(input[0])) {
            cout &lt;&lt; "Invalid input\n";
            continue;
        }
        choice = input[0] - '0';

        try {
            if (choice == 1) {
                RegularUser user;
                user.login();
            }
            else if (choice == 2) {
                RegularUser user;
                user.signup();
            }
            else if (choice == 3) {
                Admin admin;
                admin.login();
            }
            else {
                cout &lt;&lt; "Invalid choice\n";
            }
        }
        catch (InputException& e) {
            cout &lt;&lt; "Error: " &lt;&lt; e.what() &lt;&lt; endl;
        }
    }

    return 0;
}

