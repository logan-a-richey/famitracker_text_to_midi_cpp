// project_reader.cpp

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_set>
#include <stdexcept>

#include "project_reader.h"

#include "project.h"
#include "macro.h"
#include "dpcm_sample.h"
#include "groove.h"
#include "instrument.h"
#include "key_dpcm.h"
#include "track.h"

#include "constants.h"
#include "string_helpers.hpp"
#include "key_gen.hpp"
// #include "container_printing.hpp"

// Ctor and Init Dtable
ProjectReader::ProjectReader() {
    static std::unordered_set<std::string> song_information_tags = { "TITLE", "AUTHOR", "COPYRIGHT", "COMMENT" };
    static std::unordered_set<std::string> global_settings_tags = { "MACHINE", "FRAMERATE", "EXPANSION", "VIBRATO", "SPLIT", "N163CHANNELS" };
    static std::unordered_set<std::string> macro_tags = { "MACRO", "MACROVRC6", "MACRON163", "MACROS5B" };
    static std::unordered_set<std::string> basic_inst_tags = { "INST2A03", "INSTVRC6", "INSTN163", "INSTS5B" };
    
    // init dispatch table
    dtable.clear();
    
    // metadata handlers
    for (const auto& tag : song_information_tags) {
        dtable[tag] = [this](Project& project, const std::string& line) { handle_song_information(project, line); };
    }
    for (const auto& tag : global_settings_tags) {
        dtable[tag] = [this](Project& project, const std::string& line) { handle_global_settings(project, line); };
    }

    // macro handlers
    for (const auto& tag : macro_tags) {
        dtable[tag] = [this](Project& project, const std::string& line) { handle_macro(project, line); };
    }
    
    // dpcm handlers
    dtable["DPCMDEF"] = [this](Project& project, const std::string& line) { handle_dpcm_def(project, line); };
    dtable["DPCM"] = [this](Project& project, const std::string& line) { handle_dpcm_data(project, line); };

    // groove handlers
    dtable["GROOVE"] = [this](Project& project, const std::string& line) { handle_groove(project, line); };
    dtable["USEGROOVE"] = [this](Project& project, const std::string& line) { handle_use_groove(project, line); };

    // instrument handlers
    for (const auto& tag : basic_inst_tags) {
        dtable[tag] = [this](Project& project, const std::string& line) { handle_inst_basic(project, line); };
    }
    dtable["INSTVRC7"] = [this](Project& project, const std::string& line) { handle_inst_vrc7(project, line); };
    dtable["INSTFDS"] = [this](Project& project, const std::string& line) { handle_inst_fds(project, line); };
    
    // special handlers 
    dtable["KEYDPCM"] =  [this](Project& project, const std::string& line) { handle_key_dpcm(project, line); };
    dtable["FDSWAVE"] = [this](Project& project, const std::string& line) { handle_fds_wave(project, line); };
    dtable["FDSMOD"] = [this](Project& project, const std::string& line) { handle_fds_mod(project, line); };
    dtable["FDSMACRO"] = [this](Project& project, const std::string& line) { handle_fds_macro(project, line); };
    dtable["N163WAVE"] = [this](Project& project, const std::string& line) { handle_n163_wave(project, line); };

    // track handlers
    dtable["TRACK"] = [this](Project& project, const std::string& line) { handle_track(project, line); };
    dtable["COLUMNS"] = [this](Project& project, const std::string& line) { handle_columns(project, line); };
    dtable["ORDER"] = [this](Project& project, const std::string& line) { handle_order(project, line); };
    dtable["PATTERN"] = [this](Project& project, const std::string& line) { handle_pattern(project, line); };
    dtable["ROW"] = [this](Project& project, const std::string& line) { handle_row(project, line); };
}

void ProjectReader::handle_song_information(Project& project, const std::string& line) {
    std::stringstream ss(line);
    std::string tag;
    ss >> tag;

    std::string value = get_quote(line);

    if (value.empty()) { return; }

    if (tag == "TITLE") { project.title = value;    }
    else if (tag == "AUTHOR") { project.author = value; }
    else if (tag == "COPYRIGHT") { project.copyright = value; }
    else if (tag == "COMMENT") { project.comments.push_back(value); }
    else { std::cerr << "[E] Unknown tag: " << tag << std::endl; }
}

