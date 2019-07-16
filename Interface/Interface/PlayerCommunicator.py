from Constants import*

import time
import os
import sys
from subprocess import PIPE, Popen
from threading  import Thread
from queue import Queue, Empty

ON_POSIX = 'posix' in sys.builtin_module_names

class PlayerCommunicator:
    def __init__(self,player_bot : str,player_id : str):
        self.human = False
        self.id = player_id
        self.player_process = Popen(player_bot,shell=True, stdout=PIPE, bufsize=1, stdin=PIPE, close_fds=ON_POSIX)
        self.q = Queue()
        self.t = Thread(target=self.enqueue_output, args=(self.player_process.stdout, self.q))
        self.t.daemon = True # thread dies with the program
        self.t.start()

    def send(self, msg):
        self.player_process.stdin.write(bytes(msg + "\n", 'UTF-8'))
        self.player_process.stdin.flush()

    def receive(self, cast):
        msg = ""
        start = time.time()
        while msg=="":
            try:  
                data = self.q.get_nowait() # or q.get(timeout=.1)
            except Empty:
                if time.time() - start > TIME_LIMIT:
                    return ""
            else:
                msg = data.decode("utf-8").strip()
                
        return cast(msg)

    def finish(self):
        self.player_process.kill()

    def enqueue_output(self,out, queue):
        while True:
            line = out.readline()
            queue.put(line)
        out.close()

    #def enqueue_output(self,out, queue):
    #   for line in iter(out.readline, b''):
    #       queue.put(line)
    #   out.close()
