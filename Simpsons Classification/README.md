# Классификация персонажей Симпсонов

Датасет взят с курса МФТИ по глубокому обучению (а они, в свою очередь, взяли его отсюда:
https://www.kaggle.com/datasets/alexattia/the-simpsons-characters-dataset).

Для того, чтобы датасет был более сбалансированным, часть изображений аугментировали с
torchvision.transforms.
Была локально установлена сеть ResNet152. Все веса на слоях N <= 7 заморозили, добавили FC-слой на 42 выхода 
в конце (по числу меток персонажей), обучали только их. 
Метрика - accuracy.

## Требования

- Python 3.8+
- Установленные зависимости (указаны в requirements.txt)
- CUDA-совместимая видеокарта
  
## Установка

Как установить зависимости:
- Для Python: `pip install -r requirements.txt`
  
## Запуск

Как запустить:
- Python: `jupyter notebook`

## Пример использования
```python
TRAIN_DIR = Path(my_path_to_train)
TEST_DIR = Path(my_path_to_test)

train_val_files = sorted(list(TRAIN_DIR.rglob('*.jpg')))
test_files = sorted(list(TEST_DIR.rglob('*.jpg')))
...
```
