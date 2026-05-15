import csv
import random
import os
from typing import List, Tuple

class TestTableGenerator:
    def __init__(self, num_rows: int = 100, num_cols: int = 5):
        """
        Инициализация генератора тестовых таблиц.
        
        Args:
            num_rows: количество строк для генерации
            num_cols: количество столбцов (без учета столбца с нумерацией)
        """
        self.num_rows = num_rows
        self.num_cols = num_cols
        self.column_names = self._generate_column_names()
        self.data = {}
        
    def _generate_column_names(self) -> List[str]:
        """Генерация названий столбцов (A, B, C, ... до ZZ)"""
        names = []
        for i in range(self.num_cols):
            if i < 26:
                names.append(chr(ord('A') + i))
            else:
                # Для столбцов больше 26 используем AA, AB, и т.д.
                first = chr(ord('A') + (i // 26) - 1)
                second = chr(ord('A') + (i % 26))
                names.append(f"{first}{second}")
        return names
    
    def _get_random_positive_number(self, max_value: int = 1000) -> int:
        """Генерация случайного положительного целого числа"""
        return random.randint(1, max_value)
    
    def _get_random_operation(self) -> str:
        """Выбор случайной операции"""
        return random.choice(['+', '-', '*', '/'])
    
    def _get_random_cell_reference(self, current_row: int, current_col_idx: int) -> Tuple[str, int]:
        """
        Получение случайной ссылки на существующую ячейку.
        Может ссылаться на любую существующую ячейку, кроме текущей.
        """
        available_refs = []
        
        # Собираем все существующие ячейки
        for row in self.data:
            for col_idx, col_name in enumerate(self.column_names):
                # Исключаем текущую ячейку
                if row == current_row and col_idx == current_col_idx:
                    continue
                if self.data[row][col_idx] is not None:
                    available_refs.append((col_name, row))
        
        if available_refs:
            return random.choice(available_refs)
        return None, None
    
    def _generate_formula(self, current_row: int, current_col_idx: int) -> str:
        """
        Генерация формулы вида =ARG1 OP ARG2 (без пробелов)
        Все числа в формуле положительные
        """
        # С вероятностью 50% используем числа, иначе ссылки на ячейки
        use_numbers = random.choice([True, False])
        
        if use_numbers:
            arg1 = str(self._get_random_positive_number())
            arg2 = str(self._get_random_positive_number())
        else:
            # Получаем ссылки на существующие ячейки
            ref1_col, ref1_row = self._get_random_cell_reference(current_row, current_col_idx)
            ref2_col, ref2_row = self._get_random_cell_reference(current_row, current_col_idx)
            
            if ref1_col is None or ref2_col is None:
                # Если нет доступных ссылок, используем числа
                arg1 = str(self._get_random_positive_number())
                arg2 = str(self._get_random_positive_number())
            else:
                arg1 = f"{ref1_col}{ref1_row}"
                arg2 = f"{ref2_col}{ref2_row}"
        
        operation = self._get_random_operation()
        
        # Избегаем деления на ноль
        if operation == '/':
            if (use_numbers and int(arg2) == 0) or (not use_numbers and arg2.isdigit() and int(arg2) == 0):
                operation = '+'
        
        # Формула без пробелов
        return f"={arg1}{operation}{arg2}"
    
    def generate_table(self, formula_probability: float = 0.3):
        """
        Генерация тестовой таблицы.
        
        Args:
            formula_probability: вероятность создания формулы в ячейке (0-1)
        """
        # Генерируем случайные положительные номера строк
        row_numbers = random.sample(range(1, self.num_rows * 3 + 1), self.num_rows)
        row_numbers.sort()
        
        # Инициализируем структуру данных
        self.data = {}
        
        for row_num in row_numbers:
            self.data[row_num] = [None] * self.num_cols
            
            for col_idx in range(self.num_cols):
                if random.random() < formula_probability:
                    formula = self._generate_formula(row_num, col_idx)
                    self.data[row_num][col_idx] = formula
                else:
                    self.data[row_num][col_idx] = str(self._get_random_positive_number())
    
    def save_to_csv(self, filename: str):
        """Сохранение таблицы в CSV файл с Unix line endings (\\n)"""
        with open(filename, 'w', newline='', encoding='utf-8') as csvfile:
            # Используем lineterminator='\n' для Unix line endings
            writer = csv.writer(csvfile, lineterminator='\n')
            header = [''] + self.column_names
            writer.writerow(header)
            
            for row_num in sorted(self.data.keys()):
                row_data = [str(row_num)] + self.data[row_num]
                writer.writerow(row_data)
        
        print(f"CSV файл успешно создан: {filename} (строк: {len(self.data)}, столбцов: {self.num_cols})")
    
    def save_to_csv_with_cell(self, filename: str):
        """Сохранение таблицы в CSV файл со столбцом 'Cell' с Unix line endings (\\n)"""
        cols_with_cell = self.column_names + ['Cell']
        
        with open(filename, 'w', newline='', encoding='utf-8') as csvfile:
            # Используем lineterminator='\n' для Unix line endings
            writer = csv.writer(csvfile, lineterminator='\n')
            header = [''] + cols_with_cell
            writer.writerow(header)
            
            for row_num in sorted(self.data.keys()):
                cell_value = str(self._get_random_positive_number())
                row_data = [str(row_num)] + self.data[row_num] + [cell_value]
                writer.writerow(row_data)
        
        print(f"CSV файл успешно создан: {filename} (строк: {len(self.data)}, столбцов: {self.num_cols + 1})")

def generate_various_table_sizes():
    """
    Генерация таблиц разного размера для тестирования
    """
    os.makedirs('test_csv_files/various_sizes', exist_ok=True)
    
    # Конфигурации таблиц: (строки, столбцы, вероятность формул, название)
    table_configs = [
        # Маленькие таблицы
        (5, 2, 0.2, "tiny_2cols"),
        (5, 3, 0.3, "tiny_3cols"),
        (5, 5, 0.4, "tiny_5cols"),
        
        # Средние таблицы
        (20, 3, 0.2, "small_3cols"),
        (20, 5, 0.3, "small_5cols"),
        (20, 10, 0.3, "small_10cols"),
        (20, 15, 0.4, "small_15cols"),
        
        # Большие таблицы
        (50, 5, 0.15, "medium_5cols"),
        (50, 10, 0.2, "medium_10cols"),
        (50, 20, 0.25, "medium_20cols"),
        (50, 30, 0.3, "medium_30cols"),
        
        # Очень большие таблицы
        (100, 10, 0.1, "large_10cols"),
        (100, 20, 0.15, "large_20cols"),
        (100, 30, 0.2, "large_30cols"),
        (100, 50, 0.25, "large_50cols"),
        
        # Экстремальные таблицы (для стресс-тестирования)
        (200, 5, 0.1, "xlarge_5cols"),
        (200, 10, 0.15, "xlarge_10cols"),
        (200, 20, 0.2, "xlarge_20cols"),
    ]
    
    print("Генерация таблиц разного размера...")
    print("="*60)
    
    for rows, cols, prob, name in table_configs:
        print(f"\nСоздание таблицы: {name} ({rows} строк x {cols} столбцов)")
        generator = TestTableGenerator(num_rows=rows, num_cols=cols)
        generator.generate_table(formula_probability=prob)
        generator.save_to_csv(f'test_csv_files/various_sizes/table_{name}.csv')

def generate_tables_with_cell_column():
    """
    Генерация таблиц разного размера с дополнительным столбцом Cell
    """
    os.makedirs('test_csv_files/with_cell_column', exist_ok=True)
    
    configs = [
        (10, 3, 0.3, "small_with_cell"),
        (20, 5, 0.4, "medium_with_cell"),
        (30, 10, 0.35, "large_with_cell"),
        (50, 8, 0.3, "xlarge_with_cell"),
    ]
    
    print("\n" + "="*60)
    print("Генерация таблиц со столбцом Cell...")
    print("="*60)
    
    for rows, cols, prob, name in configs:
        print(f"\nСоздание таблицы: {name} ({rows} строк x {cols} столбцов + Cell)")
        generator = TestTableGenerator(num_rows=rows, num_cols=cols)
        generator.generate_table(formula_probability=prob)
        generator.save_to_csv_with_cell(f'test_csv_files/with_cell_column/table_{name}.csv')

def generate_tables_specific_formula_density():
    """
    Генерация таблиц с разной плотностью формул
    """
    os.makedirs('test_csv_files/formula_density', exist_ok=True)
    
    # Фиксированный размер, разная вероятность формул
    rows, cols = 30, 10
    
    densities = [
        (0.0, "no_formulas"),      # Только числа
        (0.1, "low_formulas"),     # 10% формул
        (0.3, "medium_formulas"),  # 30% формул
        (0.5, "high_formulas"),    # 50% формул
        (0.7, "very_high_formulas"), # 70% формул
        (0.9, "extreme_formulas"), # 90% формул
        (1.0, "all_formulas"),     # Только формулы
    ]
    
    print("\n" + "="*60)
    print("Генерация таблиц с разной плотностью формул...")
    print(f"Размер таблицы: {rows} строк x {cols} столбцов")
    print("="*60)
    
    for prob, name in densities:
        print(f"\nСоздание таблицы: {name} (вероятность формул: {prob*100}%)")
        generator = TestTableGenerator(num_rows=rows, num_cols=cols)
        generator.generate_table(formula_probability=prob)
        generator.save_to_csv(f'test_csv_files/formula_density/table_{name}.csv')

def generate_incremental_column_tables():
    """
    Генерация таблиц с увеличивающимся количеством столбцов
    """
    os.makedirs('test_csv_files/incremental_columns', exist_ok=True)
    
    rows = 20
    formula_prob = 0.3
    
    # Увеличиваем количество столбцов
    column_counts = [2, 5, 10, 15, 20, 26, 30, 40, 50]
    
    print("\n" + "="*60)
    print("Генерация таблиц с увеличивающимся количеством столбцов...")
    print(f"Количество строк: {rows}")
    print(f"Вероятность формул: {formula_prob*100}%")
    print("="*60)
    
    for cols in column_counts:
        print(f"\nСоздание таблицы с {cols} столбцами...")
        generator = TestTableGenerator(num_rows=rows, num_cols=cols)
        generator.generate_table(formula_probability=formula_prob)
        generator.save_to_csv(f'test_csv_files/incremental_columns/table_{rows}rows_{cols}cols.csv')

def generate_edge_case_tables():
    """
    Генерация таблиц для краевых случаев
    """
    os.makedirs('test_csv_files/edge_cases', exist_ok=True)
    
    print("\n" + "="*60)
    print("Генерация таблиц для краевых случаев...")
    print("="*60)
    
    # 1. Минимальная таблица
    print("\n1. Минимальная таблица (1 строка, 1 столбец)")
    gen1 = TestTableGenerator(num_rows=1, num_cols=1)
    gen1.generate_table(formula_probability=0.5)
    gen1.save_to_csv('test_csv_files/edge_cases/minimal_table.csv')
    
    # 2. Очень широкая таблица
    print("\n2. Очень широкая таблица (10 строк, 100 столбцов)")
    gen2 = TestTableGenerator(num_rows=10, num_cols=100)
    gen2.generate_table(formula_probability=0.2)
    gen2.save_to_csv('test_csv_files/edge_cases/very_wide_table.csv')
    
    # 3. Очень длинная таблица
    print("\n3. Очень длинная таблица (500 строк, 5 столбцов)")
    gen3 = TestTableGenerator(num_rows=500, num_cols=5)
    gen3.generate_table(formula_probability=0.1)
    gen3.save_to_csv('test_csv_files/edge_cases/very_long_table.csv')
    
    # 4. Таблица с редкими номерами строк
    print("\n4. Таблица с разреженными номерами строк")
    gen4 = TestTableGenerator(num_rows=20, num_cols=5)
    # Ручная установка разреженных номеров строк
    sparse_rows = [1, 2, 5, 10, 15, 20, 30, 50, 100, 150, 200, 300, 400, 500, 600, 700, 800, 900, 950, 999]
    gen4.data = {}
    for idx, row_num in enumerate(sparse_rows[:20]):
        gen4.data[row_num] = [str(gen4._get_random_positive_number()) for _ in range(5)]
        # Добавляем несколько формул (без пробелов)
        if idx % 3 == 0 and idx > 0:
            gen4.data[row_num][0] = f"=A{sparse_rows[idx-1]}+10"
    gen4.save_to_csv('test_csv_files/edge_cases/sparse_rows_table.csv')
    
    # 5. Таблица со сложными формулами
    print("\n5. Таблица со сложными формулами (ссылки на разные столбцы)")
    complex_formulas = TestTableGenerator(num_rows=15, num_cols=8)
    complex_formulas.generate_table(formula_probability=0.8)
    complex_formulas.save_to_csv_with_cell('test_csv_files/edge_cases/complex_formulas.csv')

def generate_all_test_tables():
    """
    Генерация всех типов тестовых таблиц
    """
    print("="*70)
    print("НАЧАЛО ГЕНЕРАЦИИ ТЕСТОВЫХ ТАБЛИЦ CSV (Unix line endings \\n)")
    print("="*70)
    
    # Создаем основные директории
    os.makedirs('test_csv_files', exist_ok=True)
    
    # Генерируем все виды таблиц
    generate_various_table_sizes()
    generate_tables_with_cell_column()
    generate_tables_specific_formula_density()
    generate_incremental_column_tables()
    generate_edge_case_tables()
    
    # Создаем пример из задания
    create_exactly_as_example()
    
    print("\n" + "="*70)
    print("ГЕНЕРАЦИЯ ЗАВЕРШЕНА")
    print("="*70)
    print("\nСтруктура сгенерированных файлов:")
    print("test_csv_files/")
    print("├── various_sizes/        # Таблицы разного размера")
    print("├── with_cell_column/     # Таблицы со столбцом Cell")
    print("├── formula_density/      # Таблицы с разной плотностью формул")
    print("├── incremental_columns/  # Таблицы с увеличивающимся числом столбцов")
    print("├── edge_cases/          # Краевые случаи")
    print("└── example_from_task.csv # Пример из задания")
    print("\nВсе файлы используют Unix line endings (\\n)")

def create_exactly_as_example():
    """Создает файл, точно соответствующий примеру из задания с Unix line endings"""
    filename = 'test_csv_files/example_from_task.csv'
    
    data = [
        ['', 'A', 'B', 'Cell'],
        ['1', '1', '0', '1'],
        ['2', '2', '= A1+Cell30', '0'],
        ['30', '0', '= B1+A1', '5']
    ]
    
    with open(filename, 'w', newline='', encoding='utf-8') as csvfile:
        writer = csv.writer(csvfile, lineterminator='\n')
        writer.writerows(data)
    
    print(f"\nФайл с примером создан: {filename}")

def generate_summary_report():
    """
    Создает отчет о сгенерированных таблицах
    """
    report_path = 'test_csv_files/generation_report.txt'
    
    with open(report_path, 'w', encoding='utf-8') as f:
        f.write("ОТЧЕТ О ГЕНЕРАЦИИ ТЕСТОВЫХ ТАБЛИЦ\n")
        f.write("="*50 + "\n\n")
        f.write("Сгенерированы следующие типы таблиц:\n\n")
        
        f.write("1. various_sizes/ - Таблицы разного размера\n")
        f.write("   - От 5x2 до 200x20\n")
        f.write("   - Разная вероятность формул\n\n")
        
        f.write("2. with_cell_column/ - Таблицы со столбцом Cell\n")
        f.write("   - Размеры от 10x3 до 50x8\n\n")
        
        f.write("3. formula_density/ - Разная плотность формул\n")
        f.write("   - От 0% до 100% формул\n")
        f.write("   - Размер 30x10\n\n")
        
        f.write("4. incremental_columns/ - Увеличивающееся число столбцов\n")
        f.write("   - От 2 до 50 столбцов\n")
        f.write("   - Фиксированное число строк (20)\n\n")
        
        f.write("5. edge_cases/ - Краевые случаи\n")
        f.write("   - Минимальная таблица (1x1)\n")
        f.write("   - Очень широкая (10x100)\n")
        f.write("   - Очень длинная (500x5)\n")
        f.write("   - Разреженные номера строк\n")
        f.write("   - Сложные формулы\n\n")
        
        f.write("ФОРМАТ ФОРМУЛ:\n")
        f.write("   - Формулы записываются без пробелов: =A1+B2, =Cell5*3, =100/2\n")
        f.write("   - Поддерживаются операции: +, -, *, /\n")
        f.write("   - Аргументы: целые положительные числа или адреса ячеек\n\n")
        
        f.write("ФОРМАТ СТРОК:\n")
        f.write("   - Используются Unix line endings (\\n)\n")
        f.write("   - Кодировка UTF-8\n\n")
        
        f.write("Все числа в таблицах положительные.\n")
    
    print(f"\nОтчет сохранен: {report_path}")

def generate_demo_table():
    """
    Генерирует демонстрационную таблицу с различными типами формул (без пробелов)
    """
    print("\n" + "="*50)
    print("Генерация демонстрационной таблицы с разными формулами")
    print("="*50)
    
    demo_data = {
        1: ['100', '200', '=A1+B1', '=Cell5*2'],
        2: ['50', '=A1-25', '=B2*3', '150'],
        3: ['=C1/2', '75', '=A3+Cell1', '=B1+C1'],
        5: ['25', '=Cell3+50', '=A5*4', '200'],
        8: ['=B5-10', '=A1+Cell3', '500', '=D8/5'],
        10: ['1000', '=Cell8*2', '=A10-B10', '750']
    }
    
    col_names = ['A', 'B', 'C', 'Cell']
    
    with open('test_csv_files/demo_table.csv', 'w', newline='', encoding='utf-8') as csvfile:
        writer = csv.writer(csvfile, lineterminator='\n')
        writer.writerow([''] + col_names)
        
        for row_num in sorted(demo_data.keys()):
            writer.writerow([str(row_num)] + demo_data[row_num])
    
    print("Демонстрационная таблица создана: test_csv_files/demo_table.csv")
    print("\nПримеры формул в демо-таблице:")
    print("  =A1+B1    - сложение")
    print("  =A1-25    - вычитание")
    print("  =C1/2     - деление")
    print("  =B2*3     - умножение")
    print("  =Cell5*2  - ссылка на столбец Cell")
    print("  =A1+Cell3 - комбинированная ссылка")

# Примеры использования
if __name__ == "__main__":
    # Устанавливаем seed для воспроизводимости
    random.seed(42)
    
    # Генерируем все тестовые таблицы
    generate_all_test_tables()
    
    # Создаем демонстрационную таблицу
    generate_demo_table()
    
    # Создаем отчет
    generate_summary_report()
    
    print("\n" + "="*50)
    print("ВАЖНО: Все файлы используют Unix line endings (\\n)")
    print("Windows-пользователи: большинство редакторов корректно отображают \\n")
    print("="*50)
    
    print("\nСОВЕТ: Для просмотра всех сгенерированных файлов")
    print("откройте директорию 'test_csv_files'")
    
    # Пример создания одной кастомной таблицы
    print("\nПример создания кастомной таблицы без пробелов в формулах:")
    custom_gen = TestTableGenerator(num_rows=25, num_cols=12)
    custom_gen.generate_table(formula_probability=0.4)
    custom_gen.save_to_csv_with_cell('test_csv_files/my_custom_table.csv')