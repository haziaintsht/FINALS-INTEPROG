#include <iostream>
#include <cstring>
#include <cctype>
#include <cstdlib>

// Exception Handling Class
class CinemaException {
private:
    char message[100];
public:
    CinemaException(const char* msg) {
        strncpy(message, msg, sizeof(message));
        message[sizeof(message)-1] = '\0';
    }
    const char* what() const { return message; }
};

// Forward declarations
class Movie;
class Screening;
class Booking;

// Singleton Pattern for the Cinema System
class CinemaSystem {
private:
    static CinemaSystem* instance;
    Movie* movies[50];
    Screening* screenings[100];
    Booking* bookings[500];
    int movieCount;
    int screeningCount;
    int bookingCount;
    
    CinemaSystem() : movieCount(0), screeningCount(0), bookingCount(0) {
        for (int i = 0; i < 50; i++) movies[i] = nullptr;
        for (int i = 0; i < 100; i++) screenings[i] = nullptr;
        for (int i = 0; i < 500; i++) bookings[i] = nullptr;
    }
    
    CinemaSystem(const CinemaSystem&) = delete;
    CinemaSystem& operator=(const CinemaSystem&) = delete;
    
public:
    static CinemaSystem* getInstance() {
        if (!instance) {
            instance = new CinemaSystem();
        }
        return instance;
    }
    
    void start() {
        std::cout << "Cinema Booking System Started\n";
    }
    
    // Movie management
    void addMovie(const char* title, const char* director, int duration);
    void editMovie(int id, const char* title, const char* director, int duration);
    void deleteMovie(int id);
    Movie* getMovie(int id);
    void listMovies();
    
    // Screening management
    void addScreening(int movieId, const char* time, int hall, int seats);
    void editScreening(int id, const char* time, int hall, int seats);
    void deleteScreening(int id);
    Screening* getScreening(int id);
    void listScreenings();
    
    // Booking management
    void addBooking(int screeningId, const char* username, int seat);
    void editBooking(int id, int newSeat);
    void cancelBooking(int id);
    Booking* getBooking(int id);
    void listBookings();
    void listUserBookings(const char* username);
    
    // Reports
    void generateMovieReport();
    void generateRevenueReport();
};

CinemaSystem* CinemaSystem::instance = nullptr;

// Movie class
class Movie {
private:
    int id;
    char title[100];
    char director[100];
    int duration; // in minutes
public:
    Movie(int id, const char* title, const char* director, int duration) 
        : id(id), duration(duration) {
        strncpy(this->title, title, sizeof(this->title)-1);
        strncpy(this->director, director, sizeof(this->director)-1);
        this->title[sizeof(this->title)-1] = '\0';
        this->director[sizeof(this->director)-1] = '\0';
    }
    
    int getId() const { return id; }
    const char* getTitle() const { return title; }
    const char* getDirector() const { return director; }
    int getDuration() const { return duration; }
    
    void setTitle(const char* newTitle) {
        strncpy(title, newTitle, sizeof(title)-1);
        title[sizeof(title)-1] = '\0';
    }
    
    void setDirector(const char* newDirector) {
        strncpy(director, newDirector, sizeof(director)-1);
        director[sizeof(director)-1] = '\0';
    }
    
    void setDuration(int newDuration) {
        duration = newDuration;
    }
    
    void display() const {
        std::cout << "ID: " << id << " | Title: " << title 
                  << " | Director: " << director 
                  << " | Duration: " << duration << " mins\n";
    }
};

// Screening class
class Screening {
private:
    int id;
    Movie* movie;
    char time[20];
    int hall;
    int totalSeats;
    bool seats[100]; // true = booked, false = available
public:
    Screening(int id, Movie* movie, const char* time, int hall, int totalSeats) 
        : id(id), movie(movie), hall(hall), totalSeats(totalSeats) {
        strncpy(this->time, time, sizeof(this->time)-1);
        this->time[sizeof(this->time)-1] = '\0';
        
        for (int i = 0; i < 100; i++) seats[i] = false;
    }
    
