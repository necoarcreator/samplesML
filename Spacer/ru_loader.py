from pathlib import Path
from pandas import read_csv
from typing import DefaultDict

def load_dictionary(encoding : str = 'cp1251') -> list[str]:
    """
    Читает файл обычного словаря с указанием кодировки
    Большое спасибо Danakt Saushkin  (https://github.com/danakt) 
    за словарь в открытом доступе. 
    """
    dict_dir = Path.cwd() / "dictionary" / 'russian.txt'
 
    with open(dict_dir, 'r', encoding=encoding) as f:
        dictionary = [line.strip() for line in f if line.strip()]
    return dictionary

def load_frequency_dictionary(encoding : str = 'utf-8', top_n : int = -1) -> dict:
    """
    Читает файл частотного словаря с указанием кодировки.
    Большое спасибо Russian National Corpus (https://ruscorpora.ru)
    за словарь в открытом доступе.
    """
    dict_dir = Path.cwd() / "dictionary" / 'ruscorpora_content.csv'
    
    dictionary = DefaultDict(int)
    source = read_csv(dict_dir, delimiter=';', encoding=encoding)
    
    for _, row in source.iterrows():
        word = str(row['word_lex_0']).strip()
        freq = int(row['hits'])
        if word:
            dictionary[word] = freq
    dictionary = sorted(dictionary.items(), key = lambda x: x[1], reverse=True)
    return dict(dictionary[:top_n])