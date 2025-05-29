#!/usr/bin/env python3
import os 
import sys
import json
import matplotlib.pyplot as plt

def main():
    try:
        fig, ax = plt.subplots(constrained_layout=True)
        # Чтение JSON из стандартного потока (stdin)
        data = json.load(sys.stdin)
        # Извлечение списка точек
        points = data["points"]

        # Добавление каждой точки на график
        for pt in points:
            filename = os.path.basename(pt["file"])  # Имя файла без пути
            ax.scatter(pt["x"], pt["y"], label=f'{filename}:{pt["group"]}', alpha=0.7)

        # Уникальные подписи
        handles, labels = ax.get_legend_handles_labels()
        by_label = dict(zip(labels, handles))
        ax.legend(
            by_label.values(),
            by_label.keys(),
            bbox_to_anchor=(1.05, 1),     # Легенда справа от графика
            loc='upper left',
            borderaxespad=0.,
            fontsize='small'
        )

        # Настройка осей и заголовка
        ax.set_title("Точки из файлов")
        ax.set_xlabel("X")
        ax.set_ylabel("Y")
        ax.grid(True)
        
        # Путь для сохранения выходного изображения
        output_dir = os.path.expanduser("~/l2_project/bin")
        output_path = os.path.join(output_dir, "output.png")
        # Сохранение график в файл
        plt.savefig(output_path, bbox_inches='tight')
    except Exception as e:
        # Обработка и вывод ошибок
        print(f"Ошибка: {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()
