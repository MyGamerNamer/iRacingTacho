import pygame
import sys

# Initialize Pygame
pygame.init()

# Set the dimensions of the window
window_size = (800, 600)
screen = pygame.display.set_mode(window_size)

# Main loop
done = False
clock = pygame.time.Clock()

try:
    while not done:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                done = True

        # Fill the screen with white
        screen.fill((255, 255, 255))

        # Draw a red square
        pygame.draw.rect(screen, (255, 0, 0), pygame.Rect(30, 30, 60, 60))

        # Update the full display surface
        pygame.display.flip()

        # Limit to 60 frames per second
        clock.tick(60)
except KeyboardInterrupt:
    done = True
finally:
    pygame.quit()
    sys.exit()
