import pandas as pd
from pathlib import Path

def load_dataset() -> pd.DataFrame:
    """Загружает и подготавливает датасет к работе"""
    dataset_path = Path.cwd() / 'dataset' / 'dataset.txt'
    dataset_path.resolve()
    with open(dataset_path, "r", encoding='utf-8') as f:
        df = f.read()

    # делим по \n строку, создаём колонки по первой встреченной запятой
    df_mapped = list(map(lambda x: x.split(',', maxsplit=1), df.split('\n')))
    
    df_dirty = pd.DataFrame(df_mapped[1:],columns=df_mapped[0])
    mask = (
        df_dirty['text_no_spaces'].notna() &           
        (df_dirty['text_no_spaces'] != '') &           
        (df_dirty['text_no_spaces'] != 'None') &       
        (df_dirty['text_no_spaces'].str.strip() != '') 
    )
    
    df_clean = df_dirty[mask].copy()
    
    return df_clean