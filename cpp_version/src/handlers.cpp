// handlers.cpp

#include "handlers.h"
#include "project.h"
#include "macro.h"
#include "string_helpers.h"
#include "utils.hpp"

#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <list>
#include <sstream>

// *****************************************************************************
// SongInformationHandler

void SongInformationHandler::handle(Project& project, const std::string& line, const std::string& tag) {
    if (tag == "TITLE") {
        project.title = get_quote(line);
    } else if (tag == "AUTHOR") {
        project.author = get_quote(line);
    } else if (tag == "COPYRIGHT") {
        project.copyright = get_quote(line);
    } else if (tag == "COMMENT") {
        std::string field = get_quote(line);
        project.comments.push_back(field);
    } else {
        std::cerr << "[WARN] Unknown Song Information tag: " << tag << std::endl;
    }
}

// *****************************************************************************
// GlobalSettingsHandler

std::string GlobalSettingsHandler::get_second_word(const std::string& line) {
    std::stringstream ss(line);
    std::string first_word;
    std::string second_word_str;
    ss >> first_word;
    ss >> second_word_str;
    return second_word_str;
}
void GlobalSettingsHandler::try_to_add_project_field(const std::string& second_word_str, int& field) {
    std::string first_word;
    // std::string second_word_str;
    int second_word_int;
    try {
        second_word_int = std::stoi(second_word_str);
        // std::cout << "First word: " << first_word << std::endl;
        // std::cout << "Second word (integer): " << second_word_int << std::endl; 
        field = second_word_int;
    } catch (const std::invalid_argument& e) { 
        std::cerr << "[E] second word is not a valid integer." << std::endl; 
        return;
    } catch (const std::out_of_range& e) { 
        std::cerr << "[E] second word is out of integer range." << std::endl; 
        return;
    }
    field = second_word_int;
}

void GlobalSettingsHandler::handle(Project& project, const std::string& line, const std::string& tag)  {
    std::string second_word_str;
    second_word_str = get_second_word(line);
    if (tag == "MACHINE") {
        try_to_add_project_field(second_word_str, project.machine);
    } else if (tag == "FRAMERATE") {
        try_to_add_project_field(second_word_str, project.framerate);
    } else if (tag == "EXPANSION") {
        try_to_add_project_field(second_word_str, project.expansion);
    } else if (tag == "VIBRATO") {
        try_to_add_project_field(second_word_str, project.vibrato);
    } else if (tag == "SPLIT") {
        try_to_add_project_field(second_word_str, project.split);
    } else if (tag == "N163CHANNELS") {
        try_to_add_project_field(second_word_str, project.n163channels);
    } else {
        std::cerr << "[WARN] Unknown Global Setting tag: " << tag << std::endl;
    }
}

// *****************************************************************************
// Macro Handler

void MacroHandler::handle(Project& project, const std::string& line, const std::string& tag) {
    // std::cout << "Handling tag: " << tag << std::endl;
    
    // TODO validate this input with regex
    std::stringstream ss(line);
    std::string first_word;
    ss >> first_word;
    int macro_type, macro_index, macro_loop, macro_release, macro_setting;
    ss >> macro_type >> macro_index >> macro_loop >> macro_release >> macro_setting;
    std::vector<int> sequence = get_number_field(line);
    
    std::cout << "LINE: " << line << std::endl;
    std::cout << "VEC TO STRING: " << vector_to_string(sequence) << "\n";

    // create macro and assign values to it
    Macro m;
    m.macro_type = macro_type;
    m.macro_index = macro_index;
    m.macro_loop = macro_loop;
    m.macro_release = macro_release;
    m.macro_setting = macro_setting;
    m.sequence = sequence;

    std::string macro_key = generate_macro_key(tag, macro_type, macro_index);
    // std::cout << "[D] macro_key_created = " << macro_key << std::endl;

    m.label = macro_key;

    project.macros[macro_key] = std::move(m);
}

// *****************************************************************************
// Instrument Handler

void InstrumentHandler::handle(Project& project, const std::string& line, const std::string& tag) {
    if (tag == "INST2A03" || tag == "INSTVRC6" || tag == "INSTS5B") {
        handle_base_inst(project, line, tag);
    } else if (tag == "INSTN163") {
        handle_inst_n163(project, line, tag);
    } else if (tag == "INSTFDS") {
        handle_inst_n163(project, line, tag);
    } else if (tag == "INSTVRC7") {
        handle_inst_vrc7(project, line, tag);
    } else {
        std::cerr << "[WARN] Unknown instrument tag: " << tag << std::endl;
        return;
    }
}

