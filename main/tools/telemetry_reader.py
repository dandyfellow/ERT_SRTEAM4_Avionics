
#!/usr/bin/env python

from OpenGL.GL import *
from OpenGL.GLU import *
import pygame
from pygame.locals import *
import serial
import csv
from datetime import datetime
import os


log_file = open("flight_log.csv", mode="w", newline="")
log_writer = csv.writer(log_file)

# Write header
log_writer.writerow([
    "timestamp", "packet_num", "pitch", "yaw", "roll",
    "ax", "ay", "az", "temp", "pressure", "altitude",
    "max_altitude", "max_altitude_reached", "parachute_deployed"
])


# === SERIAL CONFIG ===
ser = serial.Serial('COM5', 115200, timeout=1)



pitch = yaw = roll = 0.0



def create_log_file():
    if not os.path.exists("logs"):
        os.makedirs("logs")

    timestamp = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
    filename = f"logs/flight_{timestamp}.csv"
    f = open(filename, mode="w", newline="")
    writer = csv.writer(f)
    writer.writerow([
        "timestamp", "packet_num", "pitch", "yaw", "roll",
        "ax", "ay", "az", "temp", "pressure", "altitude",
        "max_altitude", "max_altitude_reached", "parachute_deployed"
    ])
    return f, writer

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

# Global variables
pitch = yaw = roll = 0.0
ax = ay = az = 0.0
temp = altitude = pressure = max_altitude = 0.0
packet_num = 1
id = 0
max_altitude_reached = deploy_main_para_parachute = False

number_packet_received = 0
def parse_serial():
    global pitch, yaw, roll, ax, ay, az, temp, pressure, altitude
    global max_altitude, max_altitude_reached, deploy_main_para_parachute
    global packet_num, id
    global number_packet_received

    try:
        line = ser.readline().decode('utf-8').strip()


        if line.startswith("*123*--For_Python--*456*"):
            data_start = line.find("ID:")
            if data_start != -1:
                data_part = line[data_start:]
                data_items = data_part.split(',')
                data = {}
                for item in data_items:
                    if ':' in item:
                        key, value = item.split(':')
                        data[key.strip()] = float(value.strip())

                number_packet_received += 1
                # Parse all fields
                id = int(data.get("ID", 0))
                packet_num = int(data.get("PACKET", 0))
                pitch = data.get("PITCH", 0.0)
                yaw = data.get("YAW", 0.0)
                roll = data.get("ROLL", 0.0)
                ax = data.get("AX", 0.0)
                ay = data.get("AY", 0.0)
                az = data.get("AZ", 0.0)
                temp = data.get("TEMP", 0.0)
                pressure = data.get("PRESS", 0.0)
                altitude = data.get("ALT", 0.0)
                max_altitude = data.get("MAX_ALT", 0.0)
                max_altitude_reached = bool(int(data.get("MAX_ALT_REACHED", 0)))
                deploy_main_para_parachute = bool(int(data.get("DEPLOYED", 0)))
        else:
            print(line)
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

def draw_text(x, y, text_string):
    font = pygame.font.SysFont("Courier", 18, True)
    text_surface = font.render(text_string, True, (255, 255, 255), (0, 0, 0))
    text_data = pygame.image.tostring(text_surface, "RGBA", True)

    glWindowPos2d(x, y)
    glDrawPixels(text_surface.get_width(), text_surface.get_height(), GL_RGBA, GL_UNSIGNED_BYTE, text_data)



def main():
    width = 1450
    height = 825
    spacing = 25
    global number_packet_received

    pygame.init()
    screen = pygame.display.set_mode((width, height), DOUBLEBUF | OPENGL)
    pygame.display.set_caption("ESP32 3D Orientation Viewer")

    resize(width, height)
    init()
    clock = pygame.time.Clock()

    font = pygame.font.SysFont("Courier", 18, True)

    running = True
    while running:
        for event in pygame.event.get():
            if event.type == QUIT or (event.type == KEYDOWN and event.key == K_ESCAPE):
                running = False

            # KEYDOWN BLOCK START
            if event.type == KEYDOWN:
                if event.key == K_s:
                    print("[→] Sending START to ESP32")
                    ser.write(b"START\n")
                    number_packet_received = 0
            # KEYDOWN BLOCK END



        parse_serial()
        draw()

        log_writer.writerow([
            datetime.now().isoformat(timespec='milliseconds'),
            packet_num, pitch, yaw, roll,
            ax, ay, az, temp, pressure, altitude,
            max_altitude, int(max_altitude_reached), int(deploy_main_para_parachute)
        ])


        # Display telemetry data
        draw_text(10, height - 1 * spacing, f"PITCH:{pitch:6.2f}   | YAW:  {yaw     :6.2f}   | ROLL: {roll:6.2f}")
        draw_text(10, height - 2 * spacing, f"AX:   {ax   :6.2f}g  | AY:   {ay      :6.2f}g  | AZ: {az:6.2f}g")
        draw_text(10, height - 3 * spacing, f"TEMP: {temp :6.2f}°C | ALT.: {altitude:6.2f}m")
        draw_text(width / 2, height - spacing, f"Max alt.: {max_altitude:6.2f}m   Reached: {max_altitude_reached}")
        draw_text(width / 2, height - 2 * spacing, f"Starting alt.: not yet")
        if packet_num > 0:
            draw_text(width / 2, height - 3 * spacing, f"% packets received: {(number_packet_received+1) / packet_num * 100:6.2f}%")
        draw_text(width / 2, height - 4 * spacing, f"Main parachute deployed: {deploy_main_para_parachute}")

        pygame.display.flip()
        clock.tick(60)

    pygame.quit()
    ser.close()
    log_file.close()


if __name__ == '__main__':
    log_file, log_writer = create_log_file()
    main()
