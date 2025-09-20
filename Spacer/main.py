import pandas as pd, numpy as np
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
                refined = greedy_segmenter.segment(word).split()
                refined_filtered = []

                # Проверяем батчем
                refined_is_good = bert_validator.is_good_word_batch(refined)

                for w, w_is_good in zip(refined, refined_is_good):
                    if w_is_good:
                        refined_filtered.append(w)
                    else:
                        # Fallback: жадная сегментация SentencePiece
                        greedy_result = sp_segmenter.fit_predict(w).split()
                        refined_filtered.extend(greedy_result)

                if len(refined_filtered) > 1:
                    final_words.extend(refined_filtered)
                else:
                    final_words.append(word)

        results.append(" ".join(final_words))
    return results

def run_pipe():
    ds = load_dataset()
    dp = DPSpacer()
    greedy = GreedySpacer()
    bert = WordValidator()
    sentencepiece = SentencePieceTokenizer()

    return refine_segmentation_batch(bert, dp, greedy, sentencepiece, ds['text_no_spaces'])