void ProjectReader::handle_global_settings(Project& project, const std::string& line) {
    // TODO use regex to get the number value
    
    std::stringstream ss(line);
    std::string tag;
    int value;
    
    ss >> tag;
    ss >> value;

    if (tag == "MACHINE") { project.machine = value; }
    else if (tag == "FRAMERATE") { project.framerate = value; }
    else if (tag == "EXPANSION") { project.expansion = value; }
    else if (tag == "VIBRATO") { project.vibrato = value; }
    else if (tag == "SPLIT") { project.split = value; }
    else if (tag == "N163CHANNELS") { project.n163channels = value; }
    else { std::cerr << "Unknown tag: " << tag << std::endl; }
}

void ProjectReader::handle_macro(Project& project, const std::string& line) {
    static std::unordered_map<std::string, InstrumentFamily> inst_t_map = {
        {"MACRO", INST_2A03},
        {"MACROVRC6", INST_VRC6},
        {"MACRON163", INST_N163},
        {"MACROS5B", INST_S5B}
    };
    static std::unordered_map<int, MacroType> macro_t_map = {
        {0, VOL}, {1, ARP}, {2, PIT}, {3, HPI}, {4, DUT}
    };

    std::stringstream ss(line);
    std::string tag;
    ss >> tag;

    int type, index, loop, release, setting;
    ss >> type >> index >> loop >> release >> setting;
    
    std::string text_after_colon = get_text_after_colon(line);
    std::vector<int> sequence = get_int_list(text_after_colon);

    InstrumentFamily inst_t = inst_t_map[tag];
    MacroType macro_t = macro_t_map[type];
    
    std::string macro_key = generate_macro_key(inst_t, macro_t, index);
    
    Macro m(inst_t, macro_t, index, loop, release, setting, std::move(sequence));

    project.macros[macro_key] = std::move(m);
}


void ProjectReader::handle_dpcm_def(Project& project, const std::string& line) {
    std::stringstream ss(line);
    std::string tag;
    int dpcm_idx, sample_size;

    ss >> tag >> dpcm_idx >> sample_size;
    std::string name = get_quote(line);
    
    DpcmSample obj(dpcm_idx, sample_size, name);
    current_dpcm_idx = dpcm_idx;
    project.dpcm_samples[dpcm_idx] = std::move(obj);
}

void ProjectReader::handle_dpcm_data(Project& project, const std::string& line) {
    auto it = project.dpcm_samples.find(current_dpcm_idx);
    if (it == project.dpcm_samples.end() ) {
        return;
    }
    
    std::string text_after_colon = get_text_after_colon(line);
    std::vector<int> numbers = get_hex_list(text_after_colon);
    
    DpcmSample& obj = it->second;
    for (const auto& num : numbers) {
        obj.data.push_back(num);
    }
}

void ProjectReader::handle_groove(Project& project, const std::string& line) {
    std::stringstream ss(line);
    std::string tag;
    int idx, size;

    ss >> tag >> idx >> size;
    std::string text_after_colon = get_text_after_colon(line);
    std::vector<int> seq = get_int_list(text_after_colon);

    Groove g(idx, size, seq);
    project.grooves.insert( {idx, std::move(g)} );
}

void ProjectReader::handle_use_groove(Project& project, const std::string& line) {
    std::string text_after_colon = get_text_after_colon(line);
    std::vector<int> nums = get_int_list(text_after_colon);
    for (const auto& num : nums) {
        project.use_groove.insert(num);
    }
}

