# project_parser.py

class ProjectParser:
    def __init__(self):
        pass
    
    def parse_track(self, project, track):
        
    def parse_project(self, project) -> None:
        print(len(project.tracks))
        for ti, track in enumerate(project.tracks):
            print("Parsing track {} : {}".format(ti, track.name));
            self.parse_track(project, track)
