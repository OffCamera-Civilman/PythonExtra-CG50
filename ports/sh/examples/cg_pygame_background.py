"""PythonUltra fx-CG50 pygame.image.load() hardware test.

Copy an uncompressed 24-bit or 32-bit BMP named ``background.bmp`` beside
this script. A 396x224 image is ideal for a full-screen test with no scaling.
Press EXIT to leave the example.
"""

import pygame


pygame.init()
screen = pygame.display.set_mode((396, 224))
background = pygame.image.load("background.bmp")

running = True
while running:
    for current_event in pygame.event.get():
        if current_event.type == pygame.QUIT:
            running = False
        elif (current_event.type == pygame.KEYDOWN and
              current_event.key == pygame.K_ESCAPE):
            running = False

    screen.blit(background, (0, 0))
    pygame.display.flip()

pygame.quit()
