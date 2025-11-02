#!/usr/bin/env python3

import os
import sys
import json
import re

from typing import List, Dict, Callable 

from project import Project 
from track import Track 
from project_reader import ProjectReader
from project_parser import ProjectParser 
from project_exporter import ProjectExporter

def main():
    try:
        input_file = sys.argv[1] 
    except Exception as e:
        print("Usage error: ./main <input_file.txt>")
        exit(1)
    
    cwd = os.getcwd()
    output_path = cwd
    
    project_reader = ProjectReader()
    project_parser = ProjectParser()
    project_exporter = ProjectExporter()
    
    # create the project data structure
    project = Project()
    
    # load data into project
    project_reader.read_project(project, input_file)
    project.display()
    
    # unroll patterns
    project_parser.parse_project(project)
    
    # export project text into midi file
    project_exporter.export_project(project, output_path)

if __name__ == "__main__":
    main()

