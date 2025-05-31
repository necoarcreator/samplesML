# RAG для системы "вопрос-ответ"

Использовались воинские уставы ВС РФ в качесвте исходных документов.
Для разделения документов на батчи (которое, в свою очередь, нужно для 
сохранения контекста внутри одного батча) использовался RecursiveCharacterTextSplitter
из langchain.text_splitter.
В качестве ретривера взят https://huggingface.co/ai-forever/sbert_large_nlu_r (неплохо
работает с русскоязычными документами).
Векторная БД - FAISS (у неё высокая производительность, она open source).
Генератор - Qwen 2.5 через API Gradio Client (современная модель с бесплатным API).
Метрика - Cosine Sim запроса и ответов (ранее - ручная оценка).

## Требования

- Python 3.8+
- Установленные зависимости (указаны в requirements.txt)
- CUDA-совместимая видеокарта
  
## Установка

Как установить зависимости:
- Для Python: `pip install -r requirements.txt`
- Распаковать данные из .zip файла ("сырые" файлы .txt / данные после
  применения регулярных выражений .jsonl / эмбеддинги .faiss)
## Запуск

Как запустить:
- Python: `jupyter notebook`

## Пример использования
```python
documents = []
text = ""
for i in range(1, my_docs_len):
    with open(my_docs_names, "r", encoding="utf-8") as doc:
        text = doc.read()
        documents.append({"tome" : i, "text" : text})
...
query_len = my_query_len

queries = [""] * query_len
for j in range(my_query_len):
    queries[j] = input()
...
```
