# project_reader.py

from typing import Dict, Callable 

from track import Track
from helpers import (
    get_quote,
    generate_token,
    contains_only_periods_and_spaces
)

class ProjectReader: 
    def __init__(self):
        self.dispatch_table: Dict[str, callable] = { }
        self.setup_dtable()
    
    def setup_dtable(self):
        # song metadata handlers
        tags= ["TITLE", "AUTHOR", "COPYRIGHT"]
        for tag in tags:
            self.dispatch_table[tag] = self.handle_song_information

        self.dispatch_table["COMMENT"] = self.handle_comment
        
        tags = ["MACHINE", "FRAMERATE", "EXPANSION", "VIBRATO", "SPLIT", "N163CHANNELS"]
        for tag in tags:
            self.dispatch_table[tag] = self.handle_global_settings
        
        # track handlers
        self.dispatch_table["TRACK"] = self.handle_track
        self.dispatch_table["COLUMNS"] = self.handle_columns
        self.dispatch_table["ORDER"] = self.handle_order
        self.dispatch_table["PATTERN"] = self.handle_pattern
        self.dispatch_table["ROW"] = self.handle_row

    # metadata handlers
    def handle_song_information(self, project: "Project", line: str, tag: str):
        value = get_quote(line)
        setattr(project, tag.lower(), value)
    
    def handle_comment(self, project: "Project", line: str, tag: str):
        value = get_quote(line)
        project.comment.append(value)

    def handle_global_settings(self, project: "Project", line: str, tag: str):
        value = int(line.split()[-1])
        setattr(project, tag.lower(), value)
    
    # track handlers
    def handle_track(self, project: "Project", line: str, tag: str):
        num_rows, speed, tempo = [int(v) for v in line.split()[1:4]]
        name = get_quote(line)

        t = Track(num_rows, speed, tempo, name)
        project.tracks.append(t)
    
    def handle_columns(self, project: "Project", line: str, tag: str):
        ct = project.tracks[-1]
        if not ct: raise ValueError("Track not initialized")
        
        eff_cols = [int(v) for v in line.split(":")[1].strip().split()]
        num_cols = len(eff_cols)

        ct.eff_cols = eff_cols
        ct.num_cols = num_cols
    
    def handle_order(self, project: "Project", line: str, tag: str):
        ct = project.tracks[-1]
        if not ct: raise ValueError("Track not initialized")
        
        order = line.split()[1]
        patterns = [int(v, 16) for v in line.split(":")[1].strip().split()]
        ct.orders[order] = patterns
    
    def handle_pattern(self, project: "Project", line: str, tag: str):
        ct = project.tracks[-1]
        if not ct: raise ValueError("Track not initialized")
        
        self.current_pattern = int(line.split()[-1], 16)
        
    def handle_row(self, project: "Project", line: str, tag: str):
        ct = project.tracks[-1]
        if not ct: raise ValueError("Track not initialized")
        
        row_idx = int(line.split()[1], 16)
        tokens = [t.strip() for t in line.split(":")[1:] ]

        for col_idx, t in enumerate(tokens):
            if contains_only_periods_and_spaces(t): continue
            tk = generate_token(self.current_pattern, row_idx, col_idx)
            ct.tokens[tk] = t
        
    def _process_line(self, project, line):
        tag = line.split()[0]
        if tag not in self.dispatch_table:
            return
        
        func = self.dispatch_table[tag]
        func(project, line, tag)

    def read_project(self, project, input_file) -> None:
        try:
            with open(input_file, 'r') as file:
                for line in file:
                    line = line.strip()

                    # skip blank lines
                    if not line: continue

                    # skip comment lines
                    if line.startswith("#"): continue
                    
                    self._process_line(project, line)

        except Exception as e:
            print("An error has occured in read_project(): {}".format(e))
            exit(1)