void InstrumentHandler::handle_base_inst(Project& project, const std::string& line, const std::string& tag){
    // init vars
    std::string first_word;
    int index, seq_vol, seq_arp, seq_pit, seq_hpi, seq_dut;
    std::string name;
    
    // parse line
    std::stringstream ss(line);
    ss >> first_word;
    ss >> index >> seq_vol >> seq_arp >> seq_pit >> seq_hpi >> seq_dut;
    name = get_quote(line);
    
    // instantiate a new instrument object
    BaseInst inst;
    
    // assign values to instrument
    inst.index = index; 
    inst.seq_vol = seq_vol; 
    inst.seq_arp = seq_arp; 
    inst.seq_pit = seq_pit; 
    inst.seq_hpi = seq_hpi; 
    inst.seq_dut = seq_dut; 
    
    // for instrument family lookup
    static std::unordered_map<std::string, std::string> inst_to_macro_family = {
        // {"INST2A03", "MACRO2A03"},
        {"INST2A03", "MACRO"},
        {"INSTVRC6", "MACROVRC6"},
        {"INSTS5B", "MACROS5B"}
    };
    
    // macro type constants
    static int VOL = 0;
    static int ARP = 1; 
    static int PIT = 2; 
    static int HPI = 3; 
    static int DUT = 4;
    
    // for storing the correct fields during iteration
    struct InstFields {
        int macro_type;
        int macro_index;
        std::optional<Macro>* inst_macro_ptr;
    };

    std::vector<InstFields> inst_fields = {
        {VOL, seq_vol, &inst.macro_vol},
        {ARP, seq_arp, &inst.macro_arp}, 
        {PIT, seq_pit, &inst.macro_pit}, 
        {HPI, seq_hpi, &inst.macro_hpi}, 
        {DUT, seq_dut, &inst.macro_dut}
    };
    
    // assign macros to instrument from project.macros if they exist
    for (const auto& field : inst_fields) {
        if (field.macro_index < 0) { 
            continue; 
        }
        if (field.macro_type < 0 || field.macro_type > 5) { 
            continue; 
        }

        std::string macro_family = inst_to_macro_family[tag];
        std::string macro_key = generate_macro_key(macro_family, field.macro_type, field.macro_index);
        
        // std::cout << "[D] macro lookup : " << macro_key << std::endl;

        auto it = project.macros.find(macro_key);

        if (it != project.macros.end() ) {
            // assign macro to instrument Macro* pointer
            *field.inst_macro_ptr = (it->second);
            std::cout << "[D] FOUND instrument macro: " << macro_key << std::endl;
        } else {
            std::cerr << "[WARN] NOT FOUND not find instrument macro: " << macro_key << std::endl;
        }
    }
}

// TODO
void InstrumentHandler::handle_inst_vrc7(
    [[maybe_unused]] Project& project,
    [[maybe_unused]] const std::string& line,
    [[maybe_unused]] const std::string& tag
) {
    std::cout << "[TODO] Handle VRC7: Under construction! " << std::endl;
}

// TODO
void InstrumentHandler::handle_inst_n163(
    [[maybe_unused]] Project& project,
    [[maybe_unused]] const std::string& line,
    [[maybe_unused]] const std::string& tag
) {
    std::cout << "[TODO] Handle N163: Under construction! " << std::endl;
}

// TODO
void InstrumentHandler::handle_inst_fds (
    [[maybe_unused]] Project& project,
    [[maybe_unused]] const std::string& line,
    [[maybe_unused]] const std::string& tag
) {
    std::cout << "[TODO] Handle FDS: Under construction! " << std::endl;
}

// *****************************************************************************
// Track Handler

void TrackHandler::handle(Project& project, const std::string& line, const std::string& tag) {
    std::cout << "Handling tag: " << tag << std::endl;
    
    if (tag == "TRACK") {
        handle_track(project, line, tag);
    } else if (tag == "COLUMNS") {
        handle_columns(project, line, tag);
    } else if (tag == "ORDER") {
        handle_order(project, line, tag);
    } else if (tag == "PATTERN") {
        handle_pattern(project, line, tag);
    } else if (tag == "ROW") {
        handle_row(project, line, tag);
    } else {
        std::cerr << "[WARN] Unknown Track Handler tag: " << tag << std::endl;
    }
}

// TODO
void TrackHandler::handle_track(
    [[maybe_unused]] Project& project, 
    [[maybe_unused]] const std::string& line, 
    [[maybe_unused]] const std::string& tag
) {
    std::cout << "[TODO] TrackHandler method: " << tag << std::endl;
}

// TODO
void TrackHandler::handle_columns(
    [[maybe_unused]] Project& project, 
    [[maybe_unused]] const std::string& line, 
    [[maybe_unused]] const std::string& tag
) {
    std::cout << "[TODO] TrackHandler method: " << tag << std::endl;
}

// TODO
void TrackHandler::handle_order(
    [[maybe_unused]] Project& project, 
    [[maybe_unused]] const std::string& line, 
    [[maybe_unused]] const std::string& tag
) {
    std::cout << "[TODO] TrackHandler method: " << tag << std::endl;
}

// TODO
void TrackHandler::handle_pattern(
    [[maybe_unused]] Project& project, 
    [[maybe_unused]] const std::string& line, 
    [[maybe_unused]] const std::string& tag
) {
    std::cout << "[TODO] TrackHandler method: " << tag << std::endl;
}

// TODO
void TrackHandler::handle_row(
    [[maybe_unused]] Project& project, 
    [[maybe_unused]] const std::string& line, 
    [[maybe_unused]] const std::string& tag
) {
    std::cout << "[TODO] TrackHandler method: " << tag << std::endl;
}

