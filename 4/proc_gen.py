import random

class V2I:
    def __init__(self, x, y):
        self.x = int(x)
        self.y = int(y)

    def __repr__(self):
        return f'V2({self.x}, {self.y})'
    
    def add(self, v):
        return V2I(self.x + v.x, self.y + v.y)

class Map:
    cols = 128
    rows = 32
    tiles = [' '] * cols * rows
    rooms = []

    def bound_check(self, col, row):
        return col >= 0 and col < self.cols and row >= 0 and row < self.rows

    def set_tile(self, col, row, tile):
        if self.bound_check(col, row):
            self.tiles[row * self.cols + col] = tile

    def get_tile(self, col, row):
        if self.bound_check(col, row):
            return self.tiles[row * self.cols + col]

    def get_str(self):
        str = ""
        for row in range(self.rows):
            for col in range(self.cols):
                str += self.get_tile(col, row)
            str += "\n"
        return str

    def get_map_center(self):
        return V2I(self.cols / 2, self.rows / 2)

    def get_fill_rate(self, desired_tile):
        desired_tiles = 0
        for row in range(self.rows):
            for col in range(self.cols):
                if self.get_tile(col, row) == desired_tile:
                    desired_tiles += 1
        return desired_tiles / (self.rows * self.cols)

    def generate_box(self):
        for row in range(self.rows):
            for col in range(self.cols):
                if row == 0 or col == 0 or row == self.rows - 1 or col == self.cols - 1:
                    self.set_tile(col, row, '#')
                else:
                    self.set_tile(col, row, '.')

    def fill(self, tile):
        for row in range(self.rows):
            for col in range(self.cols):
                self.set_tile(col, row, tile)

    def carve_room(self, room):
        for row in range(room.y + 1, room.get_max().y - 1):
            for col in range(room.x + 1, room.get_max().x - 1):
                self.set_tile(col, row, '.')

    def carve_horizontal_line(self, y, x0, x1):
        x0, x1 = swap_in_order(x0, x1)
        for x in range(x0, x1 + 1):
            self.set_tile(x, y, '.')

    def carve_vertical_line(self, x, y0, y1):
        y0, y1 = swap_in_order(y0, y1)
        for y in range(y0, y1 + 1):
            self.set_tile(x, y, '.')

    def carve_corridor(self, a, b):
        option = random.randrange(1)
        if option == 0:
            self.carve_horizontal_line(a.y, a.x, b.x)
            self.carve_vertical_line(b.x, a.y, b.y)
        else:
            self.carve_vertical_line(a.x, a.y, b.y)
            self.carve_horizontal_line(b.y, a.x, b.x)

    def create_up_to_n_rooms(self, n, min_w, max_w, min_h, max_h):
        for i in range(n):
            new_room = Map.Room.create_random(min_w, max_w, min_h, max_h, self.cols, self.rows)
            valid = True
            for existing_room in self.rooms:
                if Map.Room.intersect(new_room, existing_room):
                    valid = False
                    break
            if valid:
                self.rooms.append(new_room)

    def generate_rooms(self):
        self.fill('#')
        self.create_up_to_n_rooms(10, 7, 20, 7, 15)
        self.create_up_to_n_rooms(10, 5, 15, 5, 13)
        self.create_up_to_n_rooms(100000, 5, 8, 5, 6)
        for room in self.rooms:
            self.carve_room(room)

        for i in range(len(self.rooms) - 1):
            a = self.rooms[i]
            b = self.rooms[i + 1]
            self.carve_corridor(a.get_center(), b.get_center())

        print(f"Generated rooms: {len(map.rooms)}")

    def generate_random_walk(self, desired_fill_rate):
        self.fill('#')
        center_pos = self.get_map_center()
        starting_pos = V2I(random.randint(center_pos.x - 5, center_pos.x + 5), random.randint(center_pos.y - 5, center_pos.y + 5))
        self.set_tile(starting_pos.x, starting_pos.y, '.')
        curr_pos = starting_pos
        total_tiles = self.cols * self.rows
        placed_tiles = 0
        fill_rate = placed_tiles / total_tiles
        iterations = 0
        while fill_rate < desired_fill_rate:
            next_dir = random.choice([V2I(1, 0), V2I(-1, 0), V2I(0, 1), V2I(0, -1)])
            tentative_pos = curr_pos.add(next_dir)
            if self.bound_check(tentative_pos.x, tentative_pos.y):
                curr_pos = tentative_pos
            if self.get_tile(curr_pos.x, curr_pos.y) != '.':
                self.set_tile(curr_pos.x, curr_pos.y, '.')
                placed_tiles += 1
                fill_rate = placed_tiles / total_tiles
            iterations += 1

        print(f"Random walk: Achieved fill_rate {fill_rate} after {iterations} iterations")

    class Room:
        def __init__(self, x, y, w, h):
            self.x = x
            self.y = y
            self.w = w
            self.h = h

        def get_max(self):
            return V2I(self.x + self.w, self.y + self.h)
        
        def get_center(self):
            return V2I(self.x + self.w / 2, self.y + self.h / 2)

        def __repr__(self):
            return f'Room({self.x}, {self.y}, {self.w}, {self.h})'

        def create_random(min_w, max_w, min_h, max_h, map_cols, map_rows):
            w = random.randint(min_w, max_w)
            h = random.randint(min_h, max_h)
            x = random.randint(0, map_cols - w)
            y = random.randint(0, map_rows - h)
            return Map.Room(x, y, w, h)

        def intersect(a, b):
            a_max = a.get_max()
            b_max = b.get_max()
            return b_max.x >= a.x and b.x <= a_max.x and b_max.y >= a.y and b.y <= a_max.y

def swap_in_order(x0, x1):
    if x0 > x1:
        return x1, x0
    else:
        return x0, x1


def main():
    map = Map()
    tiles = [' '] * 4 * 4
    # map.generate_box()
    # map.generate_rooms()
    map.generate_random_walk(0.6)
    print(map.get_str())

if __name__ == "__main__":
    main()
