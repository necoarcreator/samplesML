import numpy as np, re
from collections import defaultdict
import torch
import torch.nn as nn
from transformers import BertTokenizer, BertPreTrainedModel, BertModel
from dotenv import load_dotenv

load_dotenv()
MODEL_DIR = 'CrabInHoney/morphbert-tiny-v2-morpheme-segmentation-ru'
MAX_LEN = 32
DEVICE = torch.device("cuda" if torch.cuda.is_available() else "cpu")
ID2TAG = {0: 'END', 1: 'HYPH', 2: 'LINK', 3: 'POSTFIX', 4: 'PREF', 5: 'ROOT', 6: 'SUFF'}
NUM_MORPH_CLASSES = len(ID2TAG)

class GreedySpacer:
    def __init__(self, word_list : list[str] = None, encoding : str = 'cp1251', 
                 language : str = 'russian', max_numbers : int = 1000):
        """
        Инициализация эвристического алгоритма.
        word_list : список известных слов,
        language : язык для базовых правил
        """
        if language != 'russian':
            raise NotImplementedError("The only language currently supported is \'russian\'.")
        
        self.word_freq = defaultdict(int)
        self.max_word_len = 0
        self.language = language
        
        if word_list is None:
            from ru_loader import load_dictionary
            word_list = [word for word in load_dictionary(encoding) if len(word) > 3]

            
        for word in word_list:
            self.word_freq[word] += 1
            self.max_word_len = max(self.max_word_len, len(word))
        
        # нужно будет для экранирования чисел далее
        for i in range(max_numbers):
            placeholder = f"¤{i}¤"
            self.word_freq[placeholder] = 1
            self.max_word_len = max(self.max_word_len, len(placeholder))

    def max_match_segment(self, text_to_parse : str) -> str:
        """
        Жадная сегментация текста с использованием словаря
        """
        text_to_parse = text_to_parse.lower()
        result = []
        i = 0
        n = len(text_to_parse)
        while i < n:
            # ищем следующее слово длиной не более max_word_len и до конца строки
            found = False
            for j in range(min(self.max_word_len, n - i), 0, -1):
                candidate = text_to_parse[i: i + j]
                if candidate in self.word_freq:
                    result.append(candidate)
                    i += j
                    found = True
                    break
            if not found:
                start = i
                # идём вперёд, пока не встретим начало известного слова
                while i < n:
                    found_inner = False
                    for j in range(min(self.max_word_len, n - i), 0, -1):
                        candidate = text_to_parse[i:i + j]
                        if candidate in self.word_freq:
                            found_inner = True
                            break
                    if found_inner:
                        break
                    i += 1
                # добавляем неизвестные слова, не разделяя на пробелы
                unknown_chunk = text_to_parse[start:i]
                if unknown_chunk:
                    result.append(unknown_chunk)

        return " ".join(result)
    
    def process_numbers(self, text_to_parse : str) -> str:
        """
        Обработка чисел и единиц измерения
        """
        patterns = [
            (r'(\d+)([a-zA-Zа-яА-Я]{1, 3})(?=[а-яА-Яa-zA-Z]|$)', r'\1 \2'), # 14айфонов -> 14 айфонов
            (r'(\d+)(%|с|мин|ч|м|км|см|мм|кг|г|мг|°C|°F|л|м²|м³|Вт|кВт|л/мин)(?=[а-яА-Яa-zA-Z]|$)', r'\1 \2'), # 23°C -> 23 °C
            (r'(\d+)[\.,](\d+)', r'\1,\2') #1.2 -> 1,2 по правилам русского
        ]

        for pattern, replacement in patterns:
            text_to_parse = re.sub(pattern, replacement, text_to_parse)

        return text_to_parse
    
    def process_punctuation(self, text_to_parse : str) -> str:
        """
        Обработка знаков препинания
        """
        punctuation_patterns = [
            (r'(\s|^)([.,!?;:])(?=\w)', r'\1\2 '), # .тестовое -> . тестовое
            (r'(\w)(-)(\w)', r'\1 \2 \3'), # анаграмма-это -> анаграмма - это

        ]

        for pattern, replacement in punctuation_patterns:
            text_to_parse = re.sub(pattern, replacement, text_to_parse)
        
        return text_to_parse
    
    def fit_predict(self, dataset_to_parse: list[str]) -> list[str]:
        """
        Предобрабатывает датасет перед применением более сложной модели.
        """
        result = []
        for text in dataset_to_parse:
            text = self.process_numbers(text)
            text = self.process_punctuation(text)

            float_pattern = r'\d+,\d+'
            floats = re.findall(float_pattern, text)
            placeholder_map = {}
            for i, flt in enumerate(floats):
                placeholder = f"¤{i}¤"
                text = text.replace(flt, placeholder, 1)
                placeholder_map[placeholder] = flt

            text = self.max_match_segment(text)
            
            for placeholder, original in placeholder_map.items():
                text = text.replace(placeholder, original)

            text = re.sub(r'\s+([.,!?:;])\s*', r'\1 ', text)
            text = re.sub(r'\s+', ' ', text).strip()

            result.append(text)

        return result
    
    def eval_f1(self, dataset_to_parse : list[str] = None, dataset_ideal : list[str] = None) -> float:
        """
        Вычисляет f1 по тестовому датасету.
        """
        total_tp = 0
        total_fp = 0
        total_fn = 0

        for model_output, ideal in zip(dataset_to_parse, dataset_ideal):
            # получаем позиции пробелов (индексы, где стоят пробелы)
            pred_spaces = {i for i, char in enumerate(model_output) if char == ' '}
            true_spaces = {i for i, char in enumerate(ideal) if char == ' '}

            tp = len(pred_spaces & true_spaces)
            fp = len(pred_spaces - true_spaces)
            fn = len(true_spaces - pred_spaces)

            total_tp += tp
            total_fp += fp
            total_fn += fn

        # общая precision и recall по всем примерам
        precision = total_tp / (total_tp + total_fp) if (total_tp + total_fp) > 0 else 1.0
        recall = total_tp / (total_tp + total_fn) if (total_tp + total_fn) > 0 else 1.0

        if precision + recall == 0:
            return 0.0
        else:
            return 2 * (precision * recall) / (precision + recall)
        

