
#!/usr/bin/env python

from OpenGL.GL import *
from OpenGL.GLU import *
import pygame
from pygame.locals import *
import serial
import csv
from datetime import datetime
import os
import time


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
temp = altitude = pressure = max_altitude = starting_altitude = 0.0
packet_num = 1
id = 0
max_altitude_reached = deploy_main_para_parachute = False

packet_num_before = 0
number_packet_failed = 0
system_armed = False
packet_failed = False

def parse_serial():
    global pitch, yaw, roll, ax, ay, az, temp, pressure, altitude
    global max_altitude, max_altitude_reached, deploy_main_para_parachute
    global packet_num, id
    global packet_num_before
    global number_packet_failed
    global system_armed
    global packet_failed
    global starting_altitude

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


                # Parse all fields
                id = int(data.get("ID", 0))
                packet_num = int(data.get("PACKET", 0))

                #print(f"packet_num {packet_num} | packets failed {number_packet_failed}")

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
                starting_altitude = data.get("START_ALT", 0.0)
                max_altitude_reached = bool(int(data.get("MAX_ALT_REACHED", 0)))
                deploy_main_para_parachute = bool(int(data.get("DEPLOYED", 0)))

                #print(f"System armed: {system_armed} packet_before={packet_num_before} | packet_now={packet_num}")

                if(system_armed == True and packet_num_before == packet_num) :
                    number_packet_failed += 1
                    packet_failed = True

                if(packet_num <= 5):
                    number_packet_failed = 0
                    packet_failed = False

                packet_num_before = packet_num

        elif line.startswith("System_armed_:_true"):
            system_armed = True
            print("Updated the python system_armed (if it works, remove the reset)")
        elif line.startswith("System_armed_:_false"):
            system_armed = False
            print("Updated the python system_armed (if it works, remove the reset)")

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

def draw_text(x, y, text_string, color=(255, 255, 255)):
    font = pygame.font.SysFont("Courier", 18, True)
    text_surface = font.render(text_string, True, color, (0, 0, 0))
    text_data = pygame.image.tostring(text_surface, "RGBA", True)

    glWindowPos2d(x, y)
    glDrawPixels(text_surface.get_width(), text_surface.get_height(), GL_RGBA, GL_UNSIGNED_BYTE, text_data)



def main():
    global number_packet_failed
    global system_armed
    global packet_failed
    width = 1200
    height = 600
    #width = 1450
    #height = 825
    spacing = 25

    pygame.init()
    screen = pygame.display.set_mode((width, height), DOUBLEBUF | OPENGL)
    pygame.display.set_caption("ESP32 3D Orientation Viewer")

    resize(width, height)
    init()
    clock = pygame.time.Clock()

    font = pygame.font.SysFont("Courier", 18, True)

    running = True
    while running:
        packet_failed = False

        #print(f"System armed: {system_armed}") #debug

        for event in pygame.event.get():
            if event.type == QUIT or (event.type == KEYDOWN and event.key == K_ESCAPE):
                if(system_armed == True) :
                    print("[→] Turning system off")
                    ser.write(b"START\n")
                running = False

            # KEYDOWN BLOCK START
            if event.type == KEYDOWN:
                if event.key == K_s:
                    print("[→] Sending START to ESP32")
                    ser.write(b"START\n")
                    system_armed = (not system_armed)
                    if(system_armed == True):
                        number_packet_failed = 0
                elif event.key == K_r:
                    print("[→] Switches the system_armed boolean")
                    system_armed = (not system_armed)

                elif event.key == K_b:
                    print("[→] Turning GPIO2 on/off")
                    ser.write(b"BLINK\n")




            # KEYDOWN BLOCK END

        parse_serial()
        draw()

        if(pitch != 0 and yaw != 0 and roll != 0 and ax != 0 and ay != 0 and az != 0 and temp != 0 and pressure != 0 and altitude != 0):
            log_writer.writerow([
                datetime.now().isoformat(timespec='milliseconds'),
                packet_num, pitch, yaw, roll,
                ax, ay, az, temp, pressure, altitude,
                max_altitude, int(max_altitude_reached), int(deploy_main_para_parachute)
            ])


        col_spacing = 150
        row_spacing = 25
        right_x = width - col_spacing  # shift left from right edge
        top_y = height - row_spacing

        draw_text(right_x - 3 * col_spacing, top_y - row_spacing - 15, f"-----------------------------------------------------------")
        if(max_altitude_reached == True):
            draw_text(right_x - 1 * col_spacing-15, top_y,"Max Alt Reached!",   color=(255, 0, 0))
            draw_text(right_x - 1 * col_spacing, top_y - row_spacing, f"{max_altitude:.2f}",     color=(255, 0, 0))
        else:
            draw_text(right_x - 1 * col_spacing, top_y,     "Max Alt",   color=(200, 200, 200))
            draw_text(right_x - 1 * col_spacing, top_y - row_spacing, f"{max_altitude:.2f}",     color=(255, 255, 255))

    # === Header Row ===
        draw_text(right_x - 0 * col_spacing, top_y,     "Start Alt", color=(200, 200, 200))

        draw_text(right_x - 2 * col_spacing, top_y,     "Alt",       color=(200, 200, 200))
        draw_text(right_x - 3 * col_spacing, top_y,     "Alt Δ",     color=(200, 200, 200))

        # === Value Row ===
        draw_text(right_x - 0 * col_spacing, top_y - row_spacing, f"{starting_altitude:.2f}", color=(255, 255, 255))
        draw_text(right_x - 2 * col_spacing, top_y - row_spacing, f"{altitude:.2f}",         color=(255, 255, 255))
        draw_text(right_x - 3 * col_spacing, top_y - row_spacing, f"{altitude-starting_altitude:.2f}",    color=(255, 255, 255))

        # Display telemetry data
        draw_text(10, height - 1 * spacing, f"PITCH:{pitch:6.2f}   | YAW:  {yaw     :6.2f}   | ROLL: {roll:6.2f}")
        draw_text(10, height - 2 * spacing, f"AX:   {ax   :6.2f}g  | AY:   {ay      :6.2f}g  | AZ: {az:6.2f}g")
        draw_text(10, height - 3 * spacing, f"TEMP: {temp :6.2f}°C")

        draw_text(width / 2, height - 3 * spacing, f"% packets received:{((packet_num - number_packet_failed) / packet_num) * 100:4.2f}%",)

        if(packet_failed == False):
            draw_text(width / 2 + 300, height - 3 * spacing, f"Receiving (fail:{number_packet_failed})", (128, 255, 0))
        else :
            draw_text(width / 2 + 300, height - 3 * spacing, f"   Fail!  (fail:{number_packet_failed})", (255, 0, 0))

        if(deploy_main_para_parachute == True):
            draw_text(10,  10, f"Main parachute deployed: {deploy_main_para_parachute}", color=(0,255,0))
        else :
            if(max_altitude_reached == True):
                draw_text(10,  10, f"Main parachute deployed: {deploy_main_para_parachute}", color=(100,100,100))
            else:
                draw_text(10,  10, f"Main parachute deployed: {deploy_main_para_parachute}", color=(200,200,200))
        if(system_armed == True):
            draw_text(10,  10+spacing, f"SYSTEM ARMED", color=(0,255,0))
        else:
            draw_text(10,  10+spacing, f"SYSTEM UNARMED", color=(180, 180, 180))




        pygame.display.flip()
        clock.tick(60)

    time.sleep(0.25)
    pygame.quit()
    ser.close()
    log_file.close()


if __name__ == '__main__':
    log_file, log_writer = create_log_file()
    main()
