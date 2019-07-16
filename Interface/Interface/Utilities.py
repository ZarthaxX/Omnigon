
class Point:
    def __init__(self,_x,_y):
        self.x = _x
        self.y = _y
        
    def __eq__(self, other):
        return self.x == other.x and self.y == other.y
    
    def __str__(self):
        return str(self.x) + " " + str(self.y)