class DPSpacer:
    def __init__(self, word_freq: dict = None, max_word_len : int = 20, 
                 min_score : float = -15, break_penalty : float = 0.5):
        self.word_freq = word_freq
        self.max_word_len = max_word_len
        self.min_score = min_score
        self.break_penalty = break_penalty

        if word_freq is None:
            from ru_loader import load_frequency_dictionary
            self.word_freq = load_frequency_dictionary()
        self.word_freq = {word: np.log(freq + 1) for word, freq in self.word_freq.items() if len(word) > 2}

    def segment(self, text: str) -> str:
        """
        Сегментация текста с использованием динамического программирования
        """

        text = text.lower()
        n = len(text)
        dp = [-np.inf] * (n + 1)
        dp[0] = 0
        parent = [-1] * (n + 1)
        word_at = [""] * (n + 1)

        for i in range(n):
            if dp[i] == -np.inf:
                continue
            # пробуем все слова, начинающиеся в i + 1
            for j in range(1, min(self.max_word_len, n - i) + 1):
                candidate = text[i:i + j]
                raw_score = self.word_freq.get(candidate, -np.inf)

                if raw_score == -np.inf:
                    score = -10  # штраф за неизвестное слово
                else:
                    score = raw_score

                # Штраф/бонусы
                if len(candidate) <= 5:
                    score -= 2.5 * (5 - len(candidate))
                if len(candidate) >= 6:
                    score += 2

                # Порог отсечения
                if score < self.min_score:
                    continue
                # Обновление
                new_score = dp[i] + score - self.break_penalty
                end_pos = i + j
                # print(f"Candidate: {candidate}, score: {score:.2f}, new_score: {new_score:.2f}, dp[i]: {dp[i]:.2f}")
                if new_score > dp[end_pos]:
                    dp[end_pos] = new_score
                    parent[end_pos] = i
                    word_at[end_pos] = candidate

        # Восстановление пути
        result = []
        i = n
        while i > 0 and parent[i] != -1:
            word = word_at[i]
            if not word:
                break
            result.append(word)
            next_i = parent[i]
            if next_i >= i:
                break
            i = next_i

        return " ".join(reversed(result))
    
    def process_numbers(self, text_to_parse : str) -> str:
        """
        Обработка чисел и единиц измерения
        """
        patterns = [
            (r'(\d+)([a-zA-Zа-яА-Я]{1, 3})(?=[а-яА-Яa-zA-Z]|$)', r'\1 \2'), # 14айфонов -> 14 айфонов
            (r'(\d+)(%|с|мин|ч|м|км|см|мм|кг|г|мг|°C|°F|л|м²|м³|Вт|кВт|л/мин)(?=[а-яА-Яa-zA-Z]|$)', r'\1 \2'), # 23°C -> 23 °C
            (r'(\d+)[\.,](\d+)', r'\1,\2') #1.2 -> 1,2 по правилам русского
        ]

        for pattern, replacement in patterns:
            text_to_parse = re.sub(pattern, replacement, text_to_parse)

        return text_to_parse
    
    def process_punctuation(self, text_to_parse : str) -> str:
        """
        Обработка знаков препинания
        """
        punctuation_patterns = [
            (r'(\s|^)([.,!?;:])(?=\w)', r'\1\2 '), # .тестовое -> . тестовое
            (r'(\w)(-)(\w)', r'\1 \2 \3'), # анаграмма-это -> анаграмма - это

        ]

        for pattern, replacement in punctuation_patterns:
            text_to_parse = re.sub(pattern, replacement, text_to_parse)
        
        return text_to_parse
    
    def fit_predict(self, dataset_to_parse: list[str]) -> list[str]:
        """
        Предобрабатывает датасет перед применением более сложной модели.
        """
        result = []
        for text in dataset_to_parse:
            if text is None or text.strip() == "":
                result.append("")
                continue
            
            text = self.process_numbers(text)
            text = self.process_punctuation(text)

            float_pattern = r'\d+,\d+'
            floats = re.findall(float_pattern, text)
            placeholder_map = {}
            for i, flt in enumerate(floats):
                placeholder = f"¤{i}¤"
                text = text.replace(flt, placeholder, 1)
                placeholder_map[placeholder] = flt

            text = self.segment(text)
            
            for placeholder, original in placeholder_map.items():
                text = text.replace(placeholder, original)

            text = re.sub(r'\s+([.,!?:;])\s*', r'\1 ', text)
            text = re.sub(r'\s+', ' ', text).strip()

            result.append(text)

        return result
    
    def eval_f1(self, dataset_to_parse : list[str] = None, dataset_ideal : list[str] = None) -> float:
        """
        Вычисляет f1 по тестовому датасету.
        """
        total_tp = 0
        total_fp = 0
        total_fn = 0

        for model_output, ideal in zip(dataset_to_parse, dataset_ideal):
            # получаем позиции пробелов (индексы, где стоят пробелы)
            pred_spaces = {i for i, char in enumerate(model_output) if char == ' '}
            true_spaces = {i for i, char in enumerate(ideal) if char == ' '}

            tp = len(pred_spaces & true_spaces)
            fp = len(pred_spaces - true_spaces)
            fn = len(true_spaces - pred_spaces)

            total_tp += tp
            total_fp += fp
            total_fn += fn

        # общая precision и recall по всем примерам
        precision = total_tp / (total_tp + total_fp) if (total_tp + total_fp) > 0 else 1.0
        recall = total_tp / (total_tp + total_fn) if (total_tp + total_fn) > 0 else 1.0

        if precision + recall == 0:
            return 0.0
        else:
            return 2 * (precision * recall) / (precision + recall)
        
