
#!/usr/bin/env python

from OpenGL.GL import *
from OpenGL.GLU import *
import pygame
from pygame.locals import *
import serial

# === SERIAL CONFIG ===
ser = serial.Serial('COM5', 115200, timeout=1)

pitch = yaw = roll = 0.0

def resize(width, height):
    if height == 0:
        height = 1
    glViewport(0, 0, width, height)
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    gluPerspective(45, width / float(height), 0.1, 100.0)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()

def init():
    glShadeModel(GL_SMOOTH)
    glClearColor(0.0, 0.0, 0.0, 0.0)
    glClearDepth(1.0)
    glEnable(GL_DEPTH_TEST)
    glDepthFunc(GL_LEQUAL)
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST)

def draw_box():
    glBegin(GL_QUADS)
    glColor3f(1, 0, 0)  # Red
    glVertex3f( 1, 1,-1)
    glVertex3f(-1, 1,-1)
    glVertex3f(-1, 1, 1)
    glVertex3f( 1, 1, 1)

    glColor3f(0, 1, 0)  # Green
    glVertex3f( 1,-1, 1)
    glVertex3f(-1,-1, 1)
    glVertex3f(-1,-1,-1)
    glVertex3f( 1,-1,-1)

    glColor3f(0, 0, 1)  # Blue
    glVertex3f(-1, 1, 1)
    glVertex3f(-1, 1,-1)
    glVertex3f(-1,-1,-1)
    glVertex3f(-1,-1, 1)

    glColor3f(1, 1, 0)  # Yellow
    glVertex3f( 1, 1,-1)
    glVertex3f( 1, 1, 1)
    glVertex3f( 1,-1, 1)
    glVertex3f( 1,-1,-1)

    glColor3f(1, 0, 1)  # Magenta
    glVertex3f( 1, 1, 1)
    glVertex3f(-1, 1, 1)
    glVertex3f(-1,-1, 1)
    glVertex3f( 1,-1, 1)

    glColor3f(0, 1, 1)  # Cyan
    glVertex3f( 1,-1,-1)
    glVertex3f(-1,-1,-1)
    glVertex3f(-1, 1,-1)
    glVertex3f( 1, 1,-1)
    glEnd()

def parse_serial():
    global pitch, yaw, roll
    pitch = 0.2
    yaw = 0.5
    roll = -0.3
    try:
        line = ser.readline().decode('utf-8').strip()

        # Always show terminal output
        print(line)

        if line.startswith("*123*--For_Python--*456*"):
            parts = line.split(' ', 1)
            if len(parts) == 2:
                data_part = parts[1]
                data_items = data_part.split(',')
                data = {}
                for item in data_items:
                    if ':' in item:
                        key, value = item.split(':')
                        data[key.strip()] = float(value.strip())

                # Extract only the needed orientation data
                pitch = data.get("PITCH", 0.0)
                yaw = data.get("YAW", 0.0)
                roll = data.get("ROLL", 0.0)

    except Exception as e:
        print(f"[!] Serial parse error: {e}")


def draw():
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glLoadIdentity()
    glTranslatef(0.0, 0.0, -6.0)

    # Apply rotations
    glRotatef(pitch * 57.2958, 1.0, 0.0, 0.0)  # Pitch
    glRotatef(roll * 57.2958, 0.0, 0.0, 1.0)   # Roll
    glRotatef(yaw * 57.2958, 0.0, 1.0, 0.0)    # Yaw

    draw_box()

def main():
    pygame.init()
    screen = pygame.display.set_mode((800, 600), DOUBLEBUF | OPENGL)
    pygame.display.set_caption("ESP32 3D Orientation Viewer")

    resize(800, 600)
    init()

    clock = pygame.time.Clock()

    running = True
    while running:
        for event in pygame.event.get():
            if event.type == QUIT or (
                    event.type == KEYDOWN and event.key == K_ESCAPE
            ):
                running = False

        parse_serial()
        draw()
        pygame.display.flip()
        clock.tick(60)

    pygame.quit()
    ser.close()

if __name__ == '__main__':
    main()
