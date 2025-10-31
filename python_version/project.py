# project.py

class Project:
    def __init__(self):
        self.title: str = ""
        self.author: str = ""
        self.copyright: str = ""
        
        self.comment: List[str] = []
        
        self.machine: int = 0
        self.framerate: int = 0
        self.expansion: int = 0
        self.vibrato: int = 0
        self.split: int = 0
        self.n163channels: int = 0
        
        self.tracks: List["Track"] = []

    def display(self):
        print("--- Song Information ---")
        for a in ["title", "author", "copyright"]:
            print("self.{} => \'{}\'".format(a, getattr(self, a)))
        if self.comment:
            print("\n--- Song Comment ---")
            print("\"\"\"")
            for line in self.comment:
                print(line)
            print("\"\"\"")
        
        print("\n--- Global Settings ---")
        for a in ["machine", "framerate", "expansion", "vibrato", "split", "n163channels"]:
            print("self.{} => {}".format(a, getattr(self, a)))
        
        print("\n--- Tracks ---")
        for idx, track in enumerate(self.tracks):
            print("track {}: \'{}\', {} tokens".format(
                idx, 
                track.name, 
                len(track.tokens)
            ))



