// project_reader.cpp

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_set>
#include <stdexcept>
#include <list>

#include "project_reader.h"

#include "project.h"
#include "macro.h"
#include "dpcm_sample.h"
#include "groove.h"
#include "instrument.h"
#include "key_dpcm.h"
#include "track.h"

#include "constants.h"
#include "string_helpers.h"
#include "key_gen.h"
// #include "container_printing.hpp"

ProjectReader::ProjectReader() {
    /*
    ProjectReader constructor
    Initialize ProjectReader helper class.
    Sets up dispatch table `dtable` by inserting std::pair<std::string, lambda function>
    The lambda function has a signature of void(ProjectReader::*)(Project&, const std::string&), where  Project& is a mutable reference.
    While parsing the data, we keep track of the persistent variables `current_dpcm_idx` and `current_pattern` that get set on their respective lines.
    [this] binds the function pointer to the ProjectReader class namepsace by virtue of std::function.
    */

    static std::list<std::string> song_information_tags = { "TITLE", "AUTHOR", "COPYRIGHT", "COMMENT" };
    static std::list<std::string> global_settings_tags = { "MACHINE", "FRAMERATE", "EXPANSION", "VIBRATO", "SPLIT", "N163CHANNELS" };
    static std::list<std::string> macro_tags = { "MACRO", "MACROVRC6", "MACRON163", "MACROS5B" };
    static std::list<std::string> basic_inst_tags = { "INST2A03", "INSTVRC6", "INSTN163", "INSTS5B" };
    
    // init dispatch table
    dtable.clear();
    
    // metadata handlers
    for (const auto& tag : song_information_tags) {
        dtable.insert( {tag, [this](Project& project, const std::string& line) { handle_song_information(project, line); }});
    }
    for (const auto& tag : global_settings_tags) {
        dtable.insert( {tag, [this](Project& project, const std::string& line) { handle_global_settings(project, line); }});
    }

    // macro handlers
    for (const auto& tag : macro_tags) {
        dtable.insert( {tag, [this](Project& project, const std::string& line) { handle_macro(project, line); }});
    }
    
    // dpcm handlers
    dtable.insert({"DPCMDEF", [this](Project& project, const std::string& line) { handle_dpcm_def(project, line); }});
    dtable.insert({"DPCM", [this](Project& project, const std::string& line) { handle_dpcm_data(project, line); }});

    // groove handlers
    dtable.insert({"GROOVE", [this](Project& project, const std::string& line) { handle_groove(project, line); }});
    dtable.insert({"USEGROOVE", [this](Project& project, const std::string& line) { handle_use_groove(project, line); }});

    // instrument handlers
    for (const auto& tag : basic_inst_tags) {
        dtable.insert( {tag, [this](Project& project, const std::string& line) { handle_inst_basic(project, line); }});
    }
    dtable.insert({"INSTVRC7", [this](Project& project, const std::string& line) { handle_inst_vrc7(project, line); }});
    dtable.insert({"INSTFDS", [this](Project& project, const std::string& line) { handle_inst_fds(project, line); }});
    
    // special handlers 
    dtable.insert({"KEYDPCM",  [this](Project& project, const std::string& line) { handle_key_dpcm(project, line); }});
    dtable.insert({"FDSWAVE", [this](Project& project, const std::string& line) { handle_fds_wave(project, line); }});
    dtable.insert({"FDSMOD", [this](Project& project, const std::string& line) { handle_fds_mod(project, line); }});
    dtable.insert({"FDSMACRO", [this](Project& project, const std::string& line) { handle_fds_macro(project, line); }});
    dtable.insert({"N163WAVE", [this](Project& project, const std::string& line) { handle_n163_wave(project, line); }});

    // track handlers
    dtable.insert({"TRACK", [this](Project& project, const std::string& line) { handle_track(project, line); }});
    dtable.insert({"COLUMNS", [this](Project& project, const std::string& line) { handle_columns(project, line); }});
    dtable.insert({"ORDER", [this](Project& project, const std::string& line) { handle_order(project, line); }});
    dtable.insert({"PATTERN", [this](Project& project, const std::string& line) { handle_pattern(project, line); }});
    dtable.insert({"ROW", [this](Project& project, const std::string& line) { handle_row(project, line); }});
}

ProjectReader::~ProjectReader() {
    /*
    ProjectReader destructor.
    Explicitly call the clear method on the dtable.
    */

    dtable.clear();
}

