# track.py

class Track:
    def __init__(self, num_rows_, speed_, tempo_, name_):
        self.name: str = name_
        self.num_rows: int = num_rows_
        self.speed: int = speed_
        self.tempo: int = tempo_
        
        self.num_cols: int = 5
        self.eff_cols: List[int] = []
        
        self.orders: Dict[str, List[int]] = {}
        self.tokens: Dict[tuple, str] = {}