    int getId() const { return id; }
    Movie* getMovie() const { return movie; }
    const char* getTime() const { return time; }
    int getHall() const { return hall; }
    int getTotalSeats() const { return totalSeats; }
    
    void setTime(const char* newTime) {
        strncpy(time, newTime, sizeof(time)-1);
        time[sizeof(time)-1] = '\0';
    }
    
    void setHall(int newHall) { hall = newHall; }
    void setTotalSeats(int newSeats) { totalSeats = newSeats; }
    
    bool isSeatAvailable(int seat) const {
        if (seat < 1 || seat > totalSeats) return false;
        return !seats[seat-1];
    }
    
    bool bookSeat(int seat) {
        if (!isSeatAvailable(seat)) return false;
        seats[seat-1] = true;
        return true;
    }
    
    bool cancelSeat(int seat) {
        if (seat < 1 || seat > totalSeats) return false;
        if (!seats[seat-1]) return false;
        seats[seat-1] = false;
        return true;
    }
    
    void display() const {
        std::cout << "ID: " << id << " | Movie: " << movie->getTitle()
                  << " | Time: " << time << " | Hall: " << hall
                  << " | Available seats: " << getAvailableSeats() << "/" << totalSeats << "\n";
    }
    
    int getAvailableSeats() const {
        int count = 0;
        for (int i = 0; i < totalSeats; i++) {
            if (!seats[i]) count++;
        }
        return count;
    }
};

// Booking class
class Booking {
private:
    int id;
    Screening* screening;
    char username[20];
    int seat;
public:
    Booking(int id, Screening* screening, const char* username, int seat) 
        : id(id), screening(screening), seat(seat) {
        strncpy(this->username, username, sizeof(this->username)-1);
        this->username[sizeof(this->username)-1] = '\0';
    }
    
    int getId() const { return id; }
    Screening* getScreening() const { return screening; }
    const char* getUsername() const { return username; }
    int getSeat() const { return seat; }
    
    void setSeat(int newSeat) { seat = newSeat; }
    
    void display() const {
        std::cout << "ID: " << id << " | User: " << username 
                  << " | Movie: " << screening->getMovie()->getTitle()
                  << " | Time: " << screening->getTime() 
                  << " | Hall: " << screening->getHall()
                  << " | Seat: " << seat << "\n";
    }
};

// Implement CinemaSystem methods
void CinemaSystem::addMovie(const char* title, const char* director, int duration) {
    if (movieCount >= 50) throw CinemaException("Maximum movies reached");
    movies[movieCount] = new Movie(movieCount+1, title, director, duration);
    movieCount++;
}

void CinemaSystem::editMovie(int id, const char* title, const char* director, int duration) {
    Movie* movie = getMovie(id);
    if (!movie) throw CinemaException("Movie not found");
    
    movie->setTitle(title);
    movie->setDirector(director);
    movie->setDuration(duration);
}

void CinemaSystem::deleteMovie(int id) {
    int index = -1;
    for (int i = 0; i < movieCount; i++) {
        if (movies[i]->getId() == id) {
            index = i;
            break;
        }
    }
    
    if (index == -1) throw CinemaException("Movie not found");
    
    delete movies[index];
    for (int i = index; i < movieCount-1; i++) {
        movies[i] = movies[i+1];
    }
    movieCount--;
}

Movie* CinemaSystem::getMovie(int id) {
    for (int i = 0; i < movieCount; i++) {
        if (movies[i]->getId() == id) {
            return movies[i];
        }
    }
    return nullptr;
}

void CinemaSystem::listMovies() {
    std::cout << "\n=== MOVIES ===\n";
    for (int i = 0; i < movieCount; i++) {
        movies[i]->display();
    }
}

void CinemaSystem::addScreening(int movieId, const char* time, int hall, int seats) {
    if (screeningCount >= 100) throw CinemaException("Maximum screenings reached");
    Movie* movie = getMovie(movieId);
    if (!movie) throw CinemaException("Movie not found");
    
    screenings[screeningCount] = new Screening(screeningCount+1, movie, time, hall, seats);
    screeningCount++;
}

