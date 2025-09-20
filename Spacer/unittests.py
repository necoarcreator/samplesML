from numpy import isclose
from main import DPSpacer, GreedySpacer

def test_heuristic():
    spacer = GreedySpacer(word_list=["привет", "мир"])
    assert spacer.max_match_segment("приветмир") == "привет мир"
    assert spacer.max_match_segment("приветxxxмир") == "привет xxx мир"
    spacer2 = GreedySpacer(word_list=['это', 'пример', 'текста', 'без', 'пробелов', 'который',
             'нужно', 'разделить', 'на', 'слова'])
    assert spacer2.fit_predict(["этопримертекстабезпробелов"]) == ['это пример текста без пробелов']
    assert spacer2.fit_predict(["100кмэтопримерчисла2.5мтекста"]) == ['100 км это пример числа 2,5 м текста']
    assert spacer2.fit_predict(["100.0", "100,0"]) == ['100,0', '100,0']
    assert spacer2.fit_predict(["привет,какдела?этотест!"]) == ['привет,какдела? это тест!']
    
    spacer_pro = GreedySpacer()
    assert spacer_pro.fit_predict(["привет,какдела?этотест!"]) == ['привет, как дела? это тест!'] or ['привет, как дела? этот ест!']

    model_dataset = [
        'это пример текста без пробелов', '100 км это пример числа 2,5 м текста',
        '100,0', 'привет,какдела? это тест!']
    
    ideal_dataset = [
        'это пример текста без пробелов', '100 км это пример числа 2,5 м текста',
        '100,0', 'привет, как дела? это тест!'
    ]
    assert isclose(spacer2.eval_f1(model_dataset, ideal_dataset), 0.78, atol=1e-2)

def test_dp():

    spacer_pro = DPSpacer()
    assert spacer_pro.fit_predict(["привет,какдела?этотест!"]) == ['привет, как дела? это тест!'] or ['привет, как дела? этот ест!']

    model_dataset = [
        'это пример текст абезпробелов', '100 кмэто пример числа2,5 мтекста',
        '100,0', 'при вет,какдела?этотест!']
    
    ideal_dataset = [
        'это пример текста без пробелов', '100 км это пример числа 2,5 м текста',
        '100,0', 'привет, как дела? это тест!'
    ]
    assert isclose(spacer_pro.eval_f1(model_dataset, ideal_dataset), 0.26, atol=1e-2)