void ProjectReader::handle_inst_basic(Project& project, const std::string& line) {
    static std::unordered_map<std::string, InstrumentFamily> inst_t_map = {
        {"INST2A03", INST_2A03},
        {"INSTVRC6", INST_VRC6},
        {"INSTN163", INST_N163},
        {"INSTS5B" , INST_S5B}
    };

    /* TODO - DEBUG : floating point segfault error?
    std::cout << "[D] skipping inst basic for floating point error" << std::endl;
    return;
    */

    std::stringstream ss(line);
    std::string tag;
    ss >> tag;

    int index, seq_vol, seq_arp, seq_pit, seq_hpi, seq_dut;
    ss >>index >> seq_vol >> seq_arp >> seq_pit >> seq_hpi >> seq_dut;

    std::string name = get_quote(line);
    
    InstrumentFamily inst_t = inst_t_map[tag]; 

    Instrument inst(inst_t, index, seq_vol, seq_arp, seq_pit, seq_hpi, seq_dut, name);
    
    // assign special N163 settings
    if (inst_t == INST_N163) {
        int w_size, w_pos, w_count;
        ss >> w_size >> w_pos >> w_count;
        
        inst.n163_settings.w_size = w_size;
        inst.n163_settings.w_pos = w_pos;
        inst.n163_settings.w_count = w_count;
    }

    // assign macros
    struct Zip {
        MacroType macro_t;
        int macro_idx;
        std::optional<Macro>* macro_ptr;
    };

    std::vector<Zip> fields {
        {VOL, seq_vol, &inst.mac_vol},
        {ARP, seq_arp, &inst.mac_arp},
        {PIT, seq_pit, &inst.mac_pit},
        {HPI, seq_hpi, &inst.mac_hpi},
        {DUT, seq_dut, &inst.mac_dut}
    };
    
    for (const auto& field : fields) {
        std::string macro_key = generate_macro_key(inst_t, field.macro_t, field.macro_idx);
        
        auto it = project.macros.find(macro_key);
        if (it != project.macros.end() ) {
            // std::cout << "[D] Found macro: " << macro_key << std::endl;
            *field.macro_ptr = (it->second);
        }
    }
    
    // std::cout << "[D] Instrument added : " << inst.index << ": '" << inst.name << "'\n";
    project.instruments.insert( {index, std::move(inst) } );
}

void ProjectReader::handle_inst_vrc7( Project& project, const std::string& line) {
    std::stringstream ss(line);
    std::string tag;
    int index, patch;
    std::vector<int> registers;
    registers.resize(8);

    ss >> tag;
    ss >> index >> patch;
    
    for (int i = 0; i < 8; ++i) {
        std::string hex_num;
        ss >> hex_num;
        int num = convert_hex_str_to_int(hex_num);
        registers.push_back(num);
    }

    std::string name = get_quote(line);

    InstrumentFamily inst_t = INST_VRC7;
    Instrument inst(inst_t, index, -1, -1, -1, -1, -1, name);
    inst.vrc7_settings.patch = patch;
    inst.vrc7_settings.registers = std::move(registers);
    
    // std::cout << "[D] VRC7 Instrument added : " << inst.index << ": '" << inst.name << "'\n";
    project.instruments.insert( {index, std::move(inst) } );
}

void ProjectReader::handle_inst_fds( Project& project, const std::string& line) {
    std::stringstream ss(line);
    
    std::string tag;
    int index;
    bool mod_enable;
    int mod_speed, mod_depth, mod_delay;

    ss >> tag;
    ss >> index;
    ss >> mod_enable >> mod_speed >> mod_depth >> mod_delay;
    std::string name = get_quote(line);

    InstrumentFamily inst_t = INST_FDS;
    Instrument inst(inst_t, index, -1, -1, -1, -1, -1, name);
    
    inst.fds_settings.mod_enable = mod_enable;
    inst.fds_settings.mod_speed = mod_speed;
    inst.fds_settings.mod_depth = mod_depth;
    inst.fds_settings.mod_delay = mod_delay;

    // std::cout << "[D] FDS Instrument added : " << inst.index << ": '" << inst.name << "'\n";
    project.instruments.insert( {index, std::move(inst) } );
}

void ProjectReader::handle_key_dpcm( Project& project, const std::string& line) {
    std::stringstream ss(line);
    std::string tag;
    int inst_idx, octave, note, sample, pitch, loop, loop_point, delta;

    ss >> tag;
    ss >> inst_idx >> octave >> note >> sample;
    ss >> pitch >> loop >> loop_point >> delta;
    
    auto it = project.instruments.find(inst_idx);
    if (it == project.instruments.end() ) {
        return;
    }
    Instrument& foundObject = it->second;
    KeyDpcm obj(inst_idx, octave, note, sample, pitch, loop, loop_point, delta);
    
    int midi_pitch = (octave * 12) + note;
    foundObject.key_dpcm_notes.insert( {midi_pitch, std::move(obj) } );
}