void CinemaSystem::editScreening(int id, const char* time, int hall, int seats) {
    Screening* screening = getScreening(id);
    if (!screening) throw CinemaException("Screening not found");
    
    screening->setTime(time);
    screening->setHall(hall);
    screening->setTotalSeats(seats);
}

void CinemaSystem::deleteScreening(int id) {
    int index = -1;
    for (int i = 0; i < screeningCount; i++) {
        if (screenings[i]->getId() == id) {
            index = i;
            break;
        }
    }
    
    if (index == -1) throw CinemaException("Screening not found");
    
    delete screenings[index];
    for (int i = index; i < screeningCount-1; i++) {
        screenings[i] = screenings[i+1];
    }
    screeningCount--;
}

Screening* CinemaSystem::getScreening(int id) {
    for (int i = 0; i < screeningCount; i++) {
        if (screenings[i]->getId() == id) {
            return screenings[i];
        }
    }
    return nullptr;
}

void CinemaSystem::listScreenings() {
    std::cout << "\n=== SCREENINGS ===\n";
    for (int i = 0; i < screeningCount; i++) {
        screenings[i]->display();
    }
}

void CinemaSystem::addBooking(int screeningId, const char* username, int seat) {
    if (bookingCount >= 500) throw CinemaException("Maximum bookings reached");
    Screening* screening = getScreening(screeningId);
    if (!screening) throw CinemaException("Screening not found");
    
    if (!screening->bookSeat(seat)) throw CinemaException("Seat not available");
    
    bookings[bookingCount] = new Booking(bookingCount+1, screening, username, seat);
    bookingCount++;
}

void CinemaSystem::editBooking(int id, int newSeat) {
    Booking* booking = getBooking(id);
    if (!booking) throw CinemaException("Booking not found");
    
    Screening* screening = booking->getScreening();
    if (!screening->cancelSeat(booking->getSeat())) {
        throw CinemaException("Failed to cancel old seat");
    }
    
    if (!screening->bookSeat(newSeat)) {
        // Rebook the old seat if new seat isn't available
        screening->bookSeat(booking->getSeat());
        throw CinemaException("New seat not available");
    }
    
    booking->setSeat(newSeat);
}

void CinemaSystem::cancelBooking(int id) {
    int index = -1;
    for (int i = 0; i < bookingCount; i++) {
        if (bookings[i]->getId() == id) {
            index = i;
            break;
        }
    }
    
    if (index == -1) throw CinemaException("Booking not found");
    
    Booking* booking = bookings[index];
    if (!booking->getScreening()->cancelSeat(booking->getSeat())) {
        throw CinemaException("Failed to cancel seat");
    }
    
    delete bookings[index];
    for (int i = index; i < bookingCount-1; i++) {
        bookings[i] = bookings[i+1];
    }
    bookingCount--;
}

Booking* CinemaSystem::getBooking(int id) {
    for (int i = 0; i < bookingCount; i++) {
        if (bookings[i]->getId() == id) {
            return bookings[i];
        }
    }
    return nullptr;
}

void CinemaSystem::listBookings() {
    std::cout << "\n=== ALL BOOKINGS ===\n";
    for (int i = 0; i < bookingCount; i++) {
        bookings[i]->display();
    }
}

void CinemaSystem::listUserBookings(const char* username) {
    std::cout << "\n=== YOUR BOOKINGS ===\n";
    bool found = false;
    for (int i = 0; i < bookingCount; i++) {
        if (strcmp(bookings[i]->getUsername(), username) == 0) {
            bookings[i]->display();
            found = true;
        }
    }
    if (!found) std::cout << "No bookings found.\n";
}

void CinemaSystem::generateMovieReport() {
    std::cout << "\n=== MOVIE REPORT ===\n";
    listMovies();
    std::cout << "Total movies: " << movieCount << "\n";
}

void CinemaSystem::generateRevenueReport() {
    std::cout << "\n=== REVENUE REPORT ===\n";
    std::cout << "Total screenings: " << screeningCount << "\n";
    std::cout << "Total bookings: " << bookingCount << "\n";
    std::cout << "Estimated revenue: $" << bookingCount * 10 << "\n";
}

// Strategy Pattern for Authentication
class AuthStrategy {
public:
    virtual ~AuthStrategy() {}
    virtual bool authenticate(const char* username, const char* password) = 0;
};

class AdminAuthStrategy : public AuthStrategy {
public:
    bool authenticate(const char* username, const char* password) override {
        return strcmp(username, "admin") == 0 && strcmp(password, "admin123") == 0;
    }
};

class UserAuthStrategy : public AuthStrategy {
private:
    struct User {
        char username[20];
        char password[20];
    };
    User users[10];
    int userCount;
    
public:
    UserAuthStrategy() : userCount(0) {
        addUser("user1", "pass1");
        addUser("user2", "pass2");
    }
    
    void addUser(const char* username, const char* password) {
        if (userCount < 10) {
            strncpy(users[userCount].username, username, sizeof(users[userCount].username)-1);
            strncpy(users[userCount].password, password, sizeof(users[userCount].password)-1);
            userCount++;
        }
    }
    
    bool authenticate(const char* username, const char* password) override {
        for (int i = 0; i < userCount; i++) {
            if (strcmp(users[i].username, username) == 0 && 
                strcmp(users[i].password, password) == 0) {
                return true;
            }
        }
        return false;
    }
};

// Base User Class
class User {
protected:
    char username[20];
    AuthStrategy* authStrategy;
    CinemaSystem* cinema;
    
public:
    User(const char* uname, AuthStrategy* strategy, CinemaSystem* cinema) 
        : authStrategy(strategy), cinema(cinema) {
        strncpy(username, uname, sizeof(username)-1);
        username[sizeof(username)-1] = '\0';
    }
    
    virtual ~User() {
        delete authStrategy;
    }
    
    virtual void displayDashboard() = 0;
    virtual void showMenu() = 0;
    
    bool login(const char* password) {
        return authStrategy->authenticate(username, password);
    }
    
    const char* getUsername() const { return username; }
};

// Admin Class
class Admin : public User {
public:
    Admin(const char* uname, CinemaSystem* cinema) 
        : User(uname, new AdminAuthStrategy(), cinema) {}
    
    void displayDashboard() override {
        std::cout << "\n=== ADMIN DASHBOARD ===\n";
        std::cout << "1. Add Movie\n";
        std::cout << "2. Edit Movie\n";
        std::cout << "3. Delete Movie\n";
        std::cout << "4. Add Screening\n";
        std::cout << "5. Edit Screening\n";
        std::cout << "6. Delete Screening\n";
        std::cout << "7. View All Bookings\n";
        std::cout << "8. Generate Movie Report\n";
        std::cout << "9. Generate Revenue Report\n";
        std::cout << "10. Logout\n";
    }
    
    void showMenu() override {
        int choice;
        do {
            displayDashboard();
            std::cout << "Enter your choice: ";
            std::cin >> choice;
            
            try {
                switch(choice) {
                    case 1: addMovie(); break;
                    case 2: editMovie(); break;
                    case 3: deleteMovie(); break;
                    case 4: addScreening(); break;
                    case 5: editScreening(); break;
                    case 6: deleteScreening(); break;
                    case 7: cinema->listBookings(); break;
                    case 8: cinema->generateMovieReport(); break;
                    case 9: cinema->generateRevenueReport(); break;
                    case 10: std::cout << "Logging out...\n"; break;
                    default: std::cout << "Invalid choice!\n";
                }
            } catch (const CinemaException& e) {
                std::cerr << "Error: " << e.what() << "\n";
            }
        } while (choice != 10);
    }
    
private:
    void addMovie() {
        char title[100], director[100];
        int duration;
        
        std::cout << "\n--- Add Movie ---\n";
        std::cout << "Enter movie title: ";
        std::cin.ignore();
        std::cin.getline(title, sizeof(title));
        
        std::cout << "Enter director: ";
        std::cin.getline(director, sizeof(director));
        
        std::cout << "Enter duration (minutes): ";
        std::cin >> duration;
        
        cinema->addMovie(title, director, duration);
        std::cout << "Movie added successfully!\n";
    }
    
