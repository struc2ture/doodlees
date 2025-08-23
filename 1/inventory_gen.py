import random

ITEMS = [
    'Piece of a broken clay pot',
    'Half-eaten googlut',
    'Quill',
    'Broken-off sword hilt"',
    'Scrap paper',
    'Inkpot with dried ink',
    'Round stone',
    'Slingshot',
    'Sling',
    'Piece of old-world circuitry',
    'Bandage',
    'Burnt book',
    'Schematic',
    'Stick',
    'Stale loaf of bread',
    'Glass bottle',
    'Blown glass figurine',
    'Glass ice',
    'Brown dust',
    'Bear claw',
    'Glass mask',
    'Broken glasses',
    
]

def main():
    inventory = random.sample(ITEMS, 3)
    print(inventory)

if __name__ == "__main__":
    main()