void ProjectReader::handle_fds_wave( Project& project, const std::string& line) {
    std::stringstream ss(line);
    std::string tag;
    int inst_idx;

    ss >> tag >> inst_idx;
    
    // check for existence of instrument
    auto it = project.instruments.find(inst_idx);
    if (it == project.instruments.end()) {
        return;
    }

    std::string text_after_colon = get_text_after_colon(line);
    std::vector<int> numbers = get_int_list(text_after_colon);
    
    Instrument& foundObject = it->second;
    foundObject.fds_settings.fds_wave = std::move(numbers);
}

void ProjectReader::handle_fds_mod( Project& project, const std::string& line) {
    std::stringstream ss(line);
    std::string tag;
    int inst_idx;

    ss >> tag >> inst_idx;
    
    // check for existence of instrument
    auto it = project.instruments.find(inst_idx);
    if (it == project.instruments.end()) {
        return;
    }

    std::string text_after_colon = get_text_after_colon(line);
    std::vector<int> numbers = get_int_list(text_after_colon);
    
    Instrument& foundObject = it->second;
    foundObject.fds_settings.fds_mod = std::move(numbers);
}

void ProjectReader::handle_fds_macro( Project& project, const std::string& line) {
    std::stringstream ss(line);
    std::string tag;
    int inst_idx, type, loop, release, setting;
    
    ss >> tag;
    ss >> inst_idx >> type >> loop >> release >> setting;
    
    MacroType macro_t;
    switch(type) {
        case 0:
            macro_t = VOL;
            break;
        case 1:
            macro_t = ARP;
            break;
        case 2:
            macro_t = PIT;
            break;
        default:
            return;
    }

    // check for existence of instrument
    auto it = project.instruments.find(inst_idx);
    if (it == project.instruments.end()) {
        return;
    }
    Instrument& foundObject = it->second;
    
    std::string text_after_colon = get_text_after_colon(line);
    std::vector<int> sequence = get_int_list(text_after_colon);
    
    Macro fds_macro( INST_FDS, macro_t, inst_idx, loop, release, setting, std::move(sequence) );
   
    // assign newly created macro to fds instrument
    switch(type) {
        case 0:
            foundObject.mac_vol = fds_macro;
            break;
        case 1:
            foundObject.mac_arp = fds_macro;
            break;
        case 2:
            foundObject.mac_pit = fds_macro;
            break;
        default:
            return;
    }
    std::cout << "[D] Assigned FDS macro to instrument: " << foundObject.name << std::endl;
    
    // save the macro to project.macros in case we want it later
    std::string macro_key = generate_macro_key(INST_FDS, macro_t, inst_idx);
    project.macros.insert( {macro_key, fds_macro} ); 
}

void ProjectReader::handle_n163_wave( Project& project, const std::string& line) {
    std::stringstream ss(line);
    std::string tag;
    int inst_idx;
    int wave_idx;

    ss >> tag >> inst_idx >> wave_idx;

    std::string text_after_colon = get_text_after_colon(line);
    std::vector<int> numbers = get_int_list(text_after_colon);

    // check for existence of instrument
    auto it = project.instruments.find(inst_idx);
    if (it == project.instruments.end()) {
        return;
    }
    Instrument& foundObject = it->second;

    foundObject.n163_settings.wave_table.insert( {wave_idx, std::move(numbers) } ); 
    
    std::cout << "[D] Assigned N163 wave to instrument: " << foundObject.name << std::endl;
}

void ProjectReader::handle_track( Project& project, const std::string& line) {
    std::string tag;
    int num_rows, speed, tempo;

    std::stringstream ss(line);
    ss >> tag;
    ss >> num_rows >> speed >> tempo;
    std::string name = get_quote(line);
    
    Track track(name, num_rows, speed, tempo);
    
    // std::cout << "Added track: " << track.name << std::endl;
    project.tracks.push_back(std::move(track));
}

