import os
import sys
import warnings
warnings.filterwarnings("ignore", category=DeprecationWarning)
import json

def sum():
    num1 = ""
    num2 = ""
    json_data = os.environ["REQUEST_BODY"]
    json_load = (json.loads(json_data))
    for x in json_load:
        if x == num1:
            if json_load[x] != "":
                num1 = json_load[x]
            else:
                return "Erro: primeiro numero nao preenchido"
        else if x == num2:
            if json_load[x] != "":
                num2 = json_load[x]
            else:
                return "Erro: segundo numero nao preenchido"
    try:
        result = float(num1) + float(num2)
        return result
    except ValueError:
        return "Erro: Entradas inválidas"

print(str(sum_numbers(num1, num2))+"")