    void editMovie() {
        int id;
        char title[100], director[100];
        int duration;
        
        cinema->listMovies();
        std::cout << "\n--- Edit Movie ---\n";
        std::cout << "Enter movie ID to edit: ";
        std::cin >> id;
        
        std::cout << "Enter new title: ";
        std::cin.ignore();
        std::cin.getline(title, sizeof(title));
        
        std::cout << "Enter new director: ";
        std::cin.getline(director, sizeof(director));
        
        std::cout << "Enter new duration (minutes): ";
        std::cin >> duration;
        
        cinema->editMovie(id, title, director, duration);
        std::cout << "Movie edited successfully!\n";
    }
    
    void deleteMovie() {
        int id;
        
        cinema->listMovies();
        std::cout << "\n--- Delete Movie ---\n";
        std::cout << "Enter movie ID to delete: ";
        std::cin >> id;
        
        cinema->deleteMovie(id);
        std::cout << "Movie deleted successfully!\n";
    }
    
    void addScreening() {
        int movieId, hall, seats;
        char time[20];
        
        cinema->listMovies();
        std::cout << "\n--- Add Screening ---\n";
        std::cout << "Enter movie ID: ";
        std::cin >> movieId;
        
        std::cout << "Enter time (HH:MM): ";
        std::cin >> time;
        
        std::cout << "Enter hall number: ";
        std::cin >> hall;
        
        std::cout << "Enter total seats: ";
        std::cin >> seats;
        
        cinema->addScreening(movieId, time, hall, seats);
        std::cout << "Screening added successfully!\n";
    }
    
    void editScreening() {
        int id, hall, seats;
        char time[20];
        
        cinema->listScreenings();
        std::cout << "\n--- Edit Screening ---\n";
        std::cout << "Enter screening ID to edit: ";
        std::cin >> id;
        
        std::cout << "Enter new time (HH:MM): ";
        std::cin >> time;
        
        std::cout << "Enter new hall number: ";
        std::cin >> hall;
        
        std::cout << "Enter new total seats: ";
        std::cin >> seats;
        
        cinema->editScreening(id, time, hall, seats);
        std::cout << "Screening edited successfully!\n";
    }
    
    void deleteScreening() {
        int id;
        
        cinema->listScreenings();
        std::cout << "\n--- Delete Screening ---\n";
        std::cout << "Enter screening ID to delete: ";
        std::cin >> id;
        
        cinema->deleteScreening(id);
        std::cout << "Screening deleted successfully!\n";
    }
};

// Regular User Class
class RegularUser : public User {
public:
    RegularUser(const char* uname, CinemaSystem* cinema) 
        : User(uname, new UserAuthStrategy(), cinema) {}
    
    void displayDashboard() override {
        std::cout << "\n=== USER DASHBOARD ===\n";
        std::cout << "1. Browse Movies\n";
        std::cout << "2. Browse Screenings\n";
        std::cout << "3. Book Ticket\n";
        std::cout << "4. Modify Booking\n";
        std::cout << "5. Cancel Booking\n";
        std::cout << "6. View My Bookings\n";
        std::cout << "7. Logout\n";
    }
    