void ProjectReader::handle_columns( Project& project, const std::string& line ) {
    if (project.tracks.empty()) {
        std::cerr << "[E] Cannot process COLUMNS until a Track has been initialized." << std::endl;
        exit(1);
    }
    std::string text_after_colon = get_text_after_colon(line);
    std::vector<int> numbers = get_int_list(text_after_colon);
    int num_cols = numbers.size();

    Track& current_track = project.tracks.back();
    current_track.num_cols = num_cols;
    current_track.eff_cols = std::move(numbers);
}

void ProjectReader::handle_order( Project& project, const std::string& line) {
    if (project.tracks.empty()) {
        std::cerr << "[E] Cannot process ORDER until a Track has been initialized." << std::endl;
        exit(1);
    }
    std::stringstream ss(line);
    std::string tag;
    ss >> tag;
    ss >> tag;
    int order_idx = convert_hex_str_to_int(tag);

    std::string text_after_colon = get_text_after_colon(line);
    std::vector<int> numbers = get_hex_list(text_after_colon);

    Track& current_track = project.tracks.back();
    current_track.orders.insert( {order_idx, std::move(numbers) } );
}

void ProjectReader::handle_pattern( [[maybe_unused]] Project& project, const std::string& line) {
    /* 
    // Not actually a requirement for this TAG:
    
    if (project.tracks.empty()) {
        std::cerr << "[E] Cannot process PATTERN until a Track has been initialized." << std::endl;
        exit(1);
    }
    Track& t = project.tracks.back();
    */

    std::stringstream ss(line);
    std::string tag;
    ss >> tag; // tag
    ss >> tag; // field but it is a string. need to convert it to int.
    current_pattern = convert_hex_str_to_int(tag);

    // Now current_pattern will be stored persistently.
    // This is important for handle_row() to add the token to the correct pattern.
}

void ProjectReader::handle_row( Project& project, const std::string& line) {
    if (project.tracks.empty()) {
        std::cerr << "[E] Cannot process ROW until a Track has been initialized." << std::endl;
        exit(1);
    }
    Track& current_track = project.tracks.back();

    std::stringstream ss(line);
    std::string tag, second_word;
    ss >> tag;
    ss >> tag;

    int row_idx = convert_hex_str_to_int(tag);

    std::vector<std::string> tokens = get_colon_fields(line);
    // std::cout << "[D] tokens.size = " << tokens.size() << std::endl;

    // Add tokens
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (contains_only_spaces_and_periods(tokens[i])) {
            // std::cout << "Skipping token: " << tokens[i] << std::endl;
            continue;
        }
        std::string token_key = generate_token_key(current_pattern, row_idx, i);
        current_track.tokens.insert( {token_key, tokens[i]} );
        // std::cout << "[D] Added item: key='" << token_key << "', val='" << tokens[i] << "'" << std::endl;
    }
}

void ProjectReader::process_line(Project& project, const std::string& line) {
    // Process a Famitracker Line.
    // std::cout << "[D] LINE = " << line << std::endl;

    std::stringstream ss(line);
    std::string tag;
    ss >> tag;

    // Skip blank lines and comment lines.
    if (tag.empty()) { return; }
    if (tag[0] == '#') { return; }

    // Find correct Handler in dispatch table and call the method.
    auto it = dtable.find(tag);
    if (it != dtable.end() && it->second != nullptr) {
        // std::cout << "[D] Dispatching tag: " << tag << std::endl;
        auto method = it->second;
        try {
            it->second(project, line);
        } 
        catch (const std::exception& e) {
            std::cerr << "[!] Exception in handler for tag " << tag << ": " << e.what() << std::endl;
        }
    } 
    else {
        std::cout << "[!] Tag not found or null handler: " << tag << std::endl;
    }
}

// *****************************************************************************
// Main entry point:

void ProjectReader::execute(Project& project, const std::string& input_file) {
    // Main method to call. Entry point to read input_file data into Project.
    std::cout << "[D] Reading project ..." << std::endl;

    std::ifstream fh(input_file);
    if (!fh.is_open()) {
        std::cerr << "[E] Could not open file: " << input_file << std::endl;
        exit(1);
    }

    std::string line;
    while (std::getline(fh, line)) {
        process_line(project, line);
    }
}