void ProjectReader::handle_song_information(Project& project, const std::string& line) {
    /*
    Handle a line with format:

    TITLE [title]
        - title: string - the document's title
    AUTHOR [author]
        - author: string - the document's author
    COPYRIGHT [copyright]
        - copyright: string - the document's copyright information
    COMMENT [comment]
        - comment: string - line of text to add
    */

    std::stringstream ss(line);
    std::string tag;
    ss >> tag;

    std::string value = get_quote(line);

    if (value.empty()) { return; }

    if (tag == "TITLE") { project.title = value; }
    else if (tag == "AUTHOR") { project.author = value; }
    else if (tag == "COPYRIGHT") { project.copyright = value; }
    else if (tag == "COMMENT") { project.comments.push_back(value); }
    else { std::cerr << "[E] Unknown tag: " << tag << std::endl; }
}

void ProjectReader::handle_global_settings(Project& project, const std::string& line) {
    /*
    Handle a line with format:
    
    MACHINE [machine]
        - machine: int[0,1] - 0 for NTSC, 1 for PAL
    FRAMERATE [fps]
        - fps: int[0,800] - music framerate, 0 for machine default
    EXPANSION [chips]
        - chips: int[0,255] - bitfield representing expansion chips used: 1=VRC6, 2=VRC7, 4=FDS, 8=MMC5, 16=N163, 32=S5B
    VIBRATO [mode]
        - mode: int[0,1] - 0 for old style vibrato, 1 for new style
    SPLIT [split]
        - split: int[0,255] - split point where Fxx effect sets tempo instead of speed
    N163CHANNELS [channels]
        - channels: int[1,8] - channels used by N163 expansion
    */

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
    /*
    Handle a line with format:

    MACRO [type] [index] [loop] [release] [setting] : [macro]
        # Defines a macro sequence for a 2A03 instrument.
        - type: int[0,4] - 0=volume, 1=arpeggio, 2=pitch, 3=hi-pitch, 4=duty
        - index: int[0,127] - index of the macro
        - loop: int[-1,253] - loop point, -1 for no loop
        - release: int[-1,253] - release point, -1 for no release
        - setting: int[0,255] - macro setting (for arpeggio: 0=absolute, 1=fixed, 2=relative, 3=scheme)
        - macro: int_list[-128,127] - macro sequence
    */

    // For fast string to constant lookup
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
    /*
    Handle a line with format:

    DPCMDEF [index] [size] [name]
        # Creates a DPCM sample of the specified size in bytes. Use the DPCM command to fill in the sample data.
        - index: int[0,63] - index of the sample
        - size: int[0,4081] - size in bytes to allocate for the sample
        - name: string - original filename of sample
    */

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
    /*
    Handle a line with format:

    DPCM : [data]
        # Fills in sample data for the sample defined by the last DPCMDEF command.
        # The data list may be of any length as long as it does not exceed the allocated sample length.
        # Multiple DPCM commands will resume filling the sample where the previous one left off.
        - data: hex_list[00,FF] - contents of the sample
    */

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
    /*
    Handle a line with format:

    GROOVE [index] [sizeof] : [groove_sequence]
        - index: int[0,63] - integer representing the groove number. default groove is the first in groove_list.
        - sizeof: int[0,63] - len of groove_sequence
        - groove_sequence: int_list[0,255] - represents speed change per row.
        - (e.g.) list = [4,3,3,3] simulates F04 F03 F03 F03 on each row. Loop groove_sequence until otherwise specified.
    */

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
    /*
    Handle a line with format:
    
    USEGROOVE : []
        - tracks: int_list[0,63] - tells the project which tracks use the default groove
    */
    
    std::string text_after_colon = get_text_after_colon(line);
    std::vector<int> nums = get_int_list(text_after_colon);
    for (const auto& num : nums) {
        project.use_groove.insert(num);
    }
}