    void showMenu() override {
        int choice;
        do {
            displayDashboard();
            std::cout << "Enter your choice: ";
            std::cin >> choice;
            
            try {
                switch(choice) {
                    case 1: cinema->listMovies(); break;
                    case 2: cinema->listScreenings(); break;
                    case 3: bookTicket(); break;
                    case 4: modifyBooking(); break;
                    case 5: cancelBooking(); break;
                    case 6: cinema->listUserBookings(username); break;
                    case 7: std::cout << "Logging out...\n"; break;
                    default: std::cout << "Invalid choice!\n";
                }
            } catch (const CinemaException& e) {
                std::cerr << "Error: " << e.what() << "\n";
            }
        } while (choice != 7);
    }
    
private:
    void bookTicket() {
        int screeningId, seat;
        
        cinema->listScreenings();
        std::cout << "\n--- Book Ticket ---\n";
        std::cout << "Enter screening ID: ";
        std::cin >> screeningId;
        
        Screening* screening = cinema->getScreening(screeningId);
        if (!screening) throw CinemaException("Screening not found");
        
        std::cout << "Available seats: " << screening->getAvailableSeats() << "\n";
        std::cout << "Enter seat number: ";
        std::cin >> seat;
        
        cinema->addBooking(screeningId, username, seat);
        std::cout << "Ticket booked successfully!\n";
    }
    
    void modifyBooking() {
        int bookingId, newSeat;
        
        cinema->listUserBookings(username);
        std::cout << "\n--- Modify Booking ---\n";
        std::cout << "Enter booking ID to modify: ";
        std::cin >> bookingId;
        
        Booking* booking = cinema->getBooking(bookingId);
        if (!booking || strcmp(booking->getUsername(), username) != 0) {
            throw CinemaException("Booking not found or not yours");
        }
        
        std::cout << "Current seat: " << booking->getSeat() << "\n";
        std::cout << "Enter new seat number: ";
        std::cin >> newSeat;
        
        cinema->editBooking(bookingId, newSeat);
        std::cout << "Booking modified successfully!\n";
    }
    
    void cancelBooking() {
        int bookingId;
        
        cinema->listUserBookings(username);
        std::cout << "\n--- Cancel Booking ---\n";
        std::cout << "Enter booking ID to cancel: ";
        std::cin >> bookingId;
        
        Booking* booking = cinema->getBooking(bookingId);
        if (!booking || strcmp(booking->getUsername(), username) != 0) {
            throw CinemaException("Booking not found or not yours");
        }
        
        cinema->cancelBooking(bookingId);
        std::cout << "Booking cancelled successfully!\n";
    }
};

// User Factory
class UserFactory {
public:
    static User* createUser(const char* username, const char* password, CinemaSystem* cinema) {
        AdminAuthStrategy adminAuth;
        if (adminAuth.authenticate(username, password)) {
            return new Admin(username, cinema);
        }
        
        UserAuthStrategy userAuth;
        if (userAuth.authenticate(username, password)) {
            return new RegularUser(username, cinema);
        }
        
        throw CinemaException("Invalid username or password");
    }
};

// Helper functions
bool isAllDigits(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) {
            return false;
        }
    }
    return true;
}

void toUpperCase(char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = toupper(str[i]);
    }
}

// Main function
int main() {
    CinemaSystem* cinema = CinemaSystem::getInstance();
    cinema->start();
    
    // Add some sample data
    try {
        cinema->addMovie("The Shawshank Redemption", "Frank Darabont", 142);
        cinema->addMovie("The Godfather", "Francis Ford Coppola", 175);
        cinema->addMovie("The Dark Knight", "Christopher Nolan", 152);
        
        cinema->addScreening(1, "14:00", 1, 50);
        cinema->addScreening(1, "18:00", 1, 50);
        cinema->addScreening(2, "15:30", 2, 40);
        cinema->addScreening(3, "20:00", 3, 60);
    } catch (const CinemaException& e) {
        std::cerr << "Initialization error: " << e.what() << "\n";
    }
    
    std::cout << "\n=== Cinema Booking System ===\n";
    std::cout << "User Authentication (Login System)\n";
    std::cout << "Users must log in to access the system.\n";
    std::cout << "Different roles: Admin and Regular User.\n\n";
    
    char username[20];
    char password[20];
    
    std::cout << "Enter username: ";
    std::cin >> username;
    
    std::cout << "Enter password: ";
    std::cin >> password;
    
    try {
        User* user = UserFactory::createUser(username, password, cinema);
        std::cout << "\nLogin successful! Welcome, " << user->getUsername() << "!\n";
        user->showMenu();
        delete user;
    } catch (const CinemaException& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
    
    return 0;
}