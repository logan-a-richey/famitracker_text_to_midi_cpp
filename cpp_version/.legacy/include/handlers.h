// handlers.h

#pragma once 

#include <string>

class Project;

// *****************************************************************************
// interface

class IHandler {
public:
    virtual ~IHandler() = default;
    virtual void handle(Project& project, const std::string& line, const std::string& tag) = 0;
};

// *****************************************************************************
// concrete handlers

class SongInformationHandler : public IHandler {
public:
    void handle(Project& project, const std::string& line, const std::string& tag) override;
};

// =====
class GlobalSettingsHandler : public IHandler {
public:
    void handle(Project& project, const std::string& line, const std::string& tag) override;
private:
    std::string get_second_word(const std::string& line); 
    void try_to_add_project_field(const std::string& second_word_str, int& field);
};

// =====
class MacroHandler : public IHandler {
public:
    void handle(Project& project, const std::string& line, const std::string& tag) override;
};

// =====
class InstrumentHandler : public IHandler {
public:
    void handle(Project& project, const std::string& line, const std::string& tag) override;
private:
    void handle_base_inst(Project& project, const std::string& line, const std::string& tag);
    void handle_inst_fds(Project& project, const std::string& line, const std::string& tag);
    void handle_inst_vrc7(Project& project, const std::string& line, const std::string& tag);
    void handle_inst_n163(Project& project, const std::string& line, const std::string& tag);
};

// =====
class TrackHandler : public IHandler {
public:
    void handle(Project& project, const std::string& line, const std::string& tag) override;
private:
    void handle_track(Project& project, const std::string& line, const std::string& tag);
    void handle_columns(Project& project, const std::string& line, const std::string& tag);
    void handle_order(Project& project, const std::string& line, const std::string& tag);
    void handle_pattern(Project& project, const std::string& line, const std::string& tag);
    void handle_row(Project& project, const std::string& line, const std::string& tag);
};
