class Map:
    cols = 128
    rows = 32
    tiles = [' '] * cols * rows

    def bound_check(self, col, row):
        return col >= 0 and col < self.cols and row >= 0 and row < self.rows

    def set_tile(self, col, row, tile):
        if self.bound_check(col, row):
            self.tiles[row * self.cols + col] = tile

    def get_tile(self, col, row):
        if self.bound_check(col, row):
            return self.tiles[row * self.cols + col]

    def generate(self):
        for row in range(self.rows):
            for col in range(self.cols):
                if row == 0 or col == 0 or row == self.rows - 1 or col == self.cols - 1:
                    self.set_tile(col, row, '#')
                else:
                    self.set_tile(col, row, '.')

    def get_str(self):
        str = ""
        for row in range(self.rows):
            for col in range(self.cols):
                str += self.get_tile(col, row)
            str += "\n"
        return str


def main():
    map = Map()
    tiles = [' '] * 4 * 4
    map.generate()
    print(map.get_str())

if __name__ == "__main__":
    main()