void ProjectReader::handle_inst_basic(Project& project, const std::string& line) {
    /*
    Handle a line in format:

    <INST> [index] [seq_vol] [seq_arp] [seq_pit] [seq_hpi] [seq_dut] [name]
        # Defines a 2A03 instrument. To add DPCM sample key mappings, use the KEYDPCM command.
        # Use the MACRO command to define sequences for use with 2A03 instruments.
        # INST can be INST2A03, INSTVRC6, or INSTS5B. 
        # Note that MMC5 appears identical to 2A03.
        - index: int[0,63] - index of the instrument
        - seq_vol: int[-1,127] - volume macro sequence, -1 for none
        - seq_arp: int[-1,127] - arpeggio macro sequence, -1 for none
        - seq_pit: int[-1,127] - pitch macro sequence, -1 for none
        - seq_hpi: int[-1,127] - hi-pitch macro sequence, -1 for none
        - seq_dut: int[-1,127] - duty macro sequence, -1 for none
        - name: string - name of the instrument

    If the instruent is INSTN163, we do all of the same steps with the addition of the 3 extra fields:

    INSTN163 [index] [seq_vol] [seq_arp] [seq_pit] [seq_hpi] [seq_wav] [w_size] [w_pos] [w_count] [name]
        # Defines a Namco 163 instrument. Use the MACRON163 command to define sequences for use with N163 instruments.
        - index: int[0,63] - index of the instrument
        - seq_vol: int[-1,127] - volume macro sequence, -1 for none
        - seq_arp: int[-1,127] - arpeggio macro sequence, -1 for none
        - seq_pit: int[-1,127] - pitch macro sequence, -1 for none
        - seq_hpi: int[-1,127] - hi-pitch macro sequence, -1 for none
        - seq_wav: int[-1,127] - wave macro sequence, -1 for none
        - w_size: int[0,32] - wave length
        - w_pos: int[0,127] - wave memory position
        - w_count: int[0,16] - number of waves
        - name: string - name of the instrument

    Thus, we can handle 4 of the 6 instrument types with this one function.
    */

    // define a map for quick string to constant look-up
    static std::unordered_map<std::string, InstrumentFamily> inst_t_map = {
        {"INST2A03", INST_2A03},
        {"INSTVRC6", INST_VRC6},
        {"INSTN163", INST_N163},
        {"INSTS5B" , INST_S5B}
    };

    std::stringstream ss(line);
    std::string tag;
    ss >> tag;

    int index, seq_vol, seq_arp, seq_pit, seq_hpi, seq_dut;
    ss >>index >> seq_vol >> seq_arp >> seq_pit >> seq_hpi >> seq_dut;

    std::string name = get_quote(line);
    
    InstrumentFamily inst_t = inst_t_map[tag]; 

    Instrument inst(inst_t, index, seq_vol, seq_arp, seq_pit, seq_hpi, seq_dut, name);
    
    // Assign special N163 settings
    if (inst_t == INST_N163) {
        int w_size, w_pos, w_count;
        ss >> w_size >> w_pos >> w_count;
        
        inst.n163_settings.w_size = w_size;
        inst.n163_settings.w_pos = w_pos;
        inst.n163_settings.w_count = w_count;
    }

    // Assign macros
    struct MacroField {
        MacroType macro_t;
        int macro_idx;
        std::optional<Macro>* macro_ptr;
    };

    std::vector<MacroField> fields {
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
    /*
    Handle line with format:

    INSTVRC7 [index] [patch] [r0] [r1] [r2] [r3] [r4] [r5] [r6] [r7] [name]
        # Defines a VRC7 instrument.
        - index: int[0,63] - index of the instrument
        - patch: int[0,15] - VRC7 patch used by the instrument
        - register: hex_list[00,FF] - custom patch register data
        - name: string - name of the instrument
    */

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
    /*
    Handle line with format:

    INSTFDS [index] [mod_enable] [mod_speed] [mod_depth] [mod_delay] [name]
        # Defines an FDS instrument.
        # Use the commands FDSWAVE, FDSMOD, and FDSMACRO to fill in the waveform, modulation table, and macro data for the FDS instrument.
        - index: int[0,63] - index of the instrument
        - mod_enable: int[0,1] - 0 for modulator disabled, 1 for enabled
        - mod_speed: int[0,4905] - modulator speed
        - mod_depth: int[0,63] - modulator depth
        - mod_delay: int[0,255] - modulator delay
        - name: string - name of the instrument
    */
    
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
    /*
    Handle line with format:

    KEYDPCM [inst] [octave] [note] [sample] [pitch] [loop] [loop_point] [delta]
        # Adds a DPCM sample mapping to a key in a 2A03 instrument.
        # Use the INST2A03 command to define the instrument before attempting to add a key mapping.
        - inst: int[0,63] - index of the instrument
        - octave: int[0,7] - octave of key
        - note: int[0,11] - scale note of key
        - sample: int[0,63] - index of the sample to play
        - pitch: int[0,15] - pitch of the sample
        - loop: int[0,1] - 0 for no loop, 1 for loop
        - loop_point: int[0,255] - loop point of the sample (32 byte increments)
        - delta: int[-1,127] - delta counter value, -1 for off
    */

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
    /*
    Handle a line with format:

    FDSWAVE [inst] : [data]
        # Defines a waveform for an FDS instrument.
        # Use the INSTFDS command to define the instrument before attempting to define its waveform.
        - inst: int[0,63] - index of the instrument
        - data: int_list[0,63] - contents of the waveform, must be 64 elements long
    */

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
    /*
    Handle a line with format:
    
    FDSMOD [inst] : [data]
        # Defines a modulator table for an FDS instrument.
        # Use the INSTFDS command to define the instrument before attempting to define its mod table.
        - inst: int[0,63] - index of the instrument
        - data: int_list[0,7] - contents of the table, must be 32 elements long
    */
    
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
    /*
    Handle a line with format:

    FDSMACRO [inst] [type] [loop] [release] [setting] : [macro]
        # Defines a macro sequence for an FDS instrument.
        # Use the INSTFDS command to define the instrument before attempting to define its macros.
        - inst: int[0,63] - index of the instrument
        - type: int[0,2] - 0=volume, 1=arpeggio, 2=pitch
        - loop: int[-1,253] - loop point, -1 for no loop
        - release: int[-1,253] - release point, -1 for no release
        - setting: int[0,255] - macro setting (for arpeggio: 0=absolute, 1=fixed, 2=relative)
        - macro: int_list[-128,127] - macro sequence
    */

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
    /*
    Handle a line with format:

    N163WAVE [inst] [wave] : [data]
        # Defines a waveform for a Namco 163 instrument.
        # Use the INSTN163 command to define the instrument before attempting to define its waveforms.
        - inst: int[0,63] - index of the instrument
        - wave: int[0,15] - index of the wave
        - data: int_list[0,15] - contents of the waveform, must be the same length as defined by the INSTN163 instrument
    */
    
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
    /*
    Handle a line with format:

    TRACK [pattern] [speed] [tempo] [name]
        # Begins a new track.
        - pattern: int[0,256] - length of patterns
        - speed: int[0,255] - track speed
        - tempo: int[0,255] - track tempo
        - name: string - name of the track
    */

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
    /*
    Handle a line with format:

    COLUMNS : [columns]
        # Sets the number of effect columns in each channel.
        # This should be specified after a TRACK command, and before using ROW commands to fill in pattern data.
        # If not specified, each channel will have the default number of effect columns (1).
        - columns: int_list[1,64] - number of effect columns for each channel, where x is number of cols designated by EXPANSION.
    */

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
    /*
    Handle line with format:

    ORDER [frame] : [list]
        # Sets the patterns to be played in each frame. Applies to current track.
        - frame: hex[00,7F] - index of frame
        - list: hex_list[00,7F] - pattern to use for each channel, len of list must match number of cols
    */

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

void ProjectReader::handle_pattern( Project& project, const std::string& line) {
    /*
    Handle line with format:

    PATTERN [pattern]
        # Sets the current pattern to be filled by subsequent ROW commands.
        - pattern: hex[00,7F] - index of pattern
    */

    if (project.tracks.empty()) {
        std::cerr << "[E] Cannot process PATTERN until a Track has been initialized." << std::endl;
        exit(1);
    }
    // Track& t = project.tracks.back();

    std::stringstream ss(line);
    std::string tag;
    ss >> tag; // tag
    ss >> tag; // field but it is a string. need to convert it to int.
    current_pattern = convert_hex_str_to_int(tag);

    // Now current_pattern will be stored persistently.
    // This is important for handle_row() to add the token to the correct pattern.
}

void ProjectReader::handle_row( Project& project, const std::string& line) {
    /*
    Handle line with format:

    ROW [row] : [c0] : [c1] : [c2] ...
        - row: string - contains music data
    ROW DATA:
    row: 
        - hex[00,7F] - index of pattern
    c0: 
        - channel data for each channel. 
        - The channel data looks like: nnn ii v eee
    nnn: note,
        - begins with a letter (C,D,E,F,G,A,B),
        - then a sharp (#,+) natural (-,.) or flat (b,f) character,
        - then an octave (0,1,2,3,4,5,6,7);
        - an empty note is "...",
        = a halt is "---",
        - a release is "===";
        - the notes for the noise channel c3 are special:
        - they begin with a hex[0,F] to specify noise pitch, followed by "-#"
    ii: 
        - hex[00,3F] - instrument index, 
        - ".." for none.
    v: 
        - hex[0,F] - volume, 
        - "." for none.
    eee: 
        - effect, begins with a letter or number specifying the effect type,
        - then the last two characters are hex[00,FF] specifying the parameter value.
        - There must be as many eee fields as specified by the COLUMNS command.
    */
    
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

    // Add tokens
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (contains_only_spaces_and_periods(tokens[i])) {
            continue;
        }
        std::string token_key = generate_token_key(current_pattern, row_idx, i);
        current_track.tokens.insert( {token_key, tokens[i]} );
    }
}

void ProjectReader::process_line(Project& project, const std::string& line) {
    /*
    Process a single line from the FamiTracker text file.
    Calls the right function handler.
    */

    std::stringstream ss(line);
    std::string tag;
    ss >> tag;

    // Skip blank lines and comment lines.
    if (tag.empty()) { 
        return; 
    }
    if (tag[0] == '#') { 
        return; 
    }

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
    /*
    Main method to call. 
    Entry point to read input_file data into Project.
    */

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