class BertForMultiTask(BertPreTrainedModel):
    def __init__(self, config, num_seg_labels=2, num_morph_labels=NUM_MORPH_CLASSES):
        super().__init__(config)
        self.bert = BertModel(config)
        self.dropout = nn.Dropout(config.hidden_dropout_prob)
        self.seg_head = nn.Linear(config.hidden_size, num_seg_labels)
        self.cls_head = nn.Linear(config.hidden_size, num_morph_labels)
    def forward(self, input_ids, attention_mask=None):
        x = self.dropout(self.bert(input_ids, attention_mask=attention_mask).last_hidden_state)
        return {"seg_logits": self.seg_head(x), "cls_logits": self.cls_head(x)}

class SentencePieceTokenizer(nn.Module):
    def __init__(self):
        from transformers import AutoTokenizer
        self.tokenizer = AutoTokenizer.from_pretrained("cointegrated/rubert-tiny2")
        
    def fit_prefict(self, text : list[str]) -> list[str]:
        tokens = self.tokenizer.tokenize(text)
        for i, token in enumerate(tokens):
            if token.startswith("##"):
                tokens[i] = token[2:]
        return tokens


class WordValidator:
    def __init__(self, model_dir=MODEL_DIR, device=DEVICE, max_len=MAX_LEN):
        self.tokenizer = BertTokenizer.from_pretrained(model_dir)
        self.model = BertForMultiTask.from_pretrained(model_dir, num_morph_labels=NUM_MORPH_CLASSES).to(device).eval()
        self.device = device
        self.max_len = max_len
        self.cache = {}  # кеширование результатов

    def is_good_word_batch(self, words: list[str]) -> list[bool]:
        # Фильтруем кеш
        uncached = [w for w in words if w not in self.cache]
        if uncached:
            self._predict_batch(uncached)
        return [self.cache[w] for w in words]

    def _predict_batch(self, words: list[str]):
        inputs = []
        for word in words:
            chars = list(word.lower())
            tokenized = self.tokenizer(
                " ".join(chars),
                return_tensors='pt',
                max_length=self.max_len,
                padding='max_length',
                truncation=True,
                add_special_tokens=True
            )
            inputs.append({
                'input_ids': tokenized['input_ids'].squeeze(0),
                'attention_mask': tokenized['attention_mask'].squeeze(0)
            })

        # Собираем батч
        batch = {
            'input_ids': torch.stack([x['input_ids'] for x in inputs]).to(self.device),
            'attention_mask': torch.stack([x['attention_mask'] for x in inputs]).to(self.device)
        }

        with torch.no_grad():
            out = self.model(**batch)

        results = []
        for i, word in enumerate(words):
            n = min(len(word), self.max_len - 2)
            if n <= 0:
                results.append(False)
                continue

            seg = torch.argmax(out['seg_logits'][i, 1:1 + n], -1).tolist()
            cls = torch.argmax(out['cls_logits'][i, 1:1 + n], -1).tolist()
            tags = [ID2TAG.get(c, f'ID:{c}') for c in cls]

            num_segments = 0
            root_spans = 0
            in_root = False
            last_tag = ''
            for tag in tags:
                if last_tag != tag:
                    num_segments += 1
                if tag == 'ROOT':
                    if not in_root:
                        root_spans += 1
                        in_root = True
                else:
                    in_root = False
                last_tag = tag

            # Эвристики
            if root_spans >= 2:
                results.append(False)
            elif len(word) <= 2 and word in ('а', 'и', 'у', 'на', 'за', 'по', 'от', 'до', 'не', 'ни', 'да', 'во', 'ко'):
                results.append(True)
            elif len(word) < 5 and num_segments > 1:
                results.append(True)
            elif num_segments >= 2 and any(t in ('HYPH', 'LINK', 'POSTFIX') for t in tags):
                results.append(False)
            elif num_segments >= 2 and any(t in ('PREF', 'SUFF', 'END') for t in tags):
                results.append(True)
            else:
                results.append(False)

        # Сохраняем в кеш
        for w, res in zip(words, results):
            self.cache[w] = res