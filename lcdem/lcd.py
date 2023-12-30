import pygame
from chars import *
import socket
import json

# Define local host and port
host = "localhost"
port = 65432

# Define grid characteristics
GRID_WIDTH = 20
GRID_HEIGHT = 4
INNER_GRID_WIDTH = 5
INNER_GRID_HEIGHT = 8

# Define grid spacing
SPACING_BETWEEN_GRIDS_X = 8
SPACING_BETWEEN_GRIDS_Y = 10
SPACING = 1
INNER_RECT_SIZE = 8

# Define colors
WHITE = (255, 255, 255)
BLUE = (0, 0, 255)

# Define grid offsets
offset_x = (INNER_GRID_WIDTH + 1) + (INNER_RECT_SIZE * INNER_GRID_WIDTH)
offset_y = (INNER_GRID_HEIGHT + 2) + (INNER_RECT_SIZE * INNER_GRID_HEIGHT)

# Variables for keeping track of program state and input
running = True
last_input = None
input_data = ""

# Mapping cases to functions
case_to_function = {
    "ph1234567890": ph,
    "km1234567890": km,
    "ah1234567890": ah,
    "dgC1234567890": dgC,
    "k11234567890": k1,
    "mt1234567890": mt,
    "ms1234567890": ms,
    "bat1234567890": bat
}

# Mapping characters to functions
char_to_function = {
    "0": zero, "1": one, "2": two, "3": three, "4": four,
    "5": five, "6": six, "7": seven, "8": eight, "9": nine,
    "A": A, "B": B, "C": C, "D": D, "E": E, "F": F,
    "G": G, "H": H, "I": I, "J": J, "K": K, "L": L,
    "M": M, "N": N, "O": O, "P": P, "Q": Q, "R": R,
    "S": S, "T": T, "U": U, "V": V, "W": W, "X": X,
    "Y": Y, "Z": Z, "a": a, "b": b, "c": c, "d": d,
    "e": e, "f": f, "g": g, "h": h, "i": i, "j": j,
    "k": k, "l": l, "m": m, "n": n, "o": o, "p": p,
    "q": q, "r": r, "s": s, "t": t, "u": u, "v": v,
    "w": w, "x": x, "y": y, "z": z, ".": dot, ",": comma,
    "+": plus, "-": minus, "!": exclamation, "?": question,
    "°": degree, "=": equals, "_": underscore, ":": colon,
    ";": semicolon, "<": less_than, ">": greater_than,
    "[": left_bracket, "]": right_bracket, "%": percentage
}

# Initialize the socket
s1 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s1.bind((host, port))
s1.setblocking(0)

# Initialize pygame
pygame.init()

# Determine window size based on grid dimensions and offsets and initialize screen
window_width = offset_x * GRID_WIDTH
window_height = offset_y * GRID_HEIGHT
window_size = (window_width, window_height)
screen = pygame.display.set_mode(window_size)
pygame.display.set_caption("LCD")
screen.fill(BLUE)
pygame.display.flip()

# Function to draw things on the screen based on the array and grid location
def drawthings(array, row, column):
    for i1 in range(INNER_GRID_WIDTH):
        for j1 in range(INNER_GRID_HEIGHT):
            index = i1 + j1 * INNER_GRID_WIDTH
            color = WHITE if array[index] == 1 else BLUE
            rect_x = (row * offset_x) + i1 * (INNER_RECT_SIZE + SPACING)
            rect_y = (column * offset_y) + j1 * (INNER_RECT_SIZE + SPACING)
            pygame.draw.rect(screen, color, (rect_x, rect_y, INNER_RECT_SIZE, INNER_RECT_SIZE))

# Program loop
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    try:
        input, addr = s1.recvfrom(1024)
        if not input:
            continue

        # Decode input data and update last input
        input_data = json.loads(input.decode())
        last_input = input_data.get("input")
        row1 = input_data.get("row", 0)
        column = input_data.get("column", 0)
        row1 = max(0, min(GRID_WIDTH-1, row1))
        column = max(0, min(GRID_HEIGHT-1, column))
        n1 = -1

        # Determine input based on whether current input is set
        current_input = last_input if input_data is None else input_data.get("input")

        if current_input is None:
            continue

        if isinstance(current_input, int):
            current_input = str(current_input)

        # If the current input matches a case, draw the function and update the input
        if current_input in case_to_function:
            drawthings(case_to_function[current_input], row1, column)
            current_input = current_input.replace(current_input, "")

        # Iterate over the length of the current input and draw the character if it exists in the map
        for l1 in str(current_input):
            n1 += 1
            row = row1 + n1

            if row > (GRID_WIDTH - 1):
                n1 = 0
                row = 0
                row1 = 0
                column += 1

                if column > (GRID_HEIGHT - 1):
                    column = 0

            drawthings(char_to_function.get(l1, space), row, column)
            pygame.display.flip()

    except socket.error:
        pass