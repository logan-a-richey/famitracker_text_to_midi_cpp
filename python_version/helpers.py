# helpers.py

from typing import Tuple

def get_quote(input_string) -> str:
    ''' get text between first quote and last quote in input_string '''
    return input_string[input_string.find("\"") + 1: input_string.rfind("\"") ]

def generate_token(pattern: int, col: int, row: int) -> Tuple[int, int, int]:
    ''' generate a hashable key '''
    return tuple([pattern, col, row])

def contains_only_periods_and_spaces(input_string: str) -> bool:
    ''' for skipping null tokens '''
    for character in input_string:
        if (character != '.' and character != ' '):
            return False
    return True
