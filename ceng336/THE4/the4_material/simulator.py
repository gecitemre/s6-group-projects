#!/usr/bin/env python
import collections
import queue
import string
import time
import os, pygame, sys, threading, serial, time, array
from pygame.locals import *
import math
import random
from collections import deque
import json
from game_utils import *

with open("cookingSimSettings.json","r") as f:
    SETTINGS = json.loads(f.read())

DISPLAY_WIDTH = 640
DISPLAY_HEIGHT = 480

FPS = SETTINGS["FPS"]
PORT = SETTINGS["PORT"]
BAUDRATE = SETTINGS["BAUDRATE"]
WAITING = 0
GETTING = 1
timeout = 100



class Miniterm:
    def __init__(self, port, baudrate, parity, simulator, rtscts, xonxoff):
        self.serial = serial.Serial(port, baudrate, parity=parity,
                                rtscts=rtscts, xonxoff=xonxoff, timeout=None)
        self.state = WAITING
        self.data = b''
        self.time = 0
        self.avg_time = -1
        self.max_time = -1
        self.min_time = -1
        self.prev_time = -1
        self.startTime = -1
        self.endTime = -1
        self.remTime = timeout
        self.cmdCount = 0
        self.commands = queue.Queue()
        self.writer_lock = threading.Lock()
        self.simulator = simulator

        self.alive = True
        self.receiver_thread = threading.Thread(target=self.reader)
        self.receiver_thread.daemon = True
        self.receiver_thread.start()


    def start(self):
        self.cmdCount = 0
        self.state = WAITING
        self.startTime = time.time()



    def stop(self):
        self.alive = False

    def join(self):
        self.receiver_thread.join(0.1)

    def updateFPS(self):
        if self.simulator.game_state != "ACTIVE":
            return
        self.time = time.time()
        diff1 = (self.time-self.startTime)*1000.0
        self.cmdCount = self.cmdCount + 1
        self.avg_time = (diff1)/self.cmdCount

        if(self.prev_time != -1):
            diff2 = (self.time-self.prev_time)*1000.0
            if SETTINGS["LOG_VERY_VERBOSE"]:
                logging.debug(f"Time difference from prev valid command: {diff2}")
            if diff2 < self.min_time or self.min_time == -1:
                self.min_time = diff2
            if diff2 > self.max_time or self.max_time == -1:
                self.max_time = diff2
        self.prev_time = self.time

    def reader(self):
        WAITING = 0
        GETTING = 1
        while self.alive:
            if self.state == WAITING:
                byte = self.serial.read()
                if byte == b'$':
                    self.state = GETTING
            elif self.state == GETTING:
                byte = self.serial.read()
                if byte == b':':
                    try:
                        if self.simulator.game_state != "ACTIVE":
                            raise InvalidCommandException(f"Command {self.data} sent during {self.simulator.game_state} mode")
                        self.command_form_check()
                        if self.data == b'W':
                            self.updateFPS()
                            self.write_status_response()
                        elif self.data.startswith(b'C'):
                            try:
                                _,customer_id, ing_1_no, ing_2_no = self.data
                                customer_id = int(customer_id)
                                ing_1_no = None if ing_1_no == 78 else int(ing_1_no)
                                ing_2_no = None if ing_2_no == 78 else int(ing_2_no)
                            except:
                                raise IllformedCommandException(str(self.data))
                            if ing_1_no == ing_2_no:
                                raise CookCommandException(customer_id, [ing_1_no, ing_2_no], self.simulator.cooking_state, "cannot cook the same ingredient twice")
                            ingredients = [ing for ing in [ing_1_no, ing_2_no] if ing is not None]
                            self.simulator.cook(customer_id, ingredients)
                            self.updateFPS()
                            self.write_status_response()
                        elif self.data.startswith(b'S'):
                            try:
                                _, customer_id, ing_no = self.data
                                customer_id = int(customer_id)
                                ing_no = int(ing_no)
                            except:
                                raise IllformedCommandException(self.data)
                            hash_reply = self.simulator.slow_cook(customer_id, ing_no)
                            self.updateFPS()
                            self.write(b'$H'+hash_reply+b':')
                        elif self.data.startswith(b'H'):
                            self.simulator.serve_slow_cooked_food(self.data[1:])
                            self.updateFPS()
                            self.write_status_response()
                        elif self.data.startswith(b'T'):
                            try:
                                _, ing_1_no = self.data
                                ing_1_no = int(ing_1_no)
                            except:
                                raise IllformedCommandException(self.data)
                            self.simulator.toss(ing_1_no)
                            self.updateFPS()
                            self.write_status_response()
                    except InvalidCommandException as e:
                        logging.error("Invalid Command: "+e.message)
                    self.data = b""
                    self.state = WAITING
                else:
                    self.data = b"".join([self.data, byte])
    
    def write_status_response(self):
        self.write(b'$R'+self.simulator.cooking_state.get_info_bytes()+b":")
    
    def command_form_check(self):
        if not (self.data == b'W' or
               (self.data.startswith(b'C') and len(self.data) == 4) or
               (self.data.startswith(b'S') and len(self.data) == 3) or
               (self.data.startswith(b'H') and len(self.data) == 17) or
               (self.data.startswith(b'T') and len(self.data) == 2)):
            raise InvalidCommandException(str(self.data))

    def write(self, strr):
        if SETTINGS["LOG_VERY_VERBOSE"]:
            logging.debug("writing "+str(strr))
        with self.writer_lock:
            self.serial.write(strr)

