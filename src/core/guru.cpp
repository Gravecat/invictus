// core/guru.cpp -- Guru Meditation error-handling and reporting system.
// Copyright © 2020, 2023 Raine "Gravecat" Simmons. Licensed under the GNU Affero General Public License v3 or any later version.

#include <csignal>
#include <iostream>
#include <memory>

#include "core/core.hpp"
#include "core/guru.hpp"
#include "core/version.hpp"
#include "terminal/terminal.hpp"
#include "terminal/window.hpp"
#include "tune/error-handling.hpp"
#include "util/filex.hpp"
#include "util/strx.hpp"


namespace invictus
{


// This has to be a non-class function because C.
void guru_intercept_signal(int sig) { core()->guru()->intercept_signal(sig); }

// Opens the output log for messages.
Guru::Guru(std::string log_filename) : cascade_count_(0), cascade_failure_(false), cascade_timer_(std::time(0)), cleanup_done_(false), console_ready_(false),
    dead_already_(false), stderr_buffer_(new std::stringstream()), stderr_old_(nullptr)
{
    if (!log_filename.size()) exit(EXIT_FAILURE);
    FileX::delete_file(log_filename);
    syslog_.open(log_filename.c_str());
    if (!syslog_.is_open()) exit(EXIT_FAILURE);
    hook_signals();
    stderr_old_ = std::cerr.rdbuf(stderr_buffer_->rdbuf());
    this->log("Welcome to Morior Invictus " + INVICTUS_VERSION_STRING + "!");
    this->log("Guru error-handling system is online.");
}

// Destructor, calls cleanup code.
Guru::~Guru() { cleanup(); }


// Checks stderr for any updates, puts them in the log if any exist.
void Guru::check_stderr()
{
    const std::string buffer = stderr_buffer_->str();
    if (!buffer.size()) return;
    Guru::nonfatal(buffer, GURU_ERROR);
    stderr_buffer_->str(std::string());
    stderr_buffer_->clear();
}

// Closes the system log gracefully.
void Guru::cleanup()
{
    if (cleanup_done_) return;
    cleanup_done_ = true;
    this->log("Guru Meditation system shutting down.");

    if (stderr_old_)
    {
        std::cerr.rdbuf(stderr_old_);
        stderr_old_ = nullptr;
    }
    if (stderr_buffer_)
    {
        delete stderr_buffer_;
        stderr_buffer_ = nullptr;
    }

    // Drop all signal hooks.
    signal(SIGABRT, SIG_IGN);
    signal(SIGSEGV, SIG_IGN);
    signal(SIGILL, SIG_IGN);
    signal(SIGFPE, SIG_IGN);
#ifdef INVICTUS_TARGET_LINUX
    signal(SIGBUS, SIG_IGN);
#endif

    this->log("The rest is silence.");
    syslog_.close();
}

// Tells Guru that we're ready to render Guru error messages on-screen.
void Guru::console_ready(bool is_ready) { console_ready_ = is_ready; }

// Guru meditation error.
void Guru::halt(std::string error, int a, int b)
{
    this->log("Critical error occurred, halting execution.", GURU_CRITICAL);
    this->log(error, GURU_CRITICAL);
    if (dead_already_)
    {
        log("Detected cleanup in process, attempting to die peacefully.", GURU_WARN);
        exit(EXIT_FAILURE);
    }
    else dead_already_ = true;
    std::string meditation_str = "Guru Meditation " + StrX::str_toupper(StrX::itoh(a, 8)) + "." + StrX::str_toupper(StrX::itoh(b, 8));
    this->log(meditation_str, GURU_CRITICAL);
    if (!console_ready_)
    {
        std::cout << error << std::endl;
        std::cout << meditation_str << std::endl;
        core()->cleanup();
        exit(EXIT_FAILURE);
    }

    // We should be fine to start a display loop here -- if anything goes horribly wrong, dead_already_ is set and we should go to exit(EXIT_FAILURE).
    bool needs_redraw = true;
    
    auto terminal = core()->terminal();
    auto guru_window = std::make_shared<Window>(39, 7);
    if (error.size() >= 37) error = error.substr(0, 37);

    while (true)
    {
        if (needs_redraw)
        {
            guru_window->move(terminal->get_midcol() - (guru_window->get_width() / 2), terminal->get_midrow() - (guru_window->get_height() / 2));
            terminal->box(guru_window, Colour::RED_BOLD, PRINT_FLAG_BLINK);
            terminal->print("Software Failure, Halting Execution", 2, 1, Colour::RED_BOLD, 0, guru_window);
            terminal->print(error, terminal->get_midcol(guru_window) - (error.size() / 2), 3, Colour::RED_BOLD, 0, guru_window);
            terminal->print(meditation_str, terminal->get_midcol(guru_window) - (meditation_str.size() / 2), 5, Colour::RED_BOLD, 0, guru_window);
            terminal->flip();
            needs_redraw = false;
        }
        if (terminal->get_key() == Key::RESIZE) needs_redraw = true;
    }
}

// As above, but with an exception instead of a string.
void Guru::halt(std::exception &e) { halt(e.what()); }

// Tells Guru to hook system failure signals.
void Guru::hook_signals()
{
    this->log("Guru Meditation hooking signals...");
    if (signal(SIGABRT, guru_intercept_signal) == SIG_ERR) halt("Failed to hook abort signal.");
    if (signal(SIGSEGV, guru_intercept_signal) == SIG_ERR) halt("Failed to hook segfault signal.");
    if (signal(SIGILL, guru_intercept_signal) == SIG_ERR) halt("Failed to hook illegal instruction signal.");
    if (signal(SIGFPE, guru_intercept_signal) == SIG_ERR) halt("Failed to hook floating-point exception signal.");
#ifdef INVICTUS_TARGET_LINUX
    if (signal(SIGBUS, guru_intercept_signal) == SIG_ERR) halt("Failed to hook bus error signal.");
#endif
}

// Catches a segfault or other fatal signal.
void Guru::intercept_signal(int sig)
{
    std::string sig_type;
    int a = 0, b = 0;
    switch(sig)
    {
        case SIGABRT: sig_type = "Software requested abort."; a = SIGABRT; break;
        case SIGFPE: sig_type = "Floating-point exception."; a = SIGFPE; break;
        case SIGILL: sig_type = "Illegal instruction."; a = SIGILL; break;
        case SIGSEGV: sig_type = "Segmentation fault."; a = SIGSEGV; break;
#ifdef INVICTUS_TARGET_LINUX
        case SIGBUS: sig_type = "Bus error."; a = SIGBUS; break;
#endif
        default: sig_type = "Intercepted unknown signal."; b = 0xFF; break;
    }

    // Disable the signals for now, to stop a cascade.
    signal(SIGABRT, SIG_IGN);
    signal(SIGSEGV, SIG_IGN);
    signal(SIGILL, SIG_IGN);
    signal(SIGFPE, SIG_IGN);
#ifdef INVICTUS_TARGET_LINUX
    signal(SIGBUS, SIG_IGN);
#endif
    halt(sig_type, a, b);
}

// Checks if the system has halted.
bool Guru::is_dead() const { return dead_already_; }

// Logs a message in the system log file.
void Guru::log(std::string msg, int type)
{
    if (!syslog_.is_open()) return;

    std::string txt_tag;
    switch(type)
    {
        case GURU_INFO: break;
        case GURU_WARN: txt_tag = "[WARN] "; break;
        case GURU_ERROR: txt_tag = "[ERROR] "; break;
        case GURU_CRITICAL: txt_tag = "[CRITICAL] "; break;
    }

    char* buffer = new char[32];
    const time_t now = time(nullptr);
    const tm *ptm = localtime(&now);
    std::strftime(&buffer[0], 32, "%H:%M:%S", ptm);
    std::string time_str = &buffer[0];
    msg = "[" + time_str + "] " + txt_tag + msg;
    syslog_ << msg << std::endl;
    delete[] buffer;
}

// Reports a non-fatal error, which will be logged but will not halt execution unless it cascades.
void Guru::nonfatal(std::string error, int type)
{
    if (cascade_failure_ || dead_already_) return;
    int cascade_weight = 0;
    std::string colour;
    switch(type)
    {
        case GURU_WARN: cascade_weight = ERROR_CASCADE_WEIGHT_WARNING; colour = "{Y}[WARNING] "; break;
        case GURU_ERROR: cascade_weight = ERROR_CASCADE_WEIGHT_ERROR; colour = "{R}[ERROR] "; break;
        case GURU_CRITICAL: cascade_weight = ERROR_CASCADE_WEIGHT_CRITICAL; colour = "{R}[CRITICAL] "; break;
        default: nonfatal("Nonfatal error reported with incorrect severity specified.", GURU_WARN); break;
    }

    this->log(error, type);

    if (cascade_weight)
    {
        time_t elapsed_seconds = std::time(0) - cascade_timer_;
        if (elapsed_seconds <= ERROR_CASCADE_TIMEOUT)
        {
            cascade_count_ += cascade_weight;
            if (cascade_count_ > ERROR_CASCADE_THRESHOLD)
            {
                cascade_failure_ = true;
                halt("Cascade failure detected!");
            }
        }
        else
        {
            cascade_timer_ = std::time(0);
            cascade_count_ = 0;
        }
    }
}

}   // namespace invictus
