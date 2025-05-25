
import time

class Stopwatch:
    def __init__(self):
        self.timer = None
        self.result = None
        self.reset()

    def reset(self):
        self.timer = time.perf_counter()

    def start(self):
        self.reset()

    def stop(self):
        self.result = time.perf_counter() - self.timer
        return self.result

    def wait(self, seconds):
        time.sleep(seconds)