CUSTOMER_SPRITE_SIZEXY = 80

def main():
    random.seed(SETTINGS["SEED"])
    pygame.init()
    clock = pygame.time.Clock()
    cooking_simulator = CookingSimulator()
    miniterm = Miniterm(PORT, BAUDRATE, 'N', cooking_simulator,  rtscts=False, xonxoff=False)
    gui_start_time = -1
    gui_final_time = -1
    gui_prev_time = -1

    customer_sprite = pygame.image.load(f"customer.jpg")
    food_judge_sprite = pygame.image.load(f"food_judge.jpg")
    talkbubble_sprite = pygame.image.load(f"talkbubble.png")
    talkbubble_sprite = pygame.transform.scale(talkbubble_sprite, (120,80))
    cooking_sprites = {strr: pygame.transform.scale(pygame.image.load(f"{strr}.png"), (30,30)) for strr in ["C", "S"]}

    game_mode_strs = ["easymode", "hardmode"]
    game_mode_sprites =  {istr : pygame.transform.scale(pygame.image.load(f"{istr}.jpg"), (250,250)) for istr in game_mode_strs}

    ingredient_sprites = {istr : pygame.transform.scale(pygame.image.load(f"{istr}.jpg"), (60,60)) for istr in INGREDIENT_LETTERS}
    ingredient_sprites_small = {istr : pygame.transform.scale(sprite, (40,40)) for (istr,sprite) in ingredient_sprites.items()}

    font_h2 = pygame.font.SysFont("Arial", 14)
    font_footer = pygame.font.SysFont("Arial", 12)

    screen = pygame.display.set_mode((DISPLAY_WIDTH, DISPLAY_HEIGHT))




    def draw_ingredient(x, y, ingredient, small=False):
        sprite = ingredient_sprites_small[ingredient.letter] if small else ingredient_sprites[ingredient.letter]
        screen.blit(sprite, (x, y))

    def draw_customer(x, y, customer):
        pygame.draw.rect(screen, (170, 170, 170), (x,y,88, 88))

        x = x+4
        y = y+4

        if customer is None:
            return
        
        if not customer.food_judge:
            screen.blit(customer_sprite, (x, y))

            customer_id_text = font_footer.render(f"id: {customer.cid:>3d}", True, (0, 0, 0))
            screen.blit(customer_id_text, (x, y-20))

            screen.blit(talkbubble_sprite, (x+CUSTOMER_SPRITE_SIZEXY+10,y-10))
            for i, ingredient in enumerate(customer.order_ingredient_container.get_list()):
                draw_ingredient(x+CUSTOMER_SPRITE_SIZEXY+20+i*50, y+5, ingredient, True)
            
            
            patience_text = font_footer.render(str(customer.get_short_patience()), True, (0, 0, 0))
            size = font_footer.size(str(customer.get_short_patience()))
            
            value = customer.patience
            pygame.draw.rect(screen, (int(255*(100-value)/100),int(255*(value)/100),0), (x, y+CUSTOMER_SPRITE_SIZEXY+10, value*70//100, size[1]))
            screen.blit(patience_text, (x+5+value*70//100, y+CUSTOMER_SPRITE_SIZEXY+10))
        else:
            screen.blit(food_judge_sprite, (x, y))

            customer_id_text = font_footer.render(f"id: {customer.cid:>3d}", True, (0, 0, 0))
            screen.blit(customer_id_text, (x, y-20))
            
            patience_text = font_footer.render(str(customer.get_short_patience()), True, (0, 0, 0))
            size = font_footer.size(str(customer.get_short_patience()))
            
            value = customer.patience
            pygame.draw.rect(screen, (int(255*(100-value)/100),int(255*(value)/100),0), (x, y+CUSTOMER_SPRITE_SIZEXY+10, value*70//100, size[1]))
            screen.blit(patience_text, (x+5+value*70//100, y+CUSTOMER_SPRITE_SIZEXY+10))

    
    def draw_held_ingredient(x, y, ingredient):
        if ingredient is None:
            y = y+35
            pygame.draw.rect(screen, (170, 170, 170), (x,y,68, 68))
            return
        if ingredient.cooking:
            screen.blit(cooking_sprites[ingredient.cooking], (x+15, y))
        
        y = y+35

        pygame.draw.rect(screen, (170, 170, 170), (x,y,68, 68))

        x = x+4
        y = y+4

        if ingredient is None:
            return

        draw_ingredient(x, y, ingredient)
    
    def draw_game_state(x, y, game_state):
        screen.blit(game_mode_sprites[game_state], (x, y))
    
    def gui_get_sim_time():
        if gui_start_time == -1:
            return 0
        if gui_final_time != -1:
            return gui_final_time-gui_start_time

        return time.time() - gui_start_time
    
    def gui_get_fps():
        nonlocal gui_prev_time
        if gui_prev_time == -1:
            gui_prev_time = time.time()
            return 0
        result = 1/(time.time()-gui_prev_time)
        gui_prev_time = time.time()
        return int(result)

    
    def draw_msg_info():
        text5 = font_h2.render(f'Money: {cooking_simulator.cooking_state.money}  Goal: {cooking_simulator.goal_money} Sent Hashes: {cooking_simulator.sent_hashes} '+
                                f'Received Answers: {cooking_simulator.received_answers}' + f' Mode: {cooking_simulator.game_state}', True, (0, 100, 0))


        text6 = font_h2.render(f'Timing[ms] Avg: {miniterm.avg_time:>4.2f} '+
                                f'Max: {miniterm.max_time:>4.2f} '+
                                f'Min: {miniterm.min_time:>4.2f} '+
                                f'Time Elapsed[s]: {int(gui_get_sim_time()):>4d} '+
                                f'FPS: {gui_get_fps():>4d}', True, (0,100,0))
        screen.blit(text5,(20.,DISPLAY_HEIGHT-60))
        screen.blit(text6,(20.,DISPLAY_HEIGHT-30))
    
    pygame.display.set_caption("CENG 336 THE4 2023")

    while True:
        screen.fill((255, 255, 255))
        draw_msg_info()
        customers = cooking_simulator.customer_container.get_list(remove_empty_slots=False)
        ingredients = cooking_simulator.ingredient_container.get_list(remove_empty_slots=False)

        for i in range(3):
            draw_customer(30, 30+i*(CUSTOMER_SPRITE_SIZEXY+50), customers[i])
        for i in range(4):
            draw_held_ingredient(300+i*(60+20),0, ingredients[i])
        if cooking_simulator.game_state == "ACTIVE":
            draw_game_state(320, 120, "easymode" if cooking_simulator.easy_mode else "hardmode")
            cooking_simulator.tick()
        
        pygame.display.update()
        for event in pygame.event.get():
            if event.type == QUIT:
                pygame.quit()
                sys.exit()
            elif event.type == KEYDOWN:
                if event.key == K_ESCAPE:
                    pygame.quit()
                    sys.exit()
                if event.key == K_e and cooking_simulator.game_state == "IDLE":
                    gui_start_time = time.time()
                    cooking_simulator.setup(True)
                    cooking_simulator.game_state = "ACTIVE"
                    cooking_simulator.start()
                    miniterm.start()
                    logging.info("GO response sent.")
                    miniterm.write(b'$GO:')
                elif event.key == K_h and cooking_simulator.game_state == "IDLE":
                    gui_start_time = time.time()
                    cooking_simulator.setup(False)
                    cooking_simulator.game_state = "ACTIVE"
                    cooking_simulator.start()
                    miniterm.start()
                    logging.info("GO response sent.")
                    miniterm.write(b'$GO:')
        
        if cooking_simulator.game_state == "ACTIVE" and gui_get_sim_time() > SETTINGS["TOTAL_TIME_MS"]/1000:
            gui_final_time = time.time()
            cooking_simulator.game_state = "END"
            logging.info("END response sent.")
            miniterm.write(b'$END:')
        
        clock.tick(FPS)
        

main()