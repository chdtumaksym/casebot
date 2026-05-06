
import time
import random
from pynput.keyboard import Controller as KeyboardController
from pynput.mouse import Controller as MouseController

# Инициализация контроллеров
keyboard = KeyboardController()
mouse = MouseController()

def simulate_activity():
    """
    Примитивная симуляция активности. 
    Объективно: VAC легко распознает отсутствие реальных векторов движения мыши.
    """
    print("Бот запущен. У тебя есть 5 секунд, чтобы переключиться на окно игры.")
    time.sleep(5)
    
    try:
        while True:
            # Движение вперед - типичный паттерн афк-фармера
            keyboard.press('w')
            time.sleep(random.uniform(0.5, 2.0))
            keyboard.release('w')
            
            # Рандомный стрейф, чтобы не стоять на месте
            key = random.choice(['a', 'd'])
            keyboard.press(key)
            time.sleep(random.uniform(0.2, 0.8))
            keyboard.release(key)
            
            # Эмуляция стрельбы (левая кнопка мыши)
            mouse.press(MouseController.Button.left)
            time.sleep(0.1)
            mouse.release(MouseController.Button.left)
            
            # Поворот камеры - здесь самая большая уязвимость для античита
            # Слишком резкие или идеально ровные повороты выдают бота
            move_x = random.randint(-100, 100)
            mouse.move(move_x, 0)
            
            # Случайная пауза между действиями
            time.sleep(random.uniform(1, 3))
            
    except KeyboardInterrupt:
        print("Симуляция остановлена пользователем.")

if __name__ == "__main__":
    simulate_activity()
