from itertools import product

numbers = [65, 20, 4, 236, 9, 61, 7, 508]
target = 189612
operators = ['+', '*']

for ops in product(operators, repeat=len(numbers)-1):
    expr = str(numbers[0])
    for num, op in zip(numbers[1:], ops):
        expr += f' {op} {num}'
    if eval(expr) == target:
        print(f"Valid: {expr} = {target}")
