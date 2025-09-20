import pandas as pd, numpy as np, re
from tqdm import tqdm
from ds_loader import load_dataset
from models import GreedySpacer, DPSpacer, WordValidator, SentencePieceTokenizer

def refine_segmentation_batch(bert_validator, dp_segmenter, 
                              greedy_segmenter, sp_segmenter, texts: list[str]) -> list[str]:
    results = []
    for text in tqdm(texts, desc='Processing texts'):
        
        # Первичная сегментация ДП
        words = dp_segmenter.segment(text).split()
        final_words = []

        # Проверяем батчем
        is_good_list = bert_validator.is_good_word_batch(words)

        for word, is_good in zip(words, is_good_list):
            if is_good:
                final_words.append(word)
            else:
                # Повторная сегментация жадным алгоритмом
                refined = greedy_segmenter.fit_predict(word)
                refined_filtered = []

                # Проверяем батчем
                refined_is_good = bert_validator.is_good_word_batch(refined)

                for w, w_is_good in zip(refined, refined_is_good):
                    if w_is_good:
                        refined_filtered.append(w)
                    else:
                        # Fallback: жадная сегментация SentencePiece
                        greedy_result = sp_segmenter.fit_predict(w)
                        refined_filtered.extend(greedy_result)

                if len(refined_filtered) > 1:
                    final_words.extend(refined_filtered)
                else:
                    final_words.append(word)

        results.append(" ".join(final_words))
    return results

def run_pipe(ds : pd.DataFrame = None) -> list[str]:
    ds = load_dataset()
    dp = DPSpacer()
    greedy = GreedySpacer()
    bert = WordValidator()
    sentencepiece = SentencePieceTokenizer()

    return refine_segmentation_batch(bert, dp, greedy, sentencepiece, ds['text_no_spaces'])

def transform_data() -> pd.DataFrame:
    ds = load_dataset()
    results = run_pipe(ds)
    if isinstance(ds, dict):
        df_result = pd.DataFrame(ds)
    else:
        df_result = ds.to_pandas() if hasattr(ds, 'to_pandas') else pd.DataFrame(ds)

    predicted_positions_list = []

    for i, row in tqdm(df_result.iterrows(), total=len(df_result), desc='Transforming data'):
        predicted_positions = [match.start() for match in re.finditer(" ", results[i])]
        predicted_positions_list.append(predicted_positions)

    df_result['predicted_positions'] = predicted_positions_list
    return df_